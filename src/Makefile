CPP = bcc32
CPPFLAGS = -WM -O2 -c

TARGET1	= RollCaster
TARGET2	= RollCasterGUI

# make gui
gui:
	make -fMakefileGUI

# make cui
cui:
	make -fMakefileCUI


all: $(TARGET1) $(TARGET2)

$(TARGET1) :
	make -fMakefileCUI clean
	make -fMakefileCUI

$(TARGET2) :
	make -fMakefileGUI clean
	make -fMakefileGUI



# cleanup
clean:
	-del *.obj
	-del *.exe
	-del *.tds

	cd caster
	make clean
	cd ..

