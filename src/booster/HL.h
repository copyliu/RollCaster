#ifndef TH075BOOSTER_HL
#define TH075BOOSTER_HL

#include "windows.h"

void FloatHL(BYTE*, int, int);//����p��	//(�����Ф�������Ф��˽���)
void FloatH(BYTE*, int);
void FloatL(BYTE*, int);
BYTE GetH(BYTE*);			//����
BYTE GetL(BYTE*);
void SetH(BYTE*, BYTE);
void SetL(BYTE*, BYTE);

#endif
