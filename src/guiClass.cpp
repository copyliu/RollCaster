#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <commctrl.h>
#include <time.h>
#include <iostream.h>
#include <fstream.h>
#include <windef.h>
#include <process.h>
#include <richedit.h>
#include <shlwapi.h>
#include "guiClass.h"
#include "guiIO.h"
#include "mutexClass.h"
#include "charConvClass.h"
using namespace std;
using namespace N_Gui;


/********************************************************
 * Misc
 ********************************************************/
namespace N_Gui{
unsigned __stdcall backgroundThread(void*);

WCHAR* TAB_LABEL[] ={
	L"Console",
	L"Log",
	L"Lobby",
	L"Settings"
};

WCHAR BUTTON_LABEL_WAIT[]      = L"Wait";
WCHAR BUTTON_LABEL_HOSTWAIT[]  = L"HostWait";
WCHAR BUTTON_LABEL_CONNECT[]   = L"Connect";
WCHAR BUTTON_LABEL_SPECTATE[]  = L"Spectate";
WCHAR BUTTON_LABEL_TH075TOOL[] = L"th075Tool";
WCHAR BUTTON_LABEL_CANCEL[]    = L"Cancel";
WCHAR BUTTON_LABEL_SEND[]      = L"Send";
WCHAR BUTTON_LABEL_SUICA[]     = L"Suica";
WCHAR BUTTON_LABEL_DISCONNECT[]  = L"Disconnect from Lobby";
WCHAR BUTTON_LABEL_SAVERESTART[] = L"Save and Restart";
WCHAR BUTTON_LABEL_AFK[]         = L"AFK";

int GetEsc(){
	if(::GetKeyState(VK_ESCAPE)<0) return 1;
	return 0;
}

WNDPROC ConsoleDefaultProc;
LRESULT CALLBACK ConsoleProc(HWND hwnd , UINT msg , WPARAM wp , LPARAM lp);

}
/********************************************************
 * Constructor, Destructor
 ********************************************************/
guiClass::guiClass(HINSTANCE hInstParam){
	::memset(baseStr, 0, sizeof(baseStr));
	::memset(editStr, 0, sizeof(editStr));
	::memset(inputStr, 0, sizeof(inputStr));
	::memset(blinkStr, 0, sizeof(blinkStr));
	::wcscpy(blinkStr, L"_");
	
	blinkStatus = false;
	lineCount = 0;
	
	hInst = hInstParam;
	editAreaHeight = 1000;
	maxLineCount = 50;
	doReturnFlg = 0;
	cursorType = 0;
	pressEscapeFlg = 0;
	autoLobbyFlg = 0;
	imgAlpha = 60;
	imgScaleDownFlg = 0;
	windowWidth = 1000;
	windowHeight = 600;
	linuxPressEscapeFlg = 0;
	settingsColorFlg = 0;
	cursorBlinkFlg = 0;
	windowSizeChangeEvent = 0;
	chatEnterKeyFlg = 0;
	windowPositionFlg = 0;
	windowPositionX = 0;
	windowPositionY = 0;
	
	::memset(suicaAccName, 0, sizeof(suicaAccName));
	::memset(imgFileName, 0, sizeof(imgFileName));
	::wcscpy(imgFileName, L"bg.bmp");
	::wcscpy(suicaURL, L"localhost");
	
	// wine test
	HKEY hKey;
	wineFlg = 0;
	if (::RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Software\\Wine", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
		wineFlg = 1;
		::RegCloseKey(hKey);
	}
	
	hImgWnd = NULL;
	hRtLib = NULL;
	
	continueFlg = 1;
	hBackgroundThread = (HANDLE)::_beginthreadex(NULL, 0, backgroundThread, (void*)this, 0, NULL);
	
	consoleLogBuf = new WCHAR[LOG_BUF_SIZE];
	consoleLogPosition = 0;
	
	chatLogBuf = new WCHAR[LOG_BUF_SIZE];
	chatLogPosition = 0;
	
	::memset(inputHistory, 0, sizeof(inputHistory));
	inputHistoryPosition = 0;
	
	//Òª—ÊÓ‘
//	hFont = ::CreateFont(13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"£Í£Ó ¥´¥·¥Ã¥¯");
	hFont = ::CreateFont(13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, NULL);
}

