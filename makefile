make:
	gcc -o frame main.c -lpthread
	clear
run:
	./frame input.txt output.txt -1 51515 passivo