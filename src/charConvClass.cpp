#include <windows.h>
#include "charConvClass.h"
using namespace std;
using namespace N_Gui;


/********************************************************
 * Shift-jis <-> UTF-16
 ********************************************************/
charConvClass::charConvClass(){
	wide = NULL;
	shift = NULL;
}

charConvClass::charConvClass(WCHAR* wParam){
	wide = NULL;
	shift = NULL;
	set(wParam);
}

charConvClass::charConvClass(char* sParam){
	wide = NULL;
	shift = NULL;
	set(sParam);
}

charConvClass::~charConvClass(){
	if(wide)  delete[] wide;
	if(shift) delete[] shift;
}

int charConvClass::set(WCHAR* wParam){
	if(wide)  delete[] wide;
	if(shift) delete[] shift;
	
	int wideLen = wcslen(wParam);
	if(wideLen == 0){
		wide  = new WCHAR[1];
		shift = new char[1];
		
		::wcscpy(wide, L"\0" );
		::strcpy(shift, "\0");
	}else{
		wide = new WCHAR[wideLen + 1];
		::memset(wide, 0, wideLen * sizeof(WCHAR) + 1);
		::wcscpy(wide, wParam );
		
		int shiftLen = ::WideCharToMultiByte(CP_ACP, 0, wide, wideLen, NULL, 0, NULL, NULL);
		shift = new char[shiftLen + 1];
		
		int ret = ::WideCharToMultiByte(CP_ACP, 0, wide, wideLen, shift, shiftLen + 1, NULL, NULL);
		shift[shiftLen] = 0;
		if(ret != shiftLen || shiftLen == 0){
			return 1;
		}
	}
	return 0;
}
	
int charConvClass::set(char* sParam){
	if(wide) delete[] wide;
	if(shift) delete[] shift;
	
	int shiftLen = strlen(sParam);
	if(shiftLen == 0){
		wide  = new WCHAR[1];
		shift = new char[1];
		
		::wcscpy(wide, L"\0" );
		::strcpy(shift, "\0");
	}else{
		shift = new char[shiftLen + 1];
		::memset(shift, 0, shiftLen + 1);
		::strncpy(shift, sParam, shiftLen);
		
		int wideLen = ::MultiByteToWideChar(CP_ACP, 0, shift, shiftLen, NULL, 0);
		wide = new WCHAR[wideLen + 1];
		
		int ret = ::MultiByteToWideChar(CP_ACP, 0, shift, shiftLen, wide, wideLen + 1);
		wide[wideLen] = 0;
		if(ret != wideLen || wideLen == 0){
			return 1;
		}
	}
	return 0;
}

