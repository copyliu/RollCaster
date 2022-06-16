#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <process.h>
#include "lobbyClass.h"
#include "guiClass.h"
#include "casterClass.h"
#include "guiIO.h"
#include "messageClass.h"
#include "mutexClass.h"
using namespace std;
using namespace N_Gui;


/********************************************************
 * Constructor, Destructor
 ********************************************************/
lobbyClass::lobbyClass(){
	buf = (BYTE*)malloc( 1024 );
	status = LOBBY_CLIENT_STATUS_DEFAULT;
	hLobbyTh = NULL;
	sLobby = INVALID_SOCKET;
	continueFlg = 0;
	checkVersionFlg= 0;
	userListFlg = 0;
	newConnectFlg = 0;
	connectFlg = 0;
 	endFlg = 0;
	memset(connectingUID, 0, sizeof(connectingUID));
	
	hMutex = CreateMutex(NULL, FALSE, NULL);
}

lobbyClass::~lobbyClass(){
	end();
	
	if( hMutex ){
		CloseHandle( hMutex );
		hMutex = NULL;
	}
	
	if( buf ){
		free( buf );
		buf = NULL;
	}
}

/********************************************************
 * Init, Connect to Server
 ********************************************************/
int lobbyClass::init(){
	guiClass* gui = g_gui;
	
	//初期化してなかったら初期化
	if( !gui || !buf ) return 0xF;
	
	//要検討
	if( status == LOBBY_CLIENT_STATUS_ERROR ){
//		end();
		return 1;
	}
	
	if( status == LOBBY_CLIENT_STATUS_DEFAULT || !continueFlg ){
		//clear
		end();
		
		//init
		status = LOBBY_CLIENT_STATUS_DEFAULT;
		continueFlg = 1;
		checkVersionFlg= 0;
		userListFlg = 0;
		reqMssgeFlg = 0;
		newConnectFlg = 0;
		connectFlg = 0;
		endFlg = 0;
		user.init();
		lobbyPort = 9555;
		myPort = 7500;
		gui->message.init();
		
		memset( lobbyAddress, 0, sizeof(lobbyAddress) );
		memset( playerName,   0, sizeof(playerName) );
		memset( playerShortMessage, 0, sizeof(playerShortMessage) );
		memset( playerLongMessage,  0, sizeof(playerLongMessage) );
		
		wcscpy( lobbyAddress,       L"localhost" );
		wcscpy( playerName,         L"default" );
		wcscpy( playerShortMessage, L"default" );
		wcscpy( playerLongMessage,  L"default" );
		
		{
			WCHAR nowDir[200];
			memset( nowDir, 0, sizeof(nowDir) );
			wcscpy( nowDir, L"fail" );
			{
				WCHAR dir[200];
				
				if( GetModuleFileName( NULL, dir, 200 ) ){
					if( PathRemoveFileSpec( dir ) ){
						dir[199] = 0;
						wcscpy( nowDir, dir );
					}
				}
				
				dir[199] = 0;
			}
			
			WCHAR iniPath[200];
			if( wcscmp( nowDir, L"fail" ) && wcslen( nowDir ) < 180 ){
				//ok
				wcscpy( iniPath ,nowDir );
				wcscat( iniPath, L"\\config_rollcaster.ini" );
				if( GetFileAttributes( iniPath ) != 0xffffffff ){
					GetPrivateProfileString( L"LOBBY", L"lobbyAddress", L"localhost", lobbyAddress, 199, iniPath );
					lobbyPort = GetPrivateProfileInt( L"LOBBY", L"lobbyPort", 9555, iniPath );
					
					GetPrivateProfileString( L"LOBBY", L"playerName", L"default\0", playerName, 49, iniPath );
					GetPrivateProfileString( L"LOBBY", L"shortMessage", L"default\0", playerShortMessage, 199, iniPath );
					GetPrivateProfileString( L"LOBBY", L"longMessage", L"default\0", playerLongMessage, 999, iniPath );
					myPort = GetPrivateProfileInt( L"PORT", L"myPort", 7500, iniPath );
					
					playerName[ 49 ] = 0;
					playerShortMessage[ 199 ] = 0;
					
					for(int i=0; i<sizeof(playerLongMessage) -1; i++){
						if(playerLongMessage[i] == '\\' && playerLongMessage[i + 1] == 'n'){
							playerLongMessage[i] = ' ';
							playerLongMessage[i + 1] = '\n';
						}
					}
				}
			}
		}
		
		if(wcslen(playerName)==0){
			wcscpy(playerName, L"Default");
		}
		
		int Counter;
		
		hLobbyTh = (HANDLE)_beginthreadex(NULL, 0, lobbyThread, this, 0, NULL);
		if( !hLobbyTh ){
			gui->message.write(L"ERROR : hLobbyTh start failed.");
			status = LOBBY_CLIENT_STATUS_ERROR;
			
			return 0xF;
		}
		Counter = 0;
		for(;;){
			Sleep( 100 );
			if( connectFlg ) break;
			if( Counter > 50 || status == LOBBY_CLIENT_STATUS_ERROR ){
				gui->message.write(L"ERROR : connect failed");
				status = LOBBY_CLIENT_STATUS_ERROR;
				
				return 0xF;
			}
			Counter++;
		}
		
		
		//バージョンチェック
		SendCmd( CHECK_VERSION );
		Counter = 0;
		for(;;){
			Sleep( 100 );
			if( checkVersionFlg ) break;
			if( Counter > 50 || status == LOBBY_CLIENT_STATUS_ERROR ){
				gui->message.write(L"ERROR : checkVersion failed");
				status = LOBBY_CLIENT_STATUS_ERROR;
				
				return 0xF;
			}
			Counter++;
		}
		
		// 新規接続要求
		{
			int outBufSize = 3000;
			BYTE* bufTemp = (BYTE*)malloc( outBufSize );
			if( !bufTemp ){
				gui->message.write(L"ERROR : newconnect bufTemp alloc dailed");
				status = LOBBY_CLIENT_STATUS_ERROR;
				
				return 0xF;
			}else{
				myString strPlayerName(playerName, 0);
				myString strPlayerShortMessage(playerShortMessage, 0);
				myString strPlayerLongMessage(playerLongMessage, 0);
				
				if( strPlayerName.fail() || strPlayerShortMessage.fail() || strPlayerLongMessage.fail() ){
					gui->message.write(L"ERROR : string conv fail");
					status = LOBBY_CLIENT_STATUS_ERROR;
					
					free( bufTemp );
					return 0xF;
				}
				
				outputBufferClass dos(bufTemp, outBufSize);
				dos.writeUTF(&strPlayerName);
				dos.writeUTF(&strPlayerLongMessage);
				dos.writeUTF(&strPlayerShortMessage);
				dos.writeShort(myPort);
				dos.writeByte(0);	//0:常に許可, 1:対戦中不許可 2:常に拒否
				dos.writeByte(0);	//IPを隠すかどうか
				
				SendCmd( NEW_CONNECT, dos.data, dos.nowData );
				
				free( bufTemp );
			}
		}
		
		Counter = 0;
		for(;;){
			Sleep( 100 );
			
			if( newConnectFlg ) break;
			if( Counter > 50 || status == LOBBY_CLIENT_STATUS_ERROR ){
			gui->message.write(L"ERROR : newconnect failed");
				status = LOBBY_CLIENT_STATUS_ERROR;
				
				return 0xF;
			}
			Counter++;
		}
		
		// ユーザー一覧の要求
		SendCmd( REQUEST_USERLIST );
		Counter = 0;
		for(;;){
			Sleep( 100 );
			if( userListFlg ) break;
			if( Counter > 50 || status == LOBBY_CLIENT_STATUS_ERROR ){
				gui->message.write(L"ERROR : req_userList failed");
				status = LOBBY_CLIENT_STATUS_ERROR;
				
				return 0xF;
			}
			Counter++;
		}
		
		
		// IM履歴の要求
		SendCmd( REQUEST_STORED_INSTANTMESSAGES, REQUEST_MESSAGE_NUM );
		Counter = 0;
		for(;;){
			Sleep( 100 );
			if( reqMssgeFlg ) break;
			if( Counter > 50 || status == LOBBY_CLIENT_STATUS_ERROR ){
				gui->message.write(L"ERROR : req_message failed");
				status = LOBBY_CLIENT_STATUS_ERROR;
				
				return 0xF;
			}
			Counter++;
		}
		
		if(hLobbyAliveTh) CloseHandle(hLobbyAliveTh);
		hLobbyAliveTh = (HANDLE)_beginthreadex(NULL, 0, lobbyAliveThread, this, 0, NULL);
		
		status = LOBBY_CLIENT_STATUS_IDLE;
	}
	
	
	if( status != LOBBY_CLIENT_STATUS_DEFAULT &&  status != LOBBY_CLIENT_STATUS_ERROR ){
		//waitに戻す
		changeStatus(LOBBY_CLIENT_STATUS_IDLE);
//		SendCmd( MODIFY_STATE, STATE_WAITING );
//		status = LOBBY_CLIENT_STATUS_IDLE;
	}
	
	if(g_gui){
		SetWindowText(g_gui->hStatusWnd, L"Connected");
	}
	return 0;
}


