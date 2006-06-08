
VERSION = $(shell svnversion -c .. | cut -d : -f 2)

all: game.png help.png

game.png: game.svg
	inkscape --file $< \
		--export-area=0:0:256:256 \
		--export-png=$@ 

help.png: help.svg
	inkscape --file $< \
		--export-png=$@ 
	mogrify -gravity south -fill white -annotate +0+0 "version $(VERSION)" $@

clean:
	rm -f game.png help.png

