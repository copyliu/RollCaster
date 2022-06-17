#include "conf.h"
#include "boosterDatClass.h"
#include "HL.h"
using namespace N_Booster;

void boosterDatClass::ReadSWAI(){
	if(eigenValueSW[0][0]){
		
		DWORD Temp = 0;
		Flg = 0;
		
		#if debug_height
			if(myGameInfo[ _para ][2][0] && *enGameInfo[ _para ][2][0]){	//実験中
//			if(*enGameInfo[ _para ][2][0]){	//実験中
				Temp = 5;
			}
			if(gameInfoPara[15][1]==4){
				Temp += 10;
			}
			
			if(*enGameInfo[ _para ][2][0] && (*enGameInfo[ _status ][0][0] == 2 || *enGameInfo[ _status ][0][0] == 9)){
				Flg = 1;
			}
		#endif
		
		Address = SWAIbase + eigenValueSW[0][3];
		for(Counter=0; Counter<4800; Counter+=160){
			Address2 = Address + Counter;
			if(*Address2 == 0){
				Counter = 4800;
			}else{
				if(*Address2 == (BYTE)myGameInfo[ _status ][6][0]){	//自分前行動
					Counter = 4800;
					for(Counter2=8; Counter2<160; Counter2+=8){
						Address3 = Address2 + Counter2;
						if(*Address3==0){
							Counter2 = 160;
						}else{
							if(!(*Address3==0x24 && myGameInfo[ _info ][4][0]==0)){
								//霊力チェック
								if(!(myGameInfo[ _info ][2][0]
								&& (statusArray[ *Address3 ][0] == 4 || statusArray[ *Address3 ][0] == 5 || statusArray[ *Address3 ][0] == 6))){
									Time = (DWORD)((gameTime - myGameInfo[ _status ][6][1]) / 4);
									if(((BYTE)(Time + 0xF) >= *(Address3 + 1) && (BYTE)Time < 3+ *(Address3 + 1)) || myGameInfo[ _status ][6][0]==0xFF){
										if(!(Flg && statusArray[ *Address3 ][0] == 3
										&& !(GetH(Address3 + 3) >= (BYTE)myGameInfo[ _para ][3][2]
										  && GetH(Address3 + 3) <= debug_height_value + (BYTE)myGameInfo[ _para ][3][2]))){	//高さ補正	//実験中
											Counter2 = 160;
											if(((BYTE)(Time + Temp) >= *(Address3 + 1) && (BYTE)Time < 3+ *(Address3 + 1)) || myGameInfo[ _status ][6][0]==0xFF){
												commandInput[0] = statusArray[ *Address3 ][2];
												decodeTime = 0;
												#if debug_mode_SWAI
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
	}
}

void boosterDatClass::CallSWAI(){
	if(eigenValueSW[0][0]){
//		Line==0は全体に対するバッファとして使用
//		SWAIBuf[0][0][0] = ;
//		SWAIBuf[0][1][0] = ;
//		SWAIBuf[0][2][0] = ;
//		SWAIBuf[0][3][0] = myGameInfo[ _status ][5][1];	//時間
//		SWAIBuf[0][4][0] = myGameInfo[ _status ][6][0];	//入力ID
//		SWAIBuf[0][5][0] = myGameInfo[ _para ][3][2];	//高さ補正情報
//		連鎖攻撃は、くらいor空中ダウン状態ではIndexを進めるという方向？
		
		if(SWAIBuf[0][4][0]==0){
			SWAIBuf[0][4][0] = 0xFF;
		}
		
		Flg = 0;
		if(myGameInfo[ _status ][2][2]){
			if(myGameInfo[ _info ][1][0] == 2 && myGameInfo[ _para ][1][0] < 3){	//敵が壁際
				if(myGameInfo[ _status ][2][2] != __5 && myGameInfo[ _status ][2][2] != __4 && myGameInfo[ _status ][2][2] != __1
//				&& myGameInfo[ _status ][2][2] != __D4 && myGameInfo[ _status ][2][2] != __D7
				&& myGameInfo[ _status ][2][2] != __22D && myGameInfo[ _status ][0][0] != 8
				&& myGameInfo[ _status ][5][0] != 0x90 && myGameInfo[ _status ][5][0] != 0x91 && myGameInfo[ _status ][5][0] != 0x92 && myGameInfo[ _status ][5][0] != 0x93
				){
					if(!(myGameInfo[ _para ][1][0] > 4 && (myGameInfo[ _status ][2][2] == __D4 || myGameInfo[ _status ][2][2] == __D7))){
						if(myGameInfo[ _info ][0][1]==0){
							//後ろ向きでない
							Flg = 1;
							if(myGameInfo[ _status ][2][2] == __D4 || myGameInfo[ _status ][2][2] == __D7){
								if(statusArray[ (BYTE)SWAIBuf[0][4][0] ][2] == __D4 || statusArray[ (BYTE)SWAIBuf[0][4][0] ][2] == __D7){
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
				if(SWAIBuf[Line][0][0]==0){
					//ライン新設
					SWAIBuf[Line][0][0] = 0xFF;
					SWAIBuf[Line][1][0] = 0;
					SWAIBuf[Line][2][0] = 0;
					SWAIBuf[Line][3][0] = SWAIBuf[0][3][0];	//前入力時間
					SWAIBuf[Line][4][0] = SWAIBuf[0][4][0];	//前行動
					SWAIBuf[Line][5][0] = 0;
					
					//相関初期値設定
					SWAIBuf[Line][1][1] = 0xFF;
					
					//印象初期値設定
					if(AIMode==2){
						SWAIBuf[Line][2][1] = 64;
					}else{
						SWAIBuf[Line][2][1] = 65;
					}
					
					//例外
					if(myGameInfo[ _status ][2][2]==__22C){
						//霊撃
						SWAIBuf[Line][1][1] = 0x60;	//相関
						SWAIBuf[Line][2][1] = 64;	//印象
					}
					if(myGameInfo[ _status ][2][2] == __D4 || myGameInfo[ _status ][2][2] == __D7){
						//後ダッシュ
						SWAIBuf[Line][1][1] = 0xFF;	//相関
						SWAIBuf[Line][2][1] = 64;	//印象
					}
					Line = 20;
				}
			}
		}
		for(Line=1; Line<20; Line++){
			if(SWAIBuf[Line][0][0]){
				if(Flg && SWAIBuf[Line][0][0]==0xFF){
					if(SWAIBuf[Line][0][0]==0xFF){
						SWAIBuf[Line][0][0] = 1;
					}else{
						SWAIBuf[Line][0][0] = SWAIBuf[Line][0][0] + 1;
					}
					Index = SWAIBuf[Line][0][0];
					
					SWAIBuf[Line][0][Index] = 0;
					Address = SWAIbase + eigenValueSW[0][3];
					for(Counter=0; Counter<4800; Counter+=160){
						Address2 = Address + Counter;
						if(*Address2 == (BYTE)SWAIBuf[Line][4][Index -1]){	//自分前行動
							Counter = 4800;
							SWAIBuf[Line][0][Index] = (DWORD)Address2;
						}else{
							if(*Address2 == 0){
								Counter = 4800;
								SWAIBuf[Line][0][Index] = (DWORD)Address2;
								*Address2 = (BYTE)SWAIBuf[Line][4][Index - 1];
							}
						}
					}
					if(Index != 1){
						SWAIBuf[Line][1][Index] = SWAIBuf[Line][1][Index - 1];	//相関
						SWAIBuf[Line][2][Index] = SWAIBuf[Line][2][Index - 1];	//印象
					}
					SWAIBuf[Line][3][Index] = (myGameInfo[ _status ][5][1] - SWAIBuf[0][3][0]) / 4;	//時間
					SWAIBuf[Line][4][Index] = myGameInfo[ _status ][5][0];	//入力（ID）
					SWAIBuf[Line][5][Index] = SWAIBuf[0][5][0];			//高さ補正情報
				}
				if(SWAIBuf[Line][0][0]==0xFF){
					Index = 1;
				}else{
					Index = SWAIBuf[Line][0][0];
				}
				if(SWAIBuf[Line][1][Index] >= __magnification){
					SWAIBuf[Line][1][Index] = SWAIBuf[Line][1][Index] - __magnification;	//相関値は減少する
				}else{
					SWAIBuf[Line][1][Index] = 0;
				}
				
				if(*enGameInfo[ _status ][2][2]){
					SWAIBuf[Line][1][Index] = SWAIBuf[Line][1][Index] * 3 / 4;
				}
				
				if(*enGameInfo[ _para ][5][1] || *enGameInfo[ _status ][9][0]==0x22){
					#if debug_mode_SWAI
						cout << "与ダメージ" << endl;
					#endif
					if(SWAIBuf[Line][2][Index] <= 245){
						SWAIBuf[Line][2][Index] = SWAIBuf[Line][2][Index] + 10;
					}else{
						SWAIBuf[Line][2][Index] = 255;
					}
					SWAIBuf[Line][1][Index] = 0;	//相関を切る
				}
				
				if(myGameInfo[ _para ][5][1]){
					#if debug_mode_SWAI
						cout << "被ダメージ" << endl;
					#endif
					if(SWAIBuf[Line][2][Index] >= 10){
						SWAIBuf[Line][2][Index] = SWAIBuf[Line][2][Index] - 10;
					}else{
						SWAIBuf[Line][2][Index] = 0;
					}
					SWAIBuf[Line][1][Index] = 0;	//相関を切る
				}
				
				if(Index >= 19 || SWAIBuf[Line][1][Index] < 5){	//相関要調整
					//終了
					#if debug_mode_SWAI
						cout << hex << playerSide << ".line" << Line << " Close" << endl;
					#endif
					
					if(SWAIBuf[Line][0][Index]){
						Address2 = (BYTE*)SWAIBuf[Line][0][Index];
						SWAIBuf[Line][0][Index] = 0;
						if(*Address2 == (BYTE)SWAIBuf[Line][4][Index -1]){	//確認
							for(Counter2=8; Counter2<160; Counter2+=8){
								Address3 = Address2 + Counter2;
								if(*(Address3)==(BYTE)SWAIBuf[Line][4][Index]){
									if(SWAIBuf[Line][2][Index] > 64){
//									if((SWAIBuf[Line][2][Index] >= 64 && AIMode==1) || (SWAIBuf[Line][2][Index] > 64 && AIMode==2)){	//実験中
//									if((SWAIBuf[Line][2][Index] >= 64 && gameMode==5) || SWAIBuf[Line][2][Index] > 64){	//実験中
										//好印象
										//時間差の処理
										if((BYTE)SWAIBuf[Line][3][Index]    < 2+ *(Address3 + 1)
										&& (BYTE)SWAIBuf[Line][3][Index] +2 >    *(Address3 + 1)){	//時間差が少ない
											FloatH(Address3 + 2, 1);
											FloatL(Address3 + 7, 1);
										}else{
											//時間修正
											if(GetH(Address3 + 2) < 0xA){
												if((BYTE)SWAIBuf[Line][3][Index] > *(Address3 + 1)){
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
										
										//高さ補正
										if((BYTE)SWAIBuf[Line][5][Index]    < 1+ GetH(Address3 + 3)
										&& (BYTE)SWAIBuf[Line][5][Index] +1 >    GetH(Address3 + 3)){
											//高さが近い
											FloatL(Address3 + 3, 1);
										}else{
											//高さが近くない
											FloatL(Address3 + 3, -1);	//精度下げ
										}
										if(GetL(Address3 + 3) < 0xA){
											//高さ補正実行
											if((BYTE)SWAIBuf[Line][5][Index] > GetH(Address3 + 3)){
												FloatH(Address3 + 3, 1);
											}
											if((BYTE)SWAIBuf[Line][5][Index] < GetH(Address3 + 3)){
												FloatH(Address3 + 3, -1);
											}
										}
										
										if(Counter2 != 8){
											if(GetL(Address3 + 7) >= GetL(Address3 - 1)){
												ULONGLONG Temp;
												Temp                       = *(ULONGLONG*)(Address3 - 8);
												*(ULONGLONG*)(Address3 - 8) = *(ULONGLONG*)(Address3);
												*(ULONGLONG*)(Address3)    = Temp;
											}
										}
									}
									if(SWAIBuf[Line][2][Index] < 64){
										//悪印象
										if((BYTE)SWAIBuf[Line][3][Index]    < 3+ *(Address3 + 1)
										&& (BYTE)SWAIBuf[Line][3][Index] +3 >    *(Address3 + 1)){	//時間差が少ない
											FloatH(Address3 + 2, -1);
											FloatL(Address3 + 7, -1);
										}
										if(Counter2 != 152){
											if(*(Address3 + 8)){
												if(GetL(Address3 + 15) >= GetL(Address3 + 7)){
													ULONGLONG Temp;
													Temp                    = *(ULONGLONG*)(Address3);
													*(ULONGLONG*)(Address3) = *(ULONGLONG*)(Address3 + 8);
													*(ULONGLONG*)(Address3 + 8)    = Temp;
												}
											}else{
												//次がない
												if(GetL(Address3 + 7) < 6){
													*(ULONGLONG*)(Address3) = 0;
												}
											}
										}
									}
									Counter2 = 160;
								}else{
									if(*(Address3)==0 || Counter2 == 152){
										Counter2 = 160;
										if(SWAIBuf[Line][2][Index] > 64){
//										if((SWAIBuf[Line][2][Index] >= 64 && AIMode==1) || (SWAIBuf[Line][2][Index] > 64 && AIMode==2)){	//実験中
//										if((SWAIBuf[Line][2][Index] >= 64 && gameMode==5) || SWAIBuf[Line][2][Index] > 64){	//実験中
											*(Address3) = (BYTE)SWAIBuf[Line][4][Index];		//入力ID
											*(Address3 + 1) = (BYTE)SWAIBuf[Line][3][Index];	//時間差
											*(Address3 + 2) = 0;
											SetH(Address3 + 3, (BYTE)SWAIBuf[Line][5][Index]);
											SetL(Address3 + 3, 8);
											*(Address3 + 7) = 8;
										}
									}
								}
							}
						}
					}
					SWAIBuf[Line][0][0] = 0;
				}
			}
		}
		
		
		SWAIBuf[0][3][0] = myGameInfo[ _status ][6][1];	//時間
		SWAIBuf[0][4][0] = myGameInfo[ _status ][6][0];	//入力ID
		SWAIBuf[0][5][0] = myGameInfo[ _para ][3][2];
		
		//固有アドレス更新
		eigenValueSW[1][2] = myGameInfo[ _para ][1][0];	//距離
		eigenValueSW[2][2] = myGameInfo[ _para ][2][2];	//自分高さ	//実験中
		eigenValueSW[3][2] = *enGameInfo[ _para ][2][0];	//相手高さ	//実験中
		eigenValueSW[4][2] = *enGameInfo[ _status ][1][0];	//テキ状態分類2
		
		
		for(Counter=9; Counter>0; Counter--){
			if(eigenValueSW[Counter][0]){
				if(eigenValueSW[Counter][2] >= eigenValueSW[Counter][0]){		//値をチェック
					eigenValueSW[Counter][2] = eigenValueSW[Counter][0] -1;
				}											//固有アドレス計算
				eigenValueSW[Counter - 1][3] = eigenValueSW[Counter][1] * eigenValueSW[Counter][2] + eigenValueSW[Counter][3];
			}
		}
	}
}
