#include "mainDatClass.h"
using namespace std;

//Ҫ��ӑ
//��Τ���inputData.init()����Ӌ���ФäƤ���

typedef struct{
	WORD mode;
	WORD targetPort;
	WORD enPort;
	char targetIP[80];
	HANDLE hPrintMutex;
	WORD boosterOnFlg;
	WORD origMenuFlg;

	WORD accessPort;
	char *accessIP;
	char *standbyIP;
}menuDataStruct;

static void Sound( mainDatClass* dat ){
	//sound
	if( dat->waveFlg ){
		if( lstrcmpW( dat->nowDir, L"fail" ) && lstrlenW( dat->nowDir ) < 280 ){
			WCHAR path[300];
			lstrcpyW( path ,dat->nowDir );
			lstrcatW( path, L"\\sound.wav\0" );
			if( !_waccess( path, 0 ) ){
				PlaySoundW( path, NULL, SND_ASYNC );
			}
		}
	}

	if( dat->beepFlg ){
		Beep(3000, 500);
	}
}

static unsigned long get_address(char *name, unsigned short *port) {
	char *colon = strchr(name, ':');
	if (colon) {
		*colon = 0;
		if (port) {
			*port = htons(atoi(colon+1));
		}
	} else {
		colon = 0;
	}

	unsigned long addr = inet_addr(name);
	if (addr != INADDR_NONE) {
		if (colon) {
			*colon = ':';
		}
		return addr;
	}

	struct hostent *hostdata;
	hostdata = gethostbyname(name);

	if (colon) {
		*colon = ':';
	}

	if (!hostdata) {
		return INADDR_NONE;
	}

	struct in_addr *in_addr = (struct in_addr *)hostdata->h_addr;

	return in_addr->s_addr;
}

static void check_clipboard(char *destString) {
	if (IsClipboardFormatAvailable(CF_TEXT) == 0) {
		return;
	}
	
	if (OpenClipboard(0) == 0) {
		return;
	}
	
	HGLOBAL mem = GetClipboardData(CF_TEXT);
	if (mem != 0) {
		const CHAR *string = (const CHAR *)GlobalLock(mem);
		if (string != 0) {
			int len = GlobalSize(mem);
			if (len > 0 && len < 79) {
				char *buf = new char[len + 1];
				char *p = buf;
				
				memcpy(buf, string, len);
				buf[len] = '\0';
				
				while (*p == ' ' || *p == '\t') {
					++p;
				}
				
				// Check if this matches the IP format.
				char *start = p;
				bool alpha = false;
				int dots = 0;
				int colons = 0;
				
				while (*p) {
					if (*p >= '0' && *p <= '9') {
						// number ok
					} else if ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '-') {
						alpha = true;
					} else if (*p == '.') {
						dots += 1;
					} else if (*p == ':') {
						colons += 1;
					} else {
						// invalid
						break;
					}
					
					++p;
				}
				
				if (!*p) {
					if (colons <= 1 && (alpha == true || dots == 3)) {
						strcpy(destString, start);
					}
				}
				
				delete[] buf;
			}
			
			GlobalUnlock(mem);
		}
	}
	
	CloseClipboard();
}

static bool read_int(int &dest) {
	char str[50];
	//cin.clear();
	cin.getline(str,50);

	if (cin.fail()) {
		return 1;
	}

	if (str[0] == '\0') {
		return 0;
	}

	int n = 0;
	do {
		if (str[n] < '0' || str[n] > '9') {
			return 1;
		}
		n++;
	} while (str[n] != '\0');
	dest = atoi(str);

	return 0;
}


int menu( menuDataStruct* menuData ){
	if( !menuData ) return 0xF;

	memset( menuData->targetIP, 0, sizeof( menuData->targetIP ) );

	WaitForSingleObject( menuData->hPrintMutex, INFINITE );
	cout << endl;
	
	if (menuData->origMenuFlg) {
		cout	<< "<�˵�>" << endl
			<< "��ʹ��ESC���������˵�. " << endl
			<< "0 : �˳�" << endl
			<< "1 : ������Ϸ" << endl
			<< "2 : ������Ϸ" << endl
			<< "3 : ��ս" << endl
			<< "4 : �㲥" << endl
			<< "5 : ��ȡ�Է���Ϣ" << endl
			<< "6 : ������Ϸ (����Ϊֹ)" << endl
			<< "7 : ��¼���м̷�����" << endl
			<< "8 : ���Զ˿�" << endl;
	} else {
		cout	<< "<�˵�>" << endl
			<< "��ʹ��ESC���������˵�. " << endl
			<< "0 : �˳�" << endl
			<< "1 : ������Ϸ" << endl
			<< "2 : ������Ϸ" << endl
			<< "3 : ��ս" << endl
			<< "4 : ����2P��Ϸ" << endl
			<< "5 : ��ȡ�Է���Ϣ" << endl
			<< "6 : ������Ϸ (����Ϊֹ)" << endl
			<< "7 : ��¼���м̷�����" << endl
			<< "8 : ���Զ˿�" << endl;
	}
	switch( menuData->boosterOnFlg ){
	case 0:
		cout << "9 : ��AI��ս" << endl;
		break;
	case 1:
		cout << "9 : ��AI��ս (AI �Զ�ѧϰ��)" << endl;
		break;
	default:
		cout << "9 : ����" << endl;
		break;
	}
	cout << "10: ��ϰģʽ" << endl;
	cout	<< "������ [" << menuData->mode << "] >";
	ReleaseMutex( menuData->hPrintMutex );

	//cin >> menuData->mode;
	int mode = menuData->mode;
	if( read_int(mode) || cin.fail() ){
		cin.clear();
		//cin.ignore(1024,'\n');
		menuData->mode = 0;
		//return 0xF;
		return 1;
	}
	menuData->mode = mode;

	//int dport = 0;

	switch( menuData->mode ){
	case 0 :
		return 0xF;
	case 1 :
		break;
	case 2 :
	case 3 :
	case 5 :
	case 6 :
	case 7 :
	case 8 :
		menuData->targetPort = menuData->enPort;

		cin.clear();
		//cin.ignore(1024,'\n');

		WaitForSingleObject( menuData->hPrintMutex, INFINITE );
		
		cout << "�������ַ [";

		if (menuData->mode == 7 || menuData->mode == 8) {
			cout << menuData->standbyIP;
		} else {
			check_clipboard(menuData->accessIP);
			cout << menuData->accessIP;
		}

		cout << "] >";
		ReleaseMutex( menuData->hPrintMutex );
		//cin >> setw(50) >> menuData->targetIP;
		cin.getline(menuData->targetIP, 80);
		if( cin.fail() ){
			cin.clear();
			//cin.ignore(1024,'\n');
			return 1;
		}
		if (menuData->targetIP[0] == '\0') {
			if (menuData->mode == 7 || menuData->mode == 8) {
				strcpy(menuData->targetIP, menuData->standbyIP);
			} else {
				strcpy(menuData->targetIP, menuData->accessIP);
			}
		}
		if (menuData->targetIP[0] == '\0') {
			return 1;
		}
		break;
	case 4 :
		break;
	case 9 :
		break;
	case 10 :
		break;
	case 11:
		break;
	default :
		return 1;
	}

	menuData->targetIP[79] = 0;
	return 0;
}

