#include "conf.h"
#include "boosterDatClass.h"
#include "HL.h"

//�LѺ��
//�r�g����

void boosterDatClass::ReadSNAI(){
	if(eigenValueSN[0][0]){
		
		DWORD Temp = 0;
		Flg = 0;
		
		#if debug_height
			if(myGameInfo[ _para ][2][0] && *enGameInfo[ _para ][2][0]){	//�g�Y��
//			if(*enGameInfo[ _para ][2][0]){
				Temp = 2;
			}
			if(gameInfoPara[15][1]==4){
				Temp += 10;
			}
			
			if(*enGameInfo[ _para ][2][0] && (*enGameInfo[ _status ][0][0] == 2 || *enGameInfo[ _status ][0][0] == 9)){
				Flg = 1;
			}
		#endif
		
		Address = SNAIbase + eigenValueSN[0][3];
		for(Counter=0; Counter<2400; Counter+=80){
			Address2 = Address + Counter;
			if(*Address2 == 0){
				Counter = 2400;
			}else{
				if(*Address2 == (BYTE)myGameInfo[ _status ][6][0]){	//�Է�ǰ�Є�
					Counter = 2400;
					for(Counter2=8; Counter2<80; Counter2+=8){
						Address3 = Address2 + Counter2;
						if(*Address3==0){
							Counter2 = 80;
						}else{
							if(!(*Address3==0x24 && myGameInfo[ _info ][4][0]==0)){
								//��������å�
//								if( GetL(Address3 + 7) > 9 ){
									if(!(myGameInfo[ _info ][2][0]
									&& (statusArray[ *Address3 ][0] == 4 || statusArray[ *Address3 ][0] == 5 || statusArray[ *Address3 ][0] == 6))){
										Counter2 = 80;
										Time = (DWORD)((gameTime - myGameInfo[ _status ][6][1]) / 4);
										if(!(Flg && statusArray[ *Address3 ][0] == 3
										&& !(GetH(Address3 + 3) >= (BYTE)myGameInfo[ _para ][3][2]
										  && GetH(Address3 + 3) <= debug_height_value + (BYTE)myGameInfo[ _para ][3][2]))){	//�ߤ��a��	//�g�Y��
											if(((BYTE)(Time + Temp) >= *(Address3 + 1) && (BYTE)Time < 3+ *(Address3 + 1)) || myGameInfo[ _status ][6][0]==0xFF){
												commandInput[0] = statusArray[ *Address3 ][2];
												decodeTime = 0;
												commandInput[3] = GetL(Address3 + 2);
												#if debug_mode_SNAI
													cout << "action : " << dec << (DWORD)commandInput[0] << endl;
												#endif
											}
										}
									}
//								}
							}
						}
					}
				}
			}
		}
	}
}

