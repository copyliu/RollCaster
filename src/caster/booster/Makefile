CPP	= bcc32
AR	= tlib
CPPFLAGS = -D_WIN32_WINNT=0x0500 -DWINVER=0x0500 -O2 -WM

TARGET	= booster.lib
OBJ1	= class.obj init.obj develop.obj file.obj roop.obj HL.obj
OBJ2	= ai_LNAI.obj ai_SNAI.obj ai_ManageAI.obj ai_SpellAI.obj ai_SWAI.obj ai_RecoverAI.obj ai_BackAI.obj script.obj

OBJP1	= +class.obj +init.obj +develop.obj +file.obj +roop.obj +HL.obj
OBJP2	= +ai_LNAI.obj +ai_SNAI.obj +ai_ManageAI.obj +ai_SpellAI.obj +ai_SWAI.obj +ai_RecoverAI.obj +ai_BackAI.obj +script.obj
LIB		=

all: $(TARGET)
$(TARGET) : $(OBJ1)
$(TARGET) : $(OBJ2)
	-del $(TARGET)
	$(AR) $(TARGET) $(OBJP1) $(OBJP2) $(LIB)

.cpp.obj:
	$(CPP) $(CPPFLAGS) -c $<

# cleanup
clean:
	-del *.obj
	-del *.tds
	-del $(TARGET)

ai_BackAI.obj		: *.h
ai_LNAI.obj			: *.h
ai_ManageAI.obj		: *.h
ai_RecoverAI.obj	: *.h
ai_SNAI.obj			: *.h
ai_SpellAI.obj		: *.h
ai_SWAI.obj			: *.h
class.obj			: *.h
develop.obj			: *.h
file.obj			: *.h
HL.obj				: *.h
init.obj			: *.h
roop.obj			: *.h
script.obj			: *.h

