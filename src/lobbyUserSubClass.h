#ifndef LOBBY_USER_SUB_CLASS
#define LOBBY_USER_SUB_CLASS

#include <windows.h>

namespace N_Gui{

class lobbyUserSubClass{
	public :
	lobbyUserSubClass();
	~lobbyUserSubClass();
	
	int registerInfo( WCHAR*, WCHAR*, WCHAR*, WCHAR*, DWORD, WORD, BYTE );
	int clear();
	
	//MD5 hash 128bitÇ16êiêîï\ãLÅ®32BYTE?
	WCHAR* name;
	WCHAR* UID;
	WCHAR* shortMessage;
	WCHAR* longMessage;
	DWORD IP;
	WORD port;
	BYTE state;
};

}

#endif
