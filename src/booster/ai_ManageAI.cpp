#include "conf.h"
#include "boosterDatClass.h"

void boosterDatClass::ManageAI(){
	//�B�i���Ĥ���󤬤ɤ��ʤ뤫�֤���ޤǷ���
	//�⤦һ��AI��׷�ӣ�
	
	
	if(commandInput[0] == __5){
		commandInput[0] = 0;
	}
	
	//���Єӡ��Է��Єӡ����ЄӤؤΥ��`����������ʳ�餤�����ˡ������
	if(commandInput[0]==0 && gameInfoPara[15][1]==5 && (*enGameInfo[ _status ][0][0]==2 || *enGameInfo[ _status ][0][0]==9 || *enGameInfo[ _status ][9][0]==0x22)){
		commandInput[0] = __5;
	}
	
	//�֥쥤���Фϥ����פ��ʤ�
	if(commandInput[0]==__7 || commandInput[0]==__8 || commandInput[0]==__9
	|| commandInput[0]==__D7 || commandInput[0]==__D8 || commandInput[0]==__D9){
		if(myGameInfo[ _info ][2][0]==2 && *enGameInfo[ _status ][0][0]!=2 && *enGameInfo[ _status ][0][0]!=9){
			commandInput[0] = 0;
		}
	}
	
	//�Ҥ��״�B�ΤȤ�
	if(statusID==0x22 && !(commandInput[0]==__1 || commandInput[0]==__4)){
		commandInput[0] = __4;
	}
	
	//����ǰ���å�����
	if(statusID ==0x10 && commandInput[0]){
		if(!(commandInput[0] == __1 || commandInput[0] == __4
		|| commandInput[0] == __3A || commandInput[0] == __3B
		|| commandInput[0] == __6A || commandInput[0] == __6B
		|| commandInput[0] == __D7 || commandInput[0] == __D8 || commandInput[0] == __D9 || commandInput[0] == __D4
		|| commandInput[0] == __D6A || commandInput[0] == __D6B || commandInput[0] == __D63A || commandInput[0] == __D63B
		|| commandInput[0] == __236D || commandInput[0] == __22D
		)){
			commandInput[0] = 0;
		}
		if(commandInput[0] == __D9){
			commandInput[0] = __9;
		}
	}
	
	//���å���Ǥ��ʤ��Ȥ�
	if(myGameInfo[ _info ][6][1]==0){
		if(commandInput[0] == __D4 || commandInput[0] == __D6){
			commandInput[0] = __4;
		}
	}
	
	//둓Ĥ����ޤ���
	if(commandInput[0]==__22C){
		if(*enGameInfo[ _status ][0][0]==0 || *enGameInfo[ _status ][0][0]==0xA || myGameInfo[ _para ][1][0] > 3){
			commandInput[0] = 0;
		}
	}
	
	//���󥿩`�Х�
	if(commandInput[0]){
		if( gameTime < intervalFlg || gameTime < enDat->intervalFlg ){
			if(!(commandInput[0] == __4 || commandInput[0] == __6
			|| commandInput[0] == __7  || commandInput[0] == __8  || commandInput[0] == __9
			|| commandInput[0] == __D4 || commandInput[0] == __D6
			|| commandInput[0] == __D7 || commandInput[0] == __D8 || commandInput[0] == __D9)){
				commandInput[0] = 0;
			}
		}
	}
	
	
	//debug
	if( scriptFlg && script_mode ){
		commandInput[0] = script.GetInput();
	}
	
}
