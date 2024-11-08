TARGET = eboot
OBJS = main.o

UNAME := $(shell uname)

EXTRA_TARGETS = EBOOT.PBP

PSP_EBOOT_TITLE = Theme Dumper by Krazynez

PSP_EBOOT_ICON = .res/icon0.png

all:
	#pack-pbp $(EXTRA_TARGETS) PARAM.SFO .res/icon0.png NULL NULL NULL NULL data.psp NULL
	python bin/psptools/pack_ms_game.py --vanity "Theme Dumper" EBOOT.PBP EBOOT.PBP

INCDIR = ./inc
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti -fno-int-conversion
ASFLAGS = $(CFLAGS) -c

LIBDIR = ./lib 
#LIBS = 
LDFLAGS = -L.

LIBS = -lpspexploit -lpsprtc


PSP_FW_VERSION = 660

BUILD_PRX = 1


PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

release: all
	@mkdir -p PSP/GAME/Theme_Dumper
	@cp EBOOT.PBP PSP/GAME/Theme_Dumper/
	@zip -r Theme_Dumper.zip PSP/ README.md

clean:
	@rm -rf $(TARGET)* prefix data.psp PSP *.zip *.o *.PBP *.SFO
	@echo Cleaned


