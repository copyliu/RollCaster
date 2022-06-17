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
 * Player List Color Change
 *
 * Refs http://home.a00.itscom.net/hatada/windows/commctrl/listview01.html
 ********************************************************/
#define CDDS_SUBITEMPREPAINT (CDDS_SUBITEM | CDDS_ITEMPREPAINT)
LRESULT guiClass::onLobbyListCustomDraw(HWND hWndParam, WPARAM wp, LPARAM lp){
	NMLVCUSTOMDRAW *plvcd = (NMLVCUSTOMDRAW*)lp;
	
	switch (plvcd->nmcd.dwDrawStage) {
	case CDDS_PREPAINT:
		return CDRF_NOTIFYITEMDRAW;
	case CDDS_ITEMPREPAINT:
		return CDRF_NOTIFYSUBITEMDRAW; 
	case CDDS_SUBITEMPREPAINT:
		if (plvcd->iSubItem == 2){
			WCHAR status[20];
			::memset(status, 0, sizeof(status));
			ListView_GetItemText(hLobbyList, plvcd->nmcd.dwItemSpec, 2, status, _countof(status) -1);
			
			if((::wcscmp(status, STATUS_LABEL_WAIT_ACCESS) == 0) || (::wcscmp(status, STATUS_LABEL_HOST_WAIT) == 0)){
				plvcd->clrText = RGB(255, 0, 0);
			}else if(::wcscmp(status, STATUS_LABEL_FIGHT) == 0){
				plvcd->clrText = RGB(0, 100, 0);
			}else{
				plvcd->clrText = RGB(0, 0, 255);
			}
		}else{
			plvcd->clrText = RGB(0, 0, 0);
		}
		return CDRF_NEWFONT; 
	}
	return ::DefWindowProc(hWndParam, WM_NOTIFY, wp, lp);
}

/********************************************************
 * Player Chat Color Change
 ********************************************************/
LRESULT guiClass::onLobbyChatCustomDraw(HWND hWndParam, WPARAM wp, LPARAM lp){
	NMLVCUSTOMDRAW *plvcd = (NMLVCUSTOMDRAW*)lp;
	
	switch (plvcd->nmcd.dwDrawStage) {
	case CDDS_PREPAINT:
		return CDRF_NOTIFYITEMDRAW;
	case CDDS_ITEMPREPAINT:
//		return CDRF_NOTIFYSUBITEMDRAW; 
//	case CDDS_SUBITEMPREPAINT:
		{
			WCHAR name[100];
			::memset(name, 0, sizeof(name));
			ListView_GetItemText(hLobbyChat, plvcd->nmcd.dwItemSpec, 0, name, _countof(name) -1);
			
			if(::wcscmp(name, LOG_SYSTEM_NAME) == 0){
				plvcd->clrText = RGB(0, 0, 255);
			}else{
				plvcd->clrText = RGB(0, 0, 0);
			}
		}
		return CDRF_NEWFONT; 
	}
	return ::DefWindowProc(hWndParam, WM_NOTIFY, wp, lp);
}

/********************************************************
 * Insert player info to player list
 ********************************************************/
//TODO
void insertUserItem(lobbyUserSubClass* user, HWND hLobbyList){
	LV_ITEM item;
	item.mask = LVIF_TEXT;
	item.iItem = 0;
	
	item.pszText = user->name;
	item.iSubItem = 0;
	ListView_InsertItem(hLobbyList, &item);
	
	item.pszText = user->UID;
	item.iSubItem = 1;
	ListView_SetItem(hLobbyList, &item);
	
	item.pszText = lobbyClass::getStatusLabel(user->state);
	item.iSubItem = 2;
	ListView_SetItem(hLobbyList, &item);
	
	WCHAR IP[20];
	::memset(IP, 0, sizeof(IP));
	convIP(user->IP, IP);
	
	item.pszText = IP;
	item.iSubItem = 3;
	ListView_SetItem(hLobbyList, &item);

	item.pszText = user->shortMessage;
	item.iSubItem = 4;
	ListView_SetItem(hLobbyList, &item);
}

/********************************************************
 * 
 ********************************************************/
void guiClass::onLobbyListRefresh(){
	lobbyClass* lobby = g_lobby;
	//TODO
	//暫定対応でいったんリストをクリアする
	//たぶんフォーカスが外れる
	ListView_DeleteAllItems(hLobbyList);
	
	lobbyUserSubClass* user;
	
	//対戦待ちを先頭に表示する
	//TODO
	for(int Counter=0; Counter < LOBBY_USER_MAX; Counter++){
		user = &(lobby->user.user[Counter]);
		if(user->port && user->state != STATE_GATHERING && user->state != STATE_HOST_GATHERING){
			insertUserItem(user, hLobbyList);
		}
	}
	
	for(int Counter=0; Counter < LOBBY_USER_MAX; Counter++){
		user = &(lobby->user.user[Counter]);
		if(user->port && user->state == STATE_HOST_GATHERING){
			insertUserItem(user, hLobbyList);
		}
	}
	
	for(int Counter=0; Counter < LOBBY_USER_MAX; Counter++){
		user = &(lobby->user.user[Counter]);
		if(user->port && user->state == STATE_GATHERING){
			insertUserItem(user, hLobbyList);
		}
	}
//	ListView_EnsureVisible(hLobbyChat, current, false );
}

/********************************************************
 * 
 ********************************************************/
void guiClass::onLobbyChatRefresh(){
	message.flush(hLobbyChat);
}

/********************************************************
 * 
 ********************************************************/
void guiClass::onPlayerListDoubleClick(){
	onBttnPressConnect();
}

/********************************************************
 * 
 ********************************************************/
void guiClass::onPlayerListSelect(){
	lobbyClass* lobby = g_lobby;
	
	int n = ListView_GetNextItem(hLobbyList, -1, LVNI_ALL | LVNI_SELECTED);
	if(n == -1){
		return;
	}
	
	WCHAR UID[40];
	::memset(UID, 0, sizeof(UID));
	ListView_GetItemText(hLobbyList, n, 1, UID, _countof(UID) -1);
	
	lobbyUserSubClass user;
	lobby->user.getUserInfo(UID, &user);
	
	int nCounter = 0;
	int i;
	for(i=0; i < (int)::wcslen(user.longMessage); i++){
		if(user.longMessage[i] == '\n'){
			nCounter++;
		}
	}
	
	int convSize = ::wcslen(user.longMessage) + nCounter;
	WCHAR* convStr = new WCHAR[convSize + 1];
	WCHAR* nowPosition = convStr;
	for(i=0; i < (int)wcslen(user.longMessage); i++){
		if(user.longMessage[i] == '\n' && nCounter){
			*nowPosition = '\r';
			nowPosition++;
			nCounter--;
		}
		*nowPosition = user.longMessage[i];
		nowPosition++;
	}
	convStr[convSize] = 0;
	
	::SetWindowText(hLobbyDetail, convStr);
	delete[] convStr;
}


