#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#define SCREENWIDTH     800
#define SCREENHEIGHT    600


typedef struct playerpad {  // Defaults 
    int position;           // SCREEN.vcenter
    int size;               // SCREENHEIGHT/16
    int speed;              // SCREENHEIGHT/80
    ALLEGRO_COLOR color;    // White
} PLAYERPAD;


typedef struct player {
    int  score;             // 0
    char scorestr[3];       // sprintf(scorestr, "%d", score)
    PLAYERPAD pad;
} PLAYER;


typedef struct ball {
    int position_x;        
    int position_y;
    int next_position_x;
    int next_position_y;
    int initial_speed_x;    // SCREENWIDTH/160
    int initial_speed_y;    // SCREENHEIGHT/120
    int speed_x;
    int speed_y;
    bool ingame;            // False
    ALLEGRO_COLOR color;    // white
} BALL;


typedef struct screen {
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
    sc->width  = al_get_display_width(display);
    sc->height = al_get_display_height(display);
    sc->border = (int)(sc->width/75);
    sc->hcenter = (int)(sc->width/2);
    sc->vcenter = (int)(sc->height/2);
    sc->bot_limit  = sc->height-sc->border;
    sc->top_limit  = sc->border;
    sc->foreground = *foreground;
    sc->background = *background;
}


void init_player(PLAYER *p, SCREEN *sc, ALLEGRO_COLOR *color) {
    /* Initialize PLAYER on SCREEN */
    p->score = 0;
    sprintf(p->scorestr, "%d", p->score);
    p->pad.position = sc->vcenter;
    p->pad.size = sc->height / 16;
    p->pad.speed = sc->height / 80;
    p->pad.color = *color;
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
    /* Scores */
    al_draw_text(font, *color, sc->hcenter-100, 2*sc->border, ALLEGRO_ALIGN_CENTER, p[0]->scorestr);
    al_draw_text(font, *color, sc->hcenter+100, 2*sc->border, ALLEGRO_ALIGN_CENTER, p[1]->scorestr);
}


void draw_pads(PLAYER **p, SCREEN *sc) {
    /* Pong player 1 */
    al_draw_filled_rectangle(   sc->border, p[0]->pad.position - p[0]->pad.size,
                              2*sc->border, p[0]->pad.position + p[0]->pad.size,
                              p[0]->pad.color);
    
    /* Pong player 2 */
    al_draw_filled_rectangle(   sc->width - 2*sc->border, p[1]->pad.position - p[1]->pad.size,
                                sc->width -   sc->border, p[1]->pad.position + p[1]->pad.size,
                              p[1]->pad.color);
}


void draw_ball(BALL *b, SCREEN *sc) {
    /* The "ball" */
    al_draw_filled_rectangle( b->position_x - sc->border/2,
                              b->position_y - sc->border/2,
                              b->position_x + sc->border/2,
                              b->position_y + sc->border/2,
                              b->color);
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

    // /* The Players */
    PLAYER p1;
    PLAYER p2;
    PLAYER players[2] = { p1, p2 };
    init_player(&p1, &sc, foreground);
    init_player(&p2, &sc, foreground);

    /* Loop status */
    bool done = false;
    bool draw = true;


    /* **********************************************************************
     *
     * Main loop 
     *
     * **********************************************************************/
    al_start_timer(timer);
    while(!done) {
        
        if(!ball.ingame) 
            init_ball(&ball, &sc, ball_color);

         if(draw) {
             draw_arena(&sc);
             draw_scores(terminusbold, foreground, players, &sc);
    //         draw_pads(players, &sc);
    //         draw_ball(&ball, &sc);
             al_flip_display();
             draw = false;
         }

    //     al_wait_for_event(event_queue, &events);

    //     /*
    //      * What is the best way to make these event captring external to
    //      * main()?
    //      */
    //     
    //     /* Window events
    //      * TODO: ADD resizeability
    //      */
    //     if(events.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
    //     {
    //         done = true;
    //     }

    //     /* Input devices events */
    //     if(events.type == ALLEGRO_EVENT_KEY_UP)
    //     {
    //         switch(events.keyboard.keycode)
    //         {
    //             case ALLEGRO_KEY_ESCAPE:
    //                 done = true;
    //                 break;
    //             case ALLEGRO_KEY_SPACE:
    //                 ball.ingame = true;
    //                 break;
    //         }
    //     }
    //     

    //     // TODO: REWRITE MOVEMENT AND COLLISION DETECTION
    //     
    //     // if(events.type == ALLEGRO_EVENT_MOUSE_AXES)
    //     // {
    //     //     p1_pos = events.mouse.y;
    //     //     if (events.mouse.dz != 0) {
    //     //         // TODO: ADD MOVE PAD FUNCTIONS
    //     //         //      REMOVE ALL THOSE CALCULATIONS FROM HERE!
    //     //         fprintf(stdout, "%d\n", events.mouse.dz);
    //     //         if (p2_pos - 2*bar_speed * events.mouse.dz < bot_limit && p2_pos - 2*bar_speed * events.mouse.dz > top_limit)
    //     //             p2_pos -= 2*bar_speed*events.mouse.dz;
    //     //     }
    //     // }



    //     // /* Timer events */
    //     // if(events.type == ALLEGRO_EVENT_TIMER)
    //     // {
    //     //     al_get_keyboard_state(&keystate);
    //     //     
    //     //     // TODO: REMOVE THE REPOSITIONING FROM HERE, CREATE A FUNCTION
    //     //     //      FOR THAT.
    //     //     
    //     //     /* Playe 1 keys */
    //     //     if(al_key_down(&keystate, ALLEGRO_KEY_S) && p1_pos < bot_limit)
    //     //         p1_pos += bar_speed;
    //     //     else if(al_key_down(&keystate, ALLEGRO_KEY_W) && p1_pos > top_limit)
    //     //         p1_pos -= bar_speed;

    //     //     /* Player 2 keys */
    //     //     if(al_key_down(&keystate, ALLEGRO_KEY_DOWN) && p2_pos < bot_limit)
    //     //         p2_pos += bar_speed;
    //     //     else if(al_key_down(&keystate, ALLEGRO_KEY_UP) && p2_pos > top_limit)
    //     //         p2_pos -= bar_speed;


    //     //     draw = true;
    //     // }

    //     // if(ball_ingame) {

    //     //     // TODO: CREATE A FUNCTION TO GAME ITERATION. TO NOT DO IT HERE
    //     //     next_ball_pos_x = ball_pos_x + ball_speed_x;
    //     //     next_ball_pos_y = ball_pos_y + ball_speed_y;

    //     //     // TODO: CREATE ROUTINES FOR COLLISION DETECTION
    //     //     //      TAKE THE CALCULATIONS OUT OF HERE, CREATE REPOSITIONING
    //     //     //      FUNCTIONS
    //     //     
    //     //     /* Check colision against BORDER */
    //     //     if ( next_ball_pos_y > top_limit && next_ball_pos_y < bot_limit) {
    //     //         ball_pos_y = next_ball_pos_y; 
    //     //     } else if (next_ball_pos_y < top_limit) {
    //     //         ball_pos_y = top_limit;
    //     //         ball_speed_y *= -1;
    //     //     } else if (next_ball_pos_y > bot_limit) {
    //     //         ball_pos_y = bot_limit;
    //     //         ball_speed_y *= -1;
    //     //     }

    //     //     /* In fact here should score a point */

    //     //     // TODO: REMOVE ALL COLISION CHECK FROM HERE
    //     //     //      CREATE SCORE FUNCTIONS
    //     //     
    //     //     if ( next_ball_pos_x >= BORDER && next_ball_pos_x <= screen_width-BORDER ) {
    //     //         ball_pos_x = next_ball_pos_x; 
    //     //     } else if (next_ball_pos_x < BORDER) {
    //     //         /* Check if the pad1 is there */
    //     //         if (( next_ball_pos_y <= p1_pos + bar_size && next_ball_pos_y >= p1_pos - bar_size) || 
    //     //            ( ball_pos_y <= p1_pos + bar_size && ball_pos_y >= p1_pos - bar_size)) {
    //     //             ball_speed_x *= -1;
    //     //             ball_pos_x = 2*BORDER;
    //     //         } else {
    //     //             /* Otherwise scores p2 */
    //     //             p2_score++;
    //     //             sprintf(p2_score_str,"%d", p2_score);
    //     //             ball_ingame=false;
    //     //         }

    //     //     } else if (next_ball_pos_x > screen_width-BORDER) {
    //     //         /* Check if the pad2 is there */
    //     //         if (( next_ball_pos_y <= p2_pos + bar_size && next_ball_pos_y >= p2_pos - bar_size) || 
    //     //            ( ball_pos_y <= p2_pos + bar_size && ball_pos_y >= p2_pos - bar_size)) {
    //     //             ball_speed_x *= -1;
    //     //             ball_pos_x = screen_width - 2*BORDER;
    //     //         } else {
    //     //             /* Otherwise scores p1*/
    //     //             p1_score++;
    //     //             sprintf(p1_score_str,"%d", p1_score);
    //     //             ball_ingame=false;
    //     //         }
    //     //     }

    //     //     draw = true;
    //     // }

    }

    /* GAME IS CLOSING, cleanup the house */
    al_clear_to_color(*background);
    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);

    return 0;
}

// vim:foldmethod=syntax:foldlevel=5