/********************************************************
 * End, Disconnect
 ********************************************************/
int lobbyClass::end(){
	//終了処理
	endFlg = 1;
	
	//logout
	if( status != LOBBY_CLIENT_STATUS_DEFAULT && status != LOBBY_CLIENT_STATUS_ERROR ){
		
		Sleep(500);
		if( SendCmd( BYE ) != SOCKET_ERROR ){
			//暫定
			int Counter;
			for( Counter = 0; Counter < 200; Counter++ ){
				Sleep(50);
				if( !continueFlg ) break;
			}
		}
	}
	
	if(g_gui){
		SetWindowText(g_gui->hStatusWnd, L"Disconnected");
	}
	
	continueFlg = 0;
	
	WaitForSingleObject( hMutex, INFINITE );
	if( sLobby != INVALID_SOCKET ){
		shutdown( sLobby, SD_BOTH );
		closesocket( sLobby );
		sLobby = INVALID_SOCKET;
	}
	ReleaseMutex( hMutex );
	
	if( hLobbyTh ){
		WaitForSingleObject( hLobbyTh, 2000 );
		CloseHandle( hLobbyTh );
		hLobbyTh = NULL;
	}
	
	if( hLobbyAliveTh ){
		WaitForSingleObject( hLobbyAliveTh, 2000 );
		CloseHandle( hLobbyAliveTh );
		hLobbyAliveTh = NULL;
	}
	
	//clear
	status = LOBBY_CLIENT_STATUS_DEFAULT;
	checkVersionFlg= 0;
	userListFlg = 0;
	newConnectFlg = 0;
	connectFlg = 0;
	endFlg = 0;
	
	return 0;
}


/********************************************************
 * SendCmd
 ********************************************************/
int lobbyClass::SendCmd( BYTE cmd, void* data, DWORD size ){
	*(int*)buf = revInt( size );
	buf[ 4 ] = cmd;
	memcpy( &buf[5], data, size );
	
	mutexClass mutex(hMutex);
	int Res = send( sLobby, (char*)buf, size + 5, 0 );
	return Res;
}

int lobbyClass::SendCmd( BYTE cmd, BYTE byteVal ){
	int ret = SendCmd(cmd, &byteVal, 1);
	return ret;
}

int lobbyClass::SendCmd( BYTE cmd ){
	buf[ 0 ] = 0;
	buf[ 1 ] = 0;
	buf[ 2 ] = 0;
	buf[ 3 ] = 0;
	buf[ 4 ] = cmd;
	
	mutexClass mutex(hMutex);
	int Res = send( sLobby, (char*)buf, 5, 0 );
	return Res;
}