guiClass::~guiClass(){
	continueFlg = 0;
	if(hBackgroundThread){
		::WaitForSingleObject(hBackgroundThread, 3000);
		::CloseHandle(hBackgroundThread);
		hBackgroundThread = NULL;
	}
	
	if(hFont){
		::DeleteObject(hFont);
	}
	
	if(chatLogBuf){
		delete[] chatLogBuf;
	}
	
	if(consoleLogBuf){
		delete[] consoleLogBuf;
	}
	
	if(hRtLib){
		::FreeLibrary(hRtLib);
	}
	
}

/********************************************************
 * Init
 ********************************************************/
int guiClass::init(){
	if(!hBackgroundThread){
		return 1;
	}
	
	if(!consoleLogBuf){
		return 1;
	}
	
	if(!chatLogBuf){
		return 1;
	}
	
	//Read ini
 	WCHAR nowDir[500];
	::memset( nowDir, 0, sizeof(nowDir) );
	if( ::GetModuleFileName( NULL, nowDir, 500 ) ){
		if( ::PathRemoveFileSpec( nowDir ) ){
			nowDir[499] = 0;
		}else{
			::memset( nowDir, 0, sizeof(nowDir) );
		}
	}else{
		::memset( nowDir, 0, sizeof(nowDir) );
	}
 	
	if( ::wcslen( nowDir )){
		WCHAR iniPath[600];
		::wcscpy( iniPath ,nowDir );
		::wcscat( iniPath, L"\\config_rollcaster.ini" );
		if( ::GetFileAttributes( iniPath ) != 0xffffffff ){
			autoLobbyFlg     = ::GetPrivateProfileInt( L"GUI", L"autoLobby", 0, iniPath );
			imgAlpha         = ::GetPrivateProfileInt( L"GUI", L"imgAlpha", 60, iniPath );
			imgScaleDownFlg  = ::GetPrivateProfileInt( L"GUI", L"imgScaleDown", 1, iniPath );
			windowWidth      = ::GetPrivateProfileInt( L"GUI", L"windowWidth", 1000, iniPath );
			windowHeight     = ::GetPrivateProfileInt( L"GUI", L"windowHeight", 600, iniPath );
			settingsColorFlg = ::GetPrivateProfileInt( L"GUI", L"settingsColor", 0, iniPath );
			cursorBlinkFlg   = ::GetPrivateProfileInt( L"GUI", L"cursorBlink", 0, iniPath );
			chatEnterKeyFlg  = ::GetPrivateProfileInt( L"GUI", L"chatEnterKey", 0, iniPath );
			windowPositionFlg= ::GetPrivateProfileInt( L"GUI", L"windowPosition", 0, iniPath );
			windowPositionX  = ::GetPrivateProfileInt( L"GUI", L"windowPositionX", 0, iniPath );
			windowPositionY  = ::GetPrivateProfileInt( L"GUI", L"windowPositionY", 0, iniPath );
			::GetPrivateProfileString( L"GUI", L"imgFileName", L"bg.bmp", imgFileName, 199, iniPath );
			::GetPrivateProfileString( L"RAN_BATTLE", L"ran_account", L"", suicaAccName, 99, iniPath );
			::GetPrivateProfileString( L"RAN_BATTLE", L"ran_server", L"", suicaURL, 99, iniPath );
		}
	}

	if( ::_waccess( imgFileName, 0 ) ){
		imgAlpha = 0;
	}
	
	if(settingsColorFlg && hRtLib == NULL){
		hRtLib = ::LoadLibrary(L"RICHED32.DLL");
		if(!hRtLib){
			settingsColorFlg = 0;
		}
	}

 	return 0;
}

/********************************************************
 * BackGround Thread
 * 
 * Blink console cursor
 ********************************************************/
