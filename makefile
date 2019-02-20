all	: tetris.o
	gcc -g tetris.c -lncurses

clean:
	rm a.out *.o
