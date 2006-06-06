
A simple tetris-like game, requires SDL, SDL_image and SDL_mixer
libraries.  (C) Copyright 2006 Ico Doornekamp


USAGE
===========================================================================

The objective of the game is to keep the level of colored blocks down
as long as possible. New blocks are falling down at an increasing
rate, blocks can be removed by creating rows of three or more blocks
of the same color. The longer the row, the more points you get.

Move the cursor around with the mouse, arrow keys or vi-keys (hjkl),
flip two neigbour blocks with the spacebar or mouse button.


    s           - (Re)start
    p           - Pause
    e		- Generate earthquake. Only once per game, costs 50 points
    space       - Flip neighbour blocks
    arrow keys  - Move cursor
    h,j,k,l     - Move cursor
    escape      - Quit game



INSTALLATION
===========================================================================

Linux:

    Make sure the SDL, SDL_image and SDL_mixer runtime libraries are
    installed

    $ tar -zxvf game-linux.tgz
    $ cd game
    $ ./game

Windows:

    The windows version comes shipped with all necassery DLL's.

    - unzip the game.zip archive
    - go to the newly created 'game' directory
    - start 'game.exe'

From source:

    Make sure the SDL, SDL_image and SDL_mixer libraries and development
    files are installed

    $ tar -zxvf game-src.tgz
    $ make
    $ ./game


Sound effects shamelessly stolen from bzflag and frozen-bubble.

--

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

