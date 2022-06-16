#ifndef UNICODE
#define UNICODE
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

#include <windows.h>
#include <commctrl.h>
#include <fstream.h>
#include "guiClass.h"
#include "guiIO.h"
#include "lobbyClass.h"
#include "lobbyUserClass.h"
#include "mutexClass.h"
#include "charConvClass.h"
using namespace std;
using namespace N_Gui;


/********************************************************
 * onCommand
 ********************************************************/
void guiClass::onCommand(int command, LPARAM lp){
	lobbyClass* lobby = g_lobby;
	
	switch(command) {
	case ID_CONSOLE:
		::SetFocus(hWnd);
		break;
	case ID_BTTN_WAIT:
		onBttnPressWait();
		::SetFocus(hWnd);
		break;
	case ID_BTTN_HOSTWAIT:
		onBttnPressHostWait();
		::SetFocus(hWnd);
		break;
	case ID_BTTN_CONNECT:
		onBttnPressConnect();
		::SetFocus(hWnd);
		break;
	case ID_BTTN_SPECTATE:
		onBttnPressSpectate();
		::SetFocus(hWnd);
		break;
	case ID_BTTN_TH075TOOL:
		onBttnPressTh075Tool();
		::SetFocus(hWnd);
		break;
	case ID_BTTN_CANCEL:
		onBttnPressCancel();
		::SetFocus(hWnd);
		break;
	case ID_BTTN_SEND:
		onBttnPressSend();
//		::SetFocus(hWnd);
		::SetFocus(hLobbyEdit);
		break;
	case ID_BTTN_DISCONNECT:
		onBttnPressDisconnect();
		::SetFocus(hWnd);
		break;
	case ID_BTTN_SUICA:
		onBttnPressSuica();
		::SetFocus(hWnd);
		break;
	case ID_BTTN_SAVERESTART:
		onBttnPressSaveRestart();
		::SetFocus(hWnd);
		break;
	case ID_BTTN_AFK:
		onBttnPressAFK();
		::SetFocus(hWnd);
		break;
	case ID_LOBBY_LIST_REFRESH:
		onLobbyListRefresh();
		break;
	case ID_LOBBY_CHAT_REFRESH:
		onLobbyChatRefresh();
		break;
	case ID_SET_FOCUS_TO_CONSOLE:
		::SetFocus(hWnd);
		break;
	case ID_SEND_CHAT_MESSAGE:
		onBttnPressSend();
		break;
	case ID_NOTICE_CASTER_GOTO_MENU:
		lobby->onNoticeCasterGotoMenu();
		break;
	case ID_NOTICE_CASTER_GOTO_DELAY_INPUT:
		lobby->onNoticeCasterGotoDelayInput(lp);
		::SetFocus(hWnd);
		break;
	case ID_NOTICE_CASTER_GOTO_DELAY_WAIT:
		lobby->onNoticeCasterGotoDelayWait();
		break;
	case ID_NOTICE_CASTER_GOTO_BATTLE:
		lobby->onNoticeCasterGotoBattle();
		break;
	case ID_NOTICE_CASTER_GOTO_SPECTATE:
		lobby->onNoticeCasterGotoSpectate();
		break;
	default:
		//DO NOTHING
		break;
	}
}


/********************************************************
 * onKeyDown
 ********************************************************/
