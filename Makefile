Program = ptree
hmain = main.h
cmain = main.c

CC = gcc
CFLAG = -ggdb -Og -pipe -Wall -Wextra -o

all: cmain clean

hmain:
	$(CC) $(hmain)
cmain: hmain
	$(CC) $(cmain) $(CFLAG) $(Program)

clean:
	rm *.gch
