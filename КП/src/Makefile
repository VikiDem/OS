all: run
	
run: getl.o
	gcc A.c getl.o -o A
	gcc B.c getl.o -o B
	gcc C.c getl.o -o C

getl.o: getl.c
	gcc -c getl.c

clean:
	rm A B C getl.o