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

// Include for threading
#include <unistd.h>
// ---------------------------- Global variables -------------------------- //
ALLEGRO_DISPLAY*    display;
ALLEGRO_EVENT_QUEUE* event_queue;
ALLEGRO_EVENT       event;

// ---------------------------- Vector3f class -------------------------- //
/**
 * This is a simple three-dimensional vector structure, which lets you do
 * addition/subtraction of vectors as well as multiplication and division by constants.
 * Example:
 *   vector3f red(1.0, 0.0, 0.0);   // Red color
 *   vector3f green(0.0, 1.0, 0.0); // Blue color
 *   vector3f yellow = red * 0.5 + green * 0.5; // Mixture of red and green
 *   cout << yellow << endl;        // Debug output
 *   al_put_pixel(x, y, yellow.as_color()); // Use with Allegro
 */
struct vector3f {
    float x, y, z;
    vector3f (float x, float y): x(x), y(y), z(0) {};
    vector3f (float x, float y, float z): x(x), y(y), z(z) {};
    vector3f (ALLEGRO_COLOR color) {
        al_unmap_rgb_f(color, &x, &y, &z);
    }
    vector3f operator+ (vector3f v) {
        return vector3f (x + v.x, y + v.y, z + v.z);
    }
    vector3f operator+= (vector3f v) {
        x += v.x; y += v.y; z += v.z;
        return *this;
    }
    vector3f operator-= (vector3f v) {
        x -= v.x; y -= v.y; z -= v.z;
        return *this;
    }
    vector3f operator- (vector3f v) {
        return vector3f (x - v.x, y - v.y, z - v.z);
    }
    vector3f operator* (float f) {
        return vector3f (x * f, y * f, z * f);
    }
    vector3f operator/ (float f) {
        return vector3f (x / f, y / f, z / f);
    }
    ALLEGRO_COLOR as_color() {
        float r = min(1.0f, max(0.0f, x));
        float g = min(1.0f, max(0.0f, y));
        float b = min(1.0f, max(0.0f, z));
        return al_map_rgb_f(r, g, b);
    }
    friend ostream& operator<< (ostream& out, vector3f& v) {
        out << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return out;
    }
};

// ---------------------------- Forward declarations -------------------------- //
void init();
void deinit();
void event_loop();
void draw();


// You will need to implement or extend these functions
void color_triangle(int x1, int y1, int x2, int y2, int x3, int y3, vector3f c1, vector3f c2, vector3f c3);
void draw_horizontal_line(float x1, vector3f color1, float x2, vector3f color2, int y);
void fill_flat_bottom_triangle(int x1, int y1, int x2, int y2, int x3, int y3, vector3f c1, vector3f c2, vector3f c3);
void fill_flat_top_triangle(int x1, int y1, int x2, int y2, int x3, int y3, vector3f c1, vector3f c2, vector3f c3);

// Utility functions
void sort_triangle_with_attributes(int& x1, int& y1, int& x2, int& y2, int& x3, int& y3, vector3f& c1, vector3f& c2, vector3f& c3);


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
    display = al_create_display(600, 200);
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
            if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || event.type == ALLEGRO_EVENT_KEY_DOWN) break;
        }
        // No animation this time, simply drawing
        draw();
        // Sleep for 1ms
        usleep(1000);
   }
}

// ---------------------------- Drawing routines -------------------------- //
// Where all the drawing is performed.

void draw() {
    //clear the buffer from last frame
    al_clear_to_color(al_color_name("white"));

    // Here we test the implementations of the algorithms.
    // We shall draw four sample triangles for each method
    int NUM_TRIANGLES = 4;
    int triangles[][3][2] = { { { 75,  20}, { 25,  75}, {125, 130} },  // Default case
                              { {125,  75}, { 75,  20}, { 25, 130} },  // Non-sequential order of along Y axis, left and right edges flipped
                              { { 25,  75}, {125,  75}, { 75,  20} },  // Lacking bottom part
                              { { 25,  25}, {125,  25}, { 75, 80} },  // Lacking top part
                            };
    vector3f red(1.0, 0.0, 0.0);
    vector3f green(0.0, 1.0, 0.0);
    vector3f blue(0.0, 0.0, 1.0);

    // Reference
    ALLEGRO_COLOR c = al_color_name("black");
    int OX = 0, OY = 25;
    for (int i = 0; i < NUM_TRIANGLES; i++) {
        color_triangle(triangles[i][0][0] + OX, triangles[i][0][1] + OY,
                       triangles[i][1][0] + OX, triangles[i][1][1] + OY,
                       triangles[i][2][0] + OX, triangles[i][2][1] + OY,
                       red, green, blue);
        al_draw_triangle(triangles[i][0][0] + OX, triangles[i][0][1] + OY,
                                triangles[i][1][0] + OX, triangles[i][1][1] + OY,
                                triangles[i][2][0] + OX, triangles[i][2][1] + OY,
                                c, 1);
        OX += 150;
    }

    // We end by flipping the buffer:
    al_flip_display();
}

