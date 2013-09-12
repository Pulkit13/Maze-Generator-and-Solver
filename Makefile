hw4: main.o mazegen.o mazesolve.o
	gcc -g -Wall -o hw4 main.o mazegen.o mazesolve.o

main.o: main.c mazegen.h mazesolve.h
	gcc -g -Wall -c main.c
mazegen.o: mazegen.c mazegen.h
	gcc -g -Wall -c mazegen.c
mazesolve.o: mazesolve.c mazesolve.h
	gcc -g -Wall -c mazesolve.c

.PHONY: clean

clean:
	rm main.o mazegen.o mazesolve.o hw4
	
