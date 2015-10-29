
all: stuff crc

stuff:
	gcc -c stuff.c stream.c
	gcc -o stuff stuff.o stream.o

crc:
	gcc -c -g crc.c stream.c
	gcc -o crc -g crc.o stream.o

clean:
	rm -f *.o crc stuff