void guiClass::onKeyDown(WPARAM wp){
	guiClass* gui = g_gui;
	int currentTab = TabCtrl_GetCurSel(gui->hTabWnd);
	
	switch(wp){
	case VK_NEXT:
		//Page UP
		if(::GetKeyState(VK_CONTROL)<0){
#ifdef TAB_CYCLE
			currentTab++;
			TabCtrl_SetCurSel(gui->hTabWnd, currentTab % TAB_MAX);
			gui->ChangeWindowStatus(currentTab % TAB_MAX);
#else
			if(currentTab + 1 < TAB_MAX){
				currentTab++;
				TabCtrl_SetCurSel(gui->hTabWnd, currentTab);
				gui->onTabSelectChange(currentTab);
			}
#endif
		}
		break;
	case VK_PRIOR:
		//Page DOWN
		if(::GetKeyState(VK_CONTROL)<0){
#ifdef TAB_CYCLE
			currentTab += TAB_MAX;
			currentTab--;
			TabCtrl_SetCurSel(gui->hTabWnd, currentTab % TAB_MAX);
			gui->onTabSelectChange(currentTab % TAB_MAX);
#else
			if(currentTab > 0){
				currentTab--;
				TabCtrl_SetCurSel(gui->hTabWnd, currentTab);
				gui->onTabSelectChange(currentTab);
			}
#endif
		}
		break;
	case VK_TAB:
		//CTRL + TAB
		if(::GetKeyState(VK_CONTROL)<0){
			if(GetKeyState(VK_SHIFT) >=0){
				currentTab++;
				TabCtrl_SetCurSel(gui->hTabWnd, currentTab % TAB_MAX);
				gui->onTabSelectChange(currentTab % TAB_MAX);
			}else{
				currentTab += TAB_MAX;
				currentTab--;
				TabCtrl_SetCurSel(gui->hTabWnd, currentTab % TAB_MAX);
				gui->onTabSelectChange(currentTab % TAB_MAX);
			}
		}
		return;
	case VK_ESCAPE:
//		::PostMessage(gui->hWnd, WM_COMMAND, ID_SET_FOCUS_TO_CONSOLE, 0);
		return;
	case VK_UP:
		if(inputHistoryPosition < INPUT_HISTORY_NUM - 1){
			if(::wcslen(inputHistory[inputHistoryPosition + 1])){
				inputHistoryPosition++;
				clearEditStr();
				addEditStr(inputHistory[inputHistoryPosition]);
				reflesh();
			}
		}
		break;
	case VK_DOWN:
		if(inputHistoryPosition > 0){
			inputHistoryPosition--;
			clearEditStr();
			addEditStr(inputHistory[inputHistoryPosition]);
			reflesh();
		}
		break;
	default:
		//DO NOTHING!
		break;
	}
}

/********************************************************
 * onCharInput
 ********************************************************/
void processKeyCode(WPARAM wp){
	guiClass* gui = g_gui;
	
	char keyDown = 0;
	switch(wp){
	case VK_RETURN:
		gui->doReturn();
		break;
	case VK_BACK:
		gui->doBackSpace();
		break;
	case VK_ESCAPE:
		//上位でキャッチしているのでここにはもう来ない
		break;
	default:
		keyDown = wp;
		break;
	}
	if(keyDown){
		char keyDownArray[2];
		keyDownArray[0] = keyDown;
		keyDownArray[1] = 0;
		charConvClass keyConv(keyDownArray);
		
		if(keyConv.wide[0] >= 0x10 && keyConv.wide[0] <= 0x19){
			//全角数字だと変な値が送られてくる
			keyConv.wide[0] += 0x20;
		}
		gui->addEditChar(keyConv.wide[0]);
	}
}

void guiClass::onCharInput(WPARAM wp){
	guiClass* gui = this;
	int currentTab = TabCtrl_GetCurSel(gui->hTabWnd);
	
	switch(currentTab){
	case TAB_CONSOLE:
	case TAB_LOG:
	case TAB_SETTINGS:
#ifdef ENABLE_LOBBY_KEY_INPUT
	case TAB_LOBBY:
#endif
		processKeyCode(wp);
		gui->reflesh();
		break;
	default:
		break;
	}
}

/********************************************************
 * onWindowSizeChange
 *
 * Frame Position,Height,Width
 ********************************************************/
typedef struct{
	int x;
	int y;
	int w;
	int h;
}frameInfoStruct;

