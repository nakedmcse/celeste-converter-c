all: lodepng converter

lodepng: lodepng.c
	gcc -O3 -c lodepng.c

converter: lodepng.o lodepng.h celeste-converter.c
	gcc -O3 -o celeste-converter celeste-converter.c convert.c files.c lodepng.o

clean:
	rm -f *.o
	rm -f celeste-converter