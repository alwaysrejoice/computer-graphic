ifeq ($(shell uname -s), Darwin)
LDFLAGS=-L/usr/local/lib -framework OpenGL -lglfw3 -lGLEW

else
LDFLAGS=-lX11 -lGL -lGLU -lglfw3 -lGLEW -lm
CPPFLAGS=
endif

CC = gcc
CFLAGS=-g -I/usr/local/include


INIT_SHADER = initshader.o
MAT = mat.o

hw5: hw5.o $(INIT_SHADER) $(MAT) common.h
	$(CC) $@.o $(INIT_SHADER) $(MAT) $(LDFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@


clean:
	-rm -r hw5 *.o core *.dSYM
