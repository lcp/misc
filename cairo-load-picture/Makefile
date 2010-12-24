CFLAGS=`pkg-config --cflags gtk+-2.0`
LIBS=`pkg-config --libs gtk+-2.0`

all:
	gcc main.c cairo-widget.c $(CFLAGS) $(LIBS) -o cairo-test

clean:
	rm -f cairo-test
