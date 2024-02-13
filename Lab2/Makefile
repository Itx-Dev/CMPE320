wish: main.o interactiveMode.o batchMode.o
	@echo "Compiling wish executable"
	@gcc main.o interactiveMode.o batchMode.o -o wish
	@make clean

main.o: main.c
	@gcc -c main.c

interactiveMode.o: interactiveMode.c
	@gcc -c interactiveMode.c

batchMode.o: batchMode.c
	@gcc -c batchMode.c

clean:
	@echo "Removing object files"
	@rm *.o