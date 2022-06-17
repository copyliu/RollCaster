#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include "caster/mainDatClass.h"
#include "casterClass.h"
#include "guiIO.h"
#include "guiClass.h"
#include "lobbyClass.h"
#include "charConvClass.h"
using namespace std;
using namespace N_Gui;

/********************************************************
 * MISC
 ********************************************************/
unsigned __stdcall casterThread(void*);


/********************************************************
 * Constructor / Destructor
 ********************************************************/
casterClass::casterClass(){
	hCasterThread = NULL;
	dat = NULL;
	
	returnToMenuFlg = 0;
	escSelectFlgBak = 0;
	::memset( &argData, 0, sizeof( argData ) );
	soundFlg = false;
}

casterClass::~casterClass(){
	gin.disableBlock();
	//TODO
	if(dat){
		dat->escSelectFlg = 0;
	}
	if(g_gui){
		g_gui->pressEscapeFlg = 1;
	}
	
	if(hCasterThread){
		::WaitForSingleObject(hCasterThread, 10000);
		::CloseHandle(hCasterThread);
		hCasterThread = NULL;
	}
}

/********************************************************
 * Start thread
 ********************************************************/
int casterClass::start(WCHAR* str){
	if(hCasterThread){
		gout << "ERROR : caster is running" << endl;
		return 1;
	}
	
	//TODO
	gin.enableBlock();
	
	hCasterThread = (HANDLE)::_beginthreadex(NULL, 0, casterThread, str, 0, NULL);
	return 0;
}

/********************************************************
 * Thread
 ********************************************************/
unsigned __stdcall casterThread(void* pAddr){
	WCHAR* argStr = (WCHAR*)pAddr;
	casterClass* caster = g_caster;
	int argc;
	char** argv;
	char* argArray[5];
	
	charConvClass conv(argStr);
	
	
	//TODO
	argArray[0] = ::strtok(conv.shift, " ");
	argArray[1] = strtok(NULL, " ");
	argArray[2] = strtok(NULL, " ");
	argArray[3] = strtok(NULL, " ");
	argArray[4] = strtok(NULL, " ");
	
	argc = 0;
	argv = argArray;
	for(int Counter=0; Counter<5; Counter++){
		if(argArray[Counter]){
			argc++;
		}
	}
	
	if(g_gui && g_gui->wineFlg){
		Sleep(500);
	}
	caster->process(argc, argv);
	
	::PostMessage(g_gui->hWnd, WM_CLOSE, 0, 0);
	return 0;

}

/********************************************************
 * Reset
 ********************************************************/
void casterClass::reset(int argMode, WCHAR* IP, WORD port, bool soundParam){
	if(soundParam){
		soundFlg = true;
	}else{
		soundFlg = false;
	}
	
	reset(argMode, IP, port);
}

void casterClass::reset(int argMode, WCHAR* IP, WORD port){
	guiClass* gui = g_gui;
	
	if(dat && gui){
		if(dat->myInfo.terminalMode != mode_root){
			returnToMenuFlg = 1;
			
			::memset( &argData, 0, sizeof( argData ) );
			
			argData.argMode = argMode;
			if(IP != NULL){
				charConvClass conv(IP);
				::strcpy(argData.targetIP, conv.shift);
			}
			argData.targetPort = port;
			
			gin.disableBlock();
			if(!escSelectFlgBak){
				escSelectFlgBak = dat->escSelectFlg;
			}
			dat->escSelectFlg = 0;
			gui->pressEscapeFlg = 1;
		}
	}
}

/********************************************************
 * Caster main
 ********************************************************/
int casterClass::process( int argc, char *argv[] ){
	
	//¥×¥í¥»¥¹¤ÎƒžÏÈ¶È¤òÔO¶¨
//	SetPriorityClass( GetCurrentProcess(), REALTIME_PRIORITY_CLASS );
	SetPriorityClass( GetCurrentProcess(), HIGH_PRIORITY_CLASS );
	
	//main
	{
		N_Caster::mainDatClass mainDat;
		if( mainDat.mainDatInit( argc, argv ) ) return 1;
		dat = &mainDat;
		
		int Res;
		for(;;){
			Res = mainDat.Entrance();
			switch(returnToMenuFlg){
			case 1:
				gout << "Go to argument mode..." << endl;
				returnToMenuFlg = 2;
				
				
				gin.enableBlock();
				dat->escSelectFlg = escSelectFlgBak;
				escSelectFlgBak = 0;
				if(g_gui){
					g_gui->pressEscapeFlg = 0;
				}
				Res = 1;
				
				dat->mainFlg = main_arg;
				dat->argData = argData;
				
				if(soundFlg){
					dat->Sound();
					soundFlg = 0;
				}
				
				char* mode;
				switch(argData.argMode){
				case CASTER_ARG_MODE_CONNECT:
					mode = "Connect";
					break;
				case CASTER_ARG_MODE_WAIT_TARGET:
					mode = "Wait Target";
					break;
				case CASTER_ARG_MODE_WAIT:
					mode = "Wait";
					break;
				case CASTER_ARG_MODE_SPECTATE:
					mode = "Spectate";
					break;
				case CASTER_ARG_MODE_STANDBY:
					mode = "Standby";
					break;
				default :
					mode = "Connect";
					break;
				}
				
				gout << endl
				     << "<Argument>" << endl
				     << "Mode : " << mode << endl
				     << "IP   : " << argData.targetIP << endl
				     << "Port : " << argData.targetPort<< endl;
				break;
			case 2:
				gout << "Go to default mode..." << endl;
				returnToMenuFlg = 3;
				dat->mainFlg = main_default;
				break;
			}
			
			if(g_gui){
				switch(dat->myInfo.terminalMode){
				case mode_root:
					::PostMessage(g_gui->hWnd, WM_COMMAND, ID_NOTICE_CASTER_GOTO_BATTLE, 0);
					break;
				case mode_branch:
					::PostMessage(g_gui->hWnd, WM_COMMAND, ID_NOTICE_CASTER_GOTO_SPECTATE, 0);
					break;
				}
			}
			
			if( Res == 0xF ) break;
			if( Res == 0 ) mainDat.backGroundRoop();
			
			if(returnToMenuFlg != 1){
				if(g_gui){
					::PostMessage(g_gui->hWnd, WM_COMMAND, ID_NOTICE_CASTER_GOTO_MENU, 0);
				}
			}
			if(returnToMenuFlg == 3){
				returnToMenuFlg = 0;
			}
		}
		
		dat = NULL;
		if( mainDat.mainDatEnd() ) return 1;
	}
	
	gout << "Good bye!" <<endl;
	Sleep(1000);
	return 0;
}

/********************************************************
 * bool func
 ********************************************************/
bool casterClass::isLobbyControl(){
	bool ret = false;
	
	if(returnToMenuFlg){
		ret = true;
	}
	
	return ret;
}

bool casterClass::isFighting(){
	bool ret = false;
	
	if(dat && dat->myInfo.terminalMode == mode_root){
		ret = true;
	}
	
	return ret;
}

