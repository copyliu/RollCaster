#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <process.h>
#include <commctrl.h>
#include <winsock2.h>
#include "guiIO.h"
#include "guiClass.h"
#include "casterClass.h"
#include "lobbyClass.h"
#include "charConvClass.h"
using namespace std;
using namespace N_Gui;

#pragma resource "icon/RollCaster.res"

/********************************************************
 * MISC
 ********************************************************/
namespace N_Gui{
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ImgWindowProc(HWND, UINT, WPARAM, LPARAM);

guiClass*    g_gui    = NULL;
casterClass* g_caster = NULL;
lobbyClass*  g_lobby  = NULL;
}

/********************************************************
 * Register WIndow Class
 ********************************************************/
ATOM registerWindowClass(WNDPROC lpfnWndProc, WCHAR* szWindowClass, HBRUSH hbr) {
	WNDCLASSEX wcex;
	wcex.cbSize 		= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= lpfnWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= (HINSTANCE) GetModuleHandle(NULL);
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= hbr;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= NULL;
	return ::RegisterClassEx(&wcex);
}

/********************************************************
 * Init Window
 ********************************************************/
int initWindow(HINSTANCE hInst, HINSTANCE hPrevInst, WCHAR* commandLine, int nCmdShow){
	if(hPrevInst){}
	
	guiClass*    gui    = g_gui;
	casterClass* caster = g_caster;
	
	WCHAR pClassName[]    = L"mainWindowClass";
	WCHAR pImgClassName[] = L"imgWindowClass";
	HWND hWnd;
	HWND hImgWnd = NULL;
	int windowWidth;
	int windowHeight;
	
	if (!registerWindowClass(WindowProc, pClassName, CreateSolidBrush(BACKGROUND_COLOR))
	    || !registerWindowClass(ImgWindowProc, pImgClassName, CreateSolidBrush(BACKGROUND_COLOR))
	    || gui->init()){
		return 1;
	}
	
	windowWidth  = gui->windowWidth;
	windowHeight = gui->windowHeight;
	
	hWnd = CreateWindowEx(
		 0,
		 pClassName,
		 WINDOW_TITLE,
		 WS_OVERLAPPEDWINDOW,
		 CW_USEDEFAULT,
		 CW_USEDEFAULT,
		 windowWidth,
		 windowHeight,
		 NULL,
		 NULL,
		 hInst,
		 NULL);
	
	if(gui->imgAlpha){
		hImgWnd = CreateWindowEx(
			 WS_EX_LAYERED | WS_EX_TRANSPARENT,
			 pImgClassName,
			 WINDOW_TITLE,
			 WS_POPUP,
			 CW_USEDEFAULT,
			 CW_USEDEFAULT,
			 windowWidth,
			 windowHeight,
			 hWnd,
			 NULL,
			 hInst,
			 NULL);
	}
	
	if(!hWnd){
		return 1;
	}
	
	ShowWindow(hWnd, nCmdShow);
	if(hImgWnd){
		ShowWindow(hImgWnd, nCmdShow);
	}
	
	if(caster->start(commandLine)){
		return 1;
	}
	
	return 0;
}

