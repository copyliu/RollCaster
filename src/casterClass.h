#ifndef CASTER_CTRL
#define CASTER_CTRL

#include <windows.h>
#include "caster/mainDatClass.h"

namespace N_Gui{

#define CASTER_ARG_MODE_CONNECT     0
#define CASTER_ARG_MODE_WAIT_TARGET 1
#define CASTER_ARG_MODE_WAIT        2
#define CASTER_ARG_MODE_SPECTATE    3
#define CASTER_ARG_MODE_STANDBY     4

class casterClass{
public:
	casterClass();
	~casterClass();
	
	int start(WCHAR*);
	int process(int, char**);
	void reset(int, WCHAR*, WORD);
	void reset(int, WCHAR*, WORD, bool);
	bool isLobbyControl();
	bool isFighting();
	
	HANDLE hCasterThread;
	N_Caster::mainDatClass* dat;
	int returnToMenuFlg;
	N_Caster::argDataStruct argData;
	
	WORD escSelectFlgBak;
	bool soundFlg;
};

extern casterClass* g_caster;

}

#endif