// ----------------------- Exercises ------------------------ //

/**
 * Draws a triangle with color gradient
 * You don't have to add anything to this method, but instead
 * implement the methods fill_flat_bottom_triangle and fill_flat_top_triangle.
 */
void color_triangle(int x1, int y1, int x2, int y2, int x3, int y3, vector3f c1, vector3f c2, vector3f c3) {
    // Sort points vertically
    sort_triangle_with_attributes(x1, y1, x2, y2, x3, y3, c1, c2, c3);

    /* Special case where triangle has flat bottom */
    if(y2 == y3) {
        fill_flat_bottom_triangle(x1, y1, x2, y2, x3, y3, c1, c2, c3);
    /* Special case where triangle has flat top */
    } else if (y1 == y2) {
        fill_flat_top_triangle(x1, y1, x2, y2, x3, y3, c1, c2, c3);
    /* General case*/
    } else {
        fill_flat_top_triangle(x1, y1, x2, y2, x3, y3, c1, c2, c3);
        fill_flat_bottom_triangle(x1, y1, x2, y2, x3, y3, c1, c2, c3);
    }

}

/**
 * --Task--
 * Finish this function.
 * It should increase y and move on both non-horizontal lines of the triangle, changing the color and x.
 */
void fill_flat_bottom_triangle(int x1, int y1, int x2, int y2, int x3, int y3, vector3f c1, vector3f c2, vector3f c3) {
    // 1) Find how much x should change, when y changes by 1 for both lines (e.g. deltaX1, deltaX2)
    //        float deltaXA = float(x1 - x3) / (y1 - y3);
    // 2) Find how much the color should change, when y changes by 1 for both lines.
    // 3) Make some variables to hold the current x and color values
    // 4) Create a loop, where we change the y by 1 and then add the change of x and color to the corresponding variables.
    // 4.1) Inside the loop call the horizontal gradient line drawing function (draw_horizontal_line).

    float deltaXA = float(x1 - x3) / (y1 - y3);
    float deltaXB = float(x1 - x2) / (y1 - y2);

    float XA = x1;
    float XB = x1;

    vector3f deltaCA = (c1 - c3) / (y1 - y3);
    vector3f deltaCB = (c1 - c2) / (y1 - y2);

    vector3f CA = c1;
    vector3f CB = c1;

    for (int y = y1; y < y2; y++) {
        draw_horizontal_line(XA, CA, XB, CB, y);

        XA += deltaXA;
        XB += deltaXB;
        CA += deltaCA;
        CB += deltaCB;
    }
}

/**
 * --Task--
 * Finish this function also.
 */
void fill_flat_top_triangle(int x1, int y1, int x2, int y2, int x3, int y3, vector3f c1, vector3f c2, vector3f c3) {
    // Same as flat bottom triangle, but start from bottom vertex.

    float deltaXA = float(x3 - x1) / (y3 - y1);
    float deltaXB = float(x3 - x2) / (y3 - y2);

    float XA = x3;
    float XB = x3;

    vector3f deltaCA = (c3 - c1) / (y3 - y1);
    vector3f deltaCB = (c3 - c2) / (y3 - y2);

    vector3f CA = c3;
    vector3f CB = c3;

    for (int y = y3; y >= y2; y--) {
        draw_horizontal_line(XA, CA, XB, CB, y);

        XA -= deltaXA;
        XB -= deltaXB;
        CA -= deltaCA;
        CB -= deltaCB;
    }
}

/**
 * --Task--
 * This method draws a horizontal line from (x1, y) to (x2, y)
 * Your task is to implement color gradient for the horizontal axis
 */
void draw_horizontal_line(float x1, vector3f color1, float x2, vector3f color2, int y) {
    if (x1 > x2) {
        swap(x1, x2);
        swap(color1, color2);
    }

    //Find how much the color should change for 1 unit of change in the x direction.
    vector3f deltaC = (color2 - color1) / (x2 - x1);

    for (int x = x1; x <= x2; x++) {
        al_put_pixel(x, y, color1.as_color());

        //Add that change to the color
        color1 += deltaC;
    }
}

// ----------------------- Utility functions ------------------------ //

/**
 * Given three triangle vertices sorts them vertically.
 * Also sorts attribute vectors attached to the vertices.
 */
void sort_triangle_with_attributes(int& x1, int& y1, int& x2, int& y2, int& x3, int& y3, vector3f& c1, vector3f& c2, vector3f& c3) {
    if (y2 > y3) {
        swap(x2, x3); swap(y2, y3); swap(c2, c3);
    }
    if (y1 > y2) {
        swap(x1, x2); swap(y1, y2); swap(c1, c2);
    }
    if (y2 > y3) {
        swap(x2, x3); swap(y2, y3); swap(c2, c3);
    }
    if (y1 == y2 && x1 > x2) {
        swap(x1, x2); swap(y1, y2); swap(c1, c2);
    }
}
