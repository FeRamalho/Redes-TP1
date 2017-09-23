make:
	gcc -o frame main.c -lpthread -lm
	clear
run:
	./frame input.txt output.txt -1 51515 passivo