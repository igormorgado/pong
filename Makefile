ALLEGRO5FLAGS=`pkg-config allegro-5 allegro_primitives-5 allegro_dialog-5 allegro_ttf-5 allegro_font-5 --cflags --libs`

all:
	gcc -o main main.c $(ALLEGRO5FLAGS)

clean:
	rm -f *.o main