void boosterDatClass::CallSNAI(){
	if(eigenValueSN[0][0]){
//		Line==0��ȫ��ˌ�����Хåե��Ȥ���ʹ��
//		SNAIBuf[0][0][0] = ;
//		SNAIBuf[0][1][0] = ;
//		SNAIBuf[0][2][0] = ;
//		SNAIBuf[0][3][0] = myGameInfo[ _status ][5][1];	//�r�g	[0][3][1] = �r�g��(Read��)
//		SNAIBuf[0][4][0] = myGameInfo[ _status ][6][0];	//����ID
//		SNAIBuf[0][5][0] = myGameInfo[ _para ][3][2];	//�ߤ��a�����
//		�B�i���Ĥϡ����餤or���Х�����״�B�Ǥ�Index���M���Ȥ�������
		
		if(SNAIBuf[0][4][0]==0){
			SNAIBuf[0][4][0] = 0xFF;
		}
		
		Flg = 0;
		if(myGameInfo[ _status ][2][2]){
			if(!(myGameInfo[ _info ][1][0] == 2 && myGameInfo[ _para ][1][0] < 3)){	//�������H�Ǥʤ�
				if(myGameInfo[ _status ][2][2] != __5 && myGameInfo[ _status ][2][2] != __4 && myGameInfo[ _status ][2][2] != __1
//				&& myGameInfo[ _status ][2][2] != __D4 && myGameInfo[ _status ][2][2] != __D7
				&& myGameInfo[ _status ][2][2] != __22D && myGameInfo[ _status ][0][0] != 8
				&& myGameInfo[ _status ][5][0] != 0x90 && myGameInfo[ _status ][5][0] != 0x91 && myGameInfo[ _status ][5][0] != 0x92 && myGameInfo[ _status ][5][0] != 0x93
				){
					if(!(myGameInfo[ _para ][1][0] > 4 && (myGameInfo[ _status ][2][2] == __D4 || myGameInfo[ _status ][2][2] == __D7))){
						if(myGameInfo[ _info ][0][1]==0){
							//����򤭤Ǥʤ�
							Flg = 1;
							if(myGameInfo[ _status ][2][2] == __D4 || myGameInfo[ _status ][2][2] == __D7){
								if(statusArray[ (BYTE)SNAIBuf[0][4][0] ][2] == __D4 || statusArray[ (BYTE)SNAIBuf[0][4][0] ][2] == __D7){
									Flg = 0;
								}
							}
						}
					}
				}
			}
		}
		
		if(Flg){
			for(Line=1; Line<20; Line++){
				if(SNAIBuf[Line][0][0]==0){
					//�饤�����O
					#if debug_mode_SNAI
						cout << "�饤�����O" << endl;
					#endif
					SNAIBuf[Line][0][0] = 0xFF;
					SNAIBuf[Line][1][0] = 0xFF;	//�L�����v
					SNAIBuf[Line][2][0] = 64;	//�����ӡ��
					SNAIBuf[Line][3][0] = SNAIBuf[0][3][0];	//ǰ�����r�g
					SNAIBuf[Line][4][0] = SNAIBuf[0][4][0];	//ǰ�Є�
					SNAIBuf[Line][5][0] = 0;
					
					//ӡ����ڂ��O��
					if(AIMode==2){
						SNAIBuf[Line][2][1] = 64;
					}else{
						SNAIBuf[Line][2][1] = 65;
					}
					//���v���ڂ��O��
					SNAIBuf[Line][1][1] = 0xFF;
					
					if(myGameInfo[ _status ][2][2]==__22C){
						//둓�
						SNAIBuf[Line][1][1] = 0x90;	//�̤����v
						SNAIBuf[Line][2][1] = 64;	//ӡ��
					}
					if(myGameInfo[ _status ][2][2] == __D4 || myGameInfo[ _status ][2][2] == __D7){
						//����å���
						SNAIBuf[Line][1][1] = 0xFF;	//�̤����v
						SNAIBuf[Line][2][1] = 64;	//ӡ��
					}
					Line = 20;
				}
			}
		}
		for(Line=1; Line<20; Line++){
			if(SNAIBuf[Line][0][0]){
				if(Flg && SNAIBuf[Line][0][0]==0xFF){
					if(SNAIBuf[Line][0][0]==0xFF){
						SNAIBuf[Line][0][0] = 1;
					}else{
						SNAIBuf[Line][0][0] = SNAIBuf[Line][0][0] + 1;
					}
					Index = SNAIBuf[Line][0][0];
					
					SNAIBuf[Line][0][Index] = 0;
					Address = SNAIbase + eigenValueSN[0][3];
					for(Counter=0; Counter<2400; Counter+=80){
						Address2 = Address + Counter;
						if(*Address2 == (BYTE)SNAIBuf[Line][4][Index -1]){	//�Է�ǰ�Є�
							Counter = 2400;
							SNAIBuf[Line][0][Index] = (DWORD)Address2;
						}else{
							if(*Address2 == 0){
								Counter = 2400;
								SNAIBuf[Line][0][Index] = (DWORD)Address2;
								*Address2 = (BYTE)SNAIBuf[Line][4][Index - 1];
							}
						}
					}
					if(Index != 1){
						SNAIBuf[Line][1][Index] = SNAIBuf[Line][1][Index - 1];	//���v
						SNAIBuf[Line][2][Index] = SNAIBuf[Line][2][Index - 1];	//ӡ��
					}
					SNAIBuf[Line][3][Index] = (myGameInfo[ _status ][5][1] - SNAIBuf[Line][3][Index -1]) / 4;	//�r�g��
					SNAIBuf[Line][4][Index] = myGameInfo[ _status ][5][0];	//������ID��
					SNAIBuf[Line][5][Index] = SNAIBuf[0][5][0];			//�ߤ��a�����
				}
				if(SNAIBuf[Line][0][0]==0xFF){
					Index = 1;
				}else{
					Index = SNAIBuf[Line][0][0];
				}
				
				//�L�����v
				if(SNAIBuf[Line][1][0] > __magnification){
					SNAIBuf[Line][1][0] = SNAIBuf[Line][1][0] - __magnification;
				}else{
					SNAIBuf[Line][1][0] = 0;
				}
				//�̤����v
				if(SNAIBuf[Line][1][Index] > __magnification){
					SNAIBuf[Line][1][Index] = SNAIBuf[Line][1][Index] - __magnification;	//���v���Ϝp�٤���
				}else{
					SNAIBuf[Line][1][Index] = 0;
				}
				
				if(*enGameInfo[ _status ][2][2]){
					SNAIBuf[Line][1][Index] = SNAIBuf[Line][1][Index] * 3 / 4;
				}
				
				if(*enGameInfo[ _para ][5][1] || *enGameInfo[ _status ][9][1]==0x22){
					#if debug_mode_SNAI
						cout << "�����`��" << endl;
					#endif
					if(SNAIBuf[Line][2][Index] <= 245){
						SNAIBuf[Line][2][Index] = SNAIBuf[Line][2][Index] + 10;
					}else{
						SNAIBuf[Line][2][Index] = 255;
					}
					SNAIBuf[Line][1][Index] = 0;	//���v���Ф�
				}
				
				if(myGameInfo[ _para ][5][1] && (myGameInfo[ _status ][0][0]==2 || myGameInfo[ _status ][0][0]==9 || myGameInfo[ _status ][9][1]==0x22)){
					#if debug_mode_SNAI
						cout << "������`��" << endl;
					#endif
					if(SNAIBuf[Line][2][Index] >= 10){
						SNAIBuf[Line][2][Index] = SNAIBuf[Line][2][Index] - 10;
					}else{
						SNAIBuf[Line][2][Index] = 0;
					}
					SNAIBuf[Line][1][Index] = 0;	//���v���Ф�
				}
				
				if(Index >= 19 || (SNAIBuf[Line][1][Index] < 5 && myGameInfo[ _input ][9][0] == 0)){	//���vҪ�{��
					//�K��
					#if debug_mode_SNAI
						cout << hex << playerSide << ".line" << Line << " Close" << endl;
					#endif
					
					if(SNAIBuf[Line][0][Index]){
						Address2 = (BYTE*)SNAIBuf[Line][0][Index];
						SNAIBuf[Line][0][Index] = 0;
						if(*Address2 == (BYTE)SNAIBuf[Line][4][Index -1]){	//�_�J
							for(Counter2=8; Counter2<80; Counter2+=8){
								Address3 = Address2 + Counter2;
								if(*(Address3)==(BYTE)SNAIBuf[Line][4][Index]){
									if(SNAIBuf[Line][2][Index] > 64){
//									if((SNAIBuf[Line][2][Index] >= 64 && AIMode==1) || (SNAIBuf[Line][2][Index] > 64 && AIMode==2)){	//�g�Y��
//									if((SNAIBuf[Line][2][Index] >= 64 && gameMode==5) || SNAIBuf[Line][2][Index] > 64){	//�g�Y��
										//��ӡ��
										#if debug_mode_SNAI
											cout << "��ӡ��" << endl;
										#endif
										//�r�g��΄I��
										if((BYTE)SNAIBuf[Line][3][Index]    < 2+ *(Address3 + 1)
										&& (BYTE)SNAIBuf[Line][3][Index] +2 >    *(Address3 + 1)){	//�r�g��٤ʤ�
											FloatH(Address3 + 2, 1);
											FloatL(Address3 + 7, 1);
										}else{
											//�r�g����
											//�����Ф�	//Ҫ��ӑ
											if((BYTE)SNAIBuf[Line][3][Index] > *(Address3 + 1)){
												if(*(Address3 + 1) < 0xFF){
													*(Address3 + 1) = *(Address3 + 1) + 1;
												}
											}
											if((BYTE)SNAIBuf[Line][3][Index] < *(Address3 + 1)){
												if(*(Address3 + 1)){
													*(Address3 + 1) = *(Address3 + 1) - 1;
												}
											}
											if(GetH(Address3 + 2) < 0xA){
												if((BYTE)SNAIBuf[Line][3][Index] > *(Address3 + 1)){
													if(*(Address3 + 1) < 0xFF){
														*(Address3 + 1) = *(Address3 + 1) + 1;
													}
												}
												if((BYTE)SNAIBuf[Line][3][Index] < *(Address3 + 1)){
													if(*(Address3 + 1)){
														*(Address3 + 1) = *(Address3 + 1) - 1;
													}
												}
											}
										}
										
										//�ߤ��a��
										if((BYTE)SNAIBuf[Line][5][Index]    < 1+ GetH(Address3 + 3)
										&& (BYTE)SNAIBuf[Line][5][Index] +1 >    GetH(Address3 + 3)){
											//�ߤ�������
											FloatL(Address3 + 3, 1);
										}else{
											//�ߤ��������ʤ�
											FloatL(Address3 + 3, -1);	//�����¤�
										}
										if(GetL(Address3 + 3) < 0xA){
											//�ߤ��a���g��
											if((BYTE)SNAIBuf[Line][5][Index] > GetH(Address3 + 3)){
												FloatH(Address3 + 3, 1);
											}
											if((BYTE)SNAIBuf[Line][5][Index] < GetH(Address3 + 3)){
												FloatH(Address3 + 3, -1);
											}
										}
										
										//�LѺ��ָ��
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
										
										if(Counter2 != 8){
											if(GetL(Address3 + 7) >= GetL(Address3 - 1)){
												ULONGLONG Temp;
												Temp                        = *(ULONGLONG*)(Address3 - 8);
												*(ULONGLONG*)(Address3 - 8) = *(ULONGLONG*)(Address3);
												*(ULONGLONG*)(Address3)     = Temp;
											}
										}
									}
									if(SNAIBuf[Line][2][Index] < 64){
										//��ӡ��
										#if debug_mode_SNAI
											cout << "��ӡ��" << endl;
										#endif
										if((BYTE)SNAIBuf[Line][3][Index]    < 3+ *(Address3 + 1)
										&& (BYTE)SNAIBuf[Line][3][Index] +3 >    *(Address3 + 1)){	//�r�g��٤ʤ�
											FloatH(Address3 + 2, -1);
											FloatL(Address3 + 7, -1);
										}
										if(Counter2 != 72){
											if(*(Address3 + 8)){
												if(GetL(Address3 + 15) >= GetL(Address3 + 7)){
													ULONGLONG Temp;
													Temp                    = *(ULONGLONG*)(Address3);
													*(ULONGLONG*)(Address3) = *(ULONGLONG*)(Address3 + 8);
													*(ULONGLONG*)(Address3 + 8)    = Temp;
												}
											}else{
												//�Τ��ʤ�
												if(GetL(Address3 + 7) < 6){
													*(ULONGLONG*)(Address3) = 0;
												}
											}
										}
									}
									Counter2 = 80;
								}else{
									if(*(Address3)==0 || Counter2 == 72){
										Counter2 = 80;
										if(SNAIBuf[Line][2][Index] > 64){
//										if((SNAIBuf[Line][2][Index] >= 64 && AIMode==1) || (SNAIBuf[Line][2][Index] > 64 && AIMode==2)){	//�g�Y��
//										if((SNAIBuf[Line][2][Index] >= 64 && gameMode==5) || SNAIBuf[Line][2][Index] > 64){	//�g�Y��
											#if debug_mode_SNAI
												cout << "��Ҏ" << endl;
											#endif
											*(Address3) = (BYTE)SNAIBuf[Line][4][Index];		//����ID
											*(Address3 + 1) = (BYTE)SNAIBuf[Line][3][Index];	//�r�g��
											*(Address3 + 2) = (BYTE)myGameInfo[ _input ][9][2];
											SetH(Address3 + 3, (BYTE)SNAIBuf[Line][5][Index]);
											SetL(Address3 + 3, 8);
											*(Address3 + 7) = 8;
										}
									}
								}
							}
						}
					}
					SNAIBuf[Line][0][0] = 0;
				}
			}
		}
		
		SNAIBuf[0][3][0] = myGameInfo[ _status ][6][1];	//�r�g
		SNAIBuf[0][4][0] = myGameInfo[ _status ][6][0];	//����ID
		SNAIBuf[0][5][0] = myGameInfo[ _para ][3][2];
		
		//���Х��ɥ쥹����
		eigenValueSN[1][2] = myGameInfo[ _para ][1][0];	//���x
		eigenValueSN[2][2] = myGameInfo[ _para ][2][2];	//�Էָߤ�	//�g�Y��
		eigenValueSN[3][2] = *enGameInfo[ _para ][2][0];	//���ָߤ�	//�g�Y��
		eigenValueSN[4][2] = *enGameInfo[ _status ][1][0];	//�ƥ�״�B���2
		
		
		for(Counter=9; Counter>0; Counter--){
			if(eigenValueSN[Counter][0]){
				if(eigenValueSN[Counter][2] >= eigenValueSN[Counter][0]){		//��������å�
					eigenValueSN[Counter][2] = eigenValueSN[Counter][0] -1;
				}											//���Х��ɥ쥹Ӌ��
				eigenValueSN[Counter - 1][3] = eigenValueSN[Counter][1] * eigenValueSN[Counter][2] + eigenValueSN[Counter][3];
			}
		}
	}
}
