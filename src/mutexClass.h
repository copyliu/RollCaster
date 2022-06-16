#ifndef MUTEX_CLASS
#define MUTEX_CLASS

#include <windows.h>

namespace N_Gui{

class mutexClass{
public:
	mutexClass(HANDLE hMutexParam){
		hMutex = hMutexParam;
		WaitForSingleObject( hMutex, INFINITE );
	}
	
	mutexClass(HANDLE hMutexParam, unsigned int time){
		hMutex = hMutexParam;
		WaitForSingleObject( hMutex, time );
	}
	
	~mutexClass(){
		ReleaseMutex( hMutex );
	}
	
	HANDLE hMutex;
};

}

#endif
