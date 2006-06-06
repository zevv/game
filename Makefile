
NAME   	= game
SRC 	= game.c main.c 

CFLAGS  += -Wall -Werror -O3 -g 
LDFLAGS += -g -lSDL -lSDL_image -lSDL_mixer

ifeq "$(target)" "win32"

BIN	+= $(NAME).exe
CROSS	+= i586-mingw32msvc-
CFLAGS	+= -Iwinlibs/include
LDFLAGS += -Lwinlibs/lib
LDFLAGS += -lmingw32 -lSDLmain -mwindows 

else

BIN	+= $(NAME)
CFLAGS	+= -I/usr/include/SDL

endif

OBJS    = $(subst .c,.o, $(SRC))
CC 	= $(CROSS)gcc
LD 	= $(CROSS)gcc

.c.o:
	$(CC) $(CFLAGS) -c $<

$(BIN):	$(OBJS) 
	$(LD) -o $@ $(OBJS) $(LDFLAGS)


clean:	
	rm -f $(OBJS) $(BIN) core img.o *.exe


dist-win32: 
	$(MAKE) clean
	$(MAKE) target=win32
	rm -f /tmp/game-win32.zip
	cp winlibs/lib/*dll .
	cd .. && zip -r /tmp/game-win32.zip game/*.dll game/game.exe game/wav/*.wav game/img/*.png game/README  -x .svn 
	rm *dll

dist-linux: 
	$(MAKE) clean
	$(MAKE)
	rm -f /tmp/game-linux.tgz
	cd .. && tar --exclude=.svn -zcvf /tmp/game-linux.tgz game/game game/wav game/img/*.png game/README

dist-src:
	$(MAKE) clean
	rm -f /tmp/game-src.tgz
	cd .. && tar --exclude=.svn -zcvf /tmp/game-src.tgz game/*.c game/*.h game/Makefile game/img game/wav game/README

dist: dist-win32 dist-linux dist-src
	rsync -P /tmp/game-win32.zip /tmp/game-linux.tgz /tmp/game-src.tgz ico@pruts.nl:~/websites/www.zevv.nl/code/game

