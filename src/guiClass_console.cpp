#ifndef UNICODE
#define UNICODE
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

#include <windows.h>
#include "guiClass.h"
#include "guiIO.h"
#include "mutexClass.h"
#include "charConvClass.h"
using namespace std;
using namespace N_Gui;


/********************************************************
 * Force Escape Press
 ********************************************************/
int guiClass::GetKeyStateReplace(int keyCode){
	int ret;
	
	if(keyCode == VK_ESCAPE && (pressEscapeFlg || linuxPressEscapeFlg)){
		ret = -1;
	}else{
		ret = ::GetKeyState(keyCode);
	}
	
	return ret;
}

/********************************************************
 * BASE STR
 *
 * TODO: Create MUTEX
 ********************************************************/
//Delete single line
void guiClass::delBaseStr(){
	WCHAR* nextLine = ::wcschr(baseStr, L'\n');
	if(!nextLine) return;
	nextLine++;
	
	lineCount--;
	WCHAR baseTemp[BASE_STR_SIZE];
	::wcscpy(baseTemp, nextLine);
	::wcscpy(baseStr, baseTemp);
}

//Delete multi line
void guiClass::delBaseStr(int delLineCount){
	WCHAR* position = baseStr;
	for(int Counter = 0;Counter < delLineCount; Counter++){
		position = ::wcschr(position, L'\n');
		if(position){
			position++;
		}else{
			//clear
			::memset(baseStr, 0, BASE_STR_SIZE * sizeof(WCHAR));
			lineCount = 0;
			return;
		}
	}
	
	lineCount -= delLineCount;
	WCHAR baseTemp[BASE_STR_SIZE];
	::wcscpy(baseTemp, position);
	::wcscpy(baseStr, baseTemp);
}

//Add a char
void guiClass::addBaseChar(WCHAR str){
	WCHAR temp[2];
	temp[0] = str;
	temp[1] = 0;
	addBaseStr(temp);
}

//Add string
void guiClass::addBaseStr(WCHAR* str){
	if(consoleLogPosition + ::wcslen(str) + 20 >= LOG_BUF_SIZE){
		consoleLogPosition = 0;
	}
	
	int margin = LOG_BUF_SIZE - ::wcslen(str);
	if(margin > 0){
		for(int Counter=0; Counter<(int)::wcslen(str); Counter++){
			if(str[Counter] == '\n'){
				if(margin > 0){
					consoleLogBuf[consoleLogPosition] = '\r';
					consoleLogPosition++;
					margin--;
				}else{
					break;
				}
			}
			consoleLogBuf[consoleLogPosition] = str[Counter];
			consoleLogPosition++;
		}
		
		consoleLogBuf[consoleLogPosition] = 0;
	}
	
	WCHAR* position = str;
	for(;;){
		position = ::wcschr(position, L'\n');
		if(position){
			position++;
			lineCount++;
			if(lineCount > maxLineCount){
				delBaseStr();
			}
		}else{
			break;
		}
	}
	if(::wcslen(baseStr) + ::wcslen(str) < BASE_STR_SIZE){
		//TODO
		//strに複数改行が含まれる場合
		::wcscat(baseStr, str);
	}
}

/********************************************************
 * EDIT STR
 ********************************************************/
//Add a Char
void guiClass::addEditChar(WCHAR c){
	WCHAR temp[2];
	temp[0] = c;
	temp[1] = 0;
	if(::wcslen(editStr) + ::wcslen(temp) < EDIT_STR_SIZE){
		::wcscat(editStr, temp);
	}
}

//Add string
void guiClass::addEditStr(WCHAR* str){
	if(::wcslen(editStr) + ::wcslen(str) < EDIT_STR_SIZE){
		::wcscat(editStr, str);
	}
}

//Clear
void guiClass::clearEditStr(){
	::memset(editStr, 0, sizeof(editStr));
}

/********************************************************
 * INPUT STR
 ********************************************************/
