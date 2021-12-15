#include "conf.h"
#include "boosterDatClass.h"
#include "HL.h"

void boosterDatClass::ReadBackAI(){
	if(eigenValueBack[0][0]){
		
		DWORD Temp = 0;
		Flg = 0;
		
		#if debug_height
			if(myGameInfo[ _para ][2][0] && *enGameInfo[ _para ][2][0]){	//�g�Y��
//			if(*enGameInfo[ _para ][2][0]){
				Temp = 1;
			}
			if(gameInfoPara[15][1]==4){
				Temp += 5;
			}
			
			
			if(*enGameInfo[ _para ][2][0] && *enGameInfo[ _status ][0][0]==2){
				Flg = 1;
			}
		#endif
		
		
		Address = BackAIbase + eigenValueBack[0][3];
		for(Counter=0; Counter<1200; Counter+=40){
			Address2 = Address + Counter;
			if(*Address2 == 0){
				Counter = 1200;
			}else{
				if(*Address2 == (BYTE)myGameInfo[ _status ][6][0]){	//�Է�ǰ�Є�
					Counter = 1200;
					Address3 = Address2 + 4;
					if(GetL(Address3 + 3) > 6){	//�u����һ������
						if(!(*Address3==0x24 && myGameInfo[ _info ][4][0]==0)){
							//��������å�
							if(!(myGameInfo[ _info ][2][0]
							&& (statusArray[ *Address3 ][0] == 4 || statusArray[ *Address3 ][0] == 5 || statusArray[ *Address3 ][0] == 6))){
								if(!(Flg && statusArray[ *Address3 ][0] == 3
								&& !(GetH(Address3 + 3) >= (BYTE)myGameInfo[ _para ][3][2]
								  && GetH(Address3 + 3) <= debug_height_value + (BYTE)myGameInfo[ _para ][3][2]))){	//�ߤ��a��	//�g�Y��
									Time = (DWORD)((gameTime - myGameInfo[ _status ][6][1]) / 10);
									if(((BYTE)(Time + Temp) >= *(Address3 + 1) && (BYTE)Time < 3+ *(Address3 + 1))){
										commandInput[0] = statusArray[ *Address3 ][2];
										decodeTime = 0;
										commandInput[3] = GetL(Address3 + 2);
										#if debug_mode_BackAI
											cout << "action : " << dec << (DWORD)commandInput[0] << endl;
										#endif
									}
								}
							}
						}
					}
				}
			}
		}
		
		
	}
}

