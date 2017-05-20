ifeq ($(shell uname -s), Darwin)
LDFLAGS= -L/usr/local/lib -framework OpenGL -lglfw3 -lGLEW
else
LDFLAGS=-lX11 -lGL -lGLU -lglut -lGLEW -lglfw -lm
endif

CC = gcc
CFLAGS=-g -I/usr/local/include

2d_animation: 2d_animation.o
	$(CC) $@.o $(LDFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	-rm -r 2d_animation *.o core *.dSYM

