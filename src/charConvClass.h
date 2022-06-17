#ifndef CHAR_CONV_CLASS
#define CHAR_CONV_CLASS

#include <windows.h>

namespace N_Gui{

class charConvClass{
public:
	WCHAR* wide;
	char* shift;
	
	charConvClass();
	
	charConvClass(WCHAR* wParam);
	
	charConvClass(char* sParam);
	
	int set(WCHAR* wParam);
	int set(char* sParam);
	
	~charConvClass();
};

}

#endif
