#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include "guiIO.h"
#include "guiClass.h"
#include "charConvClass.h"
using namespace std;
using namespace N_Gui;


/********************************************************
 * CASTER -> GUI
 *
 * Refs http://www.jah.ne.jp/~naoyuki/Writings/ExtIos.html
 ********************************************************/
template <class Ch,class Tr=std::char_traits<Ch> >
int basic_gout_streambuf<Ch,Tr>::overflow( int nCh = EOF ){
	if(g_gui){
		g_gui->addBaseChar(nCh);
	}
	return 0;
}

/********************************************************
 * GUI-> CASTER
 ********************************************************/
ginClass::ginClass(){
	blockFlg = 1;
	dummy = false;
}

void ginClass::enableBlock(){
	blockFlg = 1;
}

void ginClass::disableBlock(){
	blockFlg = 0;
}

ginClass& ginClass::operator>>(WORD& n){
	guiClass* gui = g_gui;
	
	//—vŒŸ“¢
	gui->clearInputStr();
	
	for(;;){
		if(!gui || !blockFlg){
			n = 0;
			break;
		}
		if(::wcslen(gui->inputStr)){
			n = ::_wtoi(gui->inputStr);
			gui->clearInputStr();
			break;
		}
		::Sleep(10);
	}
	return *this;
}

ginClass& ginClass::operator>>(char* str){
	guiClass* gui = g_gui;
	charConvClass conv;
	
	//—vŒŸ“¢
	gui->clearInputStr();
	
	for(;;){
		if(!gui || !blockFlg){
			::strcpy(str, "0\0");
			break;
		}
		if(::wcslen(gui->inputStr)){
			conv.set(gui->inputStr);
			::strcpy(str, conv.shift);
			gui->clearInputStr();
			break;
		}
		::Sleep(10);
	}
	return *this;
}

ginClass& ginClass::getline(char* str, int n){
	guiClass* gui = g_gui;
	charConvClass conv;
	
	//—vŒŸ“¢
	gui->clearInputStr();
	
	for(;;){
		if(!gui || !blockFlg){
			::strncpy(str, "0\0", n);
			break;
		}
		if(wcslen(gui->inputStr)){
			conv.set(gui->inputStr);
			::strncpy(str, conv.shift, n);
			gui->clearInputStr();
			break;
		}
		if(gui->doReturnFlg){
			gui->doReturnFlg = 0;
			str[0] = 0;
			break;
		}
		::Sleep(10);
	}
	return *this;
}

/********************************************************
 * Global Value
 ********************************************************/
ginClass N_Gui::gin;
goutStream N_Gui::gout;


