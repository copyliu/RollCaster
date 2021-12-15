#include "conf.h"
#include "boosterDatClass.h"


void boosterDatClass::ReadSpellAI(){
	if(eigenValueSpell[0][0]){
		if(myGameInfo[ _info ][3][0]){
			if(myGameInfo[ _info ][5][0] == 0){
				if(myGameInfo[ _para ][5][0] < 3000){	//HP�٤ʤ������Կ���  ...
					Address = SpellAIbase + eigenValueSpell[0][3];

					if(*Address == 0x25){
						Address2 = Address;
						if(*(Address2 + 1) > 90){
							//���ڥ륫�`������
							commandInput[0] = __22D;
							decodeTime = 0;
						#if debug_mode_SpellAI
								cout << "����" << endl;
							#endif
						}
					}
				}
			}else{
				//���ڥ륫�`��������
				if(myID==9){	//����
					if(statusID >= 0xC0 && statusID <= 0xC7){	//�ߥå���ϵ
						SpellAIBuf[0][0][0]=1;
					}else{
						SpellAIBuf[0][0][0]=0;
					}
				}

				if(spell_control == 0 || *enGameInfo[ _status ][1][0] == 2 || *enGameInfo[ _status ][1][0] == 5){
					if(SpellAIBuf[0][0][0]==0){
						if(myGameInfo[ _info ][2][0]==0){	//����Υ����å�
							//���ڥ�k�ӿ��ܡ�
							Address = SpellAIbase + eigenValueSpell[0][3];
							for(Counter=2; Counter<40; Counter+=2){
								Address2 = Address + Counter;
								if(*Address2 == (BYTE)myGameInfo[ _info ][8][2]){
									Counter = 40;
									if(*(Address2 + 1) > 80){
										commandInput[0] = statusArray[ *Address2 ][2];
										decodeTime = 0;
										#if debug_mode_SpellAI
											cout << "���ڥ륫�`�ɰk��" << endl;
										#endif
									}
								}
							}
						}
					}
				}
				if(SpellAIBuf[0][0][0]==1){	//2�ϰk��չ�_��
					//���ڥ��_�ţ����`���`������ʤɣ�
					Address = SpellAIbase + eigenValueSpell[0][3];

					if(spell_control == 0 || *enGameInfo[ _status ][1][0] == 2 || *enGameInfo[ _status ][1][0] == 5){
						if(myID==1){
							if(myGameInfo[ _info ][8][2] == 0xBC){	//���`���`������
								for(Counter=2; Counter<40; Counter+=2){
									Address2 = Address + Counter;
									if(*Address2 == 0){
										Counter = 40;
									}
									if(*Address2 == 0xBD || *Address2 == 0xBE){	//�k��չ�_
										if(*(Address2 + 1) > 80){
											commandInput[0] = statusArray[ *Address2 ][2];
											Counter = 40;
										}
									}
								}
							}
							if(myGameInfo[ _info ][8][2] == 0xBF){	//���`���`�����`��`�����ƥ�
								for(Counter=2; Counter<40; Counter+=2){
									Address2 = Address + Counter;
									if(*Address2 == 0){
										Counter = 40;
									}
									if(*Address2 == 0xC0 || *Address2 == 0xC1){	//�k��չ�_
										if(*(Address2 + 1) > 80){
											commandInput[0] = statusArray[ *Address2 ][2];
											Counter = 40;
										}
									}
								}
							}
						}
					}
					if(myID==9){	//����	//�ߥå���ϵ
						if(myGameInfo[ _info ][8][2] == 0xBC || myGameInfo[ _info ][8][2] == 0xBF){
							if(myGameInfo[ _para ][1][0] > 4){
								commandInput[0] = __6;
							}else{
								Address2 = Address + 20;
								if(*(Address2 + 1) > 50){	//Ҫ�{��
									commandInput[0] = statusArray[ *Address2 ][2];
								}else{
									commandInput[0] = __6;
								}
							}
						}
					}
				}
			}
		}
	}


}

