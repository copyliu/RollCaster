CPP	= bcc32
AR	= tlib
LINK	= ilink32
CPPFLAGS	= -WM -O2 -c

TARGET	= caster.lib
OBJ1	= charDat.obj charDat_di.obj inputData.obj inputDataSub.obj
OBJ2	= mainDat.obj mainDat_func.obj mainDat_mainRoop.obj mainDat_th075.obj mainDat_thread.obj
OBJ3	= mainDat_main.obj mainDat_manipMenu.obj mainDat_main_sub.obj mainDat_mainStep.obj syncData.obj syncDataSub.obj

OBJP1	= +charDat.obj +charDat_di.obj +inputData.obj +inputDataSub.obj
OBJP2	= +mainDat.obj +mainDat_func.obj +mainDat_mainRoop.obj +mainDat_th075.obj +mainDat_thread.obj
OBJP3	= +mainDat_main.obj +mainDat_manipMenu.obj +mainDat_main_sub.obj +mainDat_mainStep.obj +syncData.obj +syncDataSub.obj

all: $(TARGET)
$(TARGET) : $(OBJ1)
$(TARGET) : $(OBJ2)
$(TARGET) : $(OBJ3)
	-del $(TARGET)
	cd booster
	make
	cd ..
	$(AR) $(TARGET) $(OBJP1) $(OBJP2) $(OBJP3) +booster\booster.lib

.cpp.obj:
	$(CPP) $(CPPFLAGS) $(CONSOLEFLAGS) $<

# cleanup
clean:
	-del *.obj
	-del *.tds
	-del $(TARGET)

	cd booster
	make clean
	cd ..

charDat.obj				: *.h booster/*.h ../guiIO.h ../guiClass.h
charDat_di.obj			: *.h booster/*.h ../guiIO.h ../guiClass.h
inputData.obj			: *.h booster/*.h ../guiIO.h ../guiClass.h
inputDataSub.obj		: *.h booster/*.h ../guiIO.h ../guiClass.h
mainDat.obj				: *.h booster/*.h ../guiIO.h ../guiClass.h
mainDat_func.obj		: *.h booster/*.h ../guiIO.h ../guiClass.h
mainDat_main.obj		: *.h booster/*.h ../guiIO.h ../guiClass.h
mainDat_main_sub.obj	: *.h booster/*.h ../guiIO.h ../guiClass.h
mainDat_mainRoop.obj	: *.h booster/*.h ../guiIO.h ../guiClass.h
mainDat_mainStep.obj	: *.h booster/*.h ../guiIO.h ../guiClass.h
mainDat_manipMenu.obj	: *.h booster/*.h ../guiIO.h ../guiClass.h
mainDat_th075.obj		: *.h booster/*.h ../guiIO.h ../guiClass.h
mainDat_thread.obj		: *.h booster/*.h ../guiIO.h ../guiClass.h
syncData.obj			: *.h booster/*.h ../guiIO.h ../guiClass.h
syncDataSub.obj			: *.h booster/*.h ../guiIO.h ../guiClass.h

