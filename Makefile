PROGRAM = 20202tp1
CC = gcc
CFLAGS = -Werror -Wall -pedantic -std=c99 -g
LDLIBS = -lm

all :  $(PROGRAM)

$(PROGRAM) : main.o imagen.o pixel.o filtro.o argumentos.o endianness.o
	$(CC) $(CFLAGS) main.o imagen.o pixel.o argumentos.o filtro.o endianness.o -o $(PROGRAM) $(LDLIBS)

main.o : main.c argumentos.h
	$(CC) $(CFLAGS) -c main.c

imagen.o : imagen.c imagen.h pixel.h filtro.h endianness.h
	$(CC) $(CFLAGS) -c imagen.c

pixel.o : pixel.c pixel.h
	$(CC) $(CFLAGS) -c pixel.c

filtro.o : filtro.c filtro.h pixel.h
	$(CC) $(CFLAGS) -c filtro.c

argumentos.o : argumentos.c argumentos.h imagen.h filtro.h
	$(CC) $(CFLAGS) -c argumentos.c

endianness.o : endianness.c endianness.h
	$(CC) $(CFLAGS) -c endianness.c

clean :
	rm *.o