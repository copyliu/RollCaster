#if _MSC_VER >= 1300
#	/* definition for VC2005 */
#else
#	include <windows.h>
#endif
#include "caster\mainDatClass.h"
using namespace std;
using namespace N_Caster;

#pragma resource "icon/RollCaster.res"

int main( int argc, char *argv[] ){

	//プロセスの優先度を設定
//	SetPriorityClass( GetCurrentProcess(), REALTIME_PRIORITY_CLASS );
	SetPriorityClass( GetCurrentProcess(), HIGH_PRIORITY_CLASS );

	//main
	{
		mainDatClass mainDat;
		if( mainDat.mainDatInit( argc, argv ) ) return 1;

		int Res;
		for(;;){
			Res = mainDat.Entrance();
			if( Res == 0xF ) break;
			if( Res == 0 ) mainDat.backGroundRoop();
		}

		if( mainDat.mainDatEnd() ) return 1;
	}


	return 0;
}