void boosterDatClass::CallSpellAI(){
	if(eigenValueSpell[0][0]){
		//2byte * 20

//		SpellAIBuf[0][0][0]	//�ե饰������


		//���Х��ɥ쥹����
		//�g�Y��	//�u��������������Ƥ��ʤ���
		eigenValueSpell[1][2] = myGameInfo[ _para ][1][1];	//���x
		eigenValueSpell[2][2] = myGameInfo[ _para ][2][1];	//�Էָߤ�
		eigenValueSpell[3][2] = *enGameInfo[ _para ][2][1];	//���ָߤ�
		eigenValueSpell[4][2] = *enGameInfo[ _status ][0][0];	//�ƥ�״�B���1


		for(Counter=9; Counter>0; Counter--){
			if(eigenValueSpell[Counter][0]){
				if(eigenValueSpell[Counter][2] >= eigenValueSpell[Counter][0]){		//��������å�
					eigenValueSpell[Counter][2] = eigenValueSpell[Counter][0] -1;
				}											//���Х��ɥ쥹Ӌ��
				eigenValueSpell[Counter - 1][3] = eigenValueSpell[Counter][1] * eigenValueSpell[Counter][2] + eigenValueSpell[Counter][3];
			}
		}



		if(myGameInfo[ _status ][2][2] && myGameInfo[ _status ][2][2] != __5){
			if(myGameInfo[ _status ][0][2]==8 || myGameInfo[ _status ][5][0]==0x25){	//���ڥ��v�S
				for(Line=1; Line<10; Line++){
					if(SpellAIBuf[Line][0][0]==0){
						SpellAIBuf[Line][0][0] = 0xFF;	//������

						if(myGameInfo[ _status ][5][0]==0x25){	//���ڥ륫�`������
							SpellAIBuf[Line][4][0] = 1;
							SpellAIBuf[Line][1][1] = 0x30;//���v
							SpellAIBuf[Line][2][1] = 65;	//ӡ��	//�Τ��𤭤ʤ���ʧ��
							Counter = 0;	//���Ԥ����^
						}else{
							//��������
							SpellAIBuf[Line][4][0] = 0;
							SpellAIBuf[Line][1][1] = 128;//���v
							SpellAIBuf[Line][2][1] = 64;	//ӡ��

							//������������ʥ��ڥ�
							//���v���γ��ڂ����٤ʤ�
							//SpellAIBuf[Line][4][0]���ե饰

							if(myID==1){	//ħ��ɳ���ڥ�
								if(myGameInfo[ _status ][5][0]==0xBC || myGameInfo[ _status ][5][0]==0xBF){	//�k��
									SpellAIBuf[Line][4][0] = 2;
									SpellAIBuf[Line][1][1] = 0x30;//���v
									SpellAIBuf[Line][2][1] = 65;	//ӡ��
								}
								if(myGameInfo[ _status ][5][0]==0xBD || myGameInfo[ _status ][5][0]==0xBE
								|| myGameInfo[ _status ][5][0]==0xC0 || myGameInfo[ _status ][5][0]==0xC1){	//�k��չ�_
									SpellAIBuf[0][0][0] = 2;
									SpellAIBuf[Line][4][0] = 3;
									SpellAIBuf[Line][1][1] = 128;//���v
									SpellAIBuf[Line][2][1] = 64;	//ӡ��
								}
							}
							if(myID==2){	//�Dҹ���ڥ�
								if(myGameInfo[ _status ][5][0]==0xB0 || myGameInfo[ _status ][5][0]==0xB3
								|| myGameInfo[ _status ][5][0]==0xB6 || myGameInfo[ _status ][5][0]==0xB7
								|| myGameInfo[ _status ][5][0]==0xB9 || myGameInfo[ _status ][5][0]==0xBA){
									SpellAIBuf[Line][4][0] = 1;
									SpellAIBuf[Line][1][1] = 0x70;//���v	//��᤬�L��
									SpellAIBuf[Line][2][1] = 65;	//ӡ��
								}
							}
							if(myID==4){	//�ѥ����`���ڥ�
								if(myGameInfo[ _status ][5][0]==0xBF){
									SpellAIBuf[Line][4][0] = 1;
									SpellAIBuf[Line][1][1] = 0x60;//���v
									SpellAIBuf[Line][2][1] = 65;	//ӡ��
								}
							}
							if(myID==5){	//�������ڥ�
								if(myGameInfo[ _status ][5][0]==0xBC || myGameInfo[ _status ][5][0]==0xBF){
									SpellAIBuf[Line][4][0] = 1;
									SpellAIBuf[Line][1][1] = 0x60;//���v
									SpellAIBuf[Line][2][1] = 65;	//ӡ��
								}
							}
							if(myID==8){	//�ϥ��ڥ�
								if(myGameInfo[ _status ][5][0]==0xBF){
									SpellAIBuf[Line][4][0] = 1;
									SpellAIBuf[Line][1][1] = 0x60;//���v
									SpellAIBuf[Line][2][1] = 65;	//ӡ��
								}
							}
							if(myID==9){	//���㥹�ڥ�
								if(statusID==0xB0 || statusID==0xB3 || statusID==0xB6 || statusID==0xB9){
									SpellAIBuf[Line][4][0] = 0;
									SpellAIBuf[Line][1][1] = 0x60;//���v
									SpellAIBuf[Line][2][1] = 64;	//ӡ��
								}

								if(myGameInfo[ _status ][5][0]==0xBC || myGameInfo[ _status ][5][0]==0xBF){
									SpellAIBuf[Line][4][0] = 0;	//���ͨ�����ڥ�Ȥ��ƒQ��
									SpellAIBuf[Line][1][1] = 0x60;//���v
									SpellAIBuf[Line][2][1] = 64;	//ӡ��
								}
								if(statusID >= 0xC0 && statusID <= 0xC7){	//�ߥå���ϵ
									SpellAIBuf[Line][4][0] = 4;
									SpellAIBuf[Line][1][1] = 0xFF;//���v
									SpellAIBuf[Line][2][1] = 65;	//64;	//ӡ��	//Ҫ��ӑ	//�������ʤ�
								}
							}
							Counter = 2;	//���Ԥ����^�ʤΤ�
						}

						Address = SpellAIbase + eigenValueSpell[0][3];
						SpellAIBuf[Line][0][1] = 0;
						if(SpellAIBuf[Line][4][0] != 4){
							for(Counter; Counter<20; Counter+=2){
								Address2 = Address + Counter;
								if(*Address2 == 0){
									*(Address2) = statusID;
									*(Address2 + 1) = 95;	//���ڂ�
								}
								if(*Address2 == statusID){	//myGameInfo[ _status ][5][0]��ͬ��	//�ɤ���Ǖ����Τ��������g�Y
									Counter = 20;
									SpellAIBuf[Line][0][1] = (DWORD)Address2;
								}
							}
						}else{	//SpellAIBuf[Line][4][0] == 4	//�ߥå���ϵ
							SpellAIBuf[Line][0][1] = (DWORD)Address;
						}
						SpellAIBuf[Line][3][1] = 0;
						SpellAIBuf[Line][4][1] = (DWORD)statusID;	//(DWORD)myGameInfo[ _info ][8][0];	//���N�

						Line = 10;
					}
				}
			}
		}

		for(Line=1; Line<10; Line++){
			if(SpellAIBuf[Line][0][0]){
				if(SpellAIBuf[Line][1][1] > __magnification){
					SpellAIBuf[Line][1][1] = SpellAIBuf[Line][1][1] - __magnification;
				}else{
					SpellAIBuf[Line][1][1] = 0;
				}

				//ͨ��
				if(SpellAIBuf[Line][4][0]==0){
					if(myGameInfo[ _para ][5][1]){
						SpellAIBuf[Line][2][1] = SpellAIBuf[Line][2][1] - 10;
						SpellAIBuf[Line][1][1] = 0;
						#if debug_mode_SpellAI
							cout << "����`��" << endl;
						#endif
					}
					if(statusArray[ *enGameInfo[ _status ][9][1] ][0]==2 || (*enGameInfo[ _status ][0][0]==2 && *enGameInfo[ _para ][5][1])){
						SpellAIBuf[Line][2][1] = SpellAIBuf[Line][2][1] + 10;
						SpellAIBuf[Line][1][1] = 0;
						#if debug_mode_SpellAI
							cout << "�����`��" << endl;
						#endif
					}
					if(myID==9 && (statusID == 0xB7 || statusID == 0xBA)){
						SpellAIBuf[Line][2][1] = SpellAIBuf[Line][2][1] + 10;
						SpellAIBuf[Line][1][1] = 0;
						#if debug_mode_SpellAI
							cout << "��`��ϵ�ɹ�" << endl;
						#endif
					}
				}

				//���Ԥʤ�
				if(SpellAIBuf[Line][4][0]==1 || SpellAIBuf[Line][4][0]==2){	//���v���γ��ڂ����٤ʤ�
					if(myGameInfo[ _para ][5][1]){
						SpellAIBuf[Line][2][1] = SpellAIBuf[Line][2][1] - 10;
						SpellAIBuf[Line][1][1] = 0;
						#if debug_mode_SpellAI
							cout << "����`��" << endl;
						#endif
					}
				}

				//���`���`������ʤ�
				if(SpellAIBuf[Line][4][0]==3){
					if((myID==1 && SpellAIBuf[Line][1][1] > 60) || myID !=1){
						if(myGameInfo[ _para ][5][1]){
							if(SpellAIBuf[Line][2][1] > 10){
								SpellAIBuf[Line][2][1] = SpellAIBuf[Line][2][1] - 10;
							}else{
								SpellAIBuf[Line][2][1] = 0;
							}
							#if debug_mode_SpellAI
								cout << "����`��" << endl;
							#endif
						}
						if(*enGameInfo[ _status ][0][0]==2){
							if(SpellAIBuf[Line][2][1] < 118){
								SpellAIBuf[Line][2][1] = SpellAIBuf[Line][2][1] + 10;
							}else{
								SpellAIBuf[Line][2][1] = 128;
							}
							#if debug_mode_SpellAI
								cout << "�����`��" << endl;
							#endif
						}
					}
				}

				//�ߥå���ϵ
				if(SpellAIBuf[Line][4][0]==4){
					if(statusID >= 0xC0 && statusID <= 0xC7){
						if(*enGameInfo[ _status ][0][0]==2){
							if(SpellAIBuf[Line][2][1] < 118){
								SpellAIBuf[Line][2][1] = SpellAIBuf[Line][2][1] + 10;
							}else{
								SpellAIBuf[Line][2][1] = 128;
							}
							SpellAIBuf[Line][1][1] = 0;
							#if debug_mode_SpellAI
								cout << "�����`��" << endl;
							#endif
						}
					}else{
						SpellAIBuf[0][0][0] = 0;
						SpellAIBuf[Line][1][1] = 0;	//���v���Ф�
					}
				}

				if(SpellAIBuf[Line][1][1] < 8){
					//�K��
					/*
					#if debug_mode_SpellAI
						cout << "�K��" << endl;
					#endif
					*/
					if(SpellAIBuf[Line][0][1]){
						if(SpellAIBuf[Line][4][0] != 4){
							Address2 = (BYTE*)SpellAIBuf[Line][0][1];
							SpellAIBuf[Line][0][1] = 0;
							if(*Address2 == (BYTE)SpellAIBuf[Line][4][1]){
								if(SpellAIBuf[Line][2][1] > 64){
									//��ӡ��
									//�L���Ф��ǩ`���ʤΤ�ĸ����󤭤�
									if(*(Address2 + 1) < 109){
										*(Address2 + 1) = *(Address2 + 1) + 1;
									}
									if(SpellAIBuf[Line][4][0]==2){
										SpellAIBuf[0][0][0] = 1;	//�k�ӳɹ�
										#if debug_mode_SpellAI
											cout << "���`���`��ϵ�k�ӳɹ�" << endl;
										#endif
									}
									if(SpellAIBuf[Line][4][0]==3){
										SpellAIBuf[0][0][0] = 0;	//�k��ɹ�
										#if debug_mode_SpellAI
											cout << "���`���`��ϵ�ɹ�" << endl;
										#endif
									}

									#if debug_mode_SpellAI
										cout << "���ڥ��ӡ��" << endl;
									#endif
								}else{
									//��ӡ��
									*(Address2 + 1) = *(Address2 + 1) - (BYTE)(*(Address2 + 1) / 10);
									if(SpellAIBuf[Line][4][0]==3){
										SpellAIBuf[0][0][0] = 0;	//�k��ʧ��
										#if debug_mode_SpellAI
											cout << "���`���`��ϵʧ��" << endl;
										#endif
									}
									#if debug_mode_SpellAI
										cout << "���ڥ됙ӡ��" << endl;
									#endif
								}
							}
						}else{	//SpellAIBuf[Line][4][0] == 4;	//�ߥå���ϵ
							Address = (BYTE*)SpellAIBuf[Line][0][1];
							for(Counter=20; Counter<40; Counter+=2){
								Address2 = Address + Counter;
								if(*Address2==0){
									*(Address2) = (BYTE)SpellAIBuf[Line][4][1];
									*(Address2 + 1) = 95;
								}
								if(*Address2==(BYTE)SpellAIBuf[Line][4][1]){
									if(SpellAIBuf[Line][2][1] > 64){
										//��ӡ��
										if(*(Address2 + 1) < 110){
											*(Address2 + 1) = *(Address2 + 1) + 1;
										}
										if(Counter!=20){
											if(*(Address2 + 1) >= *(Address2 - 1)){
												//��󥯥��å�
												DWORD Temp;
												Temp                   = (DWORD)*(WORD*)(Address2 - 2);
												*(WORD*)(Address2 - 2) = *(WORD*)Address2;
												*(WORD*)Address2       = (WORD)Temp;
											}
										}
										#if debug_mode_SpellAI
											cout << "�ߥå���ϵ��ӡ��" << endl;
										#endif
									}else{
										//��ӡ��
										if(*(Address2 + 1)){
											*(Address2 + 1) = *(Address2 + 1) - 1;
										}
										if(Counter != 38 && *(Address2 + 2)){
											if(*(Address2 + 1) <= *(Address2 + 3)){
												DWORD Temp;
												Temp                   = (DWORD)*(WORD*)Address2;
												*(WORD*)Address2       = *(WORD*)(Address2 + 2);
												*(WORD*)(Address2 + 2) = (WORD)Temp;
											}
										}
										#if debug_mode_SpellAI
											cout << "�ߥå���ϵ��ӡ��" << endl;
										#endif
									}
									Counter = 40;
								}
							}
						}
					}
					SpellAIBuf[Line][0][0] = 0;
				}
			}
		}


	}
}
