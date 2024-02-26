wish: main.o processShell.o utilities.o
	@echo "Compiling wish executable"
	@gcc main.o processShell.o utilities.o -g -o wish
	@make clean

main.o: main.c
	@gcc -c -g main.c

processShell.o: processShell.c
	@gcc -c -g processShell.c

utilities.o: utilities.c
	@gcc -c -g utilities.c

clean:
	@echo "Removing object files"
	@rm *.o