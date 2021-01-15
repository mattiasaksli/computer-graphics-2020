// ---------------------------- Includes -------------------------- //
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <GLFW/glfw3.h>

// ---------------------------- Forward declarations -------------------------- //
void draw(GLFWwindow *win);
void drawTriangle(float x, float y);
int width = 640;
int height = 480;
float aspectRatio = (float)width / height;

// ---------------------------- Input -------------------------- //
// Listen for input event
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

// ---------------------------- Main -------------------------- //
int main(int argc, char *argv[]) {
    GLFWwindow *win;

    if (!glfwInit()) { //Something went wrong with GLFW initialization
        exit (EXIT_FAILURE);
    }

    win = glfwCreateWindow(width, height, "Hello GLFW!", NULL, NULL);

    if (!win) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    //Sets the win context current in OpenGL state machine.
    //Following OpenGL commands will be directed at this context.
    glfwMakeContextCurrent(win);

    //Register the callback method for input
    glfwSetKeyCallback(win, key_callback);

    const GLubyte* renderer = glGetString (GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString (GL_VERSION); // version as a string
    printf ("Renderer: %s\n", renderer);
    printf ("OpenGL version supported %s\n", version);

    //Event loop
    while (!glfwWindowShouldClose(win)) {
        draw(win);
        usleep(1000);
        glfwPollEvents();
    }

    glfwTerminate();
    exit(EXIT_SUCCESS);
    return 0;
}

// ---------------------------- Drawing -------------------------- //
/**
 * Everything related to drawing the scene
 */
void draw(GLFWwindow *win) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT); //Clear last frame

    drawTriangle(0.0f,0.0f);

    glfwSwapBuffers(win);
}
/**
 * --Task--
 * Find the correct coordinates and draw the equilateral triangle
 * In the older OpenGL there is a function glVertex*() to specify vertices.
 * See: https://www.opengl.org/sdk/docs/man2/xhtml/glVertex.xml
 */
void drawTriangle(float x, float y) {
    float r = 0.25;

    float x1 = x;
    float y1 = y + r;

    float angle = 120 * (3.14159 / 180);
    float x2 = (x1 - x) * cos(angle) - (y1 - y) * sin(angle) + x;
    float y2 = (x1 - x) * sin(angle) + (y1 - y) * cos(angle) + y;

    angle = 240 * (3.14159 / 180);
    float x3 = (x1 - x) * cos(angle) - (y1 - y) * sin(angle) + x;
    float y3 = (x1 - x) * sin(angle) + (y1 - y) * cos(angle) + y;

    //Draw triangle here
    glBegin (GL_TRIANGLES);
        glColor3f (1.0, 1.0, 0.0); //set color
        glVertex2f(x1 / aspectRatio, y1);
        glVertex2f(x2 / aspectRatio, y2);
        glVertex2f(x3 / aspectRatio, y3);
    glEnd ();
}
