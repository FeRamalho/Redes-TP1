CC = gcc

CFLAGS = -Wall -std=c99 -pedantic -pthread

RM = rm -f

OBJS = main.o

MAIN = main

$(MAIN): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -o $(MAIN)

%.o: %.c
	@$(CC) $(CFLAGS) $< -c 

clean:
	$(RM) $(MAIN) *.o
	clear

run: $(MAIN)
	./$(MAIN)
