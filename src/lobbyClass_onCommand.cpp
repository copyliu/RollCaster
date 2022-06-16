#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <process.h>
#include "lobbyClass.h"
#include "guiClass.h"
#include "casterClass.h"
#include "guiIO.h"
#include "messageClass.h"
#include "mutexClass.h"
using namespace std;
using namespace N_Gui;


/********************************************************
 * MISC
 ********************************************************/
const WCHAR PUBLIC_IMUID[] = L"f1d3ff8443297732862df21dc4e57262";


/********************************************************
 * EVENT
 ********************************************************/
int lobbyClass::onBttnPressWait(){
	if(g_caster && g_caster->isFighting()){
		g_gui->message.write(L"Conflict action.");
		return 1;
	}
	changeStatus(LOBBY_CLIENT_STATUS_WAIT);
	return 0;
}

int lobbyClass::onBttnPressHostWait(){
	if(g_caster && g_caster->isFighting()){
		g_gui->message.write(L"Conflict action.");
		return 1;
	}
	changeStatus(LOBBY_CLIENT_STATUS_HOST_WAIT);
	return 0;
}

int lobbyClass::onBttnPressAFK(){
	if(g_caster && g_caster->isFighting()){
		g_gui->message.write(L"Conflict action.");
		return 1;
	}
	changeStatus(LOBBY_CLIENT_STATUS_AFK);
	return 0;
}

int lobbyClass::onBttnPressConnect(WCHAR* UID){
	if(!isRunning()){
		return 1;
	}
	
	if(g_caster && g_caster->isFighting()){
		g_gui->message.write(L"Conflict action.");
		return 1;
	}
	
	myString targetUID(UID, 32);
	lobbyUserSubClass target;
	
	
	if(user.getUserInfo(UID, &target) != 0){
		g_gui->message.write(L"Selected UID not found.");
		return 1;
	}
	
	memset(connectingUID, 0, sizeof(connectingUID));
	wcscpy(connectingUID, UID);
	
	if( target.state == STATE_GATHERING ||
	    target.state == STATE_HOST_GATHERING){
		
//		if(status == LOBBY_CLIENT_STATUS_WAIT || status == LOBBY_CLIENT_STATUS_HOST_WAIT || status == LOBBY_CLIENT_STATUS_CLIENT_WAIT){
//			g_gui->message.write(L"Conflict action. Connect action will be ignored in a waiting state.");
//			return 1;
//		}
		changeStatus(LOBBY_CLIENT_STATUS_IDLE);
		
		//�z�X�g�҂��̓�����
		if(target.state == STATE_HOST_GATHERING){
			changeStatus(LOBBY_CLIENT_STATUS_CLIENT_WAIT);
//			this->status = LOBBY_CLIENT_STATUS_CLIENT_WAIT;
		}
		
		//�ΐ�\����
		int bufSize = 2 + targetUID.remoteSize + 1;
		BYTE* ccc = new BYTE[ bufSize ];
		outputBufferClass dos(ccc, bufSize);
		
		dos.writeUTF(&targetUID);
		dos.writeByte((BYTE)POST_FIGHT);
		
		SendCmd( POST_CLIENT_TO_CLIENT_COMMAND, ccc, dos.nowData );
		delete[] ccc;
	}else if(target.state == STATE_FIGHTING || target.state == STATE_WATCHING){
		//return onBttnPressSpectate(UID);
		//�������Ĉ�a�����邽�ߒʏ�t���[�Őڑ� 
		WCHAR destIP[30];
		memset(destIP, 0, sizeof(destIP));
		convIP(target.IP, destIP);
		if(g_caster){
			g_caster->reset(CASTER_ARG_MODE_CONNECT, destIP, target.port);
		}
		if(g_gui){
			PostMessage(g_gui->hWnd, WM_COMMAND, ID_SET_FOCUS_TO_CONSOLE, 0);
		}
	}else{
		g_gui->message.write(L"Selected player is not waiting");
		return 1;
	}
	
	return 0;
}

