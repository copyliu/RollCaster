#ifndef LOBBY_MESSAGE_SUB_CLASS
#define LOBBY_MESSAGE_SUB_CLASS

#include <windows.h>

namespace N_Gui{

class messageSubClass{
public:
	messageSubClass();
	~messageSubClass();
	
	int init();
	bool hasMessage();
	int set(WCHAR* param_from, WCHAR* param_to, LONGLONG param_time, WCHAR* param_message, BYTE param_color);
	
	WCHAR* from;
	WCHAR* to;
	WCHAR* message;
	WCHAR* time;
	BYTE color;
	bool hasData;
};

}

#endif
