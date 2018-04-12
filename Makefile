ALLEGRO5FLAGS=`pkg-config allegro-5 allegro_primitives-5 allegro_dialog-5 allegro_ttf-5 allegro_font-5 allegro_audio-5 allegro_acodec-5 --cflags --libs`

all:
	gcc -o pong main.c $(ALLEGRO5FLAGS) -lm

clean:
	rm -f *.o main pong
