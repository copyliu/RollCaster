#ifndef UNICODE
#define UNICODE
#endif

#include "lobbyClass.h"
#include "guiClass.h"
#include "casterClass.h"
#include "guiIO.h"
#include "charConvClass.h"
#include "messageClass.h"
using namespace std;
using namespace N_Gui;


/********************************************************
 * Alive Signal thread
 ********************************************************/
unsigned __stdcall N_Gui::lobbyAliveThread(void* Address){
	if( !Address ) return 1;
	lobbyClass* lobby = (lobbyClass*)Address;
	int Counter;
	
	for(;;){
		for(Counter=0; Counter < (15 * 2); Counter++){
			Sleep(500);
			if(!lobby->continueFlg) break;
		}
		if(!lobby->continueFlg) break;
		
		lobby->SendCmd(KEEP_ALIVE);
	}
	
	return 0;
}

/********************************************************
 * Lobby Recieve Thread
 ********************************************************/
unsigned __stdcall N_Gui::lobbyThread(void* Address){
	if( !Address ) return 1;
	lobbyClass* lobby = (lobbyClass*)Address;
	guiClass* gui = g_gui;
	messageClass* message = &(g_gui->message);
	
	char* recvBuf = (char*)malloc( LOBBY_BUF_SIZE );
	if( !recvBuf ) return 1;
	BYTE* cmd = &recvBuf[ 4 ];
	
	//念のため
	if( lobby->sLobby != INVALID_SOCKET ){
		closesocket( lobby->sLobby );
	}
	
	lobby->sLobby = socket( AF_INET, SOCK_STREAM, 0 );
	if( lobby->sLobby == INVALID_SOCKET ){
		gout << "ERROR : lobby socket() failed." << endl;
		lobby->status = LOBBY_CLIENT_STATUS_ERROR;
		lobby->continueFlg = 0;
		
		return 0xF;
	}
	
	
	SOCKADDR_IN Lobby;
	memset( &Lobby, 0, sizeof( Lobby ) );
	
	{
		IN_ADDR inaddr;
		charConvClass conv(lobby->lobbyAddress);
		char szIP[256];
		memset( szIP, 0, sizeof(szIP) );
		
		hostent* h_self = gethostbyname( conv.shift );
		memcpy(&inaddr, h_self->h_addr_list[0], 4);
		strcpy(szIP, inet_ntoa(inaddr));
		Lobby.sin_addr.s_addr = inet_addr( szIP );
	}
	
	Lobby.sin_port = htons( lobby->lobbyPort );
	Lobby.sin_family = AF_INET;
	
	if( connect( lobby->sLobby, (SOCKADDR*)&Lobby, sizeof( Lobby ) ) < 0 ){
		lobby->status = LOBBY_CLIENT_STATUS_ERROR;
		lobby->continueFlg = 0;
		
		closesocket( lobby->sLobby );
		lobby->sLobby = INVALID_SOCKET;
		return 0xF;
	}
//	message->write(L"debug : lobby connected");
	lobby->connectFlg = 1;
	
	
	int Counter;
	int Res;
	DWORD nowDataSize = 0;
	for(;;){
		if( !lobby->continueFlg ) break;
		
		//分割データ対応
		for(;;){
			if( nowDataSize > 4 && nowDataSize >= (DWORD)revInt( *(int*)recvBuf ) + 5 ){
				//念のため
				if( revInt( *(int*)recvBuf ) < 0 ){
					message->write(L"ERROR : subDataSize check");
					lobby->status = LOBBY_CLIENT_STATUS_ERROR;
					lobby->continueFlg = 0;
					nowDataSize = 0;
					
					break;
				}else{
					//OK
					break;
				}
			}
			
			//recv
			Res = recv( lobby->sLobby, (BYTE*)( (DWORD)recvBuf + nowDataSize ), LOBBY_BUF_SIZE - nowDataSize, 0 );
			if( !lobby->continueFlg || Res == 0 || Res == SOCKET_ERROR ){
				if( lobby->continueFlg && !lobby->endFlg ){
					message->write(L"Lobby Disconnected");
				}
				lobby->changeStatus(LOBBY_CLIENT_STATUS_ERROR);
				lobby->status = LOBBY_CLIENT_STATUS_ERROR;
				lobby->continueFlg = 0;
				nowDataSize = 0;
				
				break;
			}
			
			nowDataSize = nowDataSize + Res;
			if( nowDataSize >= LOBBY_BUF_SIZE ){
				message->write(L"ERROR : recvBuf overflow");
				lobby->status = LOBBY_CLIENT_STATUS_ERROR;
				lobby->continueFlg = 0;
				nowDataSize = 0;
				
				break;
			}
			
			if( nowDataSize > 4 ){
				if( revInt( *(int*)recvBuf ) < 0 ){
					message->write(L"ERROR : recv subDataSize check");
					lobby->status = LOBBY_CLIENT_STATUS_ERROR;
					lobby->continueFlg = 0;
					nowDataSize = 0;
					
					break;
				}
				
				if( nowDataSize >= (DWORD)revInt( *(int*)recvBuf ) + 5 ){
					//OK
					break;
				}
			}
		}
		if( lobby->status == LOBBY_CLIENT_STATUS_ERROR || lobby->continueFlg == 0 || nowDataSize == 0 ) break;
		
		
		//cmd
		switch( *cmd ){
		case RESPONSE_VERSION :
			if( revShort( *(short*)&recvBuf[ 5 ] ) != PROTOCOL_VERSION ){
				message->write(L"ERROR : PROTOCOL_VERSION check failed");
				lobby->status = LOBBY_CLIENT_STATUS_ERROR;
				lobby->continueFlg = 0;
				
				break;
			}
			lobby->checkVersionFlg = 1;
			break;
		case RESPONSE_BYE :
			lobby->continueFlg = 0;
			break;
		case RESPONSE_NEW_CONNECT :
			if( recvBuf[ 5 ] != ACCEPTED ){
				message->write(L"ERROR : RESPONSE_NEW_CONNECT check failed");
				lobby->status = LOBBY_CLIENT_STATUS_ERROR;
				lobby->continueFlg = 0;
				
				break;
			}
			lobby->newConnectFlg = 1;
			break;
		case JOIN :
			{
				inputBufferClass dis( &recvBuf[ 5 ] );
				myString name;
				myString UID;
				myString shortMessage;
				myString longMessage;
				
				DWORD IP;
				short port;
				BYTE state;
				
				dis.readUTF( &name );
				dis.readUTF( &UID );
				dis.read( &IP, 0, 4 );
				port = dis.readShort();
				dis.readByte();	//IM setting
				dis.readByte();	//IP setting
				dis.readUTF( &longMessage );
				dis.readUTF( &shortMessage );
				state = 0;
				
				lobby->user.add( UID.local, name.local, shortMessage.local, longMessage.local, IP, (WORD)port, state );
				PostMessage(gui->hWnd, WM_COMMAND, ID_LOBBY_LIST_REFRESH, 0);
			}
			break;
		case RESPONSE_USERLIST :
			{
				int userNo = revInt( *(int*)&recvBuf[ 5 ] );
				if( userNo ){
					int Counter;
					inputBufferClass dis( &recvBuf[ 5 + 4 ] );
					for( Counter = 0; Counter < userNo; Counter++ ){
						{
							myString name;
							myString UID;
							myString shortMessage;
							myString longMessage;
							
							DWORD IP;
							short port;
							BYTE state;
							
							dis.readUTF( &name );
							dis.readUTF( &UID );
							dis.read( &IP, 0, 4 );
							port = dis.readShort();
							state = dis.readByte();
							dis.readByte();	//IM setting
							dis.readByte();	//IP setting
							dis.readUTF( &longMessage );
							dis.readUTF( &shortMessage );
							
							lobby->user.add( UID.local, name.local, shortMessage.local, longMessage.local, IP, (WORD)port, state );
						}
					}
					PostMessage(gui->hWnd, WM_COMMAND, ID_LOBBY_LIST_REFRESH, 0);
				}
			}
			lobby->userListFlg = 1;
			break;
		case RESPONSE_MODIFY_STATE :
			//TODO
			break;
		case STATE_MODIFIED :
			{
				inputBufferClass dis( &recvBuf[5] );
				myString UID;
				BYTE state;
				
				dis.readUTF( &UID );
				state = dis.readByte();
				
				if( UID.fail() ){
					message->write(L"debug : STATE_MODIFIED UID conv failed");
				}else{
					lobby->user.setState( UID.local, state );
				}
				PostMessage(gui->hWnd, WM_COMMAND, ID_LOBBY_LIST_REFRESH, 0);
			}
			break;
		case DISTRIBUTE_INSTANTMESSAGE :
			{
				inputBufferClass dis( &recvBuf[5] );
				myString from;
				myString to;
				myString messageStr;
				LONGLONG time;
				BYTE color;
				
				dis.readUTF(&from);
				dis.readUTF(&to);
				dis.readUTF(&messageStr);
				time = dis.readLong();
				color = dis.readByte();
				
				message->write(from.local, to.local, time, messageStr.local, color);
			}
			break;
		case RESPONSE_STORED_INSTANTMESSAGES :
			{
				inputBufferClass dis( &recvBuf[5] );
				BYTE responseCode = dis.readByte();
				BYTE size = dis.readByte();
				if(responseCode == ACCEPTED && size > 0){
					for (int i = 0; i < size; i++) {
						myString from;
						myString to;
						myString messageStr;
						
						dis.readUTF(&from);
						dis.readUTF(&to);
						dis.readUTF(&messageStr);
						
						LONGLONG time = dis.readLong();
						BYTE color = dis.readByte();
						
						message->write(from.local, to.local, time, messageStr.local, color);
					}
				}
				lobby->reqMssgeFlg = 1;
			}
			break;
		case DISTRIBUTED_CLIENT_TO_CLIENT_COMMAND :
			{
				inputBufferClass dis(&recvBuf[5]);
				myString fromUID;
				myString toUID;
				BYTE cccCommand;
				
				dis.readUTF(&fromUID);
				dis.readUTF(&toUID);
				cccCommand = dis.readByte();
				
				int outBufSize = 2 + fromUID.remoteSize + 1;
				BYTE* cccRes = new BYTE[ outBufSize ];
				outputBufferClass dos(cccRes, outBufSize);
				
				dos.writeUTF(&fromUID);
				
				switch( cccCommand ){
				case ACCEPTED_POST_FIGHT :
					if( (wcscmp(lobby->connectingUID, fromUID.local) == 0 || lobby->status == LOBBY_CLIENT_STATUS_HOST_WAIT) &&
					    wcscmp(toUID.local, fromUID.local) != 0){
						lobbyUserSubClass fromInfo;
						if(lobby->user.getUserInfo(fromUID.local, &fromInfo) == 0){
							WCHAR destIP[30];
							memset(destIP, 0, sizeof(destIP));
							convIP(fromInfo.IP, destIP);
							
							bool soundFlg = false;
							
							if(lobby->status == LOBBY_CLIENT_STATUS_HOST_WAIT){
								int messageSize = 200 + wcslen(fromInfo.name);
								WCHAR* messageStr = new WCHAR[messageSize];
								memset(messageStr, 0, messageSize * sizeof(WCHAR));
								
								WCHAR shortUID[4];
								wcsncpy(shortUID, fromInfo.UID, 3);
								shortUID[3] = 0;
								swprintf(messageStr, L"Connect to Host : %s@%s", fromInfo.name, shortUID);
								
								message->write(messageStr);
								delete[] messageStr;
								
								soundFlg = true;
							}
							
							if(g_caster){
								g_caster->reset(CASTER_ARG_MODE_CONNECT, destIP, fromInfo.port, soundFlg);
							}
							
							PostMessage(gui->hWnd, WM_COMMAND, ID_SET_FOCUS_TO_CONSOLE, 0);
						}
					}
					break;
				case POST_FIGHT :
					if( lobby->status == LOBBY_CLIENT_STATUS_WAIT || lobby->status == LOBBY_CLIENT_STATUS_CLIENT_WAIT){
						//return ACCEPTED_POST_FIGHT
						dos.writeByte(ACCEPTED_POST_FIGHT);
						lobby->SendCmd( POST_CLIENT_TO_CLIENT_COMMAND, dos.data, dos.nowData );
						
						if(g_caster){
							g_caster->reset(CASTER_ARG_MODE_WAIT, NULL, 0);
						}
						PostMessage(gui->hWnd, WM_COMMAND, ID_SET_FOCUS_TO_CONSOLE, 0);
						
						//誰が来たのか表示
						lobbyUserSubClass fromInfo;
						if(lobby->user.getUserInfo(fromUID.local, &fromInfo) == 0){
							
							int messageSize = 200 + wcslen(fromInfo.name);
							WCHAR* messageStr = new WCHAR[messageSize];
							memset(messageStr, 0, messageSize * sizeof(WCHAR));
							
							WCHAR shortUID[4];
							wcsncpy(shortUID, fromInfo.UID, 3);
							shortUID[3] = 0;
							swprintf(messageStr, L"Connect from %s@%s", fromInfo.name, shortUID);
							
							message->write(messageStr);
							delete[] messageStr;
						}
					}else if(lobby->status == LOBBY_CLIENT_STATUS_HOST_WAIT){
						//return POST_FIGHT
						dos.writeByte(POST_FIGHT);
						lobby->SendCmd( POST_CLIENT_TO_CLIENT_COMMAND, dos.data, dos.nowData );
					}else{
						//NON_GATHERING
						dos.writeByte(NON_GATHERING);
						lobby->SendCmd( POST_CLIENT_TO_CLIENT_COMMAND, dos.data, dos.nowData );
						message->write(L"Target is not waiting");
					}
					break;
				case NON_GATHERING:
					//TODO
					break;
				default:
					//DO NOTHING
					break;
				}
				delete[] cccRes;
			}
			break;
		case LEAVE :
			{
				inputBufferClass dis( &recvBuf[5] );
				myString UID;
				
				dis.readUTF( &UID );
				if( UID.fail() ){
					message->write(L"debug : LEAVE UID conv failed");
				}else{
					lobby->user.del( UID.local );
				}
				PostMessage(gui->hWnd, WM_COMMAND, ID_LOBBY_LIST_REFRESH, 0);
			}
			break;
		case RESPONSE_KEEP_ALIVE:
			//DO NOTHING
			break;
		default :
			//none
			
			break;
		}
		
		
		//連結データ対応
		{
			//要検討
			//複数連結データ頻度によっては改良要
			DWORD subDataSize = (DWORD)revInt( *(int*)recvBuf ) + 5;
			if( nowDataSize == subDataSize ){
				nowDataSize = 0;
			}else if( nowDataSize < subDataSize ){
				message->write(L"ERROR : subDataSize check");
				break;
			}else{
				if( nowDataSize - subDataSize > LOBBY_BUF_SIZE ){
					message->write(L"ERROR : recvBufTemp size check");
					break;
				}
				
				memmove( recvBuf, (BYTE*)( (DWORD)recvBuf + subDataSize ), nowDataSize - subDataSize );
				nowDataSize = nowDataSize - subDataSize;
			}
		}
		
		Sleep( 1 );
	}
	
	if( recvBuf ){
		free( recvBuf );
		recvBuf = NULL;
		if( recvBuf ){}
	}
	
	WaitForSingleObject( lobby->hMutex, INFINITE );
	shutdown( lobby->sLobby, SD_BOTH );
	closesocket( lobby->sLobby );
	lobby->sLobby = INVALID_SOCKET;
	ReleaseMutex( lobby->hMutex );
	
	lobby->status = LOBBY_CLIENT_STATUS_ERROR;
	return 0;
}

