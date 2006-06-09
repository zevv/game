
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

all: $(BIN) img/game.png img/help.png

%.o: %.c
	$(CC) $(CFLAGS) -c $<

%.coff: %.rc
	$(WINDRES) $< $@


$(BIN):	$(OBJS) Changelog
	$(LD) -o $@ $(OBJS) $(LDFLAGS)


.PHONE: Changelog
Changelog:
	svn log . > Changelog


clean:	
	rm -f $(OBJS) $(BIN) core img.o *.exe *.coff
	rm -f img/game.png img/help.png
	rm -f Changelog


img/game.png: img/game.svg
	inkscape --file $< \
		--export-area=0:0:256:256 \
		--export-png=$@ 


img/help.png: img/help.svg
	inkscape --file $< \
		--export-png=$@ 
	mogrify -gravity south -fill white -annotate +0+0 "version $(VERSION)" $@


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
	cd .. && tar --exclude=.svn -zcf /tmp/game-linux-$(VERSION).tgz game/game game/wav game/mp3 game/img/*.png game/README.TXT game/Changelog
	md5sum /tmp/game-linux-$(VERSION).tgz > /tmp/game-linux-$(VERSION).tgz.sum

dist-src: Changelog
	$(MAKE) clean
	$(MAKE) Changelog
	$(MAKE) img/game.png img/help.png
	rm -f /tmp/game-src.tgz
	cd .. && tar --exclude=.svn -zcf /tmp/game-src-$(VERSION).tgz game/*.c game/*.h game/Makefile game/img game/wav game/mp3 game/README.TXT game/Changelog
	md5sum /tmp/game-src-$(VERSION).tgz > /tmp/game-src-$(VERSION).tgz.sum

dist: dist-win32 dist-linux dist-src Changelog
	rsync -P \
		README.TXT \
		Changelog \
		/tmp/game-win32-$(VERSION)-setup.exe \
		/tmp/game-linux-$(VERSION).tgz \
		/tmp/game-src-$(VERSION).tgz \
		/tmp/game-win32-$(VERSION)-setup.exe.sum \
		/tmp/game-linux-$(VERSION).tgz.sum \
		/tmp/game-src-$(VERSION).tgz.sum \
		ico@pruts.nl:~/websites/www.zevv.nl/code/game

