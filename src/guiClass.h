#ifndef GUI_CLASS
#define GUI_CLASS

#include <windows.h>
#include "messageClass.h"

namespace N_Gui{

//CONFIG
//#define ESCAPE_KEY_QUIT
//#define TAB_CYCLE
#define ENABLE_LOBBY_KEY_INPUT


#define BASE_STR_SIZE 2048
#define EDIT_STR_SIZE 50
#define BLINK_STR_SIZE 10
#define MAX_LINE_NUM  20
#define TAB_WINDOW_HEIGHT 30
#define LINE_HEIGHT   20
//おおよそ
#define MIN_LINE_NUM  5
#define RETURN_INTERVAL 1000

#define MAX_CHAT_LINE 200
#define LOG_BUF_SIZE 10240

#define WINDOW_TITLE             L"RollCaster"
#define TH075TOOL_EXE            L"th075tool.exe"
#define STATUS_LABEL_WAIT_ACCESS L"Wait"
#define STATUS_LABEL_HOST_WAIT   L"HostWait"
#define STATUS_LABEL_IDLE        L"Idle"
#define STATUS_LABEL_SETTING     L"Setting"
#define STATUS_LABEL_FIGHT       L"Fight"
#define STATUS_LABEL_WATCH       L"Watch"
#define STATUS_LABEL_AFK         L"AFK"
#define STATUS_LABEL_CLIENT_WAIT L"ClientWait"

#define LOG_SYSTEM_NAME          L"System"


#define INPUT_HISTORY_NUM 20
#define INPUT_HISTORY_BUF_SIZE 100


#define EDITOR_TEXT_COLOR 0xFFFFFF
#define EDITOR_BG_COLOR   BLACK_BRUSH
#define BACKGROUND_COLOR RGB(250,250,250)

#define CONSOLE_LEFT_WIDTH   300
#define LOBBY_DETAIL_WIDTH   400
#define LOBBY_LIST_WIDTH     500
#define LOBBY_LIST_HEIGHT    300
#define LOBBY_CHAT_HEIGHT    200
#define LOBBY_EDIT_HEIGHT     70
#define LOBBY_BUTTON_AREA_HEIGHT 50
#define LOBBY_BUTTON_WIDTH_L  80
#define LOBBY_BUTTON_WIDTH    80
#define LOBBY_BUTTON_HEIGHT_L 32
#define LOBBY_BUTTON_HEIGHT   25
#define LOBBY_BUTTON_PADDING   8

enum{
	TAB_CONSOLE = 0,
	TAB_LOG,
	TAB_LOBBY,
	TAB_SETTINGS,
	TAB_MAX
};

enum{
	ID_TAB = 2000,
	ID_STATUS,
	ID_CONSOLE,
	ID_LOG_CONSOLE,
	ID_LOG_CHAT,
	ID_LOBBY_LIST,
	ID_LOBBY_DETAIL,
	ID_LOBBY_CHAT,
	ID_LOBBY_EDIT,
	ID_BTTN_WAIT,
	ID_BTTN_HOSTWAIT,
	ID_BTTN_CONNECT,
	ID_BTTN_SPECTATE,
	ID_BTTN_TH075TOOL,
	ID_BTTN_CANCEL,
	ID_BTTN_SEND,
	ID_BTTN_DISCONNECT,
	ID_BTTN_SAVERESTART,
	ID_BTTN_SUICA,
	ID_BTTN_AFK,
	ID_BTTN_BACKGROUND,
	ID_SETTINGS,
	ID_LOBBY_LIST_REFRESH,
	ID_LOBBY_CHAT_REFRESH,
	ID_SET_FOCUS_TO_CONSOLE,
	ID_SEND_CHAT_MESSAGE,
	ID_NOTICE_CASTER_GOTO_MENU,
	ID_NOTICE_CASTER_GOTO_DELAY_INPUT,
	ID_NOTICE_CASTER_GOTO_DELAY_WAIT,
	ID_NOTICE_CASTER_GOTO_BATTLE,
	ID_NOTICE_CASTER_GOTO_SPECTATE,
	ID_DELAYED_WM_SIZE,
	ID_MAX
};


class guiClass{
public:
	guiClass(HINSTANCE);
	~guiClass();
	int init();
	
