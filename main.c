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


typedef struct playerpad {
    int position;
    int size;
    int speed;
} PLAYERPAD;


typedef struct player {
    int score;
    PLAYERPAD pad;
} PLAYER;


typedef struct ball {
    int position_x;
    int position_y;
    int next_position_x;
    int next_position_y;
    int initial_speed_x;
    int initial_speed_y;
    int speed_x;
    int speed_y;
    bool ingame;
} BALL;


typedef struct screen {
    int width;
    int height;
    int border;
    int hcenter;
    int vcenter;
    ALLEGRO_COLOR color;
} SCREEN;


void draw_arena(SCREEN *sc) {
    /*
     * Draw the pong arena of size width and height with color
     **/
    al_draw_filled_rectangle( 0, 0, sc.width, sc.border, sc.color);
    al_draw_filled_rectangle( 0, sc.height-sc.border, sc.width, sc.height, sc.color);
    
    int r, nmax=15;

    for(int n = 0; n<nmax; n++) {
        r=n*(sc.height/nmax)+sc.border/2
        al_draw_filled_rectangle((sc.width-sc.border)/2, r, (sc.width+sc.border)/2, r+sc.border, sc.color);
    }
}


void draw_player_pad(PLAYERPAD pad) {

}

int main(int argc, char *argv[]) {

    srand(time(NULL));

    const int FPS = 60;
    const double dt = 1.0/FPS;

    /* Reference to all screen stuff */
    SCREEN sc;

    /* The ball */
    BALL ball;

    /* The Players */
    PLAYER p1;
    PLAYER p2;

    int bot_limit, top_limit;
    /* Pong arena attributes */
    int bar_size = 50;
    int p1_pos;
    int p2_pos;
    int p1_score = 0;
    int p2_score = 0;
    char p1_score_str[3];
    char p2_score_str[3];

    /* Loop status */
    bool done = false;
    bool draw = true;

    PLAYERPAD p1;
    p1.position = 
    p1.size = 25;
    p1.speed = 16;


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

    al_set_window_title(display, "PONG!");

    screen_width  = al_get_display_width(display);
    screen_height = al_get_display_height(display);
    horz_center = screen_width / 2;
    vert_center = screen_height / 2;
    p1_pos = vert_center;
    p2_pos = vert_center;
    bot_limit = screen_height - bar_size + bar_speed - BORDER;
    top_limit = bar_size - bar_speed + BORDER;
    sprintf(p1_score_str,"%d", p1_score);
    sprintf(p2_score_str,"%d", p2_score);

    ALLEGRO_FONT *terminusbold = al_load_font("terminusbold.ttf", 80, 0);

    ALLEGRO_COLOR white      = al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR grey       = al_map_rgb(255, 200, 200);
    ALLEGRO_COLOR background = al_map_rgb(0, 0, 0);

    ALLEGRO_EVENT events;
    ALLEGRO_TIMER *timer     = al_create_timer(dt);
    ALLEGRO_TIMER *balltimer = al_create_timer(dt);
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    ALLEGRO_KEYBOARD_STATE keystate;

    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_display_event_source(display));

    /* Main loop */

    al_start_timer(timer);
    while(!done) {
        
        if(!ball_ingame) {
            /* Ball start in middle of screen */
            ball_pos_x = horz_center;
            ball_pos_y = vert_center;

            ball_direction = rand() % 2 ? 1:-1;
            ball_speed_x = ball_direction*initial_ball_speed_x;

            ball_direction = rand() % 2 ? 1:-1;
            ball_speed_y = ball_direction*initial_ball_speed_y;
        }

        if(draw) {
            
            al_clear_to_color(background);

            draw_arena(screen_width, screen_height, white);

            /* Scores */
            al_draw_text(terminusbold, white, 300, 30, ALLEGRO_ALIGN_CENTER, p1_score_str);
            al_draw_text(terminusbold, white, 500, 30, ALLEGRO_ALIGN_CENTER, p2_score_str);
            
            /* Pong player 1 */
            al_draw_filled_rectangle( BORDER, p1_pos-bar_size, 2*BORDER, p1_pos+bar_size, white);

            /* Pong player 2 */
            al_draw_filled_rectangle( 800-2*BORDER, p2_pos-bar_size, 800-BORDER, p2_pos+bar_size, white);
            
            /* The "ball" */
            al_draw_filled_rectangle( ball_pos_x - BORDER/2,
                                      ball_pos_y - BORDER/2,
                                      ball_pos_x + BORDER/2,
                                      ball_pos_y + BORDER/2,
                                      grey);

            al_flip_display();

            draw = false;
        }

        al_wait_for_event(event_queue, &events);

        if(events.type == ALLEGRO_EVENT_KEY_UP)
        {
            switch(events.keyboard.keycode)
            {
                case ALLEGRO_KEY_ESCAPE:
                    done = true;
                    break;
                case ALLEGRO_KEY_SPACE:
                    ball_ingame = true;
                    break;
            }
        }
        else if(events.type == ALLEGRO_EVENT_MOUSE_AXES)
        {
            p1_pos = events.mouse.y;
            if (events.mouse.dz != 0) {
                fprintf(stdout, "%d\n", events.mouse.dz);
                if (p2_pos - 2*bar_speed * events.mouse.dz < bot_limit && p2_pos - 2*bar_speed * events.mouse.dz > top_limit)
                    p2_pos -= 2*bar_speed*events.mouse.dz;
            }
        }
        else if(events.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            done = true;
        }

        if(events.type == ALLEGRO_EVENT_TIMER)
        {
            al_get_keyboard_state(&keystate);
            
            /* Player 2 keys */
            if(al_key_down(&keystate, ALLEGRO_KEY_DOWN) && p2_pos < bot_limit)
                p2_pos += bar_speed;
            else if(al_key_down(&keystate, ALLEGRO_KEY_UP) && p2_pos > top_limit)
                p2_pos -= bar_speed;

            /* Playe 1 keys */
            if(al_key_down(&keystate, ALLEGRO_KEY_S) && p1_pos < bot_limit)
                p1_pos += bar_speed;
            else if(al_key_down(&keystate, ALLEGRO_KEY_W) && p1_pos > top_limit)
                p1_pos -= bar_speed;

            draw = true;
        }

        if(ball_ingame) {

            next_ball_pos_x = ball_pos_x + ball_speed_x;
            next_ball_pos_y = ball_pos_y + ball_speed_y;

            /* Check colision against BORDER */
            if ( next_ball_pos_y > top_limit && next_ball_pos_y < bot_limit) {
                ball_pos_y = next_ball_pos_y; 
            } else if (next_ball_pos_y < top_limit) {
                ball_pos_y = top_limit;
                ball_speed_y *= -1;
            } else if (next_ball_pos_y > bot_limit) {
                ball_pos_y = bot_limit;
                ball_speed_y *= -1;
            }

            /* In fact here should score a point */
            if ( next_ball_pos_x >= BORDER && next_ball_pos_x <= screen_width-BORDER ) {
                ball_pos_x = next_ball_pos_x; 
            } else if (next_ball_pos_x < BORDER) {
                /* Check if the pad1 is there */
                if (( next_ball_pos_y <= p1_pos + bar_size && next_ball_pos_y >= p1_pos - bar_size) || 
                   ( ball_pos_y <= p1_pos + bar_size && ball_pos_y >= p1_pos - bar_size)) {
                    ball_speed_x *= -1;
                    ball_pos_x = 2*BORDER;
                } else {
                    /* Otherwise scores p2 */
                    p2_score++;
                    sprintf(p2_score_str,"%d", p2_score);
                    ball_ingame=false;
                }

            } else if (next_ball_pos_x > screen_width-BORDER) {
                /* Check if the pad2 is there */
                if (( next_ball_pos_y <= p2_pos + bar_size && next_ball_pos_y >= p2_pos - bar_size) || 
                   ( ball_pos_y <= p2_pos + bar_size && ball_pos_y >= p2_pos - bar_size)) {
                    ball_speed_x *= -1;
                    ball_pos_x = screen_width - 2*BORDER;
                } else {
                    /* Otherwise scores p1*/
                    p1_score++;
                    sprintf(p1_score_str,"%d", p1_score);
                    ball_ingame=false;
                }
            }

            draw = true;
        }

    }

    al_clear_to_color(background);
    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);

    return 0;
}