unsigned __stdcall N_Gui::backgroundThread(void* pAddr){
	guiClass* gui = (guiClass*)pAddr;
	
	int commonCounter = 0;
	int blinkCounter = 0;
	int consoleLogBufPositionPrev = 0;
	int chatLogBufPositionPrev = 0;
	bool blink = false;
	bool hasFocus;
	
	while(gui->continueFlg){
#ifdef ESCAPE_KEY_QUIT
		if(GetEsc()) break;
#endif
		if(gui->cursorType == 0){
			if(blinkCounter > 50){
				blinkCounter = 0;
				
				::wcscpy(gui->blinkStr, L"_");
				
				GUITHREADINFO info;
				::memset(&info, 0, sizeof(info));
				info.cbSize = sizeof(info);
				if(::GetGUIThreadInfo(NULL,&info) && info.hwndFocus == gui->hWnd){
					hasFocus = true;
				}else{
					hasFocus = false;
				}
				
				if(blink){
					blink = false;
				}else{
					blink = true;
				}
				
				switch(gui->cursorBlinkFlg){
				case 0:
				case 2:
					gui->blinkStatus = hasFocus;
					gui->reflesh(true);
					break;
				case 1:
				case 3:
					gui->blinkStatus = hasFocus && blink;
					gui->reflesh(true);
					break;
				}
			}
		}else{
			if(blinkCounter > (BLINK_STR_SIZE - 1) * 20){
				blinkCounter = 0;
			}
			if(!(blinkCounter % 20)){
				memset(gui->blinkStr, ' ', sizeof(gui->blinkStr));
				gui->blinkStr[(int)(blinkCounter / 20)] = '*';
				gui->blinkStatus = 1;
				gui->reflesh(false);
			}
		}
		
		//TODO
		if(consoleLogBufPositionPrev != gui->consoleLogPosition){
			if((commonCounter%50) == 0){
				consoleLogBufPositionPrev = gui->consoleLogPosition;
				::SetWindowText(gui->hLogConsole, gui->consoleLogBuf);
				::PostMessage(gui->hLogConsole, WM_VSCROLL, SB_BOTTOM, 0);
			}
		}
		
		//TODO
		if(chatLogBufPositionPrev != gui->chatLogPosition){
			if((commonCounter%90) == 0){
				chatLogBufPositionPrev = gui->chatLogPosition;
				::SetWindowText(gui->hLogChat, gui->chatLogBuf);
				::PostMessage(gui->hLogChat, WM_VSCROLL, SB_BOTTOM, 0);
			}
		}
		
		if(gui->windowSizeChangeEvent){
			if((commonCounter % 5) == 0){
				gui->windowSizeChangeEvent = 0;
				::PostMessage(gui->hWnd, WM_COMMAND, ID_DELAYED_WM_SIZE, 0);
			}
		}
		
		if(commonCounter > 10000){
			commonCounter = 0;
		}
		
		
		blinkCounter++;
		commonCounter++;
		::Sleep(10);
	}
	
	::PostMessage(gui->hWnd, WM_CLOSE, 0, 0);
	return 0;
}

/********************************************************
 * Settings TEXT Color
 ********************************************************/