class frameManagerClass{
public:
	frameInfoStruct frameInfoTab;
	frameInfoStruct frameInfoStatus;
	frameInfoStruct frameInfoConsole;
	frameInfoStruct frameInfoList;
	frameInfoStruct frameInfoDetail;
	frameInfoStruct frameInfoChat;
	frameInfoStruct frameInfoEdit;
	frameInfoStruct frameInfoButtonArea;
	frameInfoStruct frameInfoLogConsole;
	frameInfoStruct frameInfoLogChat;
	frameInfoStruct frameInfoSettings;
	int windowHeight;
	int windowWidth;
	void calc(){
		guiClass* gui = g_gui;
		RECT rc;
		::GetClientRect(gui->hWnd, &rc);
		
		windowHeight = rc.bottom - rc.top;
		windowWidth  = rc.right - rc.left;
		
		int x1,x2;
		int y1,y2,y3,y4;
		
		x1 = CONSOLE_LEFT_WIDTH;
		x2 = windowWidth - LOBBY_DETAIL_WIDTH;
		if(x2 < x1 + LOBBY_LIST_WIDTH) x2 = x1 + LOBBY_LIST_WIDTH;
		if(x2 > windowWidth) x2 = windowWidth;
		y1 = TAB_WINDOW_HEIGHT;
		y3 = windowHeight - LOBBY_BUTTON_AREA_HEIGHT - LOBBY_EDIT_HEIGHT;
		y4 = windowHeight - LOBBY_BUTTON_AREA_HEIGHT;
		y2 = (float)(y1 + y3) * 0.4F;
		
		if(x1 > x2) x2 = x1;
		if(y1 > y2) y2 = y1;
		if(y2 > y3) y3 = y2;
		if(y3 > y4) y4 = y3;
		
		frameInfoTab.x = 0;
		frameInfoTab.y = 0;
		frameInfoTab.w = windowWidth;
		frameInfoTab.h = y1;
		
		frameInfoConsole.x = 0;
		frameInfoConsole.y = y1;
		frameInfoConsole.w = windowWidth;
		frameInfoConsole.h = windowHeight - y1;
		if(TAB_CONSOLE != TabCtrl_GetCurSel(gui->hTabWnd)){
			frameInfoConsole.w = x1;
		}
		
		frameInfoList.x = x1;
		frameInfoList.y = y1;
		frameInfoList.w = x2 - x1;
		frameInfoList.h = y2 - y1;
		
		frameInfoDetail.x = x2;
		frameInfoDetail.y = y1;
		frameInfoDetail.w = windowWidth - x2;
		frameInfoDetail.h = y2 - y1;
		
		frameInfoChat.x = x1;
		frameInfoChat.y = y2;
		frameInfoChat.w = windowWidth - x1;
		frameInfoChat.h = y3 - y2;
		
		frameInfoEdit.x = x1;
		frameInfoEdit.y = y3;
		frameInfoEdit.w = windowWidth - x1;
		frameInfoEdit.h = y4 - y3;
		
		frameInfoButtonArea.x = x1;
		frameInfoButtonArea.y = y4;
		frameInfoButtonArea.w = windowWidth - x1;
		frameInfoButtonArea.h = windowHeight - y4;
		
		frameInfoLogConsole.x = x1;
		frameInfoLogConsole.y = y1;
		frameInfoLogConsole.w = windowWidth - x1;
		frameInfoLogConsole.h = y3 - y1;
		
		frameInfoLogChat.x = x1;
		frameInfoLogChat.y = y3;
		frameInfoLogChat.w = windowWidth - x1;
		frameInfoLogChat.h = windowHeight - y3;
		
		frameInfoSettings.x = x1;
		frameInfoSettings.y = y1;
		frameInfoSettings.w = windowWidth - x1;
		frameInfoSettings.h = y4 - y1;
		
		//TODO
		frameInfoStatus.x = windowWidth - 100 - 4;
		if(frameInfoStatus.x < x1) frameInfoStatus.x = x1;
		frameInfoStatus.y = 4;
		frameInfoStatus.w = 100;
		frameInfoStatus.h = y1 - 10;
		
	}
};

void MoveWindow(HWND hWnd, frameInfoStruct* info){
	::MoveWindow(hWnd, info->x, info->y, info->w, info->h,TRUE);
}

