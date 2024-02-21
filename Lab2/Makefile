wish: main.o processShell.o throwError.o
	@echo "Compiling wish executable"
	@gcc main.o processShell.o throwError.o -o wish
	@make clean

main.o: main.c
	@gcc -c main.c

processShell.o: processShell.c
	@gcc -c processShell.c

throwError.o: throwError.c
	@gcc -c throwError.c

clean:
	@echo "Removing object files"
	@rm *.o