int mainDatClass::Entrance(){
	#if debug_mode_main
		WaitForSingleObject( hPrintMutex, INFINITE );
		cout << "debug : Entrance()" << endl;
		ReleaseMutex( hPrintMutex );
	#endif

	menuDataStruct menuData;
	menuData.mode = lastMode;
	menuData.hPrintMutex = hPrintMutex;
	menuData.enPort = enPort;
	menuData.boosterOnFlg = boosterOnFlg;
	menuData.origMenuFlg = origMenuFlg;

	menuData.accessPort = accessPort;
	menuData.accessIP = accessIP;
	menuData.standbyIP = standbyIP;

	roopFlg = 0;
	delayTime = 0;
	localDelayTime = 0;
	rewindAmount = 0;
	continueFlg = 1;
	boosterFlg = 0;
	newCWflg = 0;
	if (boosterOnFlg == 1) {
		boosterFlg = 1;
	}

	lastCharacterA = 0;
	lastCharacterB = 0;
	nPlayers = 0;

	//SOCADDR_IN�γ��ڻ�
	WaitForSingleObject( hMutex, INFINITE );
	memset( &Away , 0, sizeof(SOCKADDR_IN) );
	memset( &Root , 0, sizeof(SOCKADDR_IN) );
	memset( &Branch , 0, sizeof(SOCKADDR_IN) );
	memset( &subBranch , 0, sizeof(SOCKADDR_IN) );
	memset( &Access , 0, sizeof(SOCKADDR_IN) );

	memset( &Leaf[0] , 0, sizeof(SOCKADDR_IN) );
	memset( &Leaf[1] , 0, sizeof(SOCKADDR_IN) );
	memset( &Leaf[2] , 0, sizeof(SOCKADDR_IN) );
	memset( &Leaf[3] , 0, sizeof(SOCKADDR_IN) );

	memset( &Ready , 0, sizeof(SOCKADDR_IN) );
	memset( &Standby[0] , 0, sizeof(SOCKADDR_IN) );
	memset( &Standby[1] , 0, sizeof(SOCKADDR_IN) );
	ReleaseMutex( hMutex );

//	Sleep(100);
	if( inputData.init() ) return 0xF;


	memset( &myInfo, 0, sizeof(myInfo) );
	memset( &enInfo, 0, sizeof(enInfo) );
	memset( &dataInfo, 0, sizeof(dataInfo) );
	myInfo.terminalMode = mode_default;
	myInitFlg = 0;
	enInitFlg = 0;
	memset( &echoFlg, 0, sizeof(echoFlg) );
	memset( &lastTime, 0, sizeof(lastTime) );
	myInfo.phase = phase_default;
	dataInfo.phase = phase_default;
	myRandNo = 0;
	enRandNo = 0;
	myRand = 0;
	enRand = 0;
	delay = 0;
	toughModeFlg = 0;
	targetMode = 0;
	enCowCaster = 0;
	winsA = 0;
	roundsWonA = 0;
	winsB = 0;
	roundsWonB = 0;
	namesLocked = 0;
	lastGameTime = 0;
	totalGameTime = 0;

	stdbyPort[0] = 0;
	stdbyPort[1] = 0;
	readyPort = 0;
	playerSideHostFlg = 0;
	portSeekFlg = 0;
	
	rollbackOk = 0;
	
	hWnd = 0;

	memset( obsIP, 0, sizeof( obsIP ) );

	for (int i = 0; i < ((11*4*2)+2); ++i) {
		palettes[i].state = PALETTE_EMPTY;
	}
	paletteCounter = 0;

	strcpy(p1PlayerName, myPlayerName);
	strcpy(p1ShortName, myShortName);
	strcpy(p2PlayerName, "");
	strcpy(p2ShortName, "");
	p1TempName = 0;
	p2TempName = 0;
	if (!p1PlayerName[0]) {
		p1TempName = 1;
	}

	//srand
//	srand( (unsigned)startTime );
//	srand( (unsigned)time( NULL ) );

	//wait
	if( mainFlg == main_default && autoWaitFlg ){
		mainFlg = main_wait;
	}

	//main
	switch( mainFlg ){
	case main_default :
		{
			int Res;
			for(;;){
				Res = menu( &menuData );
				if( Res == 0xF ){
					if( endTimeFlg ){
						//�ڄӕr�g���ʾ
						cout	<< endl
							<< "<End>" << endl
							<< "Time : ";
						double allTime = difftime( time( NULL ), startTime );
						if( allTime > 60 * 60 ) cout << (DWORD)(( allTime / 60 ) / 60) << "h ";
						allTime = allTime - (DWORD)((allTime / 60) / 60) * 60 * 60;
						cout << (DWORD)(allTime / 60)  << "min" << endl;
						Sleep(500);
					}
					return 0xF;
				}
				if( Res == 0 ) break;
				if( Res == 1 ) return 1;
			}
			lastMode = menuData.mode;
		}
		break;
	case main_file :
		//������`�ɤ����֤�IP�ȥݩ`�Ȥ������i��
		{
			WCHAR path[200];
			if( GetCurrentDirectoryW( 180, path ) < 180 ){
				//ok
				path[180] = 0;
				lstrcatW( path, L"\\info.ini\0" );

				if( GetPrivateProfileIntW( L"MAIN", L"onoff", 0, path ) ){
					//on
					int mode = GetPrivateProfileIntW( L"MAIN", L"mode", 0, path );
					switch( mode ){
					case 0 :
					case 3:
					case 4:
						//ͨ��
						//�����äΥ�`�ɤ�׷��
						if (mode == 4) {
							menuData.mode = 23;
						} else if (mode == 3) {
							menuData.mode = 22;
						} else {
							menuData.mode = 20;
						}

						//����i���z��
						iniReadStringNonUnicode( L"TARGET", L"ip", L"\0", menuData.targetIP, 79, path );
						menuData.targetIP[79] = '\0';
						menuData.targetPort = GetPrivateProfileIntW( L"TARGET", L"port", 0, path );

						if( (menuData.targetIP[0] != '\0' || menuData.mode == 23) && menuData.targetPort != 0 ){
							//ok
						}else{
							return 0xF;
						}
						break;
					case 1 :
						//�ض����ִ���
						menuData.mode = 21;
						iniReadStringNonUnicode( L"TARGET", L"ip", L"\0", menuData.targetIP, 79, path );
						menuData.targetIP[79] = '\0';

						if( menuData.targetIP[0] != '\0' ){
							//ok
						}else{
							return 0xF;
						}
						cout	<<endl
							<< "<�˵�>" << endl;
						break;
					case 2 :
						//����
						menuData.mode = 1;
						cout	<<endl
							<< "<�˵�>" << endl;
						break;
					default :
						return 0xF;
					}
				}else{
					return 0xF;
				}
			}else{
				return 0xF;
			}
		}

		mainFlg = main_end;
		break;
	case main_arg :
		//����ָ��
		//file��`�ɤΤ�����
		switch( argData.argMode ){
		case 0 :
		case 3 :
		case 4 :
			if (argData.argMode == 4) {
				menuData.mode = 23;
			} else if (argData.argMode == 3) {
				menuData.mode = 22;
			} else {
				menuData.mode = 20;
			}

			//IP
			strcpy( menuData.targetIP, argData.targetIP );
			menuData.targetIP[79] = '\0';

			//port
			if( argData.targetPort ){
				menuData.targetPort = argData.targetPort;
			}else{
				menuData.targetPort = 7500;
			}

			if( menuData.targetIP[0] != '\0' || menuData.mode == 23 ){
				//ok
			}else{
				return 0xF;
			}

			break;
		case 1 :
			//�ض����ִ���
			menuData.mode = 21;
			strcpy( menuData.targetIP, argData.targetIP );
			menuData.targetIP[79] = '\0';

			if( menuData.targetIP[0] != '\0' ){
				//ok
			}else{
				return 0xF;
			}
			cout	<<endl
				<< "<�˵�>" << endl;
			break;
		case 2 :
			//����
			menuData.mode = 1;
			cout	<<endl
				<< "<�˵�>" << endl;
			break;
		default :
			return 0xF;
		}

		mainFlg = main_end;
		break;
	case main_end :
		//�K�ˤ����
		for(;;){
			if( GetEsc() ) break;
			if( !FindWindow( NULL , windowName ) ) break;
			Sleep(200);
		}
		return 0xF;
	case main_wait :
		cout	<<endl
			<< "<�˵�>" << endl;
		menuData.mode = 1;
		break;
	default :
		return 0xF;
	}

	nowTime = timeGetTime();

	myInfo.terminalMode = mode_access;

	bool skipSpecQuestion = 0;

	switch( menuData.mode ){
	case 1 :
		//Wait for access
		if (origMenuFlg) {
			cout << "�ȴ��������..." << endl;
		} else {
			cout << "�ȴ��������..." << endl;
		}

		if( autoWaitOnFlg ){
			autoWaitFlg = 1;
		}

		myInfo.terminalMode = mode_wait;
		accessFlg = status_default;
		for(;;){
			if( accessFlg == status_ok ) break;
			if( accessFlg == status_error ) return 1;
			if( accessFlg == status_bad ) return 1;
			if( GetEsc() ){
				autoWaitFlg = 0;
				if( mainFlg == main_wait ){
					mainFlg = main_default;
				}

				return 1;
			}
			Sleep(100);
		}
		
		if (origMenuFlg) {
			cout << "���IP ";
		} else {
			cout << "���IP ";
		}
		
		cout << inet_ntoa( Away.sin_addr ) << ":" << dec << ntohs(Away.sin_port) << " ������" << endl;
		myInfo.terminalMode = mode_root;

		Sound( this );

		//playerSideHost
		playerSideHostFlg = 1;

		break;
	case 6 :
		//Tough
		toughModeFlg = 1;
	case 3 :
		if (menuData.mode == 3) {
			// Spectate
			lastTime.Access = nowTime;

			Access.sin_family = AF_INET;
			Access.sin_port = htons( menuData.targetPort );
			Access.sin_addr.s_addr = get_address(menuData.targetIP, &Access.sin_port);
			if( !Access.sin_addr.s_addr || Access.sin_addr.s_addr == 0xFFFFFFFF ){
				return 1;
			}
			strcpy(accessIP, menuData.targetIP);

			accessPort = ntohs(Access.sin_port);

			cout << "�ȴ� " << inet_ntoa(Access.sin_addr) << ":" << dec << accessPort << " ��Ϸ��ʼ" << endl;

			lastTime.Access = nowTime;
			dataInfo.phase = 0xFF;
			int counter = 9;
			for(;;){
				counter++;
				if (counter == 10) {
					SendCmd( dest_access, cmd_dataInfo );
					counter = 0;
				}

				Sleep(100);

				if( dataInfo.phase == phase_battle || dataInfo.phase == phase_read || dataInfo.phase == phase_menu ) {
					break;
				}

				if( !(Access.sin_addr.s_addr) ){
					return 1;
				}
				if( GetEsc() ) return 1;
			}

			Sound( this );

			toughModeFlg = 0;
			skipSpecQuestion = 1;
		}
	case 2 :
		//Try access

		//��Τ��ᣨҪ��ӑ����
		lastTime.Access = nowTime;

		if (menuData.mode != 3) {
			Access.sin_family = AF_INET;
			Access.sin_port = htons( menuData.targetPort );
			Access.sin_addr.s_addr = get_address(menuData.targetIP, &Access.sin_port);
			if( !Access.sin_addr.s_addr || Access.sin_addr.s_addr == 0xFFFFFFFF ){
				return 1;
			}
			strcpy(accessIP, menuData.targetIP);

			accessPort = ntohs( Access.sin_port);
		}

		cout << "�������ӵ� " << inet_ntoa(Access.sin_addr) << ":" << dec << accessPort << endl;

		lastTime.Access = nowTime;
		echoFlg.Access = 0;
		for(;;){
			SendCmd( dest_access, cmd_echo, (void *)cowcaster_id, 5 );
			Sleep(100);
			if( echoFlg.Access ) break;
			if( !(Access.sin_addr.s_addr) ) return 1;
			if( GetEsc() ) return 1;
		}
		break;
	case 4 :
		//Broadcast
		myInfo.terminalMode = mode_broadcast;

		iniReadStringNonUnicode( L"PLAYER", L"p2Name", L"", p2PlayerName, 16, iniPath );
		p2PlayerName[15] = '\0';
		cleanString(p2PlayerName, 1);

		iniReadStringNonUnicode( L"PLAYER", L"p2ShortName", L"", p2ShortName, 5, iniPath );
		p2ShortName[4] = '\0';
		cleanString(p2ShortName, 1);

		if (!p2PlayerName[0]) {
			strcpy(p2PlayerName, "Player 2");
		}

		break;
	case 5 :
		//Get Information
		//��Τ��ᣨҪ��ӑ����
		lastTime.Access = nowTime;

		Access.sin_family = AF_INET;
		Access.sin_port = htons( menuData.targetPort );
		Access.sin_addr.s_addr = get_address(menuData.targetIP, &Access.sin_port);
		if( !Access.sin_addr.s_addr || Access.sin_addr.s_addr == 0xFFFFFFFF ){
			return 1;
		}
		strcpy(accessIP, menuData.targetIP);

		accessPort = ntohs( Access.sin_port);

		cout << "���ڲ�ѯ " << inet_ntoa(Access.sin_addr) << ":" << dec << accessPort << " ����Ϸ״̬" << endl;

		lastTime.Access = nowTime;

		{
			dataInfo.phase = 0xFF;
			for(;;){
				SendCmd( dest_access, cmd_dataInfo, (void *)cowcaster_id, 5 );
				Sleep(200);
				if( dataInfo.phase != 0xFF ) break;
				if( !(Access.sin_addr.s_addr) ){
					return 1;
				}
				if( GetEsc() ) return 1;
			}

			float delayTemp = 0;
			if( GetDelay( dest_access, &delayTemp) ) return 1;

			cout.setf(ios::fixed, ios::floatfield);
			cout	<< endl
				<< "<״̬>" << endl;
			if (origMenuFlg) {
				switch( dataInfo.terminalMode ){
				case mode_root :
					cout << "ģʽ : ���ڵ�" << endl;
					break;
				case mode_broadcast :
					cout << "ģʽ : �㲥�ڵ�" << endl;
					break;
				case mode_branch :
					cout << "ģʽ : ���νڵ�" << endl;
					break;
				case mode_subbranch :
					cout << "ģʽ : ĩ�˽ڵ�" << endl;
					break;
				}
			} else {
				switch( dataInfo.terminalMode ){
				case mode_root :
					cout << "ģʽ : ���ڵ�" << endl;
					break;
				case mode_broadcast :
					cout << "ģʽ : �㲥�ڵ�" << endl;
					break;
				case mode_branch :
					cout << "ģʽ : ���νڵ�" << endl;
					break;
				case mode_subbranch :
					cout << "ģʽ : ĩ�˽ڵ�" << endl;
					break;
				}
			}
			
			cout << "�ӳ� : Լ " << setprecision( 1 ) << delayTemp << "[ms]" << endl;

			if (dataInfo.terminalMode == mode_root) {
				if (dataInfo.playerSide == 0xA) {
					cout << "λ�� : 1P" << endl;
				} else if (dataInfo.playerSide == 0xB) {
					cout << "λ�� : 2P" << endl;
				}
			}

			if (enCowCaster >= 2) {
				if (hasRemote) {
					cout << "Զ��IP : " << inet_ntoa(Remote.sin_addr) << ":" << dec << ntohs(Remote.sin_port) << endl;
				}
			}
			if (enCowCaster >= 3) {
				if (nPlayers == 1) {
					if (p1ShortName[0] && p1PlayerName[0]) {
						cout << "��� : [" << p1ShortName << "] " << p1PlayerName << endl;
					}
				} else if (nPlayers == 2) {
					cout << "��� : ";

					if (p1ShortName[0] || p1PlayerName[0]) {
						cout << "[" << p1ShortName << "] " << p1PlayerName;
					} else {
						cout << "δ֪";
					}
					cout << " vs ";
					if (p2ShortName[0] || p2PlayerName[0]) {
						cout << "[" << p2ShortName << "] " << p2PlayerName << endl;
					} else {
						cout << "δ֪" << endl;
					}
				}
			}

			switch( dataInfo.phase ){
			case phase_read :
			case phase_battle :
				if (origMenuFlg) {
					cout << "״̬ : ��ս��" << endl;
				} else {
					cout << "״̬ : ��ս��" << endl;
				}
				
				{
					BYTE ID;
					WORD printFlg = 0;
					for(;;){
						if( printFlg == 0){
							ID = dataInfo.A.ID;
							printFlg = 1;
						}else{
							ID = dataInfo.B.ID;
							printFlg = 2;
						}
						cout << getCharacterName(ID);
						if( printFlg == 1 ) cout << " vs ";
						if( printFlg > 1 ) break;
					}
				}
				cout << " ( " << dataInfo.gameTime / 2 / 60 << "sec )" << endl;
				break;
			case phase_menu :
				cout << "״̬ : ��Ϸ�˵�" << endl;
				break;
			case phase_default :
				if( targetMode == mode_wait ){
					cout << "״̬ : �ȴ���" << endl;
				}else{
					cout << "״̬ : �ȴ���" << endl;
				}
				break;
			default :
				cout << "״̬ : �ȴ���?" << endl;
				break;
			}
		}
		return 1;
	case 23 :
		if (menuData.targetIP[0] == '\0') {
			strcpy(menuData.targetIP, standbyIP);
		}
	case 7 :
		//Standby
		lastTime.Access = nowTime;

		Access.sin_family = AF_INET;
		Access.sin_port = htons( menuData.targetPort );
		Access.sin_addr.s_addr = get_address(menuData.targetIP, &Access.sin_port);
		if( !Access.sin_addr.s_addr || Access.sin_addr.s_addr == 0xFFFFFFFF ){
			return 1;
		}
		strcpy(standbyIP, menuData.targetIP);

		accessPort = ntohs(Access.sin_port);

		if (origMenuFlg) {
			cout << "���ڵ�¼�� IP ";
		} else {
			cout << "���ڵ�¼�� IP ";
		}
		
		cout << inet_ntoa(Access.sin_addr) << ":" << dec << accessPort << endl;

		lastTime.Access = nowTime;

		cout << "�ȴ��Է�����." << endl;

		{
			DWORD roopCounter = 100;
			for(;;){
				if( roopCounter > 30 ){
					SendCmd( dest_access, cmd_standby, &myPort, 2 );
					roopCounter = 0;
				}
				roopCounter++;
				Sleep(10);
				if( Ready.sin_addr.s_addr ) break;
				if( !(Access.sin_addr.s_addr) ) return 1;
				if( GetEsc() ) return 1;
			}
		}

		lastTime.Access = nowTime;
		WaitForSingleObject( hMutex, INFINITE );
		Access = Ready;
		ReleaseMutex( hMutex );
		portSeekFlg = 1;

		//ܞ����
		cout << "�ȴ��Է���Ӧ." << endl;
		echoFlg.Access = 0;
		for(;;){
			{
				SendCmd( dest_access, cmd_echo, (void *)cowcaster_id, 5 );

				SOCKADDR_IN AccessTemp = Access;
				AccessTemp.sin_port = htons( readyPort );
				SendCmd( &AccessTemp, cmd_echoes, &myPort, 2 );
			}

			Sleep(100);
			if( echoFlg.Access ) break;
			if( !(Access.sin_addr.s_addr) ) return 1;
			if( GetEsc() ) return 1;
		}
		portSeekFlg = 0;

		Sound( this );

		//echoes�����
		Sleep(200);

		cout << "�����ӵ� " << inet_ntoa(Access.sin_addr) << ":" << dec << ntohs(Access.sin_port) << "\n";

		break;
	case 8 :
		//Receive Test signal
		{
			SOCKADDR_IN addrTemp;
			memset( &addrTemp, 0, sizeof( addrTemp ) );

			addrTemp.sin_family = AF_INET;
			addrTemp.sin_port = htons( menuData.targetPort );
			addrTemp.sin_addr.s_addr = get_address(menuData.targetIP, &Access.sin_port);
			if( !addrTemp.sin_addr.s_addr || addrTemp.sin_addr.s_addr == 0xFFFFFFFF ){
				return 1;
			}


			int Res = TestPort( &addrTemp );

			cout	<< endl
				<< "<���Խ��>" << endl;
			switch( Res ){
			case status_ok :
				cout << "�˿� : ���� ( UDP." << myPort << " �л�Ӧ )" << endl;
				break;
			case status_bad :
				cout << "�˿� : ���� ( UDP." << myPort << " ��Ӧ�� )" << endl;
				break;
			case status_error :
				cout << "�˿� : �������ֹ" << endl;
				break;
			default :
				return 1;
			}
		}
		return 1;
	case 9 :
	case 10:
		//Debug ( 127.0.0.1 )
		WaitForSingleObject( hMutex, INFINITE );
		memset( &Away, 0, sizeof(Away));
		Away.sin_family = AF_INET;
		Away.sin_addr.s_addr = inet_addr( "127.0.0.1" );
		Away.sin_port = htons( myPort );
		ReleaseMutex( hMutex );

		myInfo.terminalMode = mode_debug;
		if( boosterOnFlg == 0xF ){
			boosterFlg = 0;
			newCWflg = 1;
		}else{
			boosterFlg = 1;
		}

		strcpy(p2PlayerName, "th075Booster");
		strcpy(p2ShortName, "Bstr");
		
		if (menuData.mode == 11) {
			strcpy(p2PlayerName, "Practice Dummy");
			strcpy(p2ShortName, "Dmmy");
		}

		if (menuData.mode == 10) {
			//cin.ignore(1024,'\n');

			strcpy(p2PlayerName, "Practice Dummy");
			strcpy(p2ShortName, "Dmmy");

			cout << "��ϰģʽ:" << endl;
			cout << "  1: �����" << endl;
			cout << "  2: ָ������" << endl;
			cout << "  3: ����ָ������" << endl;
			cout << "  4: ��ͨ�������⼼���񡢲������������" << endl;
			cout << "  5: ����ƶ�" << endl;
			cout << "  6: ����ƶ���ָ������" <<endl;
			cout << "  7: ��Ծ" << endl;
			cout << "  8: ������� (����϶)" << endl;
			cout << "  9: ������� (�޿�϶)" << endl;
			cout << " 10: վ�˷���" << endl;
			cout << " 11: ���˷���" << endl;
			cout << " 12: ���з���" << endl;
			cout << " 13: ���к����" << endl;
			cout << " 14: ľ׮" << endl;
			cout << "������ 1-14 [" << practiceLastMode << "]>";

			int practiceTemp;
			practiceTemp = practiceLastMode;
			//cin >> practiceTemp;
			if ( read_int(practiceTemp) || practiceTemp < 1 || practiceTemp > 14) {
				cin.clear();
				//cin.ignore(1024,'\n');
				return 1;
			}

			practiceMode = practiceTemp;
			practiceLastMode = practiceMode;

			bool setMoveAnyway = 0;

			if (practiceMode == 3 || practiceMode == 7 || practiceMode == 12) {
				cout << "��Ծ����:" << endl;
				cout << "  1: ��" << endl;
				cout << "  2: �J" << endl;
				cout << "  3: �I" << endl;
				cout << "  4: D��" << endl;
				cout << "  5: D�J" << endl;
				cout << "  6: D�I" << endl;
				cout << "������ 1-6 [" << practiceJump << "]>";

				int jumpTemp;
				jumpTemp = practiceJump;
				//cin >> jumpTemp;

				if ( read_int(jumpTemp) || jumpTemp < 1 || jumpTemp > 6) {
					cin.clear();
					//cin.ignore(1024,'\n');
					return 1;
				}

				practiceJump = jumpTemp;
			}

			cout << "�����������趨:" << endl;
			cout << "  0: ���в�����                     0: ����ԭ������" << endl;
			cout << "  1: ������ǰ����                   4: ������ǰ����" << endl;
			cout << "  2: �����������                   8: �����������" << endl;
			cout << "  3: �����������                  12: �����������" << endl;
			cout << " 16: ���к�Ӧ                    32: �����Ӧ" << endl;
			cout << " 64: �����󲻷�Ӧ" << endl;
			cout << "��ֵ��ӿ��Զ�ѡ. �������������������������Ϊ: 2+12 = 14" << endl;
			cout << "������ 0-127 [" << practiceTeching <<"]>";

			int techTemp;
			techTemp = practiceTeching;
			//cin >> techTemp;

			if ( read_int(techTemp) || techTemp < 0 || techTemp > 127) {
				cin.clear();
				//cin.ignore(1024,'\n');
				return 1;
			}

			practiceTeching = techTemp;

			if ((practiceTeching&0x30) || (practiceMode >= 8 && practiceMode <= 13 && !(practiceTeching&0x40))) {
				cout << "��Ӧ��ʽ:" << endl;
				cout << "  1: ����Ӧ" << endl;
				cout << "  2: ��" << endl;
				cout << "  3: �ߡ�" << endl;
				cout << "  4: �I" << endl;
				cout << "  5: �ߨI" << endl;
				cout << "  6: �J" << endl;
				cout << "  7: �ߨJ" << endl;
				cout << "  8: ��D" << endl;
				cout << "  9: ��D" << endl;
				cout << " 10: ָ������" << endl;
				cout << " 11: ���������Ծ/����/����" << endl;
				cout << " 12: ����������/����" << endl;
				cout << " 13: ���������Ծ/����/����/5A/5B/66A/66B" << endl;
				cout << " 14: ��� 5A/5B/66A/66B" << endl;
				cout << " 15: ��� 5A/5B/2A/2B" << endl;
				cout << "������ 1-15 [" << practiceGuard << "]>";

				int guardTemp;
				guardTemp = practiceGuard;
				//cin >> guardTemp;

				if ( read_int(guardTemp) || guardTemp < 1 || guardTemp > 15) {
					cin.clear();
					//cin.ignore(1024,'\n');
					return 1;
				}

				practiceGuard = guardTemp;

				if (practiceGuard == 10) {
					setMoveAnyway = 1;
				}
			}

			if (practiceMode == 2 || practiceMode == 3 || practiceMode == 6 || setMoveAnyway) {
				cout << "����:" <<endl;
				cout << "  1: 5A            2: 2A            3: 6A" << endl;
				cout << "  4: 5B            5: 2B            6: 6B" << endl;
				cout << "  7: 5C            8: 2C            9: 6C" << endl;
				cout << " 10: 22A          11: 22B          12: 22C" << endl;
				cout << " 13: 66A          14: 66B" << endl;
				cout << " 15: 33A          16: 33B" << endl;
				cout << " 17: 236A         18: 236B         19: 236C" << endl;
				cout << " 20: 623A         21: 623B         22: 623C" << endl;
				cout << " 23: 214A         24: 214B         25: 214C" << endl;
				cout << " 26: 412A         27: 412B         28: 412C" << endl;
				cout << " 29: 421A         30: 421B         31: 421C" << endl;
				cout << " 32: 41236A       33: 41236B" << endl;
				cout << " 34: [A]          35: [B]          36: [C]" << endl;
				cout << " 37: 236D*        38: 214D*" << endl;
				cout << " ��*ѡ����ڵ��غ�����" << endl;
				cout << "������ 1-38 [" << practiceSetMove << "]>";

				int moveTemp;
				moveTemp = practiceSetMove;
				//cin >> moveTemp;

				if ( read_int(moveTemp) || moveTemp < 1 || moveTemp > 38) {
					cin.clear();
					//cin.ignore(1024,'\n');
					return 1;
				}

				practiceSetMove = moveTemp;
			}
			if (practiceMode >= 1 && practiceMode <= 9) {
				cout << "����Ƶ���趨:" << endl;
				cout << "  1: ÿ������" << endl;
				cout << "  2: ÿ������" << endl;
				cout << "  3: ÿ��һ��" << endl;
				cout << "  4: ����һ��" << endl;
				cout << "  5: ʮ��һ��" << endl;
				cout << "  6: һ��֮��" << endl;
				cout << "  7: ����֮��" << endl;
				cout << "  8: һ������֮��" << endl;
				cout << "  9: һ��ʮ��֮��" << endl;
				cout << " 10: ����" << endl;
				cout << "������ 1-10 [" << practiceTiming << "]>";

				int timingTemp;
				timingTemp = practiceTiming;
				//cin >> timingTemp;

				if ( read_int(timingTemp) || timingTemp < 1 || timingTemp > 10) {
					cin.clear();
					//cin.ignore(1024,'\n');
					return 1;
				}

				practiceTiming = timingTemp;
			}
		} else {
			practiceMode = 0;
		}

		//sound
//		Sound( this );

		break;
	case 22 :
		// Spectate
		toughModeFlg = 1;
		lastTime.Access = nowTime;

		Access.sin_family = AF_INET;
		Access.sin_port = htons( menuData.targetPort );
		Access.sin_addr.s_addr = get_address(menuData.targetIP, &Access.sin_port);
		if( !Access.sin_addr.s_addr || Access.sin_addr.s_addr == 0xFFFFFFFF ){
			return 1;
		}
		strcpy(accessIP, menuData.targetIP);

		accessPort = ntohs(Access.sin_port);

		cout << "�ȴ���Ϸ��ʼ (" << inet_ntoa(Access.sin_addr) << ":" << dec << accessPort << ")" << endl;

		lastTime.Access = nowTime;
		dataInfo.phase = 0xFF;
		{
			int counter = 9;
			for(;;){
				counter++;
				if (counter == 10) {
					SendCmd( dest_access, cmd_dataInfo );
					counter = 0;
				}

				Sleep(100);

				if( dataInfo.phase == phase_battle || dataInfo.phase == phase_read || dataInfo.phase == phase_menu ) {
					break;
				}

				if( !(Access.sin_addr.s_addr) ){
					return 1;
				}
				if( GetEsc() ) return 1;
			}
		}

		Sound( this );

		toughModeFlg = 0;
		skipSpecQuestion = 1;
	case 20 :
		//Try access

		/*
		//Ҫ��ӑ
		//���֤Μʂ����ĕr�g��recvfrom�ǤΥ���`�ʤɣ�
		//���
		if( argData.argMode ){
			//none
		}else{
			Sleep(1000);
		}
		*/

		//��Τ��ᣨҪ��ӑ����
		lastTime.Access = nowTime;

		Access.sin_family = AF_INET;
		Access.sin_port = htons( menuData.targetPort );
		Access.sin_addr.s_addr = get_address(menuData.targetIP, &Access.sin_port);
		if( !Access.sin_addr.s_addr || Access.sin_addr.s_addr == 0xFFFFFFFF ){
			return 1;
		}


		lastTime.Access = nowTime;
		echoFlg.Access = 0;
		for(;;){
			SendCmd( dest_access, cmd_echo, (void *)cowcaster_id, 5 );
			Sleep(100);
			if( echoFlg.Access ) break;
			if( !(Access.sin_addr.s_addr) ) return 1;
			if( GetEsc() ) return 1;
		}

//		Sound( this );
		break;
	case 21 :
		//Wait for access
		if (origMenuFlg) {
			cout << "�ȴ��������. " << endl;
		} else {
			cout << "�ȴ��������. " << endl;
		}
		waitTargetIP = get_address(menuData.targetIP, 0);
		if( !waitTargetIP || waitTargetIP == 0xFFFFFFFF ){
			cout << "debug : IP conv fail" << endl;
			Sleep(1000);
			return 0xF;
		}

		myInfo.terminalMode = mode_wait_target;
		accessFlg = status_default;
		for(;;){
			if( accessFlg == status_ok ) break;
			if( accessFlg == status_error ) return 1;
			if( accessFlg == status_bad ) return 1;
			if( GetEsc() ) return 1;
			Sleep(100);
		}
//		cout << "Access from " << inet_ntoa( Away.sin_addr ) << endl;
		myInfo.terminalMode = mode_root;

		//playerSideHost
		playerSideHostFlg = 1;

		Sound( this );
		break;
	default :
		return 1;
	}

	if( myInfo.terminalMode == mode_access ){
		myVisiblePort = myPort;

		//mode_root or mode_branch or mode_subbranch
		if( Communicate() ) return 1;

		/*
		// outdated
		if ( enCowCaster >= 3 && myVisiblePort != myPort) {
			cout << "External port is reported as " << dec << myVisiblePort << endl;
		}
		 */
	}

	//test
	dataInfo.terminalMode = myInfo.terminalMode;

	if( myInfo.terminalMode != mode_debug ){
		if( boosterOnFlg == 1 ){
			boosterFlg = 1;
		}
	}

	if( myInfo.terminalMode == mode_debug ){
		myInfo.playerSide = 0xA;

		enCowCaster = cowcaster_protocol_version;
		
		delayTime = 0;

		if( boosterFlg ){
			int delayTemp;

			if (menuData.mode != 10) {
				//cin.ignore(1024,'\n');
			}

			delayTemp = lastDelayTime / 2;
			cout << "�������ӳ� 0-10 [" << delayTemp << "]>";

			//cin >> delayTemp;
			if( read_int(delayTemp) || cin.fail() ){
				cin.clear();
				//cin.ignore(1024,'\n');
				return 1;
			}
			if( delayTemp > 10 ) delayTemp = 10;
			delayTime = delayTemp * 2;
		}
		lastDelayTime = delayTime;

		myInfo.sessionNo = 1;
		myInfo.sessionID = 1 + rand()%255;
		myInfo.sessionIDNext = myInfo.sessionID;

		dataInfo.sessionNo = myInfo.sessionNo;
		dataInfo.sessionID = myInfo.sessionID;
		dataInfo.sessionIDNext = myInfo.sessionIDNext;
	}else if( myInfo.terminalMode == mode_branch || myInfo.terminalMode == mode_subbranch ){
		//���饭��饯���`��gameTIme��ʾ�ʤ�
		//�Q�餹�뤫�ɤ���

		// in case we're in spectate mode, reget the info
		dataInfo.sessionID = 0;

		for(;;){
			// SendCmd( dest_root, cmd_dataInfo );
			SendCmd( dest_root, cmd_dataInfo, (void *)cowcaster_id, 5 );
			Sleep(200);
			if( dataInfo.sessionID ) break;
			if( !(Root.sin_addr.s_addr) ){
				cout << "���� : ��ʱ (���ڵ�)" << endl;
				return 1;
			}
			if( GetEsc() ) return 1;
		}

		WaitForSingleObject( hPrintMutex, INFINITE );
		cout	<< endl
			<< "<�Է�״̬>" << endl;

		//test
		if (origMenuFlg) {
			switch( dataInfo.terminalMode ){
			case mode_root :
				cout << "ģʽ : ���ڵ�" << endl;
				break;
			case mode_broadcast :
				cout << "ģʽ : �㲥�ڵ�" << endl;
				break;
			case mode_branch :
				cout << "ģʽ : ���νڵ�" << endl;
				break;
			case mode_subbranch :
				cout << "ģʽ : ĩ�˽ڵ�" << endl;
				break;
			}
		} else {
			switch( dataInfo.terminalMode ){
			case mode_root :
				cout << "ģʽ : ���ڵ�" << endl;
				break;
			case mode_broadcast :
				cout << "ģʽ : �㲥�ڵ�" << endl;
				break;
			case mode_branch :
				cout << "ģʽ : ���νڵ�" << endl;
				break;
			case mode_subbranch :
				cout << "ģʽ : ĩ�˽ڵ�" << endl;
				break;
			}
		}

		if (dataInfo.terminalMode == mode_root) {
			if (dataInfo.playerSide == 0xA) {
				cout << "�Է�λ��: 1P" << endl;
			} else if (dataInfo.playerSide == 0xB) {
				cout << "�Է�λ��: 2P" << endl;
			}
		}

		if (enCowCaster >= 2) {
			if (hasRemote) {
				cout << "�Է�IP : " << inet_ntoa(Remote.sin_addr) << ":" << dec << ntohs(Remote.sin_port) << endl;
			}
		}
		namesLocked = 1;
		if (enCowCaster >= 3 && nPlayers == 2) {
			if (!p1ShortName[0]) {
				if (!p1PlayerName[0]) {
					if (!unknownNameFlg && dataInfo.playerSide == 0xA) {
						strcpy(p1PlayerName, inet_ntoa(Root.sin_addr));
					} else if (!unknownNameFlg && dataInfo.playerSide == 0xB && hasRemote) {
						strcpy(p1PlayerName, inet_ntoa(Remote.sin_addr));
					} else {
						strcpy(p1PlayerName, "Unknown");
					}
				}
			} else {
				if (!p1PlayerName[0]) {
					strcpy(p1ShortName, p1PlayerName);
				}
			}

			if (!p2ShortName[0]) {
				if (!p2PlayerName[0]) {
					if (!unknownNameFlg && dataInfo.playerSide == 0xB) {
						strcpy(p2PlayerName, inet_ntoa(Root.sin_addr));
					} else if (!unknownNameFlg && dataInfo.playerSide == 0xA && hasRemote) {
						strcpy(p2PlayerName, inet_ntoa(Remote.sin_addr));
					} else {
						strcpy(p2PlayerName, "Unknown");
					}
				}
			} else {
				if (!p2PlayerName[0]) {
					strcpy(p2ShortName, p2PlayerName);
				}
			}
		} else {
			// Blah.
			p1ShortName[0] = '\0';
			strcpy(p1PlayerName, "Unknown");
			p2ShortName[0] = '\0';
			strcpy(p2PlayerName, "Unknown");

			if (!unknownNameFlg) {
				if (dataInfo.playerSide == 0xA) {
					strcpy(p1PlayerName, inet_ntoa(Root.sin_addr));
					if (hasRemote) {
						strcpy(p2PlayerName, inet_ntoa(Remote.sin_addr));
					}
				} else if (dataInfo.playerSide == 0xB) {
					strcpy(p2PlayerName, inet_ntoa(Root.sin_addr));
					if (hasRemote) {
						strcpy(p1PlayerName, inet_ntoa(Remote.sin_addr));
					}
				}
			}
		}
		cout << "��� : "
		     << "[" << p1ShortName << "] " << p1PlayerName
		     << " vs "
		     << "[" << p2ShortName << "] " << p2PlayerName << endl;

		p1TempName = 0;
		p2TempName = 0;

		switch( dataInfo.phase ){
		case phase_read :
		case phase_battle :
			{
				BYTE ID;
				WORD printFlg = 0;
				for(;;){
					if( printFlg == 0){
						ID = dataInfo.A.ID;
						printFlg = 1;
					}else{
						ID = dataInfo.B.ID;
						printFlg = 2;
					}
					cout << getCharacterName(ID);
					if( printFlg == 1 ) cout << " vs ";
					if( printFlg > 1 ) break;
				}
			}
			cout << " ( " << dataInfo.gameTime / 2 / 60 << "sec )" << endl;
			break;
		case phase_default :
		case phase_menu :
		default :
			if (origMenuFlg) {
				cout << "�Է�׼����." << endl;
			} else {
				cout << "�Է�׼����." << endl;
			}
			break;
		}
		ReleaseMutex( hPrintMutex );

		if (!skipSpecQuestion) {
			WaitForSingleObject( hPrintMutex, INFINITE );
			cout	<< "���� " << timeout_time / 1000 << " ���ھ���." << endl
				<< "0 : �˳�" << endl
				<< "1 : ����" << endl
				<< "������ [1] >";
			ReleaseMutex( hPrintMutex );

			int Temp = 1;

			//cin >> setw(1) >> Temp;
			if( read_int(Temp) || cin.fail() ){
				cin.clear();
				//cin.ignore(1024,'\n');
				return 1;
			}
			if( !(Root.sin_addr.s_addr) ){
				cout << "���� : ��ʱ (���ڵ�)" << endl;
				return 1;
			}

			switch( Temp ){
			case 0 :
				return 1;
			case 1 :
				//�A��
				break;
			default :
				return 1;
			}
		}

		char join_packet[2 + 5 + 1 + 1 + 15 + 1 + 4];
		int join_len = 8;
		memcpy(join_packet, &myPort, 2);
		memcpy(join_packet+2, cowcaster_id, 5);
		join_packet[7] = 0;

		if (!anonymousObsFlg) {
			join_packet[7] = 1;

			int name_len = strlen(myShortName);
			join_packet[join_len++] = name_len;
			memcpy(&join_packet[join_len], myShortName, name_len);
			join_len += name_len;

			name_len = strlen(myPlayerName);
			join_packet[join_len++] = name_len;
			memcpy(&join_packet[join_len], myPlayerName, name_len);
			join_len += name_len;
		}

		joinRes = status_default;
		while( joinRes == status_default ){
			SendCmd( dest_root, cmd_join, join_packet, join_len );
			Sleep(100);
			if( !(Root.sin_addr.s_addr) ){
				cout << "���� : ��ʱ (���ڵ�)" << endl;
				return 1;
			}
			if( GetEsc() ) return 1;
		}
		if( joinRes == status_ok ){
			cout << "״̬ : ����" << endl;
		}else if( joinRes == status_bad ){
			cout << "״̬ : ����" << endl;
			return 1;
		}
	}else if( myInfo.terminalMode == mode_broadcast ){
		//���å����ID����
		myInfo.sessionNo = 1;
		myInfo.sessionID = 1 + rand()%255;
		myInfo.sessionIDNext = myInfo.sessionID;

		dataInfo.sessionNo = myInfo.sessionNo;
		dataInfo.sessionID = myInfo.sessionID;
		dataInfo.sessionIDNext = myInfo.sessionIDNext;

		myInfo.playerSide = 0;
		enInfo.playerSide = 0;

		{
			int delayTimeTemp;
			cout	<< endl
				<< "<�ӳ�>" << endl;

			//sideA
			cout << "������ A �ӳ� [0] >";
			delayTimeTemp = 0;
			//cin >> setw(1) >> delayTimeTemp;
			if( read_int(delayTimeTemp) || cin.fail() ){
				cin.clear();
				//cin.ignore(1024,'\n');
				return 1;
			}
			if( delayTimeTemp > 10 ) delayTimeTemp = 10;
			delayTimeA = delayTimeTemp * 2;

			//sideB
			cout << "������ B �ӳ� [0] >";
			delayTimeTemp = 0;
			//cin >> setw(1) >> delayTimeTemp;
			if( read_int(delayTimeTemp) || cin.fail() ){
				cin.clear();
				//cin.ignore(1024,'\n');
				return 1;
			}
			if( delayTimeTemp > 10 ) delayTimeTemp = 10;
			delayTimeB = delayTimeTemp * 2;
		}
	}else if( myInfo.terminalMode == mode_root ){

		//�ǩ`����ޤȤ�Ƥ������ۤ����S
		//��ۤ�
		myInitFlg = 1;
		for(;;){
			if( enInitFlg ) break;
			SendCmd( dest_away, cmd_initflg );
			if( !(Away.sin_addr.s_addr) ) return 1;
			if( GetEsc() ) return 1;
			Sleep(100);
		}

		//ͨ�Ť���AB��Q���
		if( GetPlayerSide() ) return 1;

		cout << "���ӵ���� : ";
		namesLocked = 1;
		if( !p2PlayerName[0] ) {
			if (p2ShortName[0]) {
				strcpy(p2PlayerName, p2ShortName);
			} else {
				p2TempName = 1;
				if (unknownNameFlg) {
					strcpy(p2PlayerName, "Unknown");
					cout << "Unknown" << endl;
				} else {
					strcpy(p2PlayerName, inet_ntoa(Away.sin_addr));
					cout << inet_ntoa(Away.sin_addr) << endl;
				}
			}
		} else {
			cout << p2PlayerName << endl;
		}

		if( myInfo.playerSide == 0xA ){
			int delayTimeTemp;
			//cin.ignore(1024,'\n');

			for(;;){
				if( GetDelay( dest_away, &delay ) ) return 1;

				cout.setf(ios::fixed, ios::floatfield);
				cout	<< endl
					<< "<�ӳ�>" << endl
					<< "Լ " << setprecision( 1 ) << delay << "[����] �ӳ�." << endl;
				//	<< "( DelayTime[ms] = 16[ms] x Input )" << endl

				if (delay <= 8) {
					delayTimeTemp = 1;
				} else if (rollbackFlg) {
					delayTimeTemp = (int)ceil((delay+16.0)/33.33333);
				} else {
					delayTimeTemp = (int)ceil((delay+16.0)/32.0);
				}

				if (delayTimeTemp >= 10) {
					delayTimeTemp = 10;
					if (delay >= 320) {
						cout << "��ע�� : 320�����ӳ�����Ϸ�Եļ���." << endl
						     << "         ����������������¼�����Ϸ." << endl;
					}
				}
				
				if (delayTimeTemp == 10 || !acceptRecommendedDelayFlg) {
					if (rollbackFlg) {
						cout << "�������ӳ��� [";
					} else {
						cout << "�������ӳ��� [";
					}
					cout << delayTimeTemp <<"] >";
					
					if (read_int(delayTimeTemp)) {
						cin.clear();
						return 1;
					}
				}

				//cin >> delayTimeTemp;
				//if( cin.fail() ){
				//	cin.clear();
				//	cin.ignore(1024,'\n');
				//	return 1;
				//}

				if( delayTimeTemp > 10 ) delayTimeTemp = 10;
//				if( delayTimeTemp == 0 ) delayTimeTemp = 1;
				/*
				if( delayTimeTemp == 1 ){
					if( delay > 4 ){
						delayTimeTemp = 2;
						cout << endl;
						cout << "Now 'Buffer Margin 1' is limited." << endl;
						cout << "The value is changed to 2." << endl;
						cout << "( 16ms Roop causes lag )" << endl;
					}
				}
				 */
				if( delayTimeTemp ) break;
			}
			cout << "�ӳ��� : " << delayTimeTemp << endl;
			delayTime = delayTimeTemp * 2;
		}else{
			cout	<< endl
				<< "<�ӳ�>" << endl
				<< "�ȴ��Է������ӳ���." << endl;
			for(;;){
				SendCmd( dest_away, cmd_delay );
				if( delayTime ) break;
				if( GetEsc() ) return 1;
				Sleep(100);
			}
			cout.setf(ios::fixed, ios::floatfield);
			cout << "�ӳ��� : " << delayTime / 2 << " ( ������Լ " << setprecision( 1 ) << delay << "[����]�ӳ� )" << endl;

			// swap player names
			char tempPlayerName[16];
			char tempShortName[5];
			strcpy(tempPlayerName, p1PlayerName);
			strcpy(p1PlayerName, p2PlayerName);
			strcpy(p2PlayerName, tempPlayerName);

			strcpy(tempShortName, p1ShortName);
			strcpy(p1ShortName, p2ShortName);
			strcpy(p2ShortName, tempShortName);

			bool tempTempName;
			tempTempName = p1TempName;
			p1TempName = p2TempName;
			p2TempName = tempTempName;
		}
		
		if (rollbackFlg) {
			rollbackOk = 1;
			calcRewind(delayTime, &localDelayTime, &rewindAmount);
			rollbackOk = 0;
			
			cout << endl;
			cout << "ָ�������ӳ� : " << localDelayTime/2 << endl;
			cout << "Ԥ��ع�֡�� : " << rewindAmount/2 << endl;
			cout << endl;
		} else {
			localDelayTime = delayTime;
			rewindAmount = 0;
		}

		if( myInfo.playerSide == 0xA ){
			//���å����ID�������ࣩ����
			myInfo.sessionNo = 1;
			myInfo.sessionID = 1 + rand()%255;
			myInfo.sessionIDNext = myInfo.sessionID;
		}else{
			for(;;){
				SendCmd( dest_away, cmd_session );
				if( enInfo.sessionID ) break;
				if( !(Away.sin_addr.s_addr) ) return 1;
				if( GetEsc() ) return 1;
				Sleep(100);
			}
			myInfo.sessionNo = enInfo.sessionNo;
			myInfo.sessionID = enInfo.sessionID;
			myInfo.sessionIDNext = enInfo.sessionIDNext;
		}

		myInitFlg = 2;
		for(;;){
			if( enInitFlg == 2) break;
			SendCmd( dest_away, cmd_initflg );
			if( !(Away.sin_addr.s_addr) ) return 1;
			if( GetEsc() ) return 1;
			Sleep(100);
		}
	}else{
		return 1;
	}

	if (sessionLogFlg) {
		sessionLogFile.open("sessionlog.txt", ios::app);

		if (sessionLogFile) {
			sessionLogFile << endl;

			printDate(&sessionLogFile);
			sessionLogFile << ' ';
			printTime(&sessionLogFile);

			sessionLogFile << " Session start. Players: [" << p1ShortName << "] " << p1PlayerName << " vs [" << p2ShortName << "] " << p2PlayerName << endl;
		}
	}

	if( newCWflg ){
		//cout << "debug : float CW set ON" << endl;
	}else{
		cout << "ע�� : ��ͬ������������ʧЧ" << endl;
	}

	//��Τ���
	if( inputData.init() ) return 0xF;

	if( hTh075Th ){
		if( !WaitForSingleObject( hTh075Th, 0 ) ){
			CloseHandle( hTh075Th );
			hTh075Th = NULL;

			HWND hWnd = FindWindow( NULL , windowName );
			if( hWnd ){
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				Sleep(3000);
			}
			//�͉���Υ���åɤ��_ʼ����
			hCheckEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
			hProcess = NULL;
			hTh075Th = (HANDLE)_beginthreadex(NULL, 0, th075Thread, this, 0, NULL);
			if( !hTh075Th ) return 1;

			WaitForSingleObject(hCheckEvent, 10000);
			CloseHandle( hCheckEvent );
			hCheckEvent = NULL;

			if( !hProcess ){
				CloseHandle( hTh075Th );
				hTh075Th = NULL;
				return 1;
			}
		}else{
			if( myInfo.terminalMode == mode_debug ){
				cout << "debug : th075thread already running" << endl;
			}

			HWND hWnd = FindWindow( NULL , windowName );
			if( !hWnd ){
				//�K��꤫��
				if( WaitForSingleObject(hTh075Th, 3000) ){
					CloseHandle( pi.hThread );
					CloseHandle( pi.hProcess );
				}
				CloseHandle( hTh075Th );
				hTh075Th = NULL;
				cout << "debug : th075thread restart" << endl;

				hCheckEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
				hProcess = NULL;
				hTh075Th = (HANDLE)_beginthreadex(NULL, 0, th075Thread, this, 0, NULL);
				if( !hTh075Th ) return 1;

				WaitForSingleObject(hCheckEvent, 10000);
				CloseHandle( hCheckEvent );
				hCheckEvent = NULL;

				if( !hProcess ){
					CloseHandle( hTh075Th );
					hTh075Th = NULL;
					return 1;
				}
			}
		}
	}else{
		HWND hWnd = FindWindow( NULL , windowName );
		if( hWnd ){
			DWORD PID;
			HANDLE hProcessTemp;
			GetWindowThreadProcessId( hWnd , &PID );
			hProcessTemp  = OpenProcess( PROCESS_ALL_ACCESS, FALSE, PID );

			PostMessage(hWnd, WM_CLOSE, 0, 0);
			WaitForSingleObject( hProcessTemp, 3000 );
			CloseHandle( hProcessTemp );
		}
		//�͉���Υ���åɤ��_ʼ����
		hCheckEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		hProcess = NULL;
		hTh075Th = (HANDLE)_beginthreadex(NULL, 0, th075Thread, this, 0, NULL);
		if( !hTh075Th ) return 1;

		WaitForSingleObject(hCheckEvent, 10000);
		CloseHandle( hCheckEvent );
		hCheckEvent = NULL;

		if( !hProcess ){
			CloseHandle( hTh075Th );
			hTh075Th = NULL;
			return 1;
		}
	}


	return 0;
}

