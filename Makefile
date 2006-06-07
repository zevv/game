
NAME   	= game
SRC 	= game.c main.c 
VERSION = $(shell svnversion -c . | cut -d : -f 2)

CFLAGS  += -Wall -Werror -O3 -g 
LDFLAGS += -g -lSDL -lSDL_image -lSDL_mixer

ifeq "$(target)" "win32"

OBJS	+= game.coff
BIN	+= $(NAME).exe
CROSS	+= i586-mingw32msvc-
CFLAGS	+= -Iwinlibs/include
LDFLAGS += -Lwinlibs/lib
LDFLAGS += -lmingw32 -lSDLmain -mwindows 

else

BIN	+= $(NAME)
CFLAGS	+= -I/usr/include/SDL

endif

OBJS    += $(subst .c,.o, $(SRC))

CC 	:= $(CROSS)gcc
LD 	:= $(CROSS)gcc
WINDRES := $(CROSS)windres

%.o: %.c
	$(CC) $(CFLAGS) -c $<

%.coff: %.rc
	$(WINDRES) $< $@


$(BIN):	$(OBJS) 
	$(LD) -o $@ $(OBJS) $(LDFLAGS)


clean:	
	rm -f $(OBJS) $(BIN) core img.o *.exe *.coff


dist-win32: 
	$(MAKE) clean
	$(MAKE) target=win32
	rm -f /tmp/game-win32.zip
	cp winlibs/lib/*dll .
	cd .. && zip -r /tmp/game-win32-$(VERSION).zip game/*.dll game/game.exe game/wav/*.wav game/img/*.png game/README.TXT  -x .svn 
	rm *dll

dist-linux: 
	$(MAKE) clean
	$(MAKE)
	rm -f /tmp/game-linux.tgz
	cd .. && tar --exclude=.svn -zcvf /tmp/game-linux-$(VERSION).tgz game/game game/wav game/img/*.png game/README.TXT

dist-src:
	$(MAKE) clean
	rm -f /tmp/game-src.tgz
	cd .. && tar --exclude=.svn -zcvf /tmp/game-src-$(VERSION).tgz game/*.c game/*.h game/Makefile game/img game/wav game/README.TXT

dist: dist-win32 dist-linux dist-src
	rsync -P \
		README.TXT \
		/tmp/game-win32-$(VERSION).zip \
		/tmp/game-linux-$(VERSION).tgz \
		/tmp/game-src-$(VERSION).tgz \
		ico@pruts.nl:~/websites/www.zevv.nl/code/game

