
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
STRIP 	:= $(CROSS)strip
WINDRES := $(CROSS)windres
NSIS	:= makensis


%.o: %.c
	$(CC) $(CFLAGS) -c $<

%.coff: %.rc
	$(WINDRES) $< $@


$(BIN):	$(OBJS) 
	$(LD) -o $@ $(OBJS) $(LDFLAGS)


clean:	
	rm -f $(OBJS) $(BIN) core img.o *.exe *.coff
	make -C img clean

dist-win32: 
	$(MAKE) clean
	$(MAKE) target=win32
	$(STRIP) $(NAME).exe
	$(NSIS) -V2 -DVERSION="$(VERSION)" -DNAME="$(NAME)" -DBIN="$(NAME).exe" -DDIST="/tmp/$(NAME)-win32-$(VERSION)-setup.exe" installer.nsi
	md5sum /tmp/$(NAME)-win32-$(VERSION)-setup.exe > /tmp/$(NAME)-win32-$(VERSION)-setup.exe.sum

dist-linux: 
	$(MAKE) clean
	$(MAKE)
	$(STRIP) $(BIN)
	rm -f /tmp/game-linux.tgz
	cd .. && tar --exclude=.svn -zcvf /tmp/game-linux-$(VERSION).tgz game/game game/wav game/mp3 game/img/*.png game/README.TXT
	md5sum /tmp/game-linux-$(VERSION).tgz > /tmp/game-linux-$(VERSION).tgz.sum

dist-src:
	$(MAKE) clean
	rm -f /tmp/game-src.tgz
	cd .. && tar --exclude=.svn -zcvf /tmp/game-src-$(VERSION).tgz game/*.c game/*.h game/Makefile game/img game/wav game/mp3 game/README.TXT
	md5sum /tmp/game-src-$(VERSION).tgz > /tmp/game-src-$(VERSION).tgz.sum

dist: dist-win32 dist-linux dist-src
	rsync -P \
		README.TXT \
		/tmp/game-win32-$(VERSION)-setup.exe \
		/tmp/game-linux-$(VERSION).tgz \
		/tmp/game-src-$(VERSION).tgz \
		/tmp/game-win32-$(VERSION)-setup.exe.sum \
		/tmp/game-linux-$(VERSION).tgz.sum \
		/tmp/game-src-$(VERSION).tgz.sum \
		ico@pruts.nl:~/websites/www.zevv.nl/code/game