void guiClass::onWindowSizeChange(){
	guiClass* gui = this;
	
	frameManagerClass frameManager;
	frameManager.calc();
	
	//Window
	MoveWindow(gui->hTabWnd,      &frameManager.frameInfoTab);
	MoveWindow(gui->hStatusWnd,   &frameManager.frameInfoStatus);
	MoveWindow(gui->hConsole,     &frameManager.frameInfoConsole);
	MoveWindow(gui->hLobbyList,   &frameManager.frameInfoList);
	MoveWindow(gui->hLobbyDetail, &frameManager.frameInfoDetail);
	MoveWindow(gui->hLobbyChat,   &frameManager.frameInfoChat);
	MoveWindow(gui->hLobbyEdit,   &frameManager.frameInfoEdit);
	MoveWindow(gui->hLogConsole,  &frameManager.frameInfoLogConsole);
	MoveWindow(gui->hLogChat,     &frameManager.frameInfoLogChat);
	MoveWindow(gui->hSettings,    &frameManager.frameInfoSettings);
	MoveWindow(gui->hBttnBackground, &frameManager.frameInfoButtonArea);
	
	frameInfoStruct* info;
	info = &(frameManager.frameInfoButtonArea);
	int positionX;
	int baseLine;
	int buttanHeight;
	
	//Lobby Window Buttons
	positionX = info->x + LOBBY_BUTTON_PADDING;
	baseLine = info->y + info->h - LOBBY_BUTTON_PADDING;
	
	buttanHeight = LOBBY_BUTTON_HEIGHT_L;
	::MoveWindow(gui->hBttnWait, positionX , baseLine - buttanHeight, LOBBY_BUTTON_WIDTH, buttanHeight, TRUE);
	
	positionX += LOBBY_BUTTON_PADDING + LOBBY_BUTTON_WIDTH;
	buttanHeight = LOBBY_BUTTON_HEIGHT;
	::MoveWindow(gui->hBttnHostWait, positionX , baseLine - buttanHeight, LOBBY_BUTTON_WIDTH, buttanHeight, TRUE);
	
	positionX += LOBBY_BUTTON_PADDING + LOBBY_BUTTON_WIDTH;
	buttanHeight = LOBBY_BUTTON_HEIGHT_L;
	::MoveWindow(gui->hBttnConnect, positionX , baseLine - buttanHeight, LOBBY_BUTTON_WIDTH, buttanHeight, TRUE);
	
	positionX += LOBBY_BUTTON_PADDING + LOBBY_BUTTON_WIDTH;
	buttanHeight = LOBBY_BUTTON_HEIGHT;
	::MoveWindow(gui->hBttnSpectate, positionX , baseLine - buttanHeight, LOBBY_BUTTON_WIDTH, buttanHeight, TRUE);
	
	positionX += LOBBY_BUTTON_PADDING + LOBBY_BUTTON_WIDTH;
	buttanHeight = LOBBY_BUTTON_HEIGHT;
	::MoveWindow(gui->hBttnAFK, positionX , baseLine - buttanHeight, LOBBY_BUTTON_WIDTH, buttanHeight, TRUE);
	
	positionX += LOBBY_BUTTON_PADDING + LOBBY_BUTTON_WIDTH;
	buttanHeight = LOBBY_BUTTON_HEIGHT_L;
	::MoveWindow(gui->hBttnCancel, positionX , baseLine - buttanHeight, LOBBY_BUTTON_WIDTH, buttanHeight, TRUE);
	
	positionX += LOBBY_BUTTON_PADDING + LOBBY_BUTTON_WIDTH;
	int sendButtonX = frameManager.windowWidth - LOBBY_BUTTON_PADDING * 2 - LOBBY_BUTTON_WIDTH;
	if(positionX < sendButtonX){
		positionX = sendButtonX;
	}
	buttanHeight = LOBBY_BUTTON_HEIGHT_L;
	::MoveWindow(gui->hBttnSend, positionX , baseLine - buttanHeight, LOBBY_BUTTON_WIDTH_L, buttanHeight,TRUE);
	
	//Settings Window Buttons
	positionX = info->x + LOBBY_BUTTON_PADDING;
	buttanHeight = LOBBY_BUTTON_HEIGHT_L;
	::MoveWindow(gui->hBttnDisconnect, positionX , baseLine - buttanHeight, LOBBY_BUTTON_WIDTH * 3, buttanHeight, TRUE);
	
	positionX += LOBBY_BUTTON_PADDING + LOBBY_BUTTON_WIDTH * 3;
	buttanHeight = LOBBY_BUTTON_HEIGHT;
	::MoveWindow(gui->hBttnSuica, positionX , baseLine - buttanHeight, LOBBY_BUTTON_WIDTH, buttanHeight, TRUE);
	
	positionX += LOBBY_BUTTON_PADDING + LOBBY_BUTTON_WIDTH;
	buttanHeight = LOBBY_BUTTON_HEIGHT;
	::MoveWindow(gui->hBttnTh075Tool, positionX , baseLine - buttanHeight, LOBBY_BUTTON_WIDTH, buttanHeight, TRUE);
	
	positionX += LOBBY_BUTTON_PADDING + LOBBY_BUTTON_WIDTH;
	sendButtonX = frameManager.windowWidth - LOBBY_BUTTON_PADDING * 2 - LOBBY_BUTTON_WIDTH * 2;
	if(positionX < sendButtonX){
		positionX = sendButtonX;
	}
	buttanHeight = LOBBY_BUTTON_HEIGHT_L;
	::MoveWindow(gui->hBttnSaveRestart, positionX , baseLine - buttanHeight, LOBBY_BUTTON_WIDTH * 2, buttanHeight,TRUE);
	
}