//Add string
void guiClass::addInputStr(WCHAR* str){
	if(::wcslen(str) < _countof(inputStr)){
		::wcscpy(inputStr, str);
	}
}

//Clear
void guiClass::clearInputStr(){
	::memset(inputStr, 0, sizeof(inputStr));
	doReturnFlg = 0;
}

/********************************************************
 * BACKSPACE
 *
 * Remove the last char of editStr[]
 ********************************************************/
void guiClass::doBackSpace(){
	int len = ::wcslen(editStr);
	if(len){
		editStr[len-1] = 0;
	}
}

/********************************************************
 * RETURN
 ********************************************************/
void guiClass::doReturn(){
	addBaseStr(editStr);
	addBaseStr(L"\r\n\0");
	
	if(::wcscmp(inputHistory[1], editStr)){
		for(int Counter=INPUT_HISTORY_NUM-1; Counter>1; Counter--){
			::wcsncpy(inputHistory[Counter], inputHistory[Counter-1], INPUT_HISTORY_BUF_SIZE);
		}
		//0は空欄用
		::memset(inputHistory[1], 0, INPUT_HISTORY_BUF_SIZE);
		::wcsncpy(inputHistory[1], editStr, INPUT_HISTORY_BUF_SIZE);
		inputHistory[1][INPUT_HISTORY_BUF_SIZE - 1] = 0;
	}
	inputHistoryPosition = 0;
	
	clearInputStr();
	addInputStr(editStr);
	doReturnFlg = 1;
	clearEditStr();
}

/********************************************************
 * Reflesh editor window strings
 ********************************************************/
void guiClass::reflesh(){
	reflesh(true);
}

void guiClass::reflesh(bool hasEdit){
	RECT rc;
	::GetClientRect(hConsole, &rc);
	RECT windowRect;
	::GetWindowRect(hConsole, &windowRect);
	int height = rc.bottom;
	if(height < MIN_LINE_NUM * LINE_HEIGHT) height = MIN_LINE_NUM * LINE_HEIGHT;
	if(height != editAreaHeight){
		if(height < 0 || windowRect.top < 0){
			//表示する必要がない
			return;
		}
		
		int calcLineCount = (int)(height / LINE_HEIGHT);
		if(calcLineCount > MAX_LINE_NUM){
			calcLineCount = MAX_LINE_NUM;
		}
		if(lineCount > calcLineCount){
			delBaseStr(lineCount - calcLineCount);
		}
		
		maxLineCount = calcLineCount;
		editAreaHeight = height;
	}
	
	int len = 0;
	len += ::wcslen(baseStr);
	if(hasEdit){
		len += ::wcslen(editStr);
	}
	len += ::wcslen(blinkStr);
	
	WCHAR* out = new WCHAR[len + 1];
	::memset(out, 0, (len + 1) * sizeof(WCHAR));
	
	::wcscat(out, baseStr);
	if(hasEdit){
		::wcscat(out, editStr);
	}
	if(blinkStatus){
		::wcscat(out, blinkStr);
	}
	
	bool isNeedReflesh = false;
	int size = ::GetWindowTextLength(hConsole) + 2;
	WCHAR* str = new WCHAR[size + 1];
	::memset(str, 0, (size + 1) * sizeof(WCHAR));
	::GetWindowText(hConsole , str, size);
	
	if(::wcslen(str) != ::wcslen(out) || ::wcscmp(str, out) != 0){
		isNeedReflesh = true;
	}
	delete[] str;
	
	if(isNeedReflesh){
		switch(cursorBlinkFlg){
		case 2:
		case 3:
			::SendMessage(hWnd,WM_SETREDRAW,false,0);
			::SetWindowText(hConsole, out);
			::SendMessage(hWnd,WM_SETREDRAW,true,0);
			::InvalidateRect(hWnd,NULL,true);
			break;
		default:
			::SetWindowText(hConsole, out);
			break;
		}
	}
	delete[] out;
}