//�Хå����饦���
int mainDatClass::backGroundRoop(){
	#if debug_mode
		WaitForSingleObject( hPrintMutex, INFINITE );
		cout << "debug : backGroundRoop()" << endl;
		ReleaseMutex( hPrintMutex );
	#endif

	if ( disableScreensaverFlg ) {
		SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, 0, 0, 0);
		SystemParametersInfo(SPI_SETLOWPOWERACTIVE, 0, 0, 0);
		SystemParametersInfo(SPI_SETPOWEROFFACTIVE, 0, 0, 0);
	}


	//�͉���Υ�����ɥ������
	for(;;){
		if( GetEsc() ){
			return 1;
		}
		if( FindWindow( NULL , windowName ) ) break;
		Sleep(200);
	}

	//top
	if( windowTopFlg ){
		HWND hWnd = FindWindow( NULL , windowName );
		if( hWnd ){
			SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
		}
	}

 	//windowSize
 	if( windowResizeFlg == 2 ){
 		HWND hWnd = FindWindow( NULL , windowName );
 		if( hWnd ){
 			/*
 			char path[200];
 			strcpy( path ,nowDir );
 			strcat( path, "\\d3d8.dll\0" );
 			if( !_access( path, 0 ) ){
 				SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, windowWidth, windowHeight, SWP_NOMOVE);
 			}
 			*/
 			SetWindowPos(hWnd, NULL, 0, 0, windowWidth, windowHeight, SWP_NOMOVE);
 		}
 	}
	toggleWindowTopFlg = 0;

	roopFlg = 1;
	for(;;){
		if( GetEsc() ) break;

		hWnd = FindWindow( NULL, windowName );
		if( !hWnd ) break;

		if (toggleWindowTopFlg) {
			if (windowTopFlg) {
				SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
				windowTopFlg = 0;
				cout << "����: �ö� ��" << endl;
			} else {
				SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
				windowTopFlg = 1;
				cout << "����: �ö� ��" << endl;
			}

			toggleWindowTopFlg = 0;
		}

		if( !roopFlg ) break;
		Sleep(100);
	}
	roopFlg = 0;

	if (winsA >= 1 || winsB >= 1) {
		for (int i = 0; i < 2; ++i) {
			ostream *stream;
			if (i == 0) {
				if (roundShowFlg < 1) {
					continue;
				}
				stream = &cout;
			} else if (i == 1) {
				if (!sessionLogFlg || !sessionLogFile) {
					continue;
				}
				stream = &sessionLogFile;

				printDate(stream);
				*stream << ' ';
			}

			printTime(stream);
			*stream << ' ';

			*stream << "Session results: " << dec << (int)(winsA + winsB) << " match";
			if ((winsA+winsB)>1) {
				*stream << "es";
			}

			if (humanTimeFlg && totalGameTime >= 120 && i == 0) {
				DWORD seconds = totalGameTime / 120;
				DWORD minutes = seconds / 60;
				seconds -= minutes * 60;
				
				*stream << ", " << (int)winsA << "(" << (int)roundsWonA << ") - " << (int)winsB << "(" << (int)roundsWonB << ") wins(rounds) (" << minutes << "m " << seconds << "s)" << endl;
			} else {
				*stream << ", " << (int)winsA << "(" << (int)roundsWonA << ") - " << (int)winsB << "(" << (int)roundsWonB << ") wins(rounds) (" << totalGameTime/120 << "s)" << endl;
			}
		}
	}
	sessionLogFile.close();

	if ( disableScreensaverFlg ) {
		SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, 1, 0, 0);
		SystemParametersInfo(SPI_SETLOWPOWERACTIVE, 1, 0, 0);
		SystemParametersInfo(SPI_SETPOWEROFFACTIVE, 1, 0, 0);
	}

	return 0;
}


