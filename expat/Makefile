CFLAGS=`pkg-config --libs --cflags glib-2.0` -lexpat

all: expat-parse urf-parse

expat-parse: expat-parse.c
	gcc $(CFLAGS) $@.c -o $@

urf-parse: urf-parse.c
	gcc $(CFLAGS) $@.c -o $@

clean:
	rm -f expat-parse urf-parse