/********************************************************
 * Show lobby connect dialog
 *
 * Dialog position change with Hook
 ********************************************************/
//TODO
WNDPROC defaultDialogProc;
HHOOK   hDialogHook;

LONG CALLBACK dialogProc( HWND hWnd, UINT uMsg, UINT wp, LONG lp ){
	LONG ret = CallWindowProc( defaultDialogProc, hWnd, uMsg, wp, lp );
	if ( WM_INITDIALOG == uMsg ){
		RECT windowRC;
		RECT dialogRC;
		::GetWindowRect(g_gui->hWnd, &windowRC);
		::GetWindowRect(hWnd, &dialogRC);
		int x = windowRC.left + (windowRC.right - windowRC.left - dialogRC.right + dialogRC.left) / 2;
		int y = windowRC.top + (windowRC.bottom - windowRC.top - dialogRC.bottom + dialogRC.top) / 2;
		::SetWindowPos( hWnd, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE );
		
		::SetWindowLong( hWnd, GWL_WNDPROC, LONG( defaultDialogProc ) );
		defaultDialogProc = 0;
	}
	return ret;
}

LONG CALLBACK dialogHookProc( int code, UINT wp, LONG lp ){
	const CWPRETSTRUCT* p = ( const CWPRETSTRUCT* )lp;
	if ( WM_NCCREATE == p->message ){
		::UnhookWindowsHookEx( hDialogHook );
		hDialogHook = 0;
		defaultDialogProc = ( WNDPROC )::GetWindowLong( p->hwnd, GWL_WNDPROC );
		::SetWindowLong( p->hwnd, GWL_WNDPROC, LONG( dialogProc ) );
	}
	return ::CallNextHookEx( hDialogHook, code, wp, lp );
}

void guiClass::ShowLobbyConnectDialog(){
	lobbyClass* lobby = g_lobby;
	bool bLobbyStart = false;
	
	if(autoLobbyFlg == 2){
		bLobbyStart = true;
	}else{
		hDialogHook = ::SetWindowsHookEx( WH_CALLWNDPROCRET, dialogHookProc, hInst, GetCurrentThreadId() );
		if(::MessageBox(hWnd, L"Connect to Lobby", L"Lobby", MB_YESNO) == IDYES){
			bLobbyStart = true;
		}
	}
	
	autoLobbyFlg = 0;
	
	if(bLobbyStart){
		if(lobby->init()){
			message.write(L"Lobby connect failed");
			lobby->end();
		}
	}
}

/********************************************************
 * Save and Restart
 *
 * Refs http://www.sutosoft.com/room/archives/000012.html
 ********************************************************/