	//Console
	void reflesh();
	void reflesh(bool);
	void addBaseStr(WCHAR*);
	void addBaseChar(WCHAR);
	void delBaseStr();
	void delBaseStr(int);
	void addEditChar(WCHAR);
	void addEditStr(WCHAR*);
	void clearEditStr();
	void setEditStr(WCHAR*);
	void addInputStr(WCHAR*);
	void clearInputStr();
	void doBackSpace();
	void doReturn();
	
	//window
	void onTabSelectChange(int);
	void ChangeLogWindowStatus(int);
	void ChangeLobbyWindowStatus(int);
	void ChangeSettingsWindowStatus(int);
	
	void onCreate(HWND, UINT, WPARAM, LPARAM);
	void onWindowSizeChange();
	void onCommand(int);
	void onCommand(int, LPARAM);
	void onKeyDown(WPARAM);
	void onCharInput(WPARAM);
	
	void onBttnPressWait();
	void onBttnPressHostWait();
	void onBttnPressConnect();
	void onBttnPressSpectate();
	void onBttnPressTh075Tool();
	void onBttnPressCancel();
	void onBttnPressSend();
	void onBttnPressSuica();
	void onBttnPressDisconnect();
	void onBttnPressSaveRestart();
	void onBttnPressAFK();
	
	void onLobbyListRefresh();
	void onLobbyChatRefresh();
	void onPlayerListSelect();
	void onPlayerListDoubleClick();
	LRESULT onLobbyListCustomDraw(HWND, WPARAM, LPARAM);
	LRESULT onLobbyChatCustomDraw(HWND, WPARAM, LPARAM);
	
	void ShowLobbyConnectDialog();
	void onSettingsSave();
	
	HINSTANCE hInst;
	HWND hWnd;
	HWND hImgWnd;
	HWND hConsole;
	HWND hLobbyList;
	HWND hLobbyDetail;
	HWND hLobbyChat;
	HWND hLobbyEdit;
	HWND hBttnWait;
	HWND hBttnHostWait;
	HWND hBttnConnect;
	HWND hBttnSpectate;
	HWND hBttnAFK;
	HWND hBttnCancel;
	HWND hBttnSend;
	HWND hBttnDisconnect;
	HWND hBttnSuica;
	HWND hBttnTh075Tool;
	HWND hBttnSaveRestart;
	HWND hLogConsole;
	HWND hLogChat;
	HWND hSettings;
	HWND hTabWnd;
	HWND hStatusWnd;
	HWND hDisconnect;
	HWND hSaveRestart;
	HWND hBttnBackground;
	
	HANDLE hBackgroundThread;
	HINSTANCE hRtLib;
	
	WCHAR baseStr[BASE_STR_SIZE];
	WCHAR editStr[EDIT_STR_SIZE];
	WCHAR inputStr[EDIT_STR_SIZE];
	WCHAR blinkStr[BLINK_STR_SIZE];
	
	//guiIO
	int GetKeyStateReplace(int);
	
	WCHAR inputHistory[INPUT_HISTORY_NUM][INPUT_HISTORY_BUF_SIZE];
	int  inputHistoryPosition;
	
	int continueFlg;
	int inputBlockFlg;
	bool blinkStatus;
	int lineCount;
	int editAreaHeight;
	int maxLineCount;
	int doReturnFlg;
	int cursorType;
	int consoleMode;
	int pressEscapeFlg;
	int autoLobbyFlg;
	int imgAlpha;
	int imgScaleDownFlg;
	WCHAR imgFileName[200];
	int windowWidth;
	int windowHeight;
	int linuxPressEscapeFlg;
	int settingsColorFlg;
	WCHAR suicaAccName[100];
	WCHAR suicaURL[100];
	WCHAR* consoleLogBuf;
	int consoleLogPosition;
	WCHAR* chatLogBuf;
	int chatLogPosition;
	int cursorBlinkFlg;
	int windowSizeChangeEvent;
	int wineFlg;
	int chatEnterKeyFlg;
	int windowPositionFlg;
	int windowPositionX;
	int windowPositionY;
	
	messageClass message;
	HFONT hFont;
};
extern guiClass* g_gui;

}

#endif