#ifndef LOBBY_USER_CLASS
#define LOBBY_USER_CLASS

#include <windows.h>
#include "lobbyUserSubClass.h"

namespace N_Gui{

#define LOBBY_USER_MAX 100

class lobbyUserClass{
	public :
	lobbyUserClass();
	~lobbyUserClass();
	lobbyUserSubClass user[ LOBBY_USER_MAX ];
	int add( WCHAR*, WCHAR*, WCHAR*, WCHAR*, DWORD, WORD, BYTE );
	int del( WCHAR* );
	int setState( WCHAR*, BYTE );
	int init();
	int getUserInfo(WCHAR*, lobbyUserSubClass*);
	int getState( WCHAR*, BYTE* );
	HANDLE hMutex;
};

}

#endif
