#ifndef UNICODE
#define UNICODE
#endif

#include "lobbyUserSubClass.h"
#include "guiIO.h"
using namespace std;
using namespace N_Gui;


/********************************************************
 * Constructor, Destructor
 ********************************************************/
lobbyUserSubClass::lobbyUserSubClass(){
	name = NULL;
	shortMessage = NULL;
	longMessage = NULL;
	UID = NULL;
	
	IP = 0;
	port = 0;
	state = 0;
}

lobbyUserSubClass::~lobbyUserSubClass(){
	clear();
}

/********************************************************
 * Register
 ********************************************************/
int lobbyUserSubClass::registerInfo( WCHAR* UIDTemp, WCHAR* nameTemp, WCHAR* shortMessageTemp, WCHAR* longMessageTemp, DWORD IPTemp, WORD portTemp, BYTE stateTemp ){
	if( !UIDTemp || !nameTemp || !shortMessageTemp ){
		//fail
		return 1;
	}
	
	int nameSize = wcslen(nameTemp);
	int UIDSize = wcslen(UIDTemp);
	int shortMessageSize = wcslen(shortMessageTemp);
	int longMessageSize = wcslen(longMessageTemp);
	
	clear();
	
	name = new WCHAR[ nameSize + 1 ];
	wcsncpy( name, nameTemp, nameSize );
	name[ nameSize ] = 0;
	
	UID = new WCHAR[ UIDSize + 1 ];
	wcsncpy( UID, UIDTemp, UIDSize );
	UID[ UIDSize ] = 0;
	
	shortMessage = new WCHAR[ shortMessageSize + 1 ];
	wcscpy( shortMessage, shortMessageTemp);
	shortMessage[ shortMessageSize ] = 0;
	
	longMessage = new WCHAR[ longMessageSize + 1 ];
	wcscpy(longMessage, longMessageTemp);
	longMessage[ longMessageSize ] = 0;
	
	IP = IPTemp;
	port = portTemp;
	state = stateTemp;
	
	return 0;
}

/********************************************************
 * Clear
 ********************************************************/
int lobbyUserSubClass::clear(){
	if( name ){
		delete[] name;
		name = NULL;
	}
	if( UID ){
		delete[] UID;
		UID = NULL;
	}
	if( shortMessage ){
		delete[] shortMessage;
		shortMessage = NULL;
	}
	if( longMessage ){
		delete[] longMessage;
		longMessage = NULL;
	}
	
	IP = 0;
	port = 0;
	state = 0;
	
	return 0;
}

