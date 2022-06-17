#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <time.h>
#include <stdio.h>
#include "messageSubClass.h"
using namespace std;
using namespace N_Gui;


/********************************************************
 * Constructor, Destructor
 ********************************************************/
messageSubClass::messageSubClass(){
	from = NULL;
	to = NULL;
	message = NULL;
	time = NULL;
	color = 0;
	hasData = false;
}

messageSubClass::~messageSubClass(){
	init();
}


/********************************************************
 * 
 ********************************************************/
bool messageSubClass::hasMessage(){
	return hasData;
}

/********************************************************
 * Init
 ********************************************************/
int messageSubClass::init(){
	if(from){
		delete[] from;
		from = NULL;
	}
	
	if(to){
		delete[] to;
		to = NULL;
	}
	
	if(message){
		delete[] message;
		message = NULL;
	}
	
	if(time){
		delete[] time;
		time = NULL;
	}
	
	color = 0;
	hasData = false;
	
	return 0;
}

/********************************************************
 * Set Message (for ChatLog)
 ********************************************************/
int messageSubClass::set(WCHAR* param_from, WCHAR* param_to, LONGLONG param_time, WCHAR* param_message, BYTE param_color){
	init();
	
	if(param_from){
		int fromSize = wcslen(param_from);
		from = new WCHAR[fromSize + 1];
		wcscpy(from, param_from);
		from[fromSize] = 0;
	}
	
	if(param_to){
		int toSize = wcslen(param_to);
		to = new WCHAR[toSize + 1];
		wcscpy(to, param_to);
		to[toSize] = 0;
	}
	
	if(param_message){
		int messageSize = wcslen(param_message);
		message = new WCHAR[messageSize + 1];
		wcscpy(message, param_message);
		message[messageSize] = 0;
	}
	
	if(param_time){
		long longVal = param_time / 1000;
		struct tm *local;
		
		local = localtime(&longVal);
		
		WCHAR conv_time[100];
		memset(conv_time, 0, sizeof(conv_time));
		swprintf(conv_time, L"%d/%d %02d:%02d:%02d", local->tm_mon + 1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);
		
		int timeLen = wcslen(conv_time);
		time = new WCHAR[timeLen + 1];
		wcscpy(time, conv_time);
		time[timeLen] = 0;
	}
	
	color = param_color;
	
	hasData = true;
	
	return 0;
}

