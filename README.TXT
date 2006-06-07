
A simple tetris-like game -  (C) Copyright 2006 Ico Doornekamp


USAGE
===========================================================================

The objective of the game is to keep the level of colored blocks down
as long as possible. New blocks are falling down at an increasing
rate, blocks can be removed by creating rows of three or more blocks
of the same color. The longer the row, the more points you get.

    space       - Flip two neighbour blocks. 

    arrow keys  - Move cursor

    h,j,k,l     - Move cursor (vi-fans)

    escape      - Quit game, no questions asked.

    s           - Starts the game. If a game is in progress, a new game
		  will be started instantly.

    p           - Pause game. To avoid cheating, the game will be hidden
    		  during pause.

    e		- Generate earthquake. This can be used as a last resort in
		  case of panic, and will remove approximately 30 blocks from 
		  the field. It might help you. Then again, it might not.
		  Can be used once per game and costs 50 points.




INSTALLATION
===========================================================================

Linux:

    Make sure the SDL, SDL_image and SDL_mixer runtime libraries are
    installed

    $ tar -zxvf game-<version>-linux.tgz
    $ cd game
    $ ./game

Windows:

    The windows version comes shipped as a installer program with all necessary
    DLL's included.

    - Run the setup program game-<version>-setup.exe
    - Start the game from the desktop icon or start menu

From source:

    Make sure the SDL, SDL_image and SDL_mixer libraries and development
    files are installed

    $ tar -zxvf game-<version>-src.tgz
    $ cd game
    $ make
    $ ./game


Sound effects shamelessly stolen from bzflag and frozen bubble.

