#ifndef LOBBY_CTRL
#define LOBBY_CTRL

#include <windows.h>
#include <process.h>
#include <winsock2.h>
#include <io.h>
#include <iostream.h>
#include <iomanip.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include "lobbyUserClass.h"
#include "unicodeStringConv.h"

namespace N_Gui{

#define LOBBY_BUF_SIZE 1024000

#define REQUEST_MESSAGE_NUM 15


#define PROTOCOL_VERSION	4

#define CHECK_VERSION				1
#define NEW_CONNECT					2
#define BYE							3
#define REQUEST_USERLIST			4
#define MODIFY_STATE				5
#define POST_INSTANTMESSAGE			6
#define REQUEST_STORED_INSTANTMESSAGES	7
#define POST_CLIENT_TO_CLIENT_COMMAND	8
#define KEEP_ALIVE						9

#define RESPONSE_VERSION				64
#define RESPONSE_NEW_CONNECT			65
#define RESPONSE_BYE					66
#define RESPONSE_USERLIST				67
#define RESPONSE_POST_INSTANTMESSAGE	68
#define RESPONSE_MODIFY_STATE			69
#define RESPONSE_STORED_INSTANTMESSAGES	70
#define RESPONSE_KEEP_ALIVE				71

#define JOIN						80
#define LEAVE						81
#define STATE_MODIFIED				82
#define DISTRIBUTE_INSTANTMESSAGE	83
#define DISTRIBUTED_CLIENT_TO_CLIENT_COMMAND	84

#define ERROR_EXIT_DUMMY	-1
#define EXIT_DUMMY	0
#define ACCEPTED	1
#define REFUSED		2
#define NOT_EXISTS	3
#define IO_ERROR	4

#define POST_FIGHT				1
#define ACCEPTED_POST_FIGHT		64
#define ALREADY_ACCEPTED_POST_FIGHT	65
#define NON_GATHERING			66
#define PROCESS_START_ERROR		67

#define STATE_WAITING	0
#define STATE_GATHERING	1
#define STATE_SETTING	2
#define STATE_FIGHTING	3
#define STATE_WATCHING	4
#define STATE_LEAVING	5
#define STATE_HOST_GATHERING		6
#define STATE_HOST_GATHERING_WAIT	64

//TODO
//¥¯¥é¥¤¥¢¥ó¥ÈÄÚ²¿×´‘B
//¥×¥ì¥¤¥ä©`¥¹¥Æ©`¥¿¥¹¤È·Öëx¤·¤¿¤Û¤¦¤¬¤è¤¤
enum{
	LOBBY_CLIENT_STATUS_DEFAULT = 0,
	LOBBY_CLIENT_STATUS_IDLE,
	LOBBY_CLIENT_STATUS_WAIT,
	LOBBY_CLIENT_STATUS_FIGHT,
	LOBBY_CLIENT_STATUS_SPECTATE,
	LOBBY_CLIENT_STATUS_SETTING,
	LOBBY_CLIENT_STATUS_HOST_WAIT,
	LOBBY_CLIENT_STATUS_CLIENT_WAIT,
	LOBBY_CLIENT_STATUS_AFK,
	LOBBY_CLIENT_STATUS_ERROR
};

unsigned __stdcall lobbyThread(void*);
unsigned __stdcall lobbyAliveThread(void*);

class lobbyClass{
	public:
	SOCKET sLobby;
	HANDLE hLobbyTh;
	HANDLE hLobbyAliveTh;
	HANDLE hMutex;
	BYTE* buf;
	
	lobbyClass();
	~lobbyClass();
	WORD continueFlg;
	
	int init();
	int changeStatus( WORD );
	int end();
	int SendCmd( BYTE );
	int SendCmd( BYTE, BYTE );
	int SendCmd( BYTE, void*, DWORD );
	WORD status;
	
	WORD checkVersionFlg;
	WORD userListFlg;
	WORD reqMssgeFlg;
	WORD newConnectFlg;
	WORD connectFlg;
	WORD endFlg;
	WORD acceptPostFightFlg;
	
	WCHAR playerName[ 50 ];
	WCHAR playerShortMessage[ 200 ];	//short message
	WCHAR playerLongMessage[1000];
	WORD myPort;
	
	WCHAR lobbyAddress[200];
	WORD lobbyPort;
	
	lobbyUserClass user;
	
	WCHAR connectingUID[40];
	
	int onBttnPressWait();
	int onBttnPressHostWait();
	int onBttnPressSend(WCHAR*);
	int onBttnPressConnect(WCHAR*);
	int onBttnPressSpectate(WCHAR*);
	int onBttnPressAFK();
	int onBttnPressCancel();
	int onNoticeCasterGotoMenu();
	int onNoticeCasterGotoDelayInput(int);
	int onNoticeCasterGotoDelayWait();
	int onNoticeCasterGotoBattle();
	int onNoticeCasterGotoSpectate();
	
	bool isRunning(){
		return (bool)continueFlg;
	}
	
	static WCHAR* getStatusLabel(BYTE);
};

extern lobbyClass* g_lobby;

}

#endif
