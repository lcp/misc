CFLAGS=`pkg-config --libs --cflags glib-2.0` -lexpat

all:
	gcc $(CFLAGS) expat-parse.c -o expat-parse

clean:
	rm -f expat-parse
