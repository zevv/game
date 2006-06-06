
all: game.png

game.png: game.svg
	inkscape --file $< \
		--export-area=0:0:256:256 \
		--export-png=$@ 