/********************************************************
 * WinMain
 ********************************************************/
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR commandLineParam, int nCmdShow){
	
	WSADATA wsaData;
	WORD wVersion = MAKEWORD( 2, 0);
	MSG msg;
	charConvClass conv(commandLineParam);
	WCHAR* commandLine = conv.wide;
	
	{
		//TODO
		//Save and Restartで再起動後の前プロセス終了待ち
		unsigned int size = wcslen(commandLine);
		WCHAR* param = new WCHAR[size + 1];
		memset(param, 0, (size + 1) * sizeof(WCHAR));
		wcscpy(param, commandLine);
		
		WCHAR* arg = NULL;
		unsigned int position;
		for(position=0; position + 1<size; position++){
			if(param[position] == '-'){
				position++;
				if(param[position] == 'q'){
					position++;
					arg = &param[position];
					break;
				}
			}
		}
		
		if(arg){
			for(position=0; position<wcslen(arg); position++){
				if(arg[position] == ' '){
					arg[position] = 0;
				}
			}
			
			DWORD pid = (DWORD)_wtoi(arg);
			HANDLE hWaitProcess = OpenProcess(SYNCHRONIZE, FALSE, pid);
			if (hWaitProcess != NULL) {
				if (WaitForSingleObject(hWaitProcess, 10 * 1000) == WAIT_TIMEOUT) {
					CloseHandle(hWaitProcess);
					return 0;
				}
				CloseHandle(hWaitProcess);
			}
		}
		delete[] param;
	}
	
	if( !WSAStartup(wVersion , &wsaData)){
		if( wVersion == wsaData.wVersion ){
			
			g_gui    = new guiClass(hInst);;
			g_caster = new casterClass();
			g_lobby  = new lobbyClass();
			
			if(!initWindow(hInst, hPrevInst, commandLine, nCmdShow)){
				while (GetMessage(&msg, NULL, 0, 0)) {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
			
			delete g_lobby;
			g_lobby  = NULL;
			
			delete g_caster;
			g_caster = NULL;
			
			delete g_gui;
			g_gui    = NULL;
		}
	}
	WSACleanup();
	
	return 0;
}

/********************************************************
 * ImgWindow Window Proc
 ********************************************************/
LRESULT CALLBACK N_Gui::ImgWindowProc(HWND hWnd, UINT message, WPARAM wp, LPARAM lp){
	guiClass* gui = g_gui;
	
	static HDC hdcMem = NULL;
	static HBITMAP hbmpMem = NULL;
	static HBITMAP hbmpMemPrev = NULL;
	
	switch (message) {
	case WM_CREATE:
		{
			gui->hImgWnd = hWnd;
			SetLayeredWindowAttributes(hWnd, 0, gui->imgAlpha, LWA_ALPHA);
			
			HDC hdc;

			hdc = GetDC(hWnd);
			
			hdcMem = CreateCompatibleDC(hdc);
			hbmpMem = (HBITMAP)LoadImage(NULL, gui->imgFileName, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
			if (hbmpMem == NULL) {
				ReleaseDC(hWnd, hdc);
				return -1;
			}

			hbmpMemPrev = (HBITMAP)SelectObject(hdcMem, hbmpMem);

			ReleaseDC(hWnd, hdc);
		}
		break;
	case WM_SETFOCUS:
		SetFocus(gui->hWnd);
		break;
	case WM_ERASEBKGND:
		//ちらつき防止のため
		return 1;
	case WM_PAINT:
		if(hbmpMem != NULL){
			PAINTSTRUCT ps;
			HDC hdc;
			HDC mdc;
			HBITMAP oldbmp;
			BITMAP bm;
			RECT rect;
			float scale = 1.0F;
			
			GetClientRect(hWnd , &rect);
			
			hdc = BeginPaint(hWnd, &ps);
			
			GetObject(hbmpMem, sizeof(bm), &bm);
			
			if(gui->imgScaleDownFlg || bm.bmWidth < rect.right || bm.bmHeight < rect.bottom){
				float ratio = (float)bm.bmHeight / (float)bm.bmWidth;
				
				int windowHeight = rect.bottom - rect.top;
				int windowWidth = rect.right - rect.left;
				if(ratio * windowWidth >= windowHeight){
					scale = (float)windowWidth / (float)bm.bmWidth;
				}else{
					scale = (float)windowHeight / (float)bm.bmHeight;
				}
			}
			
			SetStretchBltMode(hdc , COLORONCOLOR);
			StretchBlt(
				hdc , 0 , 0 , bm.bmWidth * scale , bm.bmHeight * scale ,
				hdcMem , 0 , 0 , bm.bmWidth , bm.bmHeight , SRCCOPY
			);
			
			EndPaint(hWnd , &ps);
		}
		break;
	case WM_DESTROY:
		if (hdcMem != NULL) {
			if (hbmpMem != NULL) {
				SelectObject(hdcMem, hbmpMemPrev);
				DeleteObject(hbmpMem);
			}
			DeleteDC(hdcMem);
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wp, lp);
	}
	return 0;
}

/********************************************************
 * Main Window Proc
 ********************************************************/
LRESULT CALLBACK N_Gui::WindowProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp){
	guiClass* gui = g_gui;
	NMHDR* nmhdr;
	
	switch (msg) {
	case WM_CREATE:
		gui->onCreate(hWnd, msg, wp, lp);
		if(gui->windowPositionFlg){
			MoveWindow(hWnd, gui->windowPositionX, gui->windowPositionY, gui->windowWidth, gui->windowHeight, TRUE);
		}else{
			//TODO
			//コンソールが出力される前にサイズ変更しておきたい 
			RECT windowRect;
			GetWindowRect(hWnd, &windowRect);
			MoveWindow(gui->hImgWnd, windowRect.left, windowRect.top, gui->windowWidth, gui->windowHeight, TRUE);
		
		}
		SetFocus(hWnd);
		break;
	case WM_DESTROY:
		if(gui->hImgWnd){
			PostMessage(gui->hImgWnd, WM_CLOSE, 0, 0);
		}
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		if((lp & 0x80000000)==0){
			gui->onKeyDown(wp);
		}
		
		//for WINE on Linux
		if(wp == VK_ESCAPE){
			gui->linuxPressEscapeFlg = 1;
		}
		break;
	case WM_KEYUP:
		//for WINE on Linux
		if(wp == VK_ESCAPE){
			gui->linuxPressEscapeFlg = 0;
		}
		break;
	case WM_CHAR:
		gui->onCharInput(wp);
		break;
	case WM_CTLCOLORSTATIC: 
		switch( GetWindowLong( (HWND)lp, GWL_ID )){
		case ID_CONSOLE:
			SetBkMode((HDC)wp, TRANSPARENT);
			SetTextColor((HDC)wp, EDITOR_TEXT_COLOR);
			return (BOOL)((HBRUSH)GetStockObject(EDITOR_BG_COLOR));
		case ID_BTTN_BACKGROUND:
			return (LRESULT)CreateSolidBrush(BACKGROUND_COLOR);
		default:
			break;
		}
		break;
	case WM_SIZE:
		gui->onWindowSizeChange();
		
		if(!gui->wineFlg){
			//WM_SIZEは連続で通知されるため、画像サイズ変更は遅延をかけて行う
			//backGroundThreadからWM_COMMANDのID_DELAYED_WM_SIZEをポストする
			gui->windowSizeChangeEvent = 1;
			break;
		}else{
			//DO NOT BREAK
		}
	case WM_MOVE:
		if(gui->hImgWnd){
			RECT rect;
			RECT windowRect;
			GetWindowRect(hWnd, &windowRect);
			GetWindowRect(gui->hConsole, &rect);
			MoveWindow(gui->hImgWnd, windowRect.left, rect.top, windowRect.right - windowRect.left, windowRect.bottom - rect.top, TRUE);
		}
		break;
	case WM_NOTIFY:
		nmhdr = (NMHDR*)lp;
		switch(wp){
		case ID_LOBBY_LIST:
			switch (nmhdr->code) {
			case LVN_ITEMCHANGED:
				//複数回送られる
				gui->onPlayerListSelect();
				break;
			case NM_DBLCLK:
				gui->onPlayerListDoubleClick();
				break;
			case NM_CUSTOMDRAW:
				return gui->onLobbyListCustomDraw(hWnd, wp, lp);
			}
			break;
		case ID_LOBBY_CHAT:
			switch (nmhdr->code) {
			case NM_CUSTOMDRAW:
				return gui->onLobbyChatCustomDraw(hWnd, wp, lp);
			}
			break;
		case ID_TAB:
			switch (nmhdr->code) {
			case TCN_SELCHANGE:
				gui->onTabSelectChange( TabCtrl_GetCurSel(gui->hTabWnd) );
				break;
			}
			break;
		}
		break;
	case WM_COMMAND:
		switch(LOWORD(wp)){
		case ID_DELAYED_WM_SIZE:
			if(gui->hImgWnd){
				RECT rect;
				RECT windowRect;
				GetWindowRect(hWnd, &windowRect);
				GetWindowRect(gui->hConsole, &rect);
				MoveWindow(gui->hImgWnd, windowRect.left, rect.top, windowRect.right - windowRect.left, windowRect.bottom - rect.top, TRUE);
			}
			break;
		default:
			gui->onCommand(LOWORD(wp), lp);
			break;
		}
		break;
	case WM_ERASEBKGND:
		if(gui->cursorBlinkFlg == 2 || gui->cursorBlinkFlg == 3){
			//ちらつき防止のため
			return 1;
		}
		return DefWindowProc(hWnd, msg, wp, lp);
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return 0;
}


