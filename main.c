#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#define SCREENWIDTH     800
#define SCREENHEIGHT    600

/* TODO:
 *      add sound effects
 *      add sprites
 *      add initial menu
 *      add modern theme and animations
 *      add graphical effects
 */

enum player_side { LEFT, RIGHT };
enum move_direction { UP, DOWN};

typedef struct playerpad {  // Defaults 
    int position_x;         // SCREEN.vcenter
    int position_y;         
    int size;               // SCREENHEIGHT/16
    int speed;              // SCREENHEIGHT/80
    int thickness;          // SCREEN.BORDER
    ALLEGRO_COLOR color;    // White
} PLAYERPAD;


typedef struct player {
    bool active;
    int  side;
    int  score;             // 0
    char scorestr[3];       // sprintf(scorestr, "%d", score)
    PLAYERPAD pad;
} PLAYER;


typedef struct ball {
    int position_x;        
    int position_y;
    int initial_speed_x;    // SCREENWIDTH/160
    int initial_speed_y;    // SCREENHEIGHT/120
    int speed_x;
    int speed_y;
    int size;
    bool ingame;            // False
    ALLEGRO_COLOR color;    // white
} BALL;


typedef struct screen {
    bool draw;
    int width;              // 800
    int height;             // 600
    int border;             // SCREENHEIGHT/75
    int hcenter;            // SCREENWIDTH/2
    int vcenter;            // SCREENHEIGHT/2
    int bot_limit;          // SCREENHEIGHT-border
    int top_limit;          // border
    ALLEGRO_COLOR foreground;
    ALLEGRO_COLOR background;
} SCREEN;


/* ********************************************************************** 
 *
 * INITIALIZATION FUNCTIONS
 *
 * **********************************************************************/

int initialize_allegro_addons(void) {
    /* Initialize all allegro addons required by the game*/

    if (!al_init_font_addon()) {
        al_show_native_message_box(NULL, "Font error", "ERROR", "Could not initialize Allegro 5 Font addon", NULL, 0);
        return -1;
    }

    if (!al_init_ttf_addon()) {
        al_show_native_message_box(NULL, "TTF error", "ERROR", "Could not initialize Allegro 5 TTF addon", NULL, 0);
        return -1;
    }

    if (!al_init_primitives_addon()) {
        al_show_native_message_box(NULL, "Primitives error", "ERROR", "Could not initialize Allegro 5 Primitives addon", NULL, 0);
        return -1;
    }

    if (!al_install_keyboard()) {
        al_show_native_message_box(NULL, "Keyboard Error", "ERROR", "Could not initialize Allegro 5 Keyboard", NULL, 0);
        return -1;
    }

    if (!al_install_mouse()) {
        al_show_native_message_box(NULL, "Mouse Error", "ERROR", "Could not initialize Allegro 5 Mouse", NULL, 0);
        return -1;
    }

    return 0;
}


void init_screen(ALLEGRO_DISPLAY *display, SCREEN *sc, ALLEGRO_COLOR *foreground, ALLEGRO_COLOR *background) {
    /* Like OO initial values ;-) */
    sc->draw = true;
    sc->width  = al_get_display_width(display);
    sc->height = al_get_display_height(display);
    sc->border = sc->width/50;
    sc->hcenter = sc->width/2;
    sc->vcenter = sc->height/2;
    sc->bot_limit  = sc->height-sc->border;
    sc->top_limit  = sc->border;
    sc->foreground = *foreground;
    sc->background = *background;
}



PLAYER * new_player(int side, SCREEN *sc, ALLEGRO_COLOR *color) {
    PLAYER *p = malloc(sizeof(PLAYER));
    p->active = true;
    p->score = 0;
    sprintf(p->scorestr, "%i", p->score);
    p->side = side;
    p->pad.thickness = sc->border;
    p->pad.position_y = sc->vcenter;

    if (p->side == LEFT) {
        p->pad.position_x = 1.5 * sc->border;
    } else if (p->side == RIGHT) {
        p->pad.position_x = sc->width-1.5*sc->border;
    }

    p->pad.size = sc->height / 16;
    p->pad.speed = sc->height / 80;
    p->pad.color = *color;

    return p;
}


void destroy_players(PLAYER **p) {
    free(p[0]);
    free(p[1]);
}


void init_ball(BALL *b, SCREEN *sc, ALLEGRO_COLOR *color) {

    /* Find a random direction in both axis */
    int dir_x = rand() % 2 ? 1:-1;
    int dir_y = rand() % 2 ? 1:-1;

    b->position_x = sc->hcenter;
    b->position_y = sc->vcenter;
    b->initial_speed_x = sc->width/160;
    b->initial_speed_y = sc->height/120;
    b->speed_x = b->initial_speed_x * dir_x;
    b->speed_y = b->initial_speed_y * dir_y;;
    b->size = sc->border;
    b->ingame = false;
    b->color = *color;
}

/* ********************************************************************** 
 * **********************************************************************/

/* ********************************************************************** 
 *
 * DRAWING FUNCTIONS
 *
 * **********************************************************************/

void draw_arena(SCREEN *sc) {
    /*
     * Draw the pong arena based on SCREEN attributes, all WxH dependant
     **/
    al_clear_to_color(sc->background);
    al_draw_filled_rectangle( 0, 0, sc->width, sc->border, sc->foreground);
    al_draw_filled_rectangle( 0, sc->height-sc->border, sc->width, sc->height, sc->foreground);

    int dot, numdots=15;

    for(int i = 0; i<numdots; i++) {
        dot = i*(sc->height/numdots)+sc->border/2;
        al_draw_filled_rectangle((sc->width-sc->border)/2, dot, (sc->width+sc->border)/2, dot+sc->border, sc->foreground);
    }
}


void draw_scores(ALLEGRO_FONT *font, ALLEGRO_COLOR *color, PLAYER **p, SCREEN *sc) {
    al_draw_text(font, *color, sc->hcenter-100, 2*sc->border, ALLEGRO_ALIGN_CENTER, p[0]->scorestr);
    al_draw_text(font, *color, sc->hcenter+100, 2*sc->border, ALLEGRO_ALIGN_CENTER, p[1]->scorestr);
}


void draw_pads(PLAYER **p) {
    /* Pong player 1 */
    al_draw_filled_rectangle( p[0]->pad.position_x - p[0]->pad.thickness/2, p[0]->pad.position_y - p[0]->pad.size,
            p[0]->pad.position_x + p[0]->pad.thickness/2, p[0]->pad.position_y + p[0]->pad.size,
            p[0]->pad.color);

    /* Pong player 2 */
    al_draw_filled_rectangle( p[1]->pad.position_x - p[1]->pad.thickness/2, p[1]->pad.position_y - p[1]->pad.size,
            p[1]->pad.position_x + p[1]->pad.thickness/2, p[1]->pad.position_y + p[1]->pad.size,
            p[1]->pad.color);
}


void draw_ball(BALL *b) {
    /* The "ball" */
    al_draw_filled_rectangle( b->position_x - b->size/2,
            b->position_y - b->size/2,
            b->position_x + b->size/2,
            b->position_y + b->size/2,
            b->color);
}


/* **********************************************************************
 * 
 * Collision functions
 *
 * **********************************************************************/

int norm(int x1, int y1, int x2, int y2) {
    return sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2));
}

bool norm_collision(BALL *b, PLAYER *p) {
    float ball_radius;
    float dist;

    dist = norm(b->position_x, b->position_y, p->pad.position_x, p->pad.position_y);
    ball_radius = b->size;

    fprintf(stdout, "%2i b_pos %4i %4i p_pos %4i %4i size: %2i rad: %5.2f dist: %5.2f\n", 
            p->side,
            b->position_x, b->position_y, 
            p->pad.position_x, p->pad.position_y,
            b->size, ball_radius, dist);

    if (dist <= ball_radius) {
        fprintf(stdout, "Ball hit pad\n");
        return true;
    }
    else
    {
        return false;
    }
}

/* **********************************************************************
 * 
 * Movement function
 *
 * **********************************************************************/


bool wall_collision(BALL *b, SCREEN *sc) {
    if (b->position_y < sc->top_limit || b->position_y > sc->bot_limit) {
        return true;
    } else {
        return false;
    }
}



void move_ball(BALL *b, PLAYER **p, SCREEN *sc) {
    if (norm_collision(b, p[0]) || norm_collision(b, p[1])) {
        fprintf(stdout, "inverting_x\n");
        b->speed_x *= -1;
    }

    // Coliding with the side bars
    if (wall_collision(b, sc)) {
        fprintf(stdout, "inverting_y\n");
        b->speed_y *= -1;
    }

    b->position_x = b->position_x + b->speed_x;
    b->position_y = b->position_y + b->speed_y;
    sc->draw = true;
}


void move_pad(PLAYER *p, int direction, SCREEN *sc) {
    switch(direction) {
        case UP:
            if (p->pad.position_y - p->pad.size > sc->top_limit) {
                p->pad.position_y -= p->pad.speed;
                sc->draw = true;
            }
            break;
        case DOWN:
            if (p->pad.position_y + p->pad.size < sc->bot_limit) {
                p->pad.position_y += p->pad.speed;
                sc->draw = true;
            }
            break;
    }
}

