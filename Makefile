CC = gcc
CFLAGS = -Iglad/include
LIBS = -lGL -lglfw -ldl -lm

triangle: main.c glad/src/gl.c
	$(CC) main.c glad/src/gl.c $(CFLAGS) $(LIBS) -o triangle

clean:
	rm -f triangle
