default: all


all: tarrasch-t3

tarrasch-t3:
	cd src/t3; make

clean:
	rm -R *o; rm tarrasch-chess