void boosterDatClass::CallBackAI(){
	if(eigenValueBack[0][0]){
//		Line==0��ȫ��ˌ�����Хåե��Ȥ���ʹ��
//		BackAIBuf[0][0][0] = ;
//		BackAIBuf[0][1][0] = ;
//		BackAIBuf[0][2][0] = ;
//		BackAIBuf[0][3][0] = myGameInfo[ _status ][5][1];	//�r�g
//		BackAIBuf[0][4][0] = myGameInfo[ _status ][6][0];	//����ID
//		BackAIBuf[0][5][0] = myGameInfo[ _para ][3][2];	//�ߤ��a�����
		
		if(BackAIBuf[0][4][0]==0){
			BackAIBuf[0][4][0] = 0xFF;
		}
		
		Flg = 0;
		if(myGameInfo[ _status ][2][2]){
			if(myGameInfo[ _status ][2][2] != __5 && myGameInfo[ _status ][2][2] != __4 && myGameInfo[ _status ][2][2] != __1
			&& myGameInfo[ _status ][2][2] != __22D && myGameInfo[ _status ][0][0] != 8
			&& myGameInfo[ _status ][5][0] != 0x90 && myGameInfo[ _status ][5][0] != 0x91 && myGameInfo[ _status ][5][0] != 0x92 && myGameInfo[ _status ][5][0] != 0x93
			){
				if(myGameInfo[ _info ][0][1]){
					//�����
					Flg = 1;
				}
			}
		}
		if(Flg){
			for(Line=1; Line<10; Line++){
				if(BackAIBuf[Line][0][0]==0){
					//�饤�����O
					#if debug_mode_BackAI
						cout << "�饤�����O" << endl;
					#endif
					BackAIBuf[Line][0][0] = 0xFF;
					BackAIBuf[Line][1][0] = 0;
					BackAIBuf[Line][2][0] = 0;
					BackAIBuf[Line][3][0] = BackAIBuf[0][3][0];	//ǰ�����r�g
					BackAIBuf[Line][4][0] = BackAIBuf[0][4][0];	//ǰ�Є�
					BackAIBuf[Line][5][0] = 0;
					
					BackAIBuf[Line][1][1] = 128;	//���v
					BackAIBuf[Line][2][1] = 64;	//ӡ��
					Line = 10;
				}
			}
		}
		for(Line=1; Line<10; Line++){
			if(BackAIBuf[Line][0][0]){
				if(Flg && BackAIBuf[Line][0][0]==0xFF){
					if(BackAIBuf[Line][0][0]==0xFF){
						BackAIBuf[Line][0][0] = 1;
					}else{
						BackAIBuf[Line][0][0] = BackAIBuf[Line][0][0] + 1;
					}
					Index = BackAIBuf[Line][0][0];
					
					BackAIBuf[Line][0][Index] = 0;
					Address = BackAIbase + eigenValueBack[0][3];
					for(Counter=0; Counter<1200; Counter+=40){
						Address2 = Address + Counter;
						if(*Address2 == (BYTE)BackAIBuf[Line][4][Index -1]){	//�Է�ǰ�Є�
							Counter = 1200;
							BackAIBuf[Line][0][Index] = (DWORD)Address2;
						}else{
							if(*Address2 == 0){
								Counter = 1200;
								BackAIBuf[Line][0][Index] = (DWORD)Address2;
								*Address2 = (BYTE)BackAIBuf[Line][4][Index - 1];
							}
						}
					}
					if(Index != 1){
						BackAIBuf[Line][1][Index] = BackAIBuf[Line][1][Index - 1];	//���v
						BackAIBuf[Line][2][Index] = BackAIBuf[Line][2][Index - 1];	//ӡ��
					}
					BackAIBuf[Line][3][Index] = (myGameInfo[ _status ][5][1] - BackAIBuf[Line][3][Index -1]) / 10;	//�r�g��
					BackAIBuf[Line][4][Index] = myGameInfo[ _status ][5][0];	//������ID��
					BackAIBuf[Line][5][Index] = BackAIBuf[0][5][0];			//�ߤ��a�����
				}
				if(BackAIBuf[Line][0][0]==0xFF){
					Index = 1;
				}else{
					Index = BackAIBuf[Line][0][0];
				}
				if(BackAIBuf[Line][1][Index] >= __magnification){
					BackAIBuf[Line][1][Index] = BackAIBuf[Line][1][Index] - __magnification;	//���v���Ϝp�٤���
				}else{
					BackAIBuf[Line][1][Index] = 0;
				}
				
				if(*enGameInfo[ _status ][2][2]){
					BackAIBuf[Line][1][Index] = BackAIBuf[Line][1][Index] * 3 / 4;
				}
				
				if(*enGameInfo[ _para ][5][1] || *enGameInfo[ _status ][9][1]==0x22){
					#if debug_mode_BackAI
						cout << "�����`��" << endl;
					#endif
					if(BackAIBuf[Line][2][Index] <= 245){
						BackAIBuf[Line][2][Index] = BackAIBuf[Line][2][Index] + 10;
					}else{
						BackAIBuf[Line][2][Index] = 255;
					}
					BackAIBuf[Line][1][Index] = 0;	//���v���Ф�
				}
				
				if(myGameInfo[ _para ][5][1] && (myGameInfo[ _status ][0][0]==2 || myGameInfo[ _status ][0][0]==9 || myGameInfo[ _status ][9][1]==0x22)){
					#if debug_mode_BackAI
						cout << "������`��" << endl;
					#endif
					if(BackAIBuf[Line][2][Index] >= 10){
						BackAIBuf[Line][2][Index] = BackAIBuf[Line][2][Index] - 10;
					}else{
						BackAIBuf[Line][2][Index] = 0;
					}
					BackAIBuf[Line][1][Index] = 0;	//���v���Ф�
				}
				
				if(Index >= 19 || (BackAIBuf[Line][1][Index] < 5 && myGameInfo[ _input ][9][0] == 0)){	//���vҪ�{��
					//�K��
					#if debug_mode_BackAI
						cout << hex << playerSide << ".line" << Line << " Close" << endl;
					#endif
					
					if(BackAIBuf[Line][0][Index]){
						Address2 = (BYTE*)BackAIBuf[Line][0][Index];
						BackAIBuf[Line][0][Index] = 0;
						if(*Address2 == (BYTE)BackAIBuf[Line][4][Index -1]){	//�_�J
							for(Counter2=4; Counter2<40; Counter2+=4){
								Address3 = Address2 + Counter2;
								if(*(Address3)==(BYTE)BackAIBuf[Line][4][Index]){
									if(BackAIBuf[Line][2][Index] > 64){
										//��ӡ��
										#if debug_mode_BackAI
											cout << "��ӡ��" << endl;
										#endif
										//�r�g��΄I��
										if((BYTE)BackAIBuf[Line][3][Index]    < 2+ *(Address3 + 1)
										&& (BYTE)BackAIBuf[Line][3][Index] +2 >    *(Address3 + 1)){	//�r�g��٤ʤ�
											//�r�g�����Ϥ�
											FloatH(Address3 + 2, 1);
											
											//�u���Ϥ�	//+3
											FloatL(Address3 + 3, 3);
										}else{
											//�r�g����
											if(GetH(Address3 + 2) < 0xA){
												if((BYTE)BackAIBuf[Line][3][Index] > *(Address3 + 1)){
													if(*(Address3 + 1) < 0xFF){
														*(Address3 + 1) = *(Address3 + 1) + 1;
													}
												}else{
													if(*(Address3 + 1)){
														*(Address3 + 1) = *(Address3 + 1) - 1;
													}
												}
											}
										}
										
										//�LѺ���r�g
										if((BYTE)myGameInfo[ _input ][9][2] > GetL(Address3 + 2)){
											if((BYTE)myGameInfo[ _input ][9][2] > 4+ GetL(Address3 + 2)){
												FloatL(Address3 + 2, 2);
											}else{
												FloatL(Address3 + 2, 1);
											}
										}
										if((BYTE)myGameInfo[ _input ][9][2] < GetL(Address3 + 2)){
											if((BYTE)myGameInfo[ _input ][9][2] +4 < GetL(Address3 + 2)){
												FloatL(Address3 + 2, -2);
											}else{
												FloatL(Address3 + 2, -1);
											}
										}
										
										//�ߤ��a��
										if((BYTE)BackAIBuf[Line][5][Index] > GetH(Address3 + 3)){
											FloatH(Address3 + 3, 1);
										}
										if((BYTE)BackAIBuf[Line][5][Index] < GetH(Address3 + 3)){
											FloatH(Address3 + 3, -1);
										}
										
										if(Counter2 != 4){
											if(GetL(Address3 + 3) >= GetL(Address3 - 1)){
												DWORD Temp;
												Temp				= *(DWORD*)(Address3 - 4);
												*(DWORD*)(Address3 - 4)	= *(DWORD*)(Address3);
												*(DWORD*)(Address3)	= Temp;
											}
										}
									}else{
										//��ӡ��Ǥʤ�
										#if debug_mode_BackAI
											cout << "��ӡ��ǤϤʤ�" << endl;
										#endif
										if((BYTE)BackAIBuf[Line][3][Index]    < 3+ *(Address3 + 1)
										&& (BYTE)BackAIBuf[Line][3][Index] +3 >    *(Address3 + 1)){	//�r�g��٤ʤ�
											//�r�g�����¤�
											FloatH(Address3 + 2, -1);
											
											//�u���¤�
											FloatL(Address3 + 3, -1);
										}
										if(Counter2 != 36){
											if(*(Address3 + 4)){
												if(GetL(Address3 + 3) <= GetL(Address3 + 7)){
													DWORD Temp;
													Temp                    = *(DWORD*)(Address3);
													*(DWORD*)(Address3) 	= *(DWORD*)(Address3 + 4);
													*(DWORD*)(Address3 + 4) = Temp;
												}
											}else{
												//�Τ��ʤ�
												//�F״�Ǥ�����Ф�
												/*
												if(*(Address3 + 3) < 10){
													*(DWORD*)(Address3) = 0;
												}
												*/
											}
										}
									}
									Counter2 = 40;
								}else{
									if(*(Address3)==0 || Counter2 == 36){
										Counter2 = 40;
										if(BackAIBuf[Line][2][Index] > 64){
											#if debug_mode_BackAI
												cout << "��Ҏ" << endl;
											#endif
											*(Address3) = (BYTE)BackAIBuf[Line][4][Index];		//����ID
											*(Address3 + 1) = (BYTE)BackAIBuf[Line][3][Index];	//�r�g��
											SetH(Address3 + 2, 8);	//�r�g���ȳ��ڂ�
											SetL(Address3 + 2, (BYTE)myGameInfo[ _input ][9][2]);	//�LѺ���r�g
											SetH(Address3 + 3, (BYTE)BackAIBuf[Line][5][Index]);
											SetL(Address3 + 3, 0);	//�u�����ڂ�
										}
									}
								}
							}
						}
					}
					BackAIBuf[Line][0][0] = 0;
				}
			}
		}
		
		BackAIBuf[0][3][0] = myGameInfo[ _status ][6][1];	//�r�g
		BackAIBuf[0][4][0] = myGameInfo[ _status ][6][0];	//����ID
		BackAIBuf[0][5][0] = myGameInfo[ _para ][3][2];
		
		
		//���Х��ɥ쥹����
		if(myGameInfo[ _para ][1][1] > 3){
			eigenValueBack[1][2] = 3;
		}else{
			eigenValueBack[1][2] = myGameInfo[ _para ][1][1];	//���x	//���ɶ�5
		}
		if(myGameInfo[ _info ][1][0] == 2 && eigenValueBack[1][2] < 2){
			eigenValueBack[1][2] = 4;
		}
		eigenValueBack[2][2] = myGameInfo[ _para ][2][1];	//�Էָߤ�	//���ɶ�3
		eigenValueBack[3][2] = *enGameInfo[ _para ][2][2];	//���ָߤ�	//���ɶ�4
		
		
		for(Counter=9; Counter>0; Counter--){
			if(eigenValueBack[Counter][0]){
				if(eigenValueBack[Counter][2] >= eigenValueBack[Counter][0]){		//��������å�
					eigenValueBack[Counter][2] = eigenValueBack[Counter][0] -1;
				}											//���Х��ɥ쥹Ӌ��
				eigenValueBack[Counter - 1][3] = eigenValueBack[Counter][1] * eigenValueBack[Counter][2] + eigenValueBack[Counter][3];
			}
		}
	}
}
