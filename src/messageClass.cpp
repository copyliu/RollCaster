#ifndef UNICODE
#define UNICODE
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <commctrl.h>
#include "messageClass.h"
#include "messageSubClass.h"
#include "mutexClass.h"
#include "guiClass.h"
using namespace std;
using namespace N_Gui;


/********************************************************
 * Constructor, Destructor
 ********************************************************/
messageClass::messageClass(){
	hMutex = ::CreateMutex(NULL, FALSE, NULL);
}

messageClass::~messageClass(){
	if( hMutex ) ::CloseHandle( hMutex );
}


/********************************************************
 * Constructor, Destructor
 ********************************************************/
int messageClass::init(){
	int ret = 0;
	
	if(!hMutex){
		ret = 1;
	}
	
	if(!ret){
		for(int i; i < MESSAGE_BUF_SIZE; i++){
			ret = messageBuf[i].init();
			if(ret){
				break;
			}
		}
	}
	
	return ret;
}


/********************************************************
 * Add Log Strings to Buffer
 ********************************************************/
int messageClass::write(WCHAR* param_from, WCHAR* param_to, LONGLONG param_time, WCHAR* param_message, BYTE color){
	mutexClass mutex(hMutex);
	
	int ret = 1;
	messageSubClass* message;
	
	for(int j=0; j < MESSAGE_BUF_SIZE; j++){
		message = &messageBuf[j];
		if(!message->hasMessage()){
			ret = message->set(param_from, param_to, param_time, param_message, color);
			break;
		}
	}
	if(g_gui){
		::PostMessage(g_gui->hWnd, WM_COMMAND, ID_LOBBY_CHAT_REFRESH, 0);
	}
	
	return ret;
}

int messageClass::write(WCHAR* param_message, int num){
	int ret;
	int size = ::wcslen(param_message) + 11;
	WCHAR* str = new WCHAR[size + 1];
	::memset(str, 0, (size + 1) * sizeof(WCHAR));
	
	::snwprintf(str, size, param_message, num);
	ret = write(NULL, NULL, NULL, str, 0);
	delete[] str;
	
	return ret;
}

int messageClass::write(WCHAR* param_message, WCHAR* param_str){
	int ret;
	int size = ::wcslen(param_message) + wcslen(param_str);
	WCHAR* str = new WCHAR[size + 1];
	::memset(str, 0, (size + 1) * sizeof(WCHAR));
	
	::snwprintf(str, size, param_message, param_str);
	str[size] = 0;
	
	ret = write(NULL, NULL, NULL, str, 0);
	delete[] str;
	
	return ret;
}

int messageClass::write(WCHAR* param_message, WCHAR* param_str, WCHAR* param_str2){
	int ret;
	int size = ::wcslen(param_message) + wcslen(param_str) + wcslen(param_str2);
	WCHAR* str = new WCHAR[size + 1];
	::memset(str, 0, (size + 1) * sizeof(WCHAR));
	
	::snwprintf(str, size, param_message, param_str, param_str2);
	str[size] = 0;
	
	ret = write(NULL, NULL, NULL, str, 0);
	delete[] str;
	
	return ret;
}

int messageClass::write(WCHAR* param_message){
	return write(NULL, NULL, NULL, param_message, 0);
}

/********************************************************
 * Flush
 ********************************************************/
int messageClass::insertSub(HWND hList, WCHAR* fromStr, WCHAR* timeStr, WCHAR* str, int n){
	LV_ITEM item;
	item.mask = LVIF_TEXT;
	time_t timer;
	struct tm *local;
	WCHAR nowTime[20];
	
	int current = ListView_GetItemCount(hList);
	while(current > MAX_CHAT_LINE){
		ListView_DeleteItem(hList, 0);
		current = ListView_GetItemCount(hList);
	}
	item.iItem = current;
	
	WCHAR dummy[] = L"\0";
	WCHAR* logName;
	WCHAR* logTime = dummy;
	WCHAR* logMessage;
	
	
	if(n == 0){
		if(fromStr != NULL){
			item.pszText = fromStr;
		}else{
			item.pszText = LOG_SYSTEM_NAME;
		}
	}else{
		item.pszText = L"";
	}
	item.iSubItem = 0;
	ListView_InsertItem(hList, &item);
	logName = item.pszText;
	
	if(n == 0){
		if(timeStr != NULL){
			item.pszText = timeStr;
		}else{
			timer = time(NULL);
			local = localtime(&timer);
			memset(nowTime, 0, 20 * sizeof(WCHAR));
			swprintf(nowTime, L"%d/%d %02d:%02d:%02d", local->tm_mon + 1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);
			
			item.pszText = nowTime;
		}
		item.iSubItem = 1;
		ListView_SetItem(hList, &item);	
		logTime = item.pszText;
	}

	item.pszText = str;
	item.iSubItem = 2;
	ListView_SetItem(hList, &item);	
	logMessage = item.pszText;
	
	//TODO
	if(g_gui){
		guiClass* gui = g_gui;
		
		int size = wcslen(logName) + wcslen(logTime) + wcslen(logMessage) + 6;
		if(gui->chatLogPosition + size + 20 > LOG_BUF_SIZE){
			gui->chatLogBuf[0] = 0;
			gui->chatLogPosition = 0;
		}
		
		if(size < LOG_BUF_SIZE){
			wcscpy(&gui->chatLogBuf[gui->chatLogPosition], logTime);
			wcscat(&gui->chatLogBuf[gui->chatLogPosition], L", ");
			wcscat(&gui->chatLogBuf[gui->chatLogPosition], logName);
			wcscat(&gui->chatLogBuf[gui->chatLogPosition], L", ");
			wcscat(&gui->chatLogBuf[gui->chatLogPosition], logMessage);
			wcscat(&gui->chatLogBuf[gui->chatLogPosition], L"\r\n");
			gui->chatLogPosition += size;
		}
	}
	
	return current;
}

int messageClass::insert(HWND hLobbyChat, messageSubClass* param_message){
	//TODO
	mutexClass mutex(hMutex);
	
	int ret = 1;
	int current = 0;
	
	WCHAR* from = param_message->from;
	WCHAR* time = param_message->time;
	WCHAR* str  = param_message->message;
	
	for(int Counter=0;Counter < 10; Counter++){
		WCHAR* retPosition = wcsstr(str, L"\n");
		if(retPosition){
			int size = (int)(retPosition - str);
			WCHAR* line = new WCHAR[size + 1];
			memset(line, 0, (size + 1) * sizeof(WCHAR));
			memcpy(line, str, size * sizeof(WCHAR));
			current = insertSub(hLobbyChat, from, time, line, Counter);
			delete[] line;
			
			str = retPosition + 1;
		}else{
			current = insertSub(hLobbyChat, from, time, str, Counter);
			ret = 0;
			break;
		}
	}
	
	ListView_EnsureVisible(hLobbyChat, current, false );
	return ret;
}


int messageClass::flush(HWND hLobbyChat){
	int ret = 0;
	messageSubClass* message;
	for(int Counter = 0; Counter < MESSAGE_BUF_SIZE; Counter++){
		message = &messageBuf[Counter];
		if(message->hasMessage()){
			ret = insert(hLobbyChat, message);
//			if(ret){
//				break;
//			}
			message->init();
		}
	}
	return ret;
}