template<typename CHAR_TYPE>
void processColorHighlighting(CHAR_TYPE iniStr, int size){
	guiClass* gui = g_gui;
	
	//Set Color
	COLORREF commentColor = RGB(70, 130, 70);
	COLORREF sectionColor = RGB(255, 0, 0);
	
	CHARFORMAT2 cf;
	cf.cbSize = sizeof(cf);
	::SendMessage(gui->hSettings, EM_GETCHARFORMAT, (WPARAM)SCF_DEFAULT, (LPARAM)&cf);
	
	cf.dwMask = CFM_COLOR;
	cf.dwEffects = 0;
	CHARRANGE range;
	
	CHAR_TYPE commentStart = NULL;
	CHAR_TYPE commentEnd;
	CHAR_TYPE sectionStart = NULL;
	CHAR_TYPE sectionEnd;
	
	for(int position=0; position<size; position++){
		//comment
		if(iniStr[position] == ';'){
			if(commentStart == NULL && sectionStart == NULL){
				commentStart = &iniStr[position];
			}
		}
		if(commentStart){
			if(iniStr[position] == '\n'){
				commentEnd = &iniStr[position];
				
				//range
				range.cpMin = (LONG)(commentStart - iniStr);
				range.cpMax = (LONG)(commentEnd - iniStr);
				
				//color
				cf.crTextColor = commentColor;
				
				//set color
				::SendMessage(gui->hSettings, EM_EXSETSEL, 0, (LPARAM)&range);
				::SendMessage(gui->hSettings, EM_SETCHARFORMAT, (WPARAM)SCF_SELECTION, (LPARAM)&cf);
				
				commentStart = NULL;
			}
		}
		
		//section
		if(iniStr[position] == '['){
			if(commentStart == NULL && sectionStart == NULL){
				sectionStart = &iniStr[position];
			}
		}
		if(sectionStart){
			if(iniStr[position] == ']'){
				sectionEnd = &iniStr[position + 1];
				
				//range
				range.cpMin = (LONG)(sectionStart - iniStr);
				range.cpMax = (LONG)(sectionEnd - iniStr);
				
				//color
				cf.crTextColor = sectionColor;
				
				//set color
				::SendMessage(gui->hSettings, EM_EXSETSEL, 0, (LPARAM)&range);
				::SendMessage(gui->hSettings, EM_SETCHARFORMAT, (WPARAM)SCF_SELECTION, (LPARAM)&cf);
				
				sectionStart = NULL;
			}
			if(iniStr[position] == '\n'){
				sectionStart = NULL;
			}
		}
	}
}

int initSettingsWindow(){
	guiClass* gui = g_gui;
	
	std::ifstream fin("config_rollcaster.ini", ios::binary);
	if (fin.fail()){
		return 1;
	}
	
	fin.seekg(0, fin.end);
	int endPos = fin.tellg();
	fin.clear();
	fin.seekg(0, fin.beg);
	int begPos = fin.tellg();
	
	int sizeSJIS = endPos - begPos;
	char* iniStrSJIS = new char[sizeSJIS + 1];
	::memset(iniStrSJIS, 0, sizeSJIS + 1);
	
	WCHAR* iniStr;
	int size;
	charConvClass iniConv;
	
	fin.read( iniStrSJIS, sizeSJIS );
	if(fin.bad() || fin.fail()){
		delete[] iniStrSJIS;
		return 1;
	}else{
		iniConv.set(iniStrSJIS);
		iniStr = iniConv.wide;
	}
	
	::SetWindowTextW(gui->hSettings, iniStr);
	size = ::wcslen(iniStr);
	
	if(gui->settingsColorFlg){
		if(!gui->wineFlg){
			//windows char
			processColorHighlighting(iniStrSJIS, sizeSJIS);
		}else{
			//wine WCHAR
			processColorHighlighting(iniStr, size);
		}
		
	}
	delete[] iniStrSJIS;
	
	return 0;
}

/********************************************************
 * Insert Columns
 ********************************************************/
void insertColumns(){
	guiClass* gui = g_gui;
	
	LVCOLUMN col;
	col.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.cx = 80;
	col.pszText = L"Name";
	col.iSubItem = 0;
	ListView_InsertColumn(gui->hLobbyList , 0 , &col);
	
	col.cx = 20;
	col.pszText = L"UID";
	col.iSubItem = 1;
	ListView_InsertColumn(gui->hLobbyList , 1 , &col);
	
	col.cx = 50;
	col.pszText = L"Status";
	col.iSubItem = 2;
	ListView_InsertColumn(gui->hLobbyList , 2 , &col);
	
	col.cx = 100;
	col.pszText = L"IP";
	col.iSubItem = 3;
	ListView_InsertColumn(gui->hLobbyList , 3 , &col);
	
	col.cx = 950;
	col.pszText = L"Description";
	col.iSubItem = 4;
	ListView_InsertColumn(gui->hLobbyList , 4 , &col);

	col.cx = 100;
	col.pszText = L"Name";
	col.iSubItem = 0;
	ListView_InsertColumn(gui->hLobbyChat , 0 , &col);
	
	col.cx = 100;
	col.pszText = L"Time";
	col.iSubItem = 1;
	ListView_InsertColumn(gui->hLobbyChat , 1 , &col);
	
	col.cx = 1000;
	col.pszText = L"Chat";
	col.iSubItem = 2;
	ListView_InsertColumn(gui->hLobbyChat , 2 , &col);
}


