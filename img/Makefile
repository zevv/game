
all: game.png help.png

game.png: game.svg
	inkscape --file $< \
		--export-area=0:0:256:256 \
		--export-png=$@ 

help.png: help.svg
	inkscape --file $< \
		--export-png=$@ 

