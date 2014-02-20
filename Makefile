default: all


all: tarrasch-t3

tarrasch-t3: tarrasch-thc
	cd src/t3; make
tarrasch-thc:
	cd src/thc; make

clean:
	rm -R *o; rm tarrasch-chess
