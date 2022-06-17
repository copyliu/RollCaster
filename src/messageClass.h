#ifndef LOBBY_MESSAGE_CLASS
#define LOBBY_MESSAGE_CLASS

#include <windows.h>
#include "messageSubClass.h"

namespace N_Gui{
#define MESSAGE_BUF_SIZE 20

class messageClass{
private:
	HANDLE hMutex;
	messageSubClass messageBuf[MESSAGE_BUF_SIZE];
	
public:
	messageClass();
	~messageClass();
	
	int write(WCHAR* param_from, WCHAR* param_to, LONGLONG param_time, WCHAR* param_message, BYTE param_color);
	int write(WCHAR* param_message);
	int write(WCHAR* param_message, int num);
	int write(WCHAR* param_message, WCHAR* param_str);
	int write(WCHAR* param_message, WCHAR* param_str, WCHAR* param_str2);
	
	int flush(HWND);
	int init();
	int insert(HWND hLobbyChat, messageSubClass* param_message);
	int insertSub(HWND hList, WCHAR* fromStr, WCHAR* timeStr, WCHAR* str, int n);
};

}

#endif
