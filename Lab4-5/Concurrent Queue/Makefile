main: main.o queue.o
	gcc -o main main.o queue.o -g
	rm -f queue.o
	rm -f main.o 

queue.o: queue.c
	gcc -c queue.c

main.o: main.c
	gcc -c main.c

clean: 
	-rm -f main.o queue.o