// ---------------------------- Includes -------------------------- //

// Include for console logging.
#include <iostream>
using namespace std;

// Use mathematics routines
#include <math.h>

// Include Allegro headers.
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>

// ---------------------------- Global variables -------------------------- //
ALLEGRO_DISPLAY*    display;
ALLEGRO_EVENT_QUEUE* event_queue;
ALLEGRO_EVENT       event;

// ---------------------------- Forward declarations -------------------------- //
void init();
void deinit();
void event_loop();
void draw();

// You will need to implement those two functions
void bresenham_line(int x1, int y1, int x2, int y2, ALLEGRO_COLOR color);

// ---------------------------- Main -------------------------- //
// The overall structure of our program is the familiar GUI event loop:

int main(){
    init();         // Initialize Allegro.
    event_loop();   // Run the event processing loop until a program is requested to quit
    deinit();       // Deinitialize

    return 0;       // This is simply required by the C++ standard to indicate successful completion
}


// ---------------------------- Allegro initialization -------------------------- //
// Initialize the Allegro system.

void init() {
    al_init();
    display = al_create_display(600, 300);
    event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_install_keyboard();
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_init_primitives_addon();
}

// ---------------------------- Allegro deinitialization -------------------------- //
// Just the opposite of "init". Destroy whatever we created.

void deinit() {
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);
}

// ---------------------------- Event loop -------------------------- //

void event_loop() {
    while(1) {
        // We only need events to close the app when necessary
        bool have_event = al_wait_for_event_timed(event_queue, &event, 0.05);
        if (have_event) {
            if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || event.type == ALLEGRO_EVENT_KEY_UP) break;
        }
        // No animation this time, simply drawing
        draw();
   }
}

// ---------------------------- Drawing routines -------------------------- //
// Where all the drawing is performed.

void draw() {
    al_clear_to_color(al_map_rgb(255,255,255));

    // Here we test the implementations of the line algorithms.
    int CX = 150;
    int CY = 150;
    int R = 100;
    int NUM_LINES = 20;
    float STEP = 2*3.14/NUM_LINES;

    // Reference
    ALLEGRO_COLOR c = al_color_name("black");
    for (int i = 0; i < NUM_LINES; i++) {
        al_draw_line(CX, CY, CX + (int)(R*cos(STEP*i)), CY + (int)(R*sin(STEP*i)), c, 1);
    }

    // Bresenham
    c = al_color_name("red");
    CX += 300;
    for (int i = 0; i < NUM_LINES; i++) {
        bresenham_line(CX, CY, CX + (int)(R*cos(STEP*i)), CY + (int)(R*sin(STEP*i)), c);
    }

    // We end by flipping the buffer:
    al_flip_display();
}

// ----------------------- Exercises ------------------------ //

/**
 * --Task--
 * Finish the Bresenham's line algorithm here.
 */
void bresenham_line(int x1, int y1, int x2, int y2, ALLEGRO_COLOR color) {

    if (x1 >= x2) { // Draw lines from left to right
        swap (x1, x2);
        swap (y1, y2);
    }

    float deltaY = y1 - y2; //Change in y
    float deltaX = x1 - x2; //Change in x
    float error = 0.0;      //Here we accumulate the error
    float deltaError = 0.0; //This is the error we make on each step (ie the slope)
    int x, y;

    if (abs(deltaX) >= abs(deltaY)) { //Non-steep lines (change in X is greater than change in Y)
        deltaError = deltaY / deltaX;

        if (deltaError <= 0) { //Ascending line: positive change in X results in negative change in Y
            y = y1;
            for (x = x1; x <= x2; x++) { //We move in x
                al_draw_pixel(x, y, color);
                error += deltaError;
                if (error < -0.5) { //Our error has become too great to ignore, move also in y
                    y -= 1;
                    error += 1.0; //We just moved one unit, change the error value accordingly
                }
            }
        } else { //Descending line
            y = y1;
            for (x = x1; x <= x2; x++) { //We move in x
                al_draw_pixel(x, y, color);
                error += deltaError;
                if (error > 0.5) { //Our error has become too great to ignore, move also in y
                    y += 1;
                    error -= 1.0; //We just moved one unit, change the error value accordingly
                }
            }
        }

    } else { //Steep line
        if(y1 > y2) { //We draw them from bottom to top
            swap (x1, x2);
            swap (y1, y2);
        }

        deltaError = deltaX / deltaY;

        if (deltaError <= 0) { //Ascending line: positive change in X results in negative change in Y
            x = x1;
            for (y = y1; y <= y2; y++) { //We move in x
                al_draw_pixel(x, y, color);
                error += deltaError;
                if (error < -0.5) { //Our error has become too great to ignore, move also in y
                    x -= 1;
                    error += 1.0; //We just moved one unit, change the error value accordingly
                }
            }
        } else { //Descending line
            x = x1;
            for (y = y1; y <= y2; y++) { //We move in x
                al_draw_pixel(x, y, color);
                error += deltaError;
                if (error > 0.5) { //Our error has become too great to ignore, move also in y
                    x += 1;
                    error -= 1.0; //We just moved one unit, change the error value accordingly
                }
            }
        }

    }

}

void bresenham_line_2(int x1, int y1, int x2, int y2, ALLEGRO_COLOR color) {

}