int lobbyClass::onBttnPressSpectate(WCHAR* UID){
	if(!isRunning()){
		return 1;
	}
	
	if(g_caster && g_caster->isFighting()){
		g_gui->message.write(L"Conflict action.");
		return 1;
	}
	
	lobbyUserSubClass target;
	if(user.getUserInfo(UID, &target) != 0){
		g_gui->message.write(L"Selected UID not found");
		return 1;
	}
	
	WCHAR destIP[30];
	memset(destIP, 0, sizeof(destIP));
	convIP(target.IP, destIP);
	
	if(g_caster){
		g_caster->reset(CASTER_ARG_MODE_SPECTATE, destIP, target.port);
	}
	
	return 0;
}

int lobbyClass::onBttnPressSend(WCHAR* messageStr){
	if(!isRunning()){
		return 1;
	}
	myString message(messageStr, wcslen(messageStr));
	myString id((WCHAR*)PUBLIC_IMUID, 0);
	
	int size = message.remoteSize;
	BYTE* buffer = new BYTE[size + 100];
	outputBufferClass dos(buffer, size + 100);
	
	BYTE color = 0;
	
	dos.writeUTF(&id);
	dos.writeUTF(&message);
	dos.writeByte(color);
	
	SendCmd(POST_INSTANTMESSAGE, dos.data, dos.nowData);
	
	delete[] buffer;
	return 0;
}

int lobbyClass::onBttnPressCancel(){
	changeStatus(LOBBY_CLIENT_STATUS_IDLE);
	return 0;
}

int lobbyClass::onNoticeCasterGotoMenu(){
	switch(status){
	case LOBBY_CLIENT_STATUS_FIGHT:
	case LOBBY_CLIENT_STATUS_SPECTATE:
	case LOBBY_CLIENT_STATUS_SETTING:
		changeStatus(LOBBY_CLIENT_STATUS_IDLE);
		break;
	case LOBBY_CLIENT_STATUS_AFK:
		//DO NOTHING
		break;
	}
	return 0;
}

int lobbyClass::onNoticeCasterGotoDelayInput(int delayParam){
	if(g_caster && g_caster->isLobbyControl()){
		WCHAR message[200];
		memset(message, 0, sizeof(message));
		int delay = delayParam / 10;
		int delaySub = delayParam % 10;
		swprintf(message, L"Please input buffer margin value. Observed delay is about %d.%d[ms] in a round.", delay, delaySub);
		g_gui->message.write(message);
	}
	return onNoticeCasterGotoBattle();
}

int lobbyClass::onNoticeCasterGotoDelayWait(){
	return onNoticeCasterGotoBattle();
}

int lobbyClass::onNoticeCasterGotoBattle(){
	switch(status){
	case LOBBY_CLIENT_STATUS_IDLE:	//���r�[�O�ΐ���ΐ풆�ɂ���
	case LOBBY_CLIENT_STATUS_WAIT:
	case LOBBY_CLIENT_STATUS_HOST_WAIT:	//�O�̂���
	case LOBBY_CLIENT_STATUS_CLIENT_WAIT:
	case LOBBY_CLIENT_STATUS_SETTING:
		changeStatus(LOBBY_CLIENT_STATUS_FIGHT);
		break;
	case LOBBY_CLIENT_STATUS_AFK:
		//�v����
//		changeStatus(LOBBY_CLIENT_STATUS_FIGHT);
		break;
	}
	return 0;
}

int lobbyClass::onNoticeCasterGotoSpectate(){
	switch(status){
	case LOBBY_CLIENT_STATUS_IDLE:	//���r�[�O�ł̊ϐ���ϐ풆�ɂ���
	case LOBBY_CLIENT_STATUS_SETTING:
		changeStatus(LOBBY_CLIENT_STATUS_SPECTATE);
		break;
	case LOBBY_CLIENT_STATUS_AFK:
		//�v����
//		changeStatus(LOBBY_CLIENT_STATUS_SPECTATE);
		break;
	}
	return 0;
}

