hptree=ptree.h
cptree=ptree.c

CC=gcc
CFLAG= -g -Wall -Wextra -o

all: cptree

hptree:
	$(CC) $(hptree)
cptree:hptree
	$(CC) $(cptree) $(CFLAG) ptree

clean:
	rm *.a;rm *.gch;rm *.o;rm *~
