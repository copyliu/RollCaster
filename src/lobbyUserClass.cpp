#ifndef UNICODE
#define UNICODE
#endif

#include "lobbyUserClass.h"
#include "lobbyUserSubClass.h"
#include "unicodeStringConv.h"
#include "guiIO.h"
#include "mutexClass.h"
using namespace std;
using namespace N_Gui;


/********************************************************
 * Constructor, Destructor
 ********************************************************/
lobbyUserClass::lobbyUserClass(){
	hMutex = CreateMutex(NULL, FALSE, NULL);
}

lobbyUserClass::~lobbyUserClass(){
	if( hMutex ) CloseHandle( hMutex );
}

/********************************************************
 * Init
 ********************************************************/
int lobbyUserClass::init(){
	mutexClass mutex( hMutex );
	for(int Counter = 0; Counter < LOBBY_USER_MAX; Counter++ ){
		user[ Counter ].clear();
	}
	return 0;
}

/********************************************************
 * Get Player Info from UID
 ********************************************************/
int lobbyUserClass::getUserInfo(WCHAR* UID, lobbyUserSubClass* userData){
	if( !UID || !userData) return NULL;
	mutexClass mutex( hMutex );
	
	int ret = 1;
	
	for(int Counter=0;Counter<LOBBY_USER_MAX;Counter++){
		if( user[ Counter ].port && !hashcmp( user[ Counter ].UID, UID ) ){
			lobbyUserSubClass* findData = &user[ Counter ];
			
			userData->registerInfo( findData->UID,
			                        findData->name,
			                        findData->shortMessage,
			                        findData->longMessage,
			                        findData->IP, findData->port, findData->state );
			ret = 0;
			break;
		}
	}
	return ret;
}

/********************************************************
 * Get State from UID
 ********************************************************/
int lobbyUserClass::getState( WCHAR* UID, BYTE* state ){
	if( !UID || !state ) return 1;
	mutexClass mutex( hMutex );
	
	int ret = 1;
	*state = 0;
	
	for(int Counter=0;Counter<LOBBY_USER_MAX;Counter++){
		if( user[ Counter ].port && !hashcmp( user[ Counter ].UID, UID ) ){
			*state = user[ Counter ].state;
			ret = 0;
			break;
		}
	}
	
	return ret;
}

/********************************************************
 * Add Player
 ********************************************************/
int lobbyUserClass::add( WCHAR* UID, WCHAR* name, WCHAR* shortMessage, WCHAR* longMessage, DWORD IP, WORD port, BYTE state ){
	if( !UID || !name || !shortMessage || !longMessage ) return 1;
	if(!port) port = 7500;
	mutexClass mutex( hMutex );
	
	int Counter;
	for( Counter = 0; Counter < LOBBY_USER_MAX; Counter++ ){
		if( user[ Counter ].port ){
			if( !hashcmp( UID, user[ Counter ].UID ) ){
//				gout << "debug : same UID detected" << endl;
				return 1;
			}
		}
	}
	for( Counter = 0; Counter < LOBBY_USER_MAX; Counter++ ){
		if( !user[ Counter ].port ){
			user[ Counter ].registerInfo( UID, name, shortMessage, longMessage, IP, port, state );
			
			return 0;
		}
	}
	
	return 1;
}

/********************************************************
 * Delete Player
 ********************************************************/
int lobbyUserClass::del( WCHAR* UID ){
	if( !UID ) return 1;
	mutexClass mutex( hMutex );
	
	int ret = 1;
	for(int Counter = 0; Counter < LOBBY_USER_MAX; Counter++ ){
		if( !hashcmp( UID, user[ Counter ].UID ) ){
			user[ Counter ].clear();
			ret = 0;
			break;
		}
	}
	
	return ret;
}

/********************************************************
 * Set State
 ********************************************************/
int lobbyUserClass::setState( WCHAR* UID , BYTE state ){
	if( !UID ) return 1;
	mutexClass mutex( hMutex );
	
	int ret = 1;
	for(int Counter = 0; Counter < LOBBY_USER_MAX; Counter++ ){
		if( !hashcmp( UID, user[ Counter ].UID ) ){
			user[ Counter ].state = state;
			ret = 0;
			break;
		}
	}
	
	return ret;
}

