.PHONY: all clean runtest compress decompress demodir

all: build/compress.exe build/decompress.exe

build/compress.exe: src/compress.c
	cl src/compress.c -o build/compress
	del compress.obj

build/decompress.exe: src/decompress.c
	cl src/decompress.c -o build/decompress
	del decompress.obj

runtest: demodir compress decompress

compress:
	@build/compress demo/input.txt demo/compr.pat

decompress:
	@build/decompress demo/compr.pat demo/output.txt

clean:
	del *.pat
	rmdir /s build
	mkdir build