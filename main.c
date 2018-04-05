#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define SCREENWIDTH 800
#define SCREENHEIGHT 600

int main(int argc, char *argv[]) {

    const int FPS = 60;
    const double dt = 1.0/FPS;

    /* Bar speed */
    int bar_speed = 16;
    
    //enum Direction { UP, DOWN, LEFT, RIGHT };

    /* Screen attributes */
    int screen_width, screen_height;
    int horz_center, vert_center;


    /* Simulation parameters */
    double ispeed = 100., jspeed = 100.;
    double istep = 0, jstep = 0.;
    double imult = 1.0, jmult = 1.;
    double i, j;

    /* Pong arena attributes */
    int border = 16;            // must be even
    int center_offset = border/2;
    int p1_pos;
    int p2_pos;
    int bar_size = 25;
    int bot_limit, top_limit;
    int p1_score = 0;
    int p2_score = 2;
    char p1_score_str[2];
    char p2_score_str[2];

    /* Loop status */
    bool done = false;
    bool draw = true;
    int state = 0;


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
    bot_limit = screen_height - bar_size + bar_speed - border;
    top_limit = bar_size - bar_speed + border;
    sprintf(p1_score_str,"%d", p1_score);
    sprintf(p2_score_str,"%d", p2_score);

    ALLEGRO_FONT *terminusbold = al_load_font("terminusbold.ttf", 80, 0);

    ALLEGRO_COLOR white      = al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR background = al_map_rgb(0, 0, 0);

    ALLEGRO_EVENT events;
    ALLEGRO_TIMER *timer = al_create_timer(dt);
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    ALLEGRO_KEYBOARD_STATE keystate;

    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_display_event_source(display));

    /* Main loop */

    al_start_timer(timer);
    while(!done) {
        
        if(draw) {
            
            al_clear_to_color(background);

            /*
             * Pong arena 
             **/
            al_draw_filled_rectangle( 0, 0,  screen_width, border, white);
            al_draw_filled_rectangle( 0, screen_height-border, screen_width, screen_height, white);

            int ri, rn, nmax=15;
            for(int n = 0; n<nmax; n++) {
                ri=n*(screen_height/nmax)+center_offset;
                rn=n*(screen_height/nmax)+border+center_offset;
                al_draw_filled_rectangle(horz_center-border/2, ri, horz_center+border/2, rn, white);
            }

            /* Scores */
            al_draw_text(terminusbold, white, 300, 30, ALLEGRO_ALIGN_CENTER, p1_score_str);
            al_draw_text(terminusbold, white, 500, 30, ALLEGRO_ALIGN_CENTER, p2_score_str);
            
            /* Pong player 1 */
            al_draw_filled_rectangle( border, p1_pos-bar_size, 2*border, p1_pos+bar_size, white);

            /* Pong player 2 */
            al_draw_filled_rectangle( 800-2*border, p2_pos-bar_size, 800-border, p2_pos+bar_size, white);

            al_flip_display();

            draw = false;
        }

        al_wait_for_event(event_queue, &events);

        if(events.type == ALLEGRO_EVENT_KEY_UP)
        {
            switch(events.keyboard.keycode)
            {
                case  ALLEGRO_KEY_ESCAPE:
                    done = true;
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
            if(al_key_down(&keystate, ALLEGRO_KEY_DOWN) && p2_pos < bot_limit)
                p2_pos += bar_speed;
            else if(al_key_down(&keystate, ALLEGRO_KEY_UP) && p2_pos > top_limit)
                p2_pos -= bar_speed;
            if(al_key_down(&keystate, ALLEGRO_KEY_S) && p1_pos < bot_limit)
                p1_pos += bar_speed;
            else if(al_key_down(&keystate, ALLEGRO_KEY_W) && p1_pos > top_limit)
                p1_pos -= bar_speed;

            draw = true;
        }
    }

    al_clear_to_color(background);
    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);

    return 0;
}

