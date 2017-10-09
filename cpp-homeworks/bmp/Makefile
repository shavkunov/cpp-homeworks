all: main.o bmp.o
	gcc main.o -std=c99 bmp.o -o main

main.o: main.c bmp.h
	gcc -c -std=c99 main.c

bmp.o: bmp.c bmp.h
	gcc -c -std=c99 bmp.c

clean: 
	rm main *.o