void guiClass::onSettingsSave(){
	hDialogHook = ::SetWindowsHookEx( WH_CALLWNDPROCRET, dialogHookProc, hInst, ::GetCurrentThreadId() );
	if(::MessageBox(hWnd, L"Save and Restart", L"Settings", MB_YESNO) == IDYES){
		//Save
		//LinuxだとGetWindowTextW後の変換で失敗することがあったため、Aで実施
		int size = ::GetWindowTextLengthA(hSettings);
		if(size){
			char* str = new char[size + 2];
			::memset(str, 0, (size + 2) * sizeof(char));
			::GetWindowTextA(hSettings , str, size + 2);
			str[size + 1] = 0;
/*
			charConvClass conv(str);
			delete[] str;
			char* iniStr = conv.shift;
*/
			char* iniStr = str;
			
			//Linuxだと変換失敗する。ウィンドウから取得する文字が異なっている? 
			if(::strlen(iniStr)==0){
				::MessageBox(hWnd, L"failed to get window strings.", L"Settings", MB_OK);
				return;
			}
			
			::_wremove(L"config_rollcaster.ini.bak");
			::_wrename(L"config_rollcaster.ini", L"config_rollcaster.ini.bak");
			std::ofstream fout("config_rollcaster.ini", ios::out|ios::binary|ios::trunc);
			fout << iniStr;
			if(!fout || fout.fail()){
				fout.close();
				::_wremove(L"config_rollcaster.ini");
				::_wrename(L"config_rollcaster.ini.bak", L"config_rollcaster.ini");
				
				hDialogHook = ::SetWindowsHookEx( WH_CALLWNDPROCRET, dialogHookProc, hInst, ::GetCurrentThreadId() );
				::MessageBox(hWnd, L"Save failed.", L"Settings", MB_OK);
				return;
			}
			::_wremove(L"config_rollcaster.ini.bak");
			delete[] str;
		}else{
			::MessageBox(hWnd, L"failed to get window strings.", L"Settings", MB_OK);
			return;
		}
		
		//Restart
		PROCESS_INFORMATION pi;
		STARTUPINFO si;

		::ZeroMemory(&si,sizeof(si));
		si.cb=sizeof(si);
		
		DWORD pid = ::GetCurrentProcessId();
		WCHAR param[20];
		WCHAR path[1024];
		::memset(param, 0, sizeof(param));
		::memset(path, 0, sizeof(path));
		::swprintf(param, L" -q%d\0", pid);
		::GetModuleFileName(NULL, path, 1000);
		::wcscat(path, param);
		::CreateProcess(NULL, path, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);

		::CloseHandle(pi.hThread);
		::CloseHandle(pi.hProcess);

		::PostMessage(hWnd, WM_CLOSE, 0, 0);
	}
}

/********************************************************
 * Change Lobby Window Status
 ********************************************************/
void guiClass::ChangeLobbyWindowStatus(int status){
	guiClass* gui = this;
	
	::ShowWindow( gui->hLobbyList,   status );
	::ShowWindow( gui->hLobbyChat,   status );
	::ShowWindow( gui->hLobbyDetail, status );
	::ShowWindow( gui->hLobbyEdit,   status );
	::ShowWindow( gui->hBttnWait,      status );
	::ShowWindow( gui->hBttnHostWait,  status );
	::ShowWindow( gui->hBttnConnect,   status );
	::ShowWindow( gui->hBttnSpectate,  status );
	::ShowWindow( gui->hBttnAFK,     status );
	::ShowWindow( gui->hBttnCancel,    status );
	::ShowWindow( gui->hBttnSend,      status );
}

/********************************************************
 * Change Settings Window Status
 ********************************************************/
void guiClass::ChangeSettingsWindowStatus(int status){
	guiClass* gui = this;
	
	::ShowWindow( gui->hSettings       , status );
	::ShowWindow( gui->hBttnDisconnect , status );
	::ShowWindow( gui->hBttnSuica      , status );
	::ShowWindow( gui->hBttnSaveRestart, status );
	::ShowWindow( gui->hBttnTh075Tool  , status );
}

/********************************************************
 * Change Log Window Status
 ********************************************************/
void guiClass::ChangeLogWindowStatus(int status){
	guiClass* gui = this;
	
	::ShowWindow( gui->hLogConsole, status );
	::ShowWindow( gui->hLogChat   , status );
}

/********************************************************
 * onTabSelectChange
 *
 * Change Window Status, Set Focus
 ********************************************************/
