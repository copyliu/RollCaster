#include "conf.h"
#include "boosterDatClass.h"
#include "HL.h"

void boosterDatClass::ReadLNAI(){
//	if(eigenValueLN[0][0] && *enGameInfo[ _status ][5][0] != 0){
	if(eigenValueLN[0][0] && *enGameInfo[ _status ][5][0] != 0 && *enGameInfo[ _status ][0][0] != 2 && *enGameInfo[ _status ][0][0] != 9){	//�g�Y��
		
		//����
		//������ManageAI���Ф��٤�����
		BYTE	commandTemp = 0;
		DWORD	decodeTimeTemp = 0;
		
		
		//�Է�ǰ�ЄӤ����
		Address = LNAIbase + eigenValueLN[0][3];
		for(Counter=1000; Counter<50000; Counter+=1000){
			Address2 = Address + Counter;
			if(*(Address2) == (BYTE)myGameInfo[ _status ][5][0]){
				Counter = 50000;
			}
			if(Counter == 49000 || *(Address2)==0){	//hit���Ƥ�����50000�ˤʤäƤ���Q
				Address2 = 0;
				Counter = 50000;
			}
		}
		if(Address2){
			for(Line=0; Line<20; Line++){
				if(LNAIBuf[Line][0][0] && LNAIBuf[Line][0][0] != 0xFF){
					if(LNAIBuf[Line][1][0] == 4 || LNAIBuf[Line][1][0] == 5 || LNAIBuf[Line][1][0] == 8){
						for(Counter2=20; Counter2<1000; Counter2+=20){
							Address3 = Address2 + Counter2;
							if(*(Address3) == 0){
								Counter2 = 1000;
							}else{
								if(*(Address3) == (BYTE)LNAIBuf[Line][2][0]){
									Counter2 = 1000;
									Address4 = Address3 + 4;
									if(*Address4 && (GetL(Address4 + 2) > 0xA || *(Address3 + 2)==0)){
										if(*(Address4 + 1) == (BYTE)((gameTime - LNAIBuf[Line][3][0]) / 10)){
											commandTemp = *Address4;
											decodeTimeTemp = 0;
										}
									}else{
										if(*(Address3 + 2)
										&& (LNAIBuf[Line][1][0]== 3 || LNAIBuf[Line][1][0]== 4 || LNAIBuf[Line][1][0]== 5
										 || LNAIBuf[Line][1][0]== 7 || LNAIBuf[Line][1][0]== 8)){
											if(*(Address3 + 1) == (BYTE)((gameTime - LNAIBuf[Line][3][0]) / 10)){
												if(GetL(Address3 + 2) > 0x7){
													commandTemp = __4;
												}else{
													commandTemp = __1;
												}
												decodeTimeTemp = 0;
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
		Address = LNAIbase + eigenValueLN[0][3];
		if(*(Address) == 0xFF){					//����ϱؤ�0xFF
			Address2 = Address;
			for(Line=0; Line<20; Line++){
				if(LNAIBuf[Line][0][0] == 0xFF){
					for(Counter2=20; Counter2<1000; Counter2+=20){
						Address3 = Address2 + Counter2;
						if(*(Address3) == 0){
							Counter2 = 1000;
						}else{
							if(*(Address3) == (BYTE)LNAIBuf[Line][2][0]){
								Counter2 = 1000;
								Address4 = Address3 + 4;
								if(*Address4 && (GetL(Address4 + 2) > 0xA || *(Address3 + 2)==0)){
									if(*(Address4 + 1) == (BYTE)((gameTime - LNAIBuf[Line][3][0]) / 10)){
										//�g�Y��	//������ΥХå����å����o���ˤ���
										if( !(*Address4==__D4 && *(Address4 + 1)<10 && myGameInfo[ _para ][1][1]<2) ){
											commandTemp = *Address4;
											decodeTimeTemp = 0;
										}
									}
								}else{
									if(*(Address3 + 2)
									&& (LNAIBuf[Line][1][0]== 3 || LNAIBuf[Line][1][0]== 4 || LNAIBuf[Line][1][0]== 5
									 || LNAIBuf[Line][1][0]== 7 || LNAIBuf[Line][1][0]== 8)){
										if(*(Address3 + 1) == (BYTE)((gameTime - LNAIBuf[Line][3][0]) / 10)){
											if(GetL(Address3 + 2) > 0x5){
												commandTemp = __4;
											}else{
												commandTemp = __1;
											}
											decodeTimeTemp = 0;
										}
									}
								}
							}
						}
					}
				}
			}
		}
		
		//����򤭤ΤȤ��΄I��
		if(myGameInfo[ _info ][0][1]){
			if(commandTemp == __D4 || commandTemp == __D6 || commandTemp == __D7 || commandTemp == __D8 || commandTemp == __D9
			|| commandTemp == __7 || commandTemp == __9
			|| commandTemp == __1 || commandTemp == __4){
				if(commandTemp == __D4 || commandTemp == __D6){
					if(commandTemp == __D4){
						commandTemp = __D6;
					}else{
						commandTemp = __D4;
					}
				}
				if(commandTemp == __D7 || commandTemp == __D9){
					if(commandTemp == __D7){
						commandTemp = __D9;
					}else{
						commandTemp = __D7;
					}
				}
				if(commandTemp == __7 || commandTemp == __9){
					if(commandTemp == __7){
						commandTemp = __9;
					}else{
						commandTemp = __7;
					}
				}
			}else{
				commandTemp = 0;
			}
		}
		if(commandTemp){
			commandInput[0] = commandTemp;
			decodeTime = decodeTimeTemp;
			commandInput[3] = 0;
		}
		
	}
}


void boosterDatClass::CallLNAI(){
	if(eigenValueLN[0][0]){
		if( !( gameTime < intervalFlg || gameTime < enDat->intervalFlg ) ){
			if(*enGameInfo[ _status ][2][2] && *enGameInfo[ _status ][2][2] != __5){
				for(Line=0; Line<20; Line++){
					if(LNAIBuf[Line][0][0]==0){
						#if debug_mode_LNAI
							cout << "�饤�����O" << endl;
						#endif
						LNAIBuf[Line][0][0] = 0xFF;					//���������ڻ�
						LNAIBuf[Line][1][0] = *enGameInfo[ _status ][0][0];	//���Єӥ����׷��
						LNAIBuf[Line][2][0] = *enGameInfo[ _status ][5][0];	//������ID
						LNAIBuf[Line][3][0] = *enGameInfo[ _status ][5][1];	//�������r�g	//[5][1]�ȤΥ��`�Щ`�ե�`�Ȥ��Ͻ���
						LNAIBuf[Line][4][0] = 0xFF;					//�����_ʼ	//�ɤ�״�B���������Τ�ָ�����ʤ��Ȥ�����ζ
						
						if(LNAIBuf[Line][1][0]==5 || LNAIBuf[Line][1][0]==8){
							LNAIBuf[Line][1][1] = 0xFF;		//���v�L��
						}else{
							LNAIBuf[Line][1][1] = 128;		//���v
						}
						LNAIBuf[Line][2][1] = 64;			//ӡ�󂎳��ڻ�
						Line = 20;
					}
				}
			}
		}
		for(Line=0; Line<20; Line++){
			if(LNAIBuf[Line][0][0]){
				//DWORD LNAIBuf[10][5][20]
				//LNAIBuf[Line][0][0]������
				//LNAIBuf[Line][1][0]�����������׷��
				//LNAIBuf[Line][2][0]������ID
				//LNAIBuf[Line][3][0]�������r�g
				//LNAIBuf[Line][4][0]ǰ����ID
				//type 0:���Х��ɥ쥹 1:���v�� 2:ӡ�� 3:�r�g 4:����ID
				
				//����������饤��΄I��
				//Line��Index�ϻ����Ĥˉ�����ʤ�
				
				/*
				//�ӻ���ɤ����뤫
				//�I������������������Ȥꤢ��������
				*/
				if(myGameInfo[ _status ][2][2] && myGameInfo[ _status ][2][2] != __5){
					if(LNAIBuf[Line][0][0] == 0xFF){
						LNAIBuf[Line][0][0] = 1;
					}else{
						LNAIBuf[Line][0][0] = LNAIBuf[Line][0][0] + 1;	//���������M���
					}
					Index = LNAIBuf[Line][0][0];
					
					//���Х��ɥ쥹Ӌ�㣨�ǩ`����ʼ��ޤǗ�����
					//�����z���������o���ä����Ϥ�0
					//0xFF�������헤˕����z��
					LNAIBuf[Line][0][Index] = 0;
					if(myGameInfo[ _status ][2][2] != __22C && myGameInfo[ _status ][2][2] != __22D && myGameInfo[ _status ][2][2] != __236D && myGameInfo[ _status ][2][2] != __214D){
						if(LNAIBuf[Line][0][0]==1){
							Counter = 0;
						}else{
							Counter = 1000;
						}
						Address = LNAIbase + eigenValueLN[0][3];
						for(Counter; Counter<50000; Counter+=1000){
							Address2 = Address + Counter;
							if(*(Address2) == 0){
								*(Address2) = (BYTE)LNAIBuf[Line][4][Index -1];
							}
							if(*(Address2) == (BYTE)LNAIBuf[Line][4][Index -1]){
								Counter = 50000;
								for(Counter2=20; Counter2<1000; Counter2+=20){
									Address3 = Address2 + Counter2;
									if(*(Address3) == 0){
										*(Address3) = (BYTE)LNAIBuf[Line][2][0];
									}
									if(*(Address3) == (BYTE)LNAIBuf[Line][2][0]){
										LNAIBuf[Line][0][Index] = (DWORD)(Address3);	//���Ѕgλ���ɥ쥹
										Counter2 = 1000;
									}
								}
							}
						}
					}
					
					if(Index != 1){
						LNAIBuf[Line][1][Index] = LNAIBuf[Line][1][Index - 1] * 3 / 4;	//���v�����@��
						LNAIBuf[Line][2][Index] = LNAIBuf[Line][2][Index - 1];		//ӡ�����@��
					}
					LNAIBuf[Line][3][Index] = myGameInfo[ _status ][5][1];	//�r�g
					LNAIBuf[Line][4][Index] = myGameInfo[ _status ][5][0];	//����
				}
				
				if(LNAIBuf[Line][0][0] == 0xFF){
					Index = 1;
				}else{
					Index = LNAIBuf[Line][0][0];
				}
				
				if(LNAIBuf[Line][1][Index] >= __magnification){
					LNAIBuf[Line][1][Index] = LNAIBuf[Line][1][Index] - __magnification;	//���v���Ϝp�٤���
				}else{
					LNAIBuf[Line][1][Index] = 0;
				}
				
				if(*enGameInfo[ _status ][2][2]){
					LNAIBuf[Line][1][Index] = LNAIBuf[Line][1][Index] * 3 / 4;
				}
				
				//�ƥ���
				if(*enGameInfo[ _para ][5][1]){
					LNAIBuf[Line][1][Index] = 0;	//���v���Ф�
				}
				
				if(myGameInfo[ _para ][5][1] && (myGameInfo[ _status ][0][0]==2 || myGameInfo[ _status ][0][0]==9 || myGameInfo[ _status ][9][0]==0x22)){
					#if debug_mode_LNAI
						cout << "����`��" << endl;
					#endif
					for(Counter=1; Counter<=Index; Counter++){
						if(LNAIBuf[Line][2][Counter] >= 10){
							LNAIBuf[Line][2][Counter] = LNAIBuf[Line][2][Counter] - 10;
						}else{
							LNAIBuf[Line][2][Counter] = 0;
						}
					}
					LNAIBuf[Line][1][Index] = 0;	//���v���Ф�
				}
				
				if(myGameInfo[ _info ][2][2] && (LNAIBuf[Line][2][0]== 0x38 || LNAIBuf[Line][2][0]== 0x39)){	//�֥쥤��
					//�꼱�I��
					if(LNAIBuf[Line][0][Index]){
						Address3 = (BYTE*)LNAIBuf[Line][0][Index];
						if(*Address3==0x38){	//__22A��ID:0x38����ͨ�ʤΤ�
						#if debug_mode_LNAI
							cout << "�϶Υ��`�ɸ���" << endl;
						#endif
							FloatL(Address3 + 2, 15);
						}
						if(*Address3==0x39){	//__22B��ID : 0x39
							#if debug_mode_LNAI
								cout << "�¶Υ��`�ɸ���" << endl;
							#endif
							FloatL(Address3 + 2, -15);
						}
					}
					LNAIBuf[Line][1][Index] = 0;		//���v���Ф�
				}
				
 				if(myGameInfo[ _status ][2][2]){
 					if(statusArray[ LNAIBuf[Line][4][Index] ][0]==0xA){
	 					//�����ǥ��`���u��	//ӡ����v��餺���ЄӤ��l�Ȥˤ���Ӥ��롣
						if(LNAIBuf[Line][0][Index]){
							Address3 = (BYTE*)LNAIBuf[Line][0][Index];
		 					if(statusArray[ LNAIBuf[Line][4][Index] ][2]==__4){
		 						FloatL(Address3 + 2, 1);
		 					}
		 					if(statusArray[ LNAIBuf[Line][4][Index] ][2]==__1){	//��������إ���
		 						FloatL(Address3 + 2, -2);
		 					}
						}
					}
				}
				
				if(Index >= 19 || LNAIBuf[Line][1][Index] < 5){	//���vҪ�{��
					//�K��
					#if debug_mode_LNAI
						cout << playerSide << ".line" << Line << " Close" << endl;
					#endif
					for(Index=1; Index<=LNAIBuf[Line][0][0]; Index++){
						if(LNAIBuf[Line][2][Index] >= 64){
							if(LNAIBuf[Line][0][Index] && LNAIBuf[Line][0][0] !=0xFF){		//���ɥ쥹���_������Ƥ�����
								Address3 = (BYTE*)LNAIBuf[Line][0][Index];
								LNAIBuf[Line][0][Index] = 0;	//���ɥ쥹����ڻ�����Τ��ᣩ
								if(*Address3 == (BYTE)LNAIBuf[Line][2][0]){	//�_�J
									
									//���������u��
									//�ǩ`���ΰ�֤�ռ����Ƥ��ޤ����顢���ˤ䤷�㤬�ߤ򱣴椷�ʤ���
									//�إå����֤˥��`�����
				 					Time = (LNAIBuf[Line][3][Index] - LNAIBuf[Line][3][0]) / 10;
									if(statusArray[ LNAIBuf[Line][4][Index] ][2] == __1 || statusArray[ LNAIBuf[Line][4][Index] ][2] == __4){
										if(statusArray[ LNAIBuf[Line][4][Index] ][0] == 0xA){
											//���`��
											if(*(Address3 + 2)){
												
												//���`�ɤΕr�g�ξ���
							 					if((BYTE)Time    < 3+ *(Address3 + 1)
							 					&& (BYTE)Time +3 >    *(Address3 + 1)){
													FloatH(Address3 + 2, 1);
												}else{
													FloatH(Address3 + 2, -1);
												}
												
												//�r�g�{��
												if(GetH( Address3 + 2 ) < 0xA){
								 					if((BYTE)Time > 2+ *(Address3 + 1) ){
														*(Address3 + 1) = *(Address3 + 1) + 1;
													}
								 					if((BYTE)(Time +2) < *(Address3 + 1)){
														*(Address3 + 1) = *(Address3 + 1) - 1;
													}
												}
												
											}else{
												*(Address3 + 1) = (BYTE)Time;
												*(Address3 + 2) = 0x88;
												*(Address3 + 3) = 0;
											}
										}else{
											//����
											//���ˤ��Ƥ���Τ˥��`�ɤ��o���ä��Ȥ����Ȥ��¤���
											//���Ȥ�һ�������¤ˤʤä��Ȥ��ϥꥻ�å�
											if(Index != 19 && AIMode ==2){
												if(statusArray[ LNAIBuf[Line][4][Index + 1] ][0] != 0xA){
													//����Τ��Єӡ��Ȥʤä��Ȥ�����إ���
													FloatH(Address3 + 2, -1);
												}
												if(GetH(Address3 + 2) < 0x4){
													*(Address3 + 1) = 0;
													*(Address3 + 2) = 0;
													*(Address3 + 3) = 0;
												}
											}
										}
									}else{
										#if debug_mode_LNAI
											cout << "��󥯥��åלʂ�" << endl;
										#endif
									 	for(Counter3=4; Counter3<20; Counter3+=4){
							 				Address4 = Address3 + Counter3;
								 			if(*(Address4) == (BYTE)statusArray[ LNAIBuf[Line][4][Index] ][2]){
								 				
								 				//�u��
								 				FloatL(Address4 + 2, 1);
							 					
							 					//����
							 					if((BYTE)Time    < 3+ *(Address4 + 1)
							 					&& (BYTE)Time +3 >    *(Address4 + 1)){	//�r�g��٤ʤ�
							 						FloatH(Address4 + 2, 1);
								 				}else{
								 					FloatH(Address4 + 2, -1);
								 				}
								 				
								 				//�r�g�{��
								 				if(GetH(Address4 + 2) < 0xA){
								 					if((BYTE)Time > 2+ *(Address4 + 1)){
														*(Address4 + 1) = *(Address4 + 1) + 1;
													}
								 					if((BYTE)Time +2 < *(Address4 + 1)){
														*(Address4 + 1) = *(Address4 + 1) - 1;
													}
												}
								 				
								 				//����椨
								 				if(Counter3 != 4){
								 					if(GetL(Address4 + 2) >= GetL(Address4 - 2)){
														#if debug_mode_LNAI
															cout << "��󥯥��å�" << endl;
														#endif
														
														DWORD Temp;
								 						Temp				= *(DWORD*)(Address4 - 4);
								 						*(DWORD*)(Address4 - 4)	= *(DWORD*)(Address4);
								 						*(DWORD*)(Address4)	= Temp;
									 				}
								 				}
								 				Counter3 = 20;
								 			}else{
								 				if(*(Address4)==0 || Counter3==16){
								 					//�����ϕ���
								 					Counter3 = 20;
								 					*(Address4)     = (BYTE)statusArray[ LNAIBuf[Line][4][Index] ][2];
								 					*(Address4 + 1) = (BYTE)Time;
								 					*(Address4 + 2) = 0x08;	//���ڂ�
								 					*(Address4 + 3) = 0;	//�դ��������ӻ����ʤɤ��{����
								 				}
							 				}
							 			}
							 		}
							 	}
							 }
						}else{
							if(((gameTime - LNAIBuf[Line][3][0]) / 10) < 1){
								Time = 0;	//���`�ɤ��뤹��٤��r�g
							}else{
								Time = (gameTime - LNAIBuf[Line][3][0]) / 10 - 1;
							}
							
							//�������W��ˤ��ʳ�餤���a�����`�ɤ���
							if(LNAIBuf[Line][0][0] == 0xFF){
								Address = LNAIbase + eigenValueLN[0][3];
								if(*Address == 0){
									*Address = 0xFF;
								}
								if(*Address == 0xFF){
									Address2 = Address;
									for(Counter2=20; Counter2<1000; Counter2+=20){
										Address3 = Address2 + Counter2;
										if(*Address3 == (BYTE)LNAIBuf[Line][2][0]){
											Counter2 = 1000;
											
											//�ЄӤΕr�g���Ȥ��{��
											Address4 = Address3 + 4;
											if(*Address4){
												#if debug_mode_LNAI
													cout << "�r�g�{��" << endl;
												#endif
							 					FloatH(Address4 + 2, -1);
								 				if(GetH(Address4 + 2) < 0xA){
								 					if((BYTE)Time > *(Address4 + 1)){
														*(Address4 + 1) = *(Address4 + 1) + 1;
													}
								 					if((BYTE)Time < *(Address4 + 1)){
														*(Address4 + 1) = *(Address4 + 1) - 1;
													}
												}
											}
											if(*(Address3 + 2)){
												//���`�ɤΕr�g���Ȥ��{��
												//΢��
												if(myGameInfo[ _info ][2][0]==2){
													if(GetH(Address3 + 2) > 4){
														FloatL(Address3 + 2, -2);
													}else{
														FloatL(Address3 + 2, 2);
													}
												}
						 						FloatH(Address3 + 2, -1);
								 				if(GetH(Address3 + 2) < 0xA){
								 					if((BYTE)Time > *(Address3 + 1)){
														*(Address3 + 1) = *(Address3 + 1) + 1;
													}
								 					if((BYTE)Time < *(Address3 + 1)){
														*(Address3 + 1) = *(Address3 + 1) - 1;
													}
												}
											}else{
												#if debug_mode_LNAI
													cout << "���`����Ҏ׷��" << endl;
												#endif
												*(Address3 + 1) = (BYTE)Time;
												*(Address3 + 2) = 0x88;
												*(Address3 + 3) = 0;
											}
										}else{
											if(*(Address3) == 0){
												#if debug_mode_LNAI
													cout << "���`����Ҏ׷��" << endl;
												#endif
												Counter2 = 1000;
												
												*(Address3) = (BYTE)LNAIBuf[Line][2][0];
												*(Address3 + 1) = (BYTE)Time;
												*(Address3 + 2) = 0x88;
												*(Address3 + 3) = 0;
											}
										}
									}
								}
							}
							
							//���������󥯥�����ʤ�
							if(LNAIBuf[Line][0][Index] && LNAIBuf[Line][0][0] !=0xFF){	//���ɥ쥹���_������Ƥ�����
								Address3 = (BYTE*)LNAIBuf[Line][0][Index];
								LNAIBuf[Line][0][Index] = 0;	//���ɥ쥹����ڻ�����Τ��ᣩ
								if(*Address3 == (BYTE)LNAIBuf[Line][2][0]){	//�_�J
									
									if(statusArray[ LNAIBuf[Line][4][Index] ][2] == __1 || statusArray[ LNAIBuf[Line][4][Index] ][2] == __4){
										if(statusArray[ LNAIBuf[Line][4][Index] ][0] == 0xA){
											//���`��
											//���`�ɤ����Τ˥ޥ��ʥ��Ȥ������Ȥ�
											//����줿
											//���`�ɤ����ʳ��ä�
											//��ΤȤ���Τ⤷�ʤ���
											//�ᡩ�Ϲ��ĤΥ����פˤ�ä����֤���
										}else{
											//����
											if(Index != 19 && AIMode == 2){
												if(statusArray[ LNAIBuf[Line][4][Index + 1] ][0] != 0xA){
													//���ˤ��Ƥ���Τ˥��`�ɤ��o�����ޥ��ʥ�ӡ��Ǥϥ��`�ɤξ��Ȥ�󤭤��¤��롣
													//���ڂ���8
													if(myGameInfo[ _info ][2][0]==2){
														if(GetH(Address3 + 2) > 4){
															FloatL(Address3 + 2, -2);
														}else{
															FloatL(Address3 + 2, 2);
														}
													}else{
														FloatH(Address3 + 2, -2);
													}
												}
												if(GetH(Address3 + 2) < 0x4){
													//���Ȥ�һ�������¤ʤ�����������
													*(Address3 + 1) = 0;
													*(Address3 + 2) = 0;
													*(Address3 + 3) = 0;
												}
											}
										}
									}else{
										#if debug_mode_LNAI
											cout << "��󥯥�����ʂ�" << endl;
										#endif
										
										if(*(Address3 + 2)==0){
											//���`�ɤ���Ҏ��׷��
											*(Address3 + 1) = (BYTE)Time;
											*(Address3 + 2) = 0x88;
											*(Address3 + 3) = 0;
										}
										
										//��󥯥�����
									 	for(Counter3=4; Counter3<20; Counter3+=4){
									 		Address4 = Address3 + Counter3;
								 			if(*(Address4) == (BYTE)statusArray[ LNAIBuf[Line][4][Index] ][2]){
												#if debug_mode_LNAI
													cout <<"�p��" << endl;
												#endif
									 			
									 			//�u������
									 			//�������إ���
							 					if((BYTE)Time    < 3+ *(Address4 + 1)
							 					&& (BYTE)Time +3 >    *(Address4 + 1)){
							 						FloatHL(Address4 + 2, -1, -1);
								 				}
							 					
							 					if(Counter3 != 16){
							 						if(*(Address4 + 4)){
								 						if(GetL(Address4 + 2) <= GetL(Address4 + 6)){
								 							//��󥯥�����
								 							DWORD Temp;
									 						Temp				= *(DWORD*)(Address4 + 4);
									 						*(DWORD*)(Address4 + 4)	= *(DWORD*)(Address4);
									 						*(DWORD*)(Address4)	= Temp;
									 					}
								 					}
								 				}
							 					Counter3 = 20;
							 				}
								 		}
								 	}
								}
							}
						}
						if(LNAIBuf[Line][0][0]==0xFF){
							LNAIBuf[Line][0][0] = 1;
						}
					}
					//ӡ�󱣴�I�����ޤ�
					
					//��Τ���0���ꥢ
					//�I���B�Ϥ�����붨���ʤ��Ǖ���
					if(LNAIBuf[Line][0][0]==0xFF){
						LNAIBuf[Line][0][0] = 1;
					}
					for(Counter=0; Counter<=LNAIBuf[Line][0][0]; Counter++){
						LNAIBuf[Line][0][Counter] = 0;
						LNAIBuf[Line][1][Counter] = 0;
						LNAIBuf[Line][2][Counter] = 0;
						LNAIBuf[Line][3][Counter] = 0;
						LNAIBuf[Line][4][Counter] = 0;
					}
					//��ꓤ����ޤ�
					
					
					//���
					LNAIBuf[Line][0][0] = 0;	//���̷��ų��ڻ�
					LNAIBuf[Line][1][0] = 0;
				}
			}
		}
		
		//���Х��ɥ쥹����
		eigenValueLN[1][2] = myGameInfo[ _para ][1][1];	//���x
		eigenValueLN[2][2] = myGameInfo[ _para ][2][1];	//�Էָߤ�
		eigenValueLN[3][2] = *enGameInfo[ _para ][2][1];	//���ָߤ�
		if(myGameInfo[ _info ][1][0] == 1){
			if(eigenValueLN[1][2] == 0 || eigenValueLN[1][2] == 1){
				eigenValueLN[1][2] = eigenValueLN[1][2] + 5;
			}
		}
		
		for(Counter=9; Counter>0; Counter--){
			if(eigenValueLN[Counter][0]){
				if(eigenValueLN[Counter][2] >= eigenValueLN[Counter][0]){		//��������å�
					eigenValueLN[Counter][2] = eigenValueLN[Counter][0] -1;
				}											//���Х��ɥ쥹Ӌ��
				eigenValueLN[Counter - 1][3] = eigenValueLN[Counter][1] * eigenValueLN[Counter][2] + eigenValueLN[Counter][3];
			}
		}
	}
}
