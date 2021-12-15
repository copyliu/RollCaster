#ifndef TH075BOOSTER_CONF
#define TH075BOOSTER_CONF

//main設定
#define debug_mode_step	 	0 //1だと一回ずつループする

//script
#define script_mode	 	0

#define auto_start	1	//萃夢想と同じフォルダであり、萃夢想を自動的に起動したいとき1にする
#define wait_time		0	//開始時に待つ秒数
#define sleep_time	7	//Sleepする時間
#define __magnification	1	//倍率	//この値はAIでも使う

//boosterDatClass設定
#define debug_mode	 	0 //1だとデバッグメッセージ表示	//倍率を5程度にしないとOSが落ちる
#define debug_mode_time_show	0 //1だと時間を表示
#define debug_mode_ConvertDat	0 //1だとConvertDatデバッグ
#define debug_mode_mainRoop	0 //1だとmainRoopデバッグ
#define debug_mode_show		0 //1だとAの各種情報表示	//倍率を上げたほうが見やすい
#define debug_mode_show2	0 //1だとAの行動状態表示
#define debug_mode_noAI		0 //1だとAI.datを開かない
#define debug_mode_AIsize_show 0 //AIsizeを表示
#define debug_mode_file		0 //file周りを表示
#define debug_mode_command	0 //command周りを表示
#define debug_mode_copy		0 //コピー動作
#define debug_mode_LNAI		0 //LNAI表示
#define debug_mode_SNAI		0 //SNAI表示
#define debug_mode_SWAI		0 //SWAI表示
#define debug_mode_SpellAI	0 //SpellAI表示
#define debug_mode_RecoverAI	0 //RecoverAI表示
#define debug_mode_BackAI	0 //BackAI表示
#define debug_mode_LocalAI	0 //LocalAI表示

#define debug_height		1 //1だと高さ補正ON
#define debug_height_value	2 //高さ補正を下方向に緩和する値

#define spell_control	0	//1だと相手が食らいorダウンのときだけスペルを発動する

#define bodyBuf_size	150	//良好
#define myBuf_size	1300	//良好
#define inBuf_size	1024	//良好
#define outBuf_size	102400	//実験中
#define AI_size		10000000	//AIの最大許容容量
#define AI_local		0	//1だとローカルAIを使用する


#endif