void guiClass::onTabSelectChange(int n){
	guiClass* gui = this;
	lobbyClass* lobby = g_lobby;
	RECT rc;
	::GetClientRect(gui->hWnd, &rc);
	
	switch(n){
	case TAB_CONSOLE:
		::ShowWindow( gui->hConsole,  SW_SHOW );
		ChangeLogWindowStatus(        SW_HIDE );
		ChangeLobbyWindowStatus(      SW_HIDE );
		ChangeSettingsWindowStatus(   SW_HIDE );
		::ShowWindow( gui->hBttnBackground, SW_HIDE );
		::MoveWindow(gui->hConsole, rc.left, TAB_WINDOW_HEIGHT, rc.right, rc.bottom - TAB_WINDOW_HEIGHT, TRUE);
		gui->cursorType = 0;
		::SetFocus(gui->hWnd);
		break;
	case TAB_LOBBY:
		::ShowWindow( gui->hConsole,  SW_SHOW );
		ChangeLogWindowStatus(        SW_HIDE );
		ChangeLobbyWindowStatus(      SW_SHOW );
		ChangeSettingsWindowStatus(   SW_HIDE );
		
		if(gui->cursorBlinkFlg == 2 || gui->cursorBlinkFlg == 3){
			::ShowWindow( gui->hBttnBackground,  SW_SHOW );
		}
		::MoveWindow(gui->hConsole, rc.left, TAB_WINDOW_HEIGHT, CONSOLE_LEFT_WIDTH, rc.bottom - TAB_WINDOW_HEIGHT, TRUE);
#ifndef ENABLE_LOBBY_KEY_INPUT
		gui->cursorType = 1;
#endif
		::SetFocus(gui->hWnd);
		
		//要検討
		if(!lobby->isRunning()){
			ShowLobbyConnectDialog();
		}
		
		break;
	case TAB_LOG:
		::ShowWindow( gui->hConsole,  SW_SHOW );
		ChangeLogWindowStatus(        SW_SHOW );
		ChangeLobbyWindowStatus(      SW_HIDE );
		ChangeSettingsWindowStatus(   SW_HIDE );
		::ShowWindow( gui->hBttnBackground,  SW_HIDE );
		::MoveWindow(gui->hConsole, rc.left, TAB_WINDOW_HEIGHT, CONSOLE_LEFT_WIDTH, rc.bottom - TAB_WINDOW_HEIGHT, TRUE);
		gui->cursorType = 0;
		::SetFocus(gui->hWnd);
		break;
	case TAB_SETTINGS:
		::ShowWindow( gui->hConsole,  SW_SHOW );
		ChangeLogWindowStatus(        SW_HIDE );
		ChangeLobbyWindowStatus(      SW_HIDE );
		ChangeSettingsWindowStatus(   SW_SHOW );
		if(gui->cursorBlinkFlg == 2){
			::ShowWindow( gui->hBttnBackground,  SW_SHOW );
		}
		::MoveWindow(gui->hConsole, rc.left, TAB_WINDOW_HEIGHT, CONSOLE_LEFT_WIDTH, rc.bottom - TAB_WINDOW_HEIGHT, TRUE);
		gui->cursorType = 0;
		::SetFocus(gui->hWnd);
		break;
	default:
		::ShowWindow( gui->hConsole,  SW_HIDE );
		ChangeLogWindowStatus(        SW_HIDE );
		ChangeLobbyWindowStatus(      SW_HIDE );
		ChangeSettingsWindowStatus(   SW_HIDE );
		::ShowWindow( gui->hBttnBackground,  SW_HIDE );
		::SetFocus(gui->hWnd);
		break;
	}
}

/********************************************************
 * onCommand
 ********************************************************/
void guiClass::onBttnPressWait(){
	lobbyClass* lobby = g_lobby;
	lobby->onBttnPressWait();
}

void guiClass::onBttnPressHostWait(){
	lobbyClass* lobby = g_lobby;
	lobby->onBttnPressHostWait();
}

void guiClass::onBttnPressConnect(){
	lobbyClass* lobby = g_lobby;
	int n = ListView_GetNextItem(hLobbyList, -1, LVNI_ALL | LVNI_SELECTED);
	
	if(n == -1){
		message.write(L"Target player is not selected.");
		return;
	}
	
	WCHAR UID[40];
	WCHAR name[100];
	WCHAR shortUID[4];
	::memset(UID, 0, sizeof(UID));
	::memset(name, 0, sizeof(name));
	ListView_GetItemText(hLobbyList, n, 1, UID, _countof(UID) -1);
	ListView_GetItemText(hLobbyList, n, 0, name, _countof(name) -1);
	::wcsncpy(shortUID, UID, 3);
	shortUID[3] = L'\0';
	message.write(L"Connect to %s@%s", name, shortUID);
	
	lobby->onBttnPressConnect(UID);
}