/********************************************************
 * CreateButton
 ********************************************************/
HWND CreateButton(WCHAR* label, HWND hWnd, int id, HINSTANCE hInst){
	HWND hButton = ::CreateWindowEx(
		0,
		L"BUTTON",
		label,
//		WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP,
		WS_CHILD,
		0, 0, 10, 10,
		hWnd,
		(HMENU)id,
		hInst,
		NULL);
	
	return hButton;
}

/********************************************************
 * For Return key detect
 *
 * Chat Edit Area
 ********************************************************/
WNDPROC origEditWindowProc;
static LRESULT CALLBACK EditWindowProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp){
	guiClass* gui = g_gui;
	
	switch (msg){
	case WM_KEYDOWN :
	case WM_CHAR:
		if(wp == VK_RETURN && gui){
			switch(gui->chatEnterKeyFlg){
			case 0:
				if(::GetKeyState(VK_SHIFT) >= 0){
					//Enter
					if(msg == WM_KEYDOWN){
						::PostMessage(gui->hWnd, WM_COMMAND, ID_SEND_CHAT_MESSAGE, 0);
					}
					return 0;
				}else{
					//Shift+Enter
					//DO NOTHING
				}
				break;
			case 1:
				if(::GetKeyState(VK_SHIFT) >= 0){
					//Enter
					//DO NOTHING
				}else{
					//Shift+Enter
					if(msg == WM_KEYDOWN){
						::PostMessage(gui->hWnd, WM_COMMAND, ID_SEND_CHAT_MESSAGE, 0);
					}
					return 0;
				}
				break;
			case 2:
			default:
				//DO NOTHING
				break;
			}
		}
		break;
	}

	if(origEditWindowProc){
		return origEditWindowProc(hWnd, msg, wp, lp);
	} else {
		return 0;
	}
}

/********************************************************
 * Console Proc
 ********************************************************/
LRESULT CALLBACK N_Gui::ConsoleProc(HWND hwnd , UINT msg , WPARAM wp , LPARAM lp) {
	switch(msg) {
	case WM_RBUTTONDOWN:
		if (IsClipboardFormatAvailable(CF_TEXT) != 0) {
			if (OpenClipboard(0) != 0) {
				HGLOBAL mem = GetClipboardData(CF_TEXT);
				if (mem != 0) {
					const CHAR *string = (const CHAR *)GlobalLock(mem);
					if (string != 0) {
						int len = GlobalSize(mem);
						if (len > 0 && len < 79) {
							charConvClass conv((char*)string);
							g_gui->addEditStr(conv.wide);
						}
						GlobalUnlock(mem);
					}
				CloseClipboard();
				}
			}
		}
		return 0;
	}
	return CallWindowProc(ConsoleDefaultProc , hwnd , msg , wp , lp);
}

/********************************************************
 * onCreate
 ********************************************************/