void score(PLAYER *p) {
    p->score += 1;
    sprintf(p->scorestr, "%i", p->score);
}


/* ********************************************************************** 
 * **********************************************************************/

int main(int argc, char *argv[]) {

    /* Init allegro or don't even start anything */
    if(!al_init()) {
        al_show_native_message_box(NULL, "Init error", "ERROR", "Could not initialize Allegro 5", NULL, 0);
        return -1;
    }

    al_set_new_display_flags(ALLEGRO_WINDOWED);
    ALLEGRO_DISPLAY *display = al_create_display(SCREENWIDTH, SCREENHEIGHT);

    if(!display) {
        al_show_native_message_box(NULL, "Window error", "ERROR", "Could not create Allegro window", NULL, 0);
        return -1;
    }

    initialize_allegro_addons();
    al_set_window_title(display, "PONG!");

    srand(time(NULL));
    const int FPS = 60;
    const double dt = 1.0/FPS;


    /* Game Standards */
    ALLEGRO_COLOR white  = al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR grey70 = al_map_rgb(200, 200, 200);
    ALLEGRO_COLOR black  = al_map_rgb(  0,   0,   0);

    ALLEGRO_COLOR *foreground = &grey70;
    ALLEGRO_COLOR *background = &black;
    ALLEGRO_COLOR *ball_color = &white;

    ALLEGRO_FONT *terminusbold = al_load_font("terminusbold.ttf", 80, 0);

    ALLEGRO_TIMER *timer       = al_create_timer(dt);

    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_display_event_source(display));

    ALLEGRO_EVENT events;
    ALLEGRO_KEYBOARD_STATE keystate;

    /* Filling screen structure attributes */
    SCREEN sc;
    init_screen(display, &sc, foreground, background);

    /* The ball */
    BALL ball;
    init_ball(&ball, &sc, ball_color);

    /* The Players */
    PLAYER *p1 = new_player(LEFT,  &sc, foreground);
    PLAYER *p2 = new_player(RIGHT, &sc, foreground);
    PLAYER *players[2] = { p1, p2 };

    /* Loop status */
    bool done = false;


    /* **********************************************************************
     *
     * Main loop 
     *
     * **********************************************************************/
    al_start_timer(timer);
    while(!done) {

        if(!ball.ingame) 
            init_ball(&ball, &sc, ball_color);

        if(sc.draw) {
            draw_arena(&sc);
            draw_scores(terminusbold, foreground, players, &sc);
            draw_pads(players);
            draw_ball(&ball);
            al_flip_display();
            sc.draw = false;
        }

        al_wait_for_event(event_queue, &events);

        /* Window events
         * TODO: ADD resizeability
         */
        if(events.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            done = true;
        }

        /* Input devices events */
        if(events.type == ALLEGRO_EVENT_KEY_UP)
        {
            switch(events.keyboard.keycode)
            {
                case ALLEGRO_KEY_ESCAPE:
                    done = true;
                    break;
                case ALLEGRO_KEY_SPACE:
                    ball.ingame = true;
                    break;
            }
        }

        /* Timer events */
        if(events.type == ALLEGRO_EVENT_TIMER)
        {
            if(ball.ingame) 
                move_ball(&ball, players, &sc);
        }

        al_get_keyboard_state(&keystate);


        /* Player 1 keys */
        if(al_key_down(&keystate, ALLEGRO_KEY_S)) 
            move_pad(p1, DOWN, &sc);
        else if(al_key_down(&keystate, ALLEGRO_KEY_W))
            move_pad(p1, UP, &sc);


        /* Player 2 keys */
        if(al_key_down(&keystate, ALLEGRO_KEY_DOWN))
            move_pad(p2, DOWN, &sc);
        else if(al_key_down(&keystate, ALLEGRO_KEY_UP))
            move_pad(p2, UP, &sc);

        /* Score Points */

        if(ball.position_x > sc.width) {
            score(p1);
            init_ball(&ball, &sc, ball_color);
            sc.draw = true;
        }

        if(ball.position_x < 0) {
            score(p2);
            init_ball(&ball, &sc, ball_color);
            sc.draw = true;
        }

    }

    /* GAME IS CLOSING, cleanup the house */
    destroy_players(players);
    al_clear_to_color(*background);
    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);

    return 0;
}

// vim:foldmethod=syntax:foldlevel=5