void guiClass::onBttnPressSpectate(){
	lobbyClass* lobby = g_lobby;
	int n = ListView_GetNextItem(hLobbyList, -1, LVNI_ALL | LVNI_SELECTED);
	
	if(n == -1){
		message.write(L"Target player is not selected.");
		return;
	}
	
	WCHAR UID[40];
	WCHAR name[100];
	WCHAR shortUID[4];
	::memset(UID, 0, sizeof(UID));
	::memset(name, 0, sizeof(name));
	ListView_GetItemText(hLobbyList, n, 1, UID, _countof(UID) -1);
	ListView_GetItemText(hLobbyList, n, 0, name, _countof(name) -1);
	::wcsncpy(shortUID, UID, 3);
	shortUID[3] = L'\0';
	message.write(L"Try spectate to %s@%s", name, shortUID);
	
	lobby->onBttnPressSpectate(UID);
}

void guiClass::onBttnPressTh075Tool(){
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	
    ::memset(&pi, 0, sizeof(pi));
	::memset(&si, 0, sizeof(si));
    
	if (!::CreateProcess(NULL , TH075TOOL_EXE , NULL , NULL , FALSE ,  NORMAL_PRIORITY_CLASS , NULL , NULL , &si , &pi)){
		message.write(L"th075Tool launch failed. Open URL...");
		
		//TODO
		::ShellExecute(NULL, L"open", L"http://wiki.mizuumi.net/w/Immaterial_and_Missing_Power/th075tool-Jp", NULL, NULL, SW_SHOWNORMAL);
		return;
	}
	::CloseHandle(pi.hProcess);
	::CloseHandle(pi.hThread);
}

void guiClass::onBttnPressCancel(){
	lobbyClass* lobby = g_lobby;
	lobby->onBttnPressCancel();
}

void guiClass::onBttnPressSend(){
	lobbyClass* lobby = g_lobby;
	
	int size = ::GetWindowTextLength(hLobbyEdit) + 2;
	WCHAR* buffer = new WCHAR[size + 1];
	::memset(buffer, 0, (size + 1) * sizeof(WCHAR));
	
	::GetWindowText(hLobbyEdit, buffer, size);
	if(::wcslen(buffer)){
		lobby->onBttnPressSend(buffer);
		::SetWindowText(hLobbyEdit, L"");
	}
	delete[] buffer;
}

void guiClass::onBttnPressDisconnect(){
	lobbyClass* lobby = g_lobby;
	
	//disconnect
	lobby->end();
	
	//window clear
	::SetWindowText(hLobbyDetail, L"");
	::SetWindowText(hLobbyEdit, L"");
	ListView_DeleteAllItems(hLobbyList);
	ListView_DeleteAllItems(hLobbyChat);
}

void guiClass::onBttnPressSuica(){
	WCHAR httpStr[]  = L"http://";
//	WCHAR suicaURL[] = L"th075ranking.dip.jp";
	WCHAR phpStr[]   = L"/acc_index.php?account=";
	
	int size = ::wcslen(httpStr) + ::wcslen(suicaURL) + ::wcslen(phpStr) + ::wcslen(suicaAccName);
	WCHAR* myURL = new WCHAR[size + 1];
	::memset(myURL, 0, (size + 1) * sizeof(WCHAR));
	::wcscpy(myURL, httpStr);
	::wcscat(myURL, suicaURL);
	if(::wcslen(suicaAccName)){
		::wcscat(myURL, phpStr);
		::wcscat(myURL, suicaAccName);
	}
	
	::ShellExecute(NULL, L"open", myURL, NULL, NULL, SW_SHOWNORMAL);
	delete[] myURL;
}

void guiClass::onBttnPressSaveRestart(){
	onSettingsSave();
}

void guiClass::onBttnPressAFK(){
	lobbyClass* lobby = g_lobby;
	lobby->onBttnPressAFK();
}

