#ifndef CASTER_CHARDAT
#define CASTER_CHARDAT

#if _MSC_VER >= 1300
#	include <windows.h>
#	define DIRECTINPUT_VERSION 0x0700//DirectX7 ?? DirectInput???g?p
#	include <dinput.h>
#	include <iostream>
#else
#	include <windows.h>
#	include <dinput.h>
#	include <iostream.h>
#endif

#define key_up 1
#define key_down 2
#define key_left 4
#define key_right 8
#define key_A 16
#define key_B 32
#define key_C 64
#define key_D 128
#define key_P 255

typedef struct{
    LONG X;
    LONG Y;
    LONG Z;
    LONG Rx;
    LONG Ry;
    LONG Rz;
    LONG S1;
    LONG S2;
    LONG P1;
    LONG P2;
    LONG P3;
    LONG P4;
    BYTE Button1;
    BYTE Button2;
    BYTE Button3;
    BYTE Button4;
    BYTE Button5;
    BYTE Button6;
    BYTE Button7;
    BYTE Button8;
    BYTE Button9;
    BYTE Button10;
    BYTE Button11;
    BYTE Button12;
    BYTE Button13;
    BYTE Button14;
    BYTE Button15;
    BYTE Button16;
    BYTE Button17;
    BYTE Button18;
    BYTE Button19;
    BYTE Button20;
    BYTE Button21;
    BYTE Button22;
    BYTE Button23;
    BYTE Button24;
    BYTE Button25;
    BYTE Button26;
    BYTE Button27;
    BYTE Button28;
    BYTE Button29;
    BYTE Button30;
    BYTE Button31;
    BYTE Button32;
} joyStatusFormat;

class charDatClass{
private:

	BYTE keyIniUp;
	BYTE keyIniDown;
	BYTE keyIniLeft;
	BYTE keyIniRight;
	BYTE keyIniA;
	BYTE keyIniB;
	BYTE keyIniC;
	BYTE keyIniD;
	BYTE keyIniP;

	joyStatusFormat joyStatus;
	BYTE	keyStatus[256];
	BYTE* Button;
	int inputBufBody[7];

	LPDIRECTINPUT di;
	LPDIRECTINPUTDEVICE2 device;
	int isPolledDevice;

	HWND hWnd;

	int init_internal();

public:
	int inputBufChar[7];
	
	charDatClass();
	~charDatClass();
	int	init();
	int init_if_kbd();
	int init_simple_kbd();
	int	diInit();
	BYTE	GetInput();
	int	SetInput(BYTE);
	int	SetBodyInput(BYTE);
	int	SetCharInput(BYTE);
	void	end();

	GUID guid;
	BYTE inputDeviceType;
	WORD enumCounter;
	WORD playerSide;

	WORD	diFlg;
	WORD* th075Flg;
	HANDLE* hProcess;
	char *windowName;
};

#endif
