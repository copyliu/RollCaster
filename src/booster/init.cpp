#include "conf.h"
#include "boosterDatClass.h"

//OpenProcessできるか、baseはあるか,兆念は函誼できたか、
//AIオ�`プンできたか、キャラ�@�Aか、�筝�してたときの�I尖 頼阻したら0を卦す
//boosterDatInitFlgはRefleshDat()で1にする
//playerSideを�笋┐燭�bodyIniFlgを1にすること
int boosterDatClass::boosterDatInit(){
	#if debug_mode
		cout << "debug : " << hex << playerSide << ".boosterDatInit() " << endl;
	#endif

	hWnd = FindWindow( NULL , casterDat->windowName );
	if(hWnd == 0) return 0xF;

	//メモリを�_隠できなかったとき
	if(AI == 0) return 0xF;

	//ボタン秘薦兜豚晒
	if(bodyIniFlg==0){
		for(Counter=1;Counter<9;Counter++){
			if(gameInfoIni[Counter][5]){	//兀されている
				Input.type = INPUT_KEYBOARD;
				Input.ki.wVk = gameInfoIni[Counter][2];
				Input.ki.wScan = MapVirtualKey(gameInfoIni[Counter][2], 0);
				Input.ki.dwFlags = KEYEVENTF_KEYUP;
				Input.ki.dwExtraInfo = 0;
				Input.ki.time = 0;

				SendInput(1, &Input, sizeof(INPUT));
				gameInfoIni[Counter][4]=0;
				gameInfoIni[Counter][5]=0;
			}
		}
	}

	baseAddress = 0x671418;	//baseAddress
	gameMode = 0;


	GetWindowThreadProcessId( hWnd , &PID );
	if( PID == 0 ) return 1;

	hProcess  = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	if(playerSide == 0xA){
		ReadProcessMemory( hProcess , (void *)(baseAddress + 0x00) , &myBase , 4, NULL );
		myIniOffset = 0x28;
	}
	if(playerSide == 0xB){
		ReadProcessMemory( hProcess , (void *)(baseAddress + 0x04) , &myBase , 4, NULL );
		myIniOffset = 0x50;
	}

	if(myBase==0){
		CloseHandle( hProcess );
		return 1;
	}
	ReadProcessMemory( hProcess , (void *)myBase , &myName , 4, NULL );
	ReadProcessMemory( hProcess , (void *)(baseAddress + 0x218) , &gameMode , 1, NULL );
	CloseHandle( hProcess );

	if(gameMode ==5){
		gameTimeAddress = baseAddress + 0x498;		//gameTimeAddress	//リプレイモ�`ドのときの�r�g
	}else{
		gameTimeAddress = baseAddress + 0x49C;		//gameTimeAddress
	}

	AIMode = 0;
	if(gameMode==0) AIMode = 0;					//story	//stop			//AIModeの�O協
	if(gameMode==1) AIMode = 0;					//arcade	//stop
	if(gameMode==2){
		if( listeningMode ) {
			AIMode = 1;						//human	//listeningMode
		}else{
			if( playerSide == 0xB ) AIMode = 2;		//human	//active
			//if( playerSide == 0xA && practiceMode == 0) AIMode = 1;		//human	//listening
		}
	}
	if(gameMode==3) AIMode = 0;					//cpu		//stop
	if(gameMode==4) AIMode = 0;					//practice	//stop
//	if(gameMode==4 && playerSide == 0xB) AIMode = 0;	//practice	//stop
//	if(gameMode==4 && playerSide == 0xA) AIMode = 1;	//practice	//listening
	if(gameMode==5) AIMode = 1;					//replay	//listening

	if(practiceMode!=0 && playerSide == 0xB) {
		srand(time(NULL));
		practiceState = 0;
		practiceTimer = 0;
		practiceTimer2 = 0;
		practiceSetMoveCommand = __5;
		practiceDeclareFlag = 0;
		practiceLastMove = __5;
		practiceUpdateFlag = 0;

		switch (practiceSetMove) {
		case  1: practiceSetMoveCommand = __A; break;
		case  2: practiceSetMoveCommand = __2A; break;
		case  3: practiceSetMoveCommand = __6A; break;
		case  4: practiceSetMoveCommand = __B; break;
		case  5: practiceSetMoveCommand = __2B; break;
		case  6: practiceSetMoveCommand = __6B; break;
		case  7: practiceSetMoveCommand = __C; break;
		case  8: practiceSetMoveCommand = __2C; break;
		case  9: practiceSetMoveCommand = __6C; break;
		case 10: practiceSetMoveCommand = __22A; break;
		case 11: practiceSetMoveCommand = __22B; break;
		case 12: practiceSetMoveCommand = __22C; break;
		case 13: practiceSetMoveCommand = __D6A; break;
		case 14: practiceSetMoveCommand = __D6B; break;
		case 15: practiceSetMoveCommand = __D63A; break;
		case 16: practiceSetMoveCommand = __D63B; break;
		case 17: practiceSetMoveCommand = __236A; break;
		case 18: practiceSetMoveCommand = __236B; break;
		case 19: practiceSetMoveCommand = __236C; break;
		case 20: practiceSetMoveCommand = __623A; break;
		case 21: practiceSetMoveCommand = __623B; break;
		case 22: practiceSetMoveCommand = __623C; break;
		case 23: practiceSetMoveCommand = __214A; break;
		case 24: practiceSetMoveCommand = __214B; break;
		case 25: practiceSetMoveCommand = __214C; break;
		case 26: practiceSetMoveCommand = __412A; break;
		case 27: practiceSetMoveCommand = __412B; break;
		case 28: practiceSetMoveCommand = __412C; break;
		case 29: practiceSetMoveCommand = __421A; break;
		case 30: practiceSetMoveCommand = __421B; break;
		case 31: practiceSetMoveCommand = __421C; break;
		case 32: practiceSetMoveCommand = __41236A; break;
		case 33: practiceSetMoveCommand = __41236B; break;
		case 34: practiceSetMoveCommand = __A_; break;
		case 35: practiceSetMoveCommand = __B_; break;
		case 36: practiceSetMoveCommand = __C_; break;
		case 37: practiceSetMoveCommand = __236D; practiceDeclareFlag = 1; break;
		case 38: practiceSetMoveCommand = __214D; practiceDeclareFlag = 1; break;
		default: break;
		}
	}

	Flg = GetName();
	if(Flg) return 1;

	if(boosterDatInitFlg){
		if(strcmp(NameTemp, Name) !=0){
			if( strcmp(NameTemp, "init") && strcmp(NameTemp, "error") && strcmp(NameTemp, "second")){	//init,error,secondのとき隠贋しない
				if( CloseAI()|| CloseSpellAI() || CloseIndividualAI() || CloseBackAI() ) return 1;

				if( eigenValueLocal[0][0] ){
					if( CloseLocalAI() ) return 1;
				}
			}
		}
	}

	if(boosterDatInitFlg){
		boosterDatInitFlg = 0;
		return 1;
	}
	if( enDat->boosterDatInitFlg == 1 ) return 1;		//�猜屬��K阻していないと�Mまない
	strcpy( enName, enDat->Name );

	//揖じキャラ揖平なときはAI[]を慌嗤する。★徭蛍のAI[]を隔たない:secondaryMode
	if(myID == enDat->myID){				//キャラIDを歌孚	//揖じキャラ揖平の����
		if( strcmp(NameTemp,"second") == 0 ){
			secondaryModeFlg = 1;			//念指secondaryModeだったら�@�A
		}else{
			if( enDat->secondaryModeFlg == 0 ){			//テキがsecodaryModeではなかったとき
				if( strcmp(Name,NameTemp) == 0 ){
					secondaryModeFlg = 0;	//徭蛍�箸�キャラ�@�Aしているときは�猜��箸�secondaryModeになることを勣箔
				}else{
					secondaryModeFlg = 1;
				}
			}else{
				secondaryModeFlg = 0;
			}
		}
		if( !secondaryModeFlg && !enDat->secondaryModeFlg ){
			return 1;
		}
	}else{
		secondaryModeFlg = 0;				//揖じキャラ揖平の���蕕任呂覆�
	}

	#if debug_mode
		if(secondaryModeFlg) cout << "debug : " << hex << playerSide << ".secondaryMode" << endl;
	#endif


	//ファイルを�_く
	if(secondaryModeFlg){
		strcpy(Name,"second\0");
	}else{
		if( !( !strcmp(NameTemp, "init") || !strcmp(NameTemp, "error") || !strcmp(NameTemp, "second") ) && !strcmp(NameTemp, Name) ){
			//念指と揖じキャラ
			if(strcmp(enNameTemp, enName) !=0){
				if( CloseIndividualAI() ) return 1;

				Flg = OpenIndividualAI(Name,enName);
				if( Flg == 0xF ) return 0xF;
				if( Flg ) memset(AI + AIsizeArray[0] + AIsizeArray[1], 0, AIsizeArray[2]);

			}
		}else{
			//念指がinit,error,secondのとき or 念指と�`うキャラ
			Flg = OpenAI(Name);
			if( Flg == 0xF ) return 0xF;
			if( Flg ) memset(AI, 0, AIsizeArray[0]);

			Flg = OpenSpellAI(Name);
			if( Flg == 0xF ) return 0xF;
			if( Flg ) memset(AI + AIsizeArray[0], 0, AIsizeArray[1]);

			Flg = OpenIndividualAI(Name,enName);
			if( Flg == 0xF ) return 0xF;
			if( Flg ) memset(AI + AIsizeArray[0] + AIsizeArray[1], 0, AIsizeArray[2]);


			if(eigenValueLocal[0][0]){
				Flg = OpenLocalAI(Name);
				if( Flg == 0xF ) return 0xF;
				if( Flg ) memset(AI + AIsizeArray[0] + AIsizeArray[1] + AIsizeArray[2], 0, AIsizeArray[3]);

			}

			Flg = OpenBackAI(Name);
			if( Flg == 0xF ) return 0xF;
			if( Flg ) memset(AI + AIsizeArray[0] + AIsizeArray[1] + AIsizeArray[2] + AIsizeArray[3], 0, AIsizeArray[4]);

		}
	}

	strcpy(enName,enDat->Name);
	strcpy(NameTemp,Name);
	strcpy(enNameTemp,enName);

	//クラスの壓り圭を�O協するものはここまで
	//兜豚晒��麻はここから

	CalcAddress();
	Flg = ConvertIni();	//駅ずCalcAddress()の瘁で佩うこと
	if(Flg) return 1;
	statusInit();

	intervalFlg = 0;
	gameTime = 0;
	gameTimeTemp = 0;
	gameTimeTemp2 = 0;
	inputTime = 0;
	inputResetTime = 0;
	decodeTime = 0;
	statusID = 0;

	memset( myGameInfo, 0, sizeof( myGameInfo ) );
	memset( commandInput, 0, sizeof( commandInput ) );
	for(Counter=0; Counter<10; Counter++){
		gameInfoIni[Counter][4] = 0;
	}
	memset( LNAIBuf, 0, sizeof( LNAIBuf ) );
	memset( SNAIBuf, 0, sizeof( SNAIBuf ) );
	memset( SWAIBuf, 0, sizeof( SWAIBuf ) );
	memset( SpellAIBuf, 0, sizeof( SpellAIBuf ) );
	memset( RecoverAIBuf, 0, sizeof( RecoverAIBuf ) );
	memset( BackAIBuf, 0, sizeof( BackAIBuf ) );

	if( secondaryModeFlg ){
		AIbase = (BYTE*)(enDat->AI);
	}else{
		AIbase = (BYTE*)AI;
	}

	//baseAi
	SNAIbase	= AIbase;
	SWAIbase	= AIbase + eigenValueSN[0][1];

	//spellAI
	SpellAIbase	= AIbase + AIsizeArray[0];

	//individualAI
	LNAIbase		= AIbase + AIsizeArray[0] + AIsizeArray[1];
	RecoverAIbase	= AIbase + AIsizeArray[0] + AIsizeArray[1] + eigenValueLN[0][1];

	//localAL
	LocalAIbase	= AIbase + AIsizeArray[0] + AIsizeArray[1] + AIsizeArray[2];

	//backAI
	BackAIbase	= AIbase + AIsizeArray[0] + AIsizeArray[1] + AIsizeArray[2] + AIsizeArray[3];

	return 0;
}