void guiClass::onCreate(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp){
	guiClass* gui = this;
	gui->hWnd = hWnd;
	RECT rc;
	::GetClientRect(hWnd, &rc);
	TC_ITEM tcItem;
	
	if(msg){}
	if(wp){}
	if(lp){}
	
	//Create Tab Window
	gui->hTabWnd = ::CreateWindowEx(
		0,
		WC_TABCONTROL,
		NULL,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | TCS_FOCUSNEVER,
		0, 0, rc.right, TAB_WINDOW_HEIGHT,
		hWnd,
		(HMENU)ID_TAB,
		gui->hInst,
		NULL);
	
	//Register Tab
	for(int Counter = 0;Counter < TAB_MAX; Counter++){
		tcItem.mask = TCIF_TEXT;
		tcItem.pszText = TAB_LABEL[Counter];
		::SendMessage(gui->hTabWnd, TCM_INSERTITEM, (WPARAM)Counter, (LPARAM)&tcItem);
	}
	
	//Create Status Window
	gui->hStatusWnd = ::CreateWindowEx(
		0,
		L"STATIC",
		L"Disconnected",
		WS_CHILD | WS_VISIBLE | SS_RIGHT,
		0, 0, 10, 10,
		hWnd,
		(HMENU)ID_STATUS,
		gui->hInst,
		NULL);
	
	//Create Console Window
	gui->hConsole = ::CreateWindowEx(
		0,
		L"STATIC",
		NULL,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | SS_NOTIFY,
		0, 0, 10, 10,
		hWnd,
		(HMENU)ID_CONSOLE,
		gui->hInst,
		NULL);
	ConsoleDefaultProc = (WNDPROC)GetWindowLong(gui->hConsole , GWL_WNDPROC);
	SetWindowLong(gui->hConsole , GWL_WNDPROC , (LONG)ConsoleProc);
	
	//Create Log Window
	gui->hLogConsole = ::CreateWindowEx(
		0,
		L"EDIT",
		L"Console Log Window",
		WS_CHILD | WS_DLGFRAME | WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | WS_CLIPSIBLINGS,
		0, 0, 10, 10,
		hWnd,
		(HMENU)ID_LOG_CONSOLE,
		gui->hInst,
		NULL);
	
	gui->hLogChat = ::CreateWindowEx(
		0,
		L"EDIT",
		L"Chat Log Window",
		WS_CHILD | WS_DLGFRAME | WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | WS_CLIPSIBLINGS,
		0, 0, 10, 10,
		hWnd,
		(HMENU)ID_LOG_CHAT,
		gui->hInst,
		NULL);
	
	//Create Settings Window
	WCHAR windowStyle[20];
	::memset(windowStyle, 0, sizeof(windowStyle));
	if(settingsColorFlg){
		::wcscpy(windowStyle, L"RICHEDIT");
	}else{
		::wcscpy(windowStyle, L"EDIT");
	}
	
	gui->hSettings = ::CreateWindowEx(
		0,
		windowStyle,
		L"Settings Window",
		WS_CHILD | WS_DLGFRAME | WS_HSCROLL | WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | WS_CLIPSIBLINGS,
		0, 0, 10, 10,
		hWnd,
		(HMENU)ID_SETTINGS,
		gui->hInst,
		NULL);
	
	//ÔO¶¨¥Õ¥¡¥¤¥ë¤ò±íÊ¾
	if(initSettingsWindow()){
		::SetWindowText(gui->hSettings, L"ini read failed");
	}
	
	//Create LobbyList Window
	::InitCommonControls();
	DWORD dwStyle;
	gui->hLobbyList = ::CreateWindowEx(
		0,
		WC_LISTVIEW,
		NULL,
		WS_CHILD | LVS_REPORT | WS_SIZEBOX | WS_VSCROLL | WS_HSCROLL | WS_TABSTOP,
		0, 0, 10, 10,
		hWnd,
		(HMENU)ID_LOBBY_LIST,
		gui->hInst,
		NULL);
	dwStyle = ListView_GetExtendedListViewStyle(gui->hLobbyList);
	dwStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	ListView_SetExtendedListViewStyle(gui->hLobbyList, dwStyle);
	
	//Create LobbyDetail Window
	gui->hLobbyDetail = ::CreateWindowEx(
		0,
		L"EDIT",
		NULL,
		WS_CHILD | WS_SIZEBOX | WS_HSCROLL | WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE,
		0, 0, 10, 10,
		hWnd,
		(HMENU)ID_LOBBY_DETAIL,
		gui->hInst,
		NULL);
	
	::SendMessage(gui->hLobbyDetail, WM_SETFONT, (WPARAM)hFont, 0);
	
	
	//Create LobbyChat Window
	gui->hLobbyChat = ::CreateWindowEx(
		0,
		WC_LISTVIEW,
		NULL,
		WS_CHILD | LVS_REPORT | WS_DLGFRAME | WS_VSCROLL | WS_HSCROLL,
		0, 0, 10, 10,
		hWnd,
		(HMENU)ID_LOBBY_CHAT,
		gui->hInst,
		NULL);
	dwStyle = ListView_GetExtendedListViewStyle(gui->hLobbyChat);
	dwStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	ListView_SetExtendedListViewStyle(gui->hLobbyChat, dwStyle);
	
	insertColumns();
	
	//Create Chat Edit Window
	gui->hLobbyEdit = ::CreateWindowEx(
		0,
		L"EDIT",
		NULL,
		WS_CHILD | WS_DLGFRAME | WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | WS_CLIPSIBLINGS,
		0, 0, 10, 10,
		hWnd,
		(HMENU)ID_LOBBY_EDIT,
		gui->hInst,
		NULL);
	
	//Enter—Ê³ö¤Îžé
	origEditWindowProc = ( WNDPROC )::GetWindowLong( gui->hLobbyEdit, GWL_WNDPROC );
	::SetWindowLong( gui->hLobbyEdit, GWL_WNDPROC, (LONG)EditWindowProc );

	//Create Button Background Window
	//¤Á¤é¤Ä¤­·ÀÖ¹¤Ç±³¾°Ãè»­¤òŸo„¿¤Ë¤¹¤ë¤È¡¢¥¦¥£¥ó¥É¥¦¥µ¥¤¥º‰ä¸ü¤Ç±³¾°¤Ë²ÐÏñ¤¬¤Ç¤­¤Æ¤·¤Þ¤¦¡£
	//¥Ü¥¿¥ó±³¾°²¿·Ö¤ò¥¦¥£¥ó¥É¥¦¤ÇÂñ¤á¤ÆŒê
	gui->hBttnBackground = ::CreateWindowEx(
		0,
		L"STATIC",
		NULL,
		WS_CHILD,
		0, 0, 10, 10,
		hWnd,
		(HMENU)ID_BTTN_BACKGROUND,
		gui->hInst,
		NULL);
	
	//Create Button
	gui->hBttnWait      = CreateButton(BUTTON_LABEL_WAIT     , hWnd, ID_BTTN_WAIT     , gui->hInst);
	gui->hBttnHostWait  = CreateButton(BUTTON_LABEL_HOSTWAIT , hWnd, ID_BTTN_HOSTWAIT , gui->hInst);
	gui->hBttnConnect   = CreateButton(BUTTON_LABEL_CONNECT  , hWnd, ID_BTTN_CONNECT  , gui->hInst);
	gui->hBttnSpectate  = CreateButton(BUTTON_LABEL_SPECTATE , hWnd, ID_BTTN_SPECTATE , gui->hInst);
	gui->hBttnTh075Tool = CreateButton(BUTTON_LABEL_TH075TOOL, hWnd, ID_BTTN_TH075TOOL, gui->hInst);
	gui->hBttnCancel    = CreateButton(BUTTON_LABEL_CANCEL   , hWnd, ID_BTTN_CANCEL   , gui->hInst);
	gui->hBttnSend      = CreateButton(BUTTON_LABEL_SEND     , hWnd, ID_BTTN_SEND     , gui->hInst);
	gui->hBttnDisconnect     = CreateButton(BUTTON_LABEL_DISCONNECT , hWnd, ID_BTTN_DISCONNECT , gui->hInst);
	gui->hBttnSaveRestart    = CreateButton(BUTTON_LABEL_SAVERESTART, hWnd, ID_BTTN_SAVERESTART, gui->hInst);
	gui->hBttnSuica          = CreateButton(BUTTON_LABEL_SUICA      , hWnd, ID_BTTN_SUICA      , gui->hInst);
	gui->hBttnAFK            = CreateButton(BUTTON_LABEL_AFK        , hWnd, ID_BTTN_AFK        , gui->hInst);
	
	//Window Position, Size set
	onWindowSizeChange();
	
	//Lobby TAB select option
	if(autoLobbyFlg == 1 || autoLobbyFlg == 2){
		TabCtrl_SetCurSel(hTabWnd, TAB_LOBBY);
		onTabSelectChange(TAB_LOBBY);
	}
}