/********************************************************
 * Get Status Label from Player Status (not innner status)
 ********************************************************/
WCHAR* lobbyClass::getStatusLabel(BYTE status){
	switch(status){
	case STATE_WAITING:
		return STATUS_LABEL_IDLE;
	case STATE_GATHERING:
		return STATUS_LABEL_WAIT_ACCESS;
	case STATE_SETTING:
		return STATUS_LABEL_SETTING;
	case STATE_FIGHTING:
		return STATUS_LABEL_FIGHT;
	case STATE_WATCHING:
		return STATUS_LABEL_WATCH;
	case STATE_LEAVING:
		return STATUS_LABEL_AFK;
	case STATE_HOST_GATHERING:
		return STATUS_LABEL_HOST_WAIT;
	case STATE_HOST_GATHERING_WAIT:
		return STATUS_LABEL_CLIENT_WAIT;
	default:
		break;
	}
	return L"INVALID";
}

/********************************************************
 * Change Status
 ********************************************************/
int lobbyClass::changeStatus( WORD statusTemp ){
	if( status == LOBBY_CLIENT_STATUS_ERROR || status == LOBBY_CLIENT_STATUS_DEFAULT ){
		return 1;
	}
	
	//TODO
	//Notice�Œʒm����ق����悳����
	guiClass* gui = g_gui;
	
	//status��ς���
	BYTE state;
	switch( statusTemp ){
	case LOBBY_CLIENT_STATUS_IDLE :
		//�܂���킵�� 
		state = STATE_WAITING;
		if(gui){
			SetWindowText(gui->hStatusWnd, getStatusLabel(state));
		}
		break;
	case LOBBY_CLIENT_STATUS_WAIT :
		state = STATE_GATHERING;
		if(gui){
			SetWindowText(gui->hStatusWnd, getStatusLabel(state));
		}
		break;
	case LOBBY_CLIENT_STATUS_FIGHT :
		state = STATE_FIGHTING;
		if(gui){
			SetWindowText(gui->hStatusWnd, getStatusLabel(state));
		}
		break;
	case LOBBY_CLIENT_STATUS_SPECTATE :
		state = STATE_WATCHING;
		if(gui){
			SetWindowText(gui->hStatusWnd, getStatusLabel(state));
		}
		break;
	case LOBBY_CLIENT_STATUS_SETTING :
		state = STATE_SETTING;
		if(gui){
			SetWindowText(gui->hStatusWnd, getStatusLabel(state));
		}
		break;
	case LOBBY_CLIENT_STATUS_HOST_WAIT :
		state = STATE_HOST_GATHERING;
		if(gui){
			SetWindowText(gui->hStatusWnd, getStatusLabel(state));
		}
		break;
	case LOBBY_CLIENT_STATUS_CLIENT_WAIT :
//		state = STATE_HOST_GATHERING_WAIT;
//		if(gui){
//			SetWindowText(gui->hStatusWnd, getStatusLabel(state));
//		}
		status = LOBBY_CLIENT_STATUS_CLIENT_WAIT;
		return 0;
	case LOBBY_CLIENT_STATUS_AFK:
		state = STATE_LEAVING;
		if(gui){
			SetWindowText(gui->hStatusWnd, getStatusLabel(state));
		}
		break;
	case LOBBY_CLIENT_STATUS_ERROR:
		if(gui){
			SetWindowText(gui->hStatusWnd, L"ERROR");
		}
		status = LOBBY_CLIENT_STATUS_ERROR;
		return 0;
	default :
		//none
		return 1;
	}
	
	SendCmd( MODIFY_STATE, state );
	status = statusTemp;
	
	return 0;
}

