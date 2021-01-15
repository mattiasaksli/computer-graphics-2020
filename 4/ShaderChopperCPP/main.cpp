// ---------------------------- Includes -------------------------- //
#include <stdlib.h>         // C++ standard library
#include <stack>            // We use the standard C++ stack implementation to create model matrix stacks
#include <unistd.h>         // Threading
#include <stdio.h>          // Input/Output
#include <GLEW/glew.h>      // OpenGL Extension Wrangler -
#include <GLFW/glfw3.h>     // Windows and input
#include <glm/glm.hpp>      // OpenGL math library
#include "shader_util.h"    // Utility methods to keep this file a bit shorter.

// --------------- Forward declarations ------------- //
GLuint createWall(glm::vec3 color);
GLuint createCube(glm::vec3 color1, glm::vec3 color2);


// --- Load the shaders declared in glsl files in the project folder ---//
shader_prog shader("chopper.vert.glsl", "chopper.frag.glsl");

GLuint cubeVAO;
GLuint leftWallVAO, rightWallVAO, backWallVAO, ceilingVAO, floorVAO, frontWallVAO;
GLuint bodyVAO, bladeVAO;

float hangarSize = 25.0;

// Global parameters for convenience.
float deltaTime;
float speedDecay = 7.0;
float speedIncrease = 12.0;
float maxSpeed = 10.0;

// Chopper transform info
glm::vec3 chopperPos = glm::vec3(0.0, -hangarSize, 0.0);
glm::vec3 chopperSpeed = glm::vec3(0.0, 0.0, 0.0);
float horizontalAngle = 0;

// Collision bounds based on hangar size and chopper size.
glm::vec3 posBounds = glm::vec3(hangarSize - 8.0, hangarSize - 2.25, hangarSize - 8.0);
glm::vec3 negBounds = glm::vec3(-hangarSize + 8.0, -hangarSize, -hangarSize + 8.0);

/**
*   *Implement the methods: initChopper, createCube and drawChopper. Read the
*   comments for guidelines and explanation. There exist very similar methods
*   for the hangar walls for you to follow.
*/


void initWalls() {
    leftWallVAO = createWall(glm::vec3(0.66, 0.66, 0.66));
    rightWallVAO = createWall(glm::vec3(0.4, 0.4, 0.4));
    backWallVAO = createWall(glm::vec3(0.52, 0.52, 0.52));
    ceilingVAO = createWall(glm::vec3(0.33, 0.33, 0.33));
    floorVAO = createWall(glm::vec3(0.22, 0.22, 0.22));
    frontWallVAO = createWall(glm::vec3(0.78, 0.78, 0.78));
}

/**
 * Use this method to initialize the 3 components of your chopper
*/
void initChopper() {
    bodyVAO = createCube(glm::vec3(0.4, 0.4, 0.4), glm::vec3(0.9, 0.9, 0.9));
    bladeVAO = createCube(glm::vec3(0.9, 0.2, 0.9), glm::vec3(0.5, 0.0, 0.5));
}

/**
* In the first practice session we used immediate mode(glBegin, followed by vertices)
* to draw a triangle. Modern versions of OpenGL no longer support it,as it happens to be
* very very (very) slow.
*
* In immediate mode your program will make one call to the GPU card for every
* vertex you want to draw. This will add up very quickly. As an example using triangles
* drawing mode to draw a single cube, would already take 36 calls.
*
* This time we will use Vertex Array Objects and Vertex Buffer Objects. These let us load
* all the vertices the into video memory and draw the objects with a single call every frame.
* Modern game engines have even further optimized this, by drawing all the objects in view, which
* use the same shader, in a single call to the graphics card.
*
* This method demonstrates how to draw a simple object consisting of three different sets
* of values: vertex coordinates, vertex colors and triangle face indexes. The first two are
* hidden away into utility method defined in shader_util.cpp.
*/
GLuint createWall(glm::vec3 color) {
    float s = hangarSize;
    /**
    * We start by creating arrays of the data, which we will later send to OpenGL.
    */

    //In this array we define coordinates for the corners of the walls.
    GLfloat vertices[] = {
                            -s, -s, 0.0,
                             s, -s, 0.0,
                             s,  s, 0.0,
                            -s,  s, 0.0
                        };

    //In this array we define faces for the triangles within the walls.
    //Each set of three vertices defines one triangle.
    GLubyte indices[] = {
                            0, 1, 2,
                            0, 2, 3
                        };

    //Colors for the 4 vertices
    GLfloat colors[] = {
                            color[0], color[1], color[2],
                            color[0], color[1], color[2],
                            color[0], color[1], color[2],
                            color[0], color[1], color[2]
                        };

    GLuint vertexArrayHandle;
    glGenVertexArrays(1, &vertexArrayHandle);
    glBindVertexArray(vertexArrayHandle);

    //Here we set up VBO-s for coordinates and colors of the vertices.
    shader.attribute3fv("position", vertices, 12);
    shader.attribute3fv("color", colors, 12);

    /**
    * To use VBO, you need to perform the following steps:
    *   1. Generate a name for the buffer.
    *   2. Bind (activate) the buffer.
    *   3. Store data in the buffer.
    *   4. Use the buffer to render data.
    *   5. Destroy the buffer.
    */

    // First step. We create a handle for our buffer
    GLuint vboHandle;
    glGenBuffers(1, &vboHandle);
    // Second step. We bind (activate) this buffer for the OpenGL state machine.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboHandle);
    // Third step. We store the vertex indexes in the VBO.
    // These define the faces of the triangles, which make up the cube/
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLfloat)*12, indices, GL_STATIC_DRAW);
    //We return handle to vertex array. Step 4 takes place in method drawHangar.
    //Step 5 we skip, as all our assets have the same lifetime as rest of the program.
    return vertexArrayHandle;
}

/**
*   Study the createWall method and create similar method for a cube. The cube will be used
*   for all 3 parts of the chopper. This means it should be a unit cube, which you can scale
*   in the drawing method.
*/
GLuint createCube(glm::vec3 color1, glm::vec3 color2) {

    GLfloat vertices[] = {
                            -0.5, -0.5, 0.5,
                            0.5, -0.5, 0.5,
                            0.5, 0.5, 0.5,
                            -0.5, 0.5, 0.5,
                            -0.5, -0.5, -0.5,
                            0.5, -0.5, -0.5,
                            0.5, 0.5, -0.5,
                            -0.5, 0.5, -0.5
                        };

    GLubyte indices[] = {
                            0, 1, 2,
                            0, 2, 3,
                            1, 5, 6,
                            1, 6, 2,
                            3, 2, 6,
                            3, 6, 7,

                            0, 4, 1,
                            1, 4, 5,
                            4, 6, 5,
                            4, 7, 6,
                            0, 3, 4,
                            4, 3, 7
                        };

    GLfloat colors[] = {
                            color1[0], color1[1], color1[2],
                            color1[0], color1[1], color1[2],
                            color1[0], color1[1], color1[2],
                            color1[0], color1[1], color1[2],

                            color2[0], color2[1], color2[2],
                            color2[0], color2[1], color2[2],
                            color2[0], color2[1], color2[2],
                            color2[0], color2[1], color2[2]
                        };

    GLuint vertexArrayHandle;
    glGenVertexArrays(1, &vertexArrayHandle);
    glBindVertexArray(vertexArrayHandle);

    //Here we set up VBO-s for coordinates and colors of the vertices.
    shader.attribute3fv("position", vertices, 36);
    shader.attribute3fv("color", colors, 36);

    /**
    * To use VBO, you need to perform the following steps:
    *   1. Generate a name for the buffer.
    *   2. Bind (activate) the buffer.
    *   3. Store data in the buffer.
    *   4. Use the buffer to render data.
    *   5. Destroy the buffer.
    */

    // First step. We create a handle for our buffer
    GLuint vboHandle;
    glGenBuffers(1, &vboHandle);
    // Second step. We bind (activate) this buffer for the OpenGL state machine.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboHandle);
    // Third step. We store the vertex indexes in the VBO.
    // These define the faces of the triangles, which make up the cube/
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLfloat) * 36, indices, GL_STATIC_DRAW);
    //We return handle to vertex array. Step 4 takes place in method drawChopper.
    //Step 5 we skip, as all our assets have the same lifetime as rest of the program.
    return vertexArrayHandle;

}

/**
*   Geometry for all the walls is centered at coordinate (0,0,0). In the drawing method
*   we will use model matrix to rotate, scale and transpose the the walls. Since we want
*   to be able to undo some rotations after drawing an object, we will keep the matrixes
*   in a stack. By pushing matrix to the stack we create a save point, which we can return
*   to by doing a pop() on the stack.
*
*   This lets us create parent - child relationship between two objects with following steps:
*       Modify the model matrix to scale, rotation and position we want it to be.
*       Draw the parent
*       push() a matrix onto the stack
*           Modify child scale, rotation and position.
*           Draw the child
*       pop() to get back to parents transformation
*       push() a matrix onto the stack
*           Modify another child scale, rotation and position.
*           Draw the child
*       pop()
*       etc..
*/
void drawHangar(glm::mat4 view, glm::mat4 projection) {
    std::stack<glm::mat4> ms;
    ms.push(glm::mat4(1.0)); //Push an identity matrix to the bottom of stack

    ms.push(ms.top());
        ms.top() = projection * view * ms.top();
        ms.top() = glm::rotate(ms.top(), glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
        ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 0.0, -hangarSize));
        // Send the current model matrix at the top of stack to the vertex shader.
        shader.uniformMatrix4fv("clipMatrix", ms.top());
        // Bind a vertex array to the current OpenGL context
        glBindVertexArray(leftWallVAO);
        // Draw elements from the current bound vertex shader to the screen buffer.
        // The drawing method will use the vertex shader for rendering, which has the
        // model matrix from top of our matrix stack currently bound to it. This will
        // result in objects from currently bound vertex array to be drawn with the
        // current scale, rotation and translation.
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
        // Pop back to last model matrix. This doesn't have the scaling, rotation and translation,
        // which were added after the last push.
    ms.pop();
    ms.push(ms.top()); //Right wall
        ms.top() = projection * view * ms.top();
        ms.top() = glm::rotate(ms.top(), glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
        ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 0.0, -hangarSize));

        shader.uniformMatrix4fv("clipMatrix", ms.top());
        glBindVertexArray(rightWallVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
    ms.pop();
    ms.push(ms.top()); //Floor
        ms.top() = projection * view * ms.top();
        ms.top() = glm::rotate(ms.top(), glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
        ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 0.0, -hangarSize));

        shader.uniformMatrix4fv("clipMatrix", ms.top());
        glBindVertexArray(floorVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
    ms.pop();
    ms.push(ms.top()); //Ceiling
        ms.top() = projection * view * ms.top();
        ms.top() = glm::rotate(ms.top(), glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
        ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 0.0, -hangarSize));

        shader.uniformMatrix4fv("clipMatrix", ms.top());
        glBindVertexArray(ceilingVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
    ms.pop();
    ms.push(ms.top()); //Back wall
        ms.top() = projection * view * ms.top();
        ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 0.0, -hangarSize));

        shader.uniformMatrix4fv("clipMatrix", ms.top());
        glBindVertexArray(backWallVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
    ms.pop();
    ms.push(ms.top()); //Front wall
        ms.top() = projection * view * ms.top();
        ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 0.0, hangarSize));
        ms.top() = glm::rotate(ms.top(), glm::radians(180.0f), glm::vec3(1.0, 0.0, 0.0));

        shader.uniformMatrix4fv("clipMatrix", ms.top());
        glBindVertexArray(frontWallVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
    ms.pop();

    /**
    *   In this example we always sent model matrix to the vertex shader.
    *   For better performance you can send model*view*projection instead.
    *   This way you will make one multiplication on CPU, instead of one for
    *   every vertex on GPU.
    */
}

void drawChopper(glm::mat4 view, glm::mat4 projection, float bladesAngle) {

    /**
    *   Hint: To get time, use glfwGetTime() function. This will return
    *   double precision seconds since GLFW was initiated.
    */

    std::stack<glm::mat4> ms;
    ms.push(glm::mat4(1.0)); // World: Push an identity matrix to the bottom of stack

        ms.push(ms.top()); // Chopper
            ms.top() = glm::translate(ms.top(), chopperPos);
            // Rotates the chopper to face the same direction as the camera.
            ms.top() = glm::rotate(ms.top(), -horizontalAngle, glm::vec3(0.0, 1.0, 0.0));

            ms.push(ms.top()); // Body
                ms.top() = projection * view * ms.top();
                ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 1.0, 0.0));
                ms.top() = glm::scale(ms.top(), glm::vec3(2.5, 2.0, 7.0));

                shader.uniformMatrix4fv("clipMatrix", ms.top());
                glBindVertexArray(bodyVAO);
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0);
            ms.pop();

            ms.push(ms.top()); // Blades
                ms.top() = glm::rotate(ms.top(), glm::radians(bladesAngle), glm::vec3(0.0, 1.0, 0.0));
                ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 2.0, 0.0));

                ms.push(ms.top()); // Blade1
                    ms.top() = projection * view * ms.top();
                    ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 0.1, 4.0));
                    ms.top() = glm::scale(ms.top(), glm::vec3(0.5, 0.2, 7.0));

                    shader.uniformMatrix4fv("clipMatrix", ms.top());
                    glBindVertexArray(bladeVAO);
                    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0);
                ms.pop();

                ms.push(ms.top()); // Blade2
                    ms.top() = projection * view * ms.top();
                    ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 0.1, -4.0));
                    ms.top() = glm::scale(ms.top(), glm::vec3(0.5, 0.2, 7.0));
                    ms.top() = glm::rotate(ms.top(), glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));

                    shader.uniformMatrix4fv("clipMatrix", ms.top());
                    glBindVertexArray(bladeVAO);
                    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0);
                ms.pop();
            ms.pop();
        ms.pop();
    ms.pop();
}

// This method updates the choppers current speed values.
// User input adds a force in a given direction, which is counteracted by the speedDecay force.
// Returns the chopper's speed vector, rotated by horizontalAngle.
glm::vec3 updateChopperSpeed(GLFWwindow *win) {
    // Get x-axis input.
    if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS) {
        if (chopperSpeed.x >= -maxSpeed)
            chopperSpeed.x -= speedIncrease * deltaTime;
        else
            chopperSpeed.x = -maxSpeed;
    }
    else if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS) {
        if (chopperSpeed.x <= maxSpeed)
            chopperSpeed.x += speedIncrease * deltaTime;
        else
            chopperSpeed.x = maxSpeed;
    }

    // Get y-axis input.
    if (glfwGetKey(win, GLFW_KEY_SPACE) == GLFW_PRESS) {
        if (chopperSpeed.y <= maxSpeed)
            chopperSpeed.y += speedIncrease * deltaTime;
        else
            chopperSpeed.y = maxSpeed;
    }
    else if (glfwGetKey(win, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        if (chopperSpeed.y >= -maxSpeed)
            chopperSpeed.y -= speedIncrease * deltaTime;
        else
            chopperSpeed.y = -maxSpeed;
    }

    // Get z-axis input.
    if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS) {
        if (chopperSpeed.z >= -maxSpeed)
            chopperSpeed.z -= speedIncrease * deltaTime;
        else
            chopperSpeed.z = -maxSpeed;
    }
    else if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS) {
        if (chopperSpeed.z <= maxSpeed)
            chopperSpeed.z += speedIncrease * deltaTime;
        else
            chopperSpeed.z = maxSpeed;
    }

    float speedDeltaError = 0.1;

    //Gradually decrease chopper x-axis speed.
    if (chopperSpeed.x > speedDeltaError) {
        chopperSpeed.x -= speedDecay * deltaTime;
    }
    else if (chopperSpeed.x < -speedDeltaError) {
        chopperSpeed.x += speedDecay * deltaTime;
    }
    else {
        chopperSpeed.x = 0.0f;
    }

    //Gradually decrease chopper y-axis speed.
    if (chopperSpeed.y > speedDeltaError) {
        chopperSpeed.y -= speedDecay * deltaTime;
    }
    else if (chopperSpeed.y < -speedDeltaError) {
        chopperSpeed.y += speedDecay * deltaTime;
    }
    else {
        chopperSpeed.y = 0.0f;
    }

    //Gradually decrease chopper z-axis speed.
    if (chopperSpeed.z > speedDeltaError) {
        chopperSpeed.z -= speedDecay * deltaTime;
    }
    else if (chopperSpeed.z < -speedDeltaError) {
        chopperSpeed.z += speedDecay * deltaTime;
    }
    else {
        chopperSpeed.z = 0.0f;
    }

    // Rotates the speed vector by the horizontalAngle amount around the y-axis.
    glm::mat3 rotationMatrix =
        glm::mat3(
            cos(horizontalAngle),  0, sin(horizontalAngle),
            0,                     1,                    0,
            -sin(horizontalAngle), 0, cos(horizontalAngle)
        );
    return rotationMatrix * chopperSpeed;
}

void updateChopperPos(glm::vec3 rotatedChopperSpeed) {
    // Updates chopper position according to chopper speed.
    chopperPos.x += rotatedChopperSpeed.x * deltaTime;
    chopperPos.y += rotatedChopperSpeed.y * deltaTime;
    chopperPos.z += rotatedChopperSpeed.z * deltaTime;
}

void checkBounds() {
    if (chopperPos.x > posBounds.x) {
        chopperPos.x = posBounds.x;
        chopperSpeed.x = 0;
    }
    else if (chopperPos.x < negBounds.x) {
        chopperPos.x = negBounds.x;
        chopperSpeed.x = 0;
    }
    if (chopperPos.y > posBounds.y) {
        chopperPos.y = posBounds.y;
        chopperSpeed.x = 0;
    }
    else if (chopperPos.y < negBounds.y) {
        chopperPos.y = negBounds.y;
        chopperSpeed.x = 0;
    }
    if (chopperPos.z > posBounds.z) {
        chopperPos.z = posBounds.z;
        chopperSpeed.x = 0;
    }
    else if (chopperPos.z < negBounds.z) {
        chopperPos.z = negBounds.z;
        chopperSpeed.x = 0;
    }
}

// ---------------------------- Input -------------------------- //
// This method is called when keyboard event happens.
// Sets GLFW window should close flag to true, when escape key is pressed.
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

// ---------------------------- Main -------------------------- //
int main(int argc, char *argv[]) {
    GLFWwindow *win;

    if (!glfwInit()) {
        exit (EXIT_FAILURE);
    }

    win = glfwCreateWindow(640, 480, "Cube Chopper!", NULL, NULL);

    if (!win) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(win);

    //Some extensions are marked as experimental. To use the latest
    //version of OpenGL supported, we have to set this flag to true.
    glewExperimental = GL_TRUE;
    GLenum status = glewInit();

    if(status != GLEW_OK) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(status));
    }

    const GLubyte* renderer = glGetString (GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString (GL_VERSION); // version as a string
    printf ("Renderer: %s\n", renderer);
    printf ("OpenGL version supported %s\n", version);

    glfwSetKeyCallback(win, key_callback);

    shader.use();

    //This is the "Camera". glm::perspective will return a projection matrix for perspective effect.
    glm::mat4 projection = glm::perspective(glm::radians(80.0f), 4.0f / 3.0f, 0.1f, 100.f);
    /**
    *   View matrix is the inverse matrix of a model matrix with cameras position.
    *   This matrix will transpose the coordinate space so, that the location
    *   specified in position vector would be moved into (0,0,0) of the coordinate space,
    *   rotated so that LookAt would be in front of the (0,0,0) and Up would be the new y axis.
    */

    glm::vec3 cameraPos = glm::vec3(chopperPos.x, chopperPos.y + 10.0, chopperPos.z + 10.0);
    glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);
    glm::mat4 view = glm::lookAt(
        cameraPos,  //Position
        chopperPos, //LookAt
        cameraUp    //Up
    );
    //shader.uniformMatrix4fv("projectionMatrix", projection);
    //shader.uniformMatrix4fv("viewMatrix", view);

    initChopper();
    initWalls();

    //Turn on depth test, so the objects closer to the camera would
    //be drawn in front of the objects further away.
    glEnable(GL_DEPTH_TEST);
    //Enable back-face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    //Clear our background to black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Disables the cursor.
    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // Sets the mouse input mode to raw input, if it is suppported.
    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(win, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    // Sets the cursor's position to the center of the screen.
    glfwSetCursorPos(win, 640/2, 480/2);

    double lastFrameTime = 0;
    float bladesAngle = 0;

    double mouseX;
    double mouseY;

    while (!glfwWindowShouldClose(win)) {
        // Calculates the time elapsed between this frame and the last frame in milliseconds.
        double currentFrameTime = glfwGetTime();
        deltaTime = float(currentFrameTime - lastFrameTime);
        lastFrameTime = currentFrameTime;

        // Updates the angle used for the rotation of the blades, based on the chopper's current speed.
        bladesAngle += 500.0f * (glm::abs(glm::length(chopperSpeed)) * 0.25f + 1.0f) * deltaTime;
        if ((int)bladesAngle / 360 != 0)
            // Keeps the angle in the range of [0, 360].
            bladesAngle = std::fmod(bladesAngle, 360.0f);

        // Gets the mouse's new position.
        glfwGetCursorPos(win, &mouseX, &mouseY);

        // Calculates the angles based on how much the mouse has moved.
        horizontalAngle -= 0.5f * deltaTime * float(640/2 - mouseX);
        if ((int)glm::degrees(horizontalAngle) / 360 != 0)
            // Keeps the angle in the range of [0, 360].
            horizontalAngle = glm::radians(std::fmod(glm::degrees(horizontalAngle), 360.0f));

        // Update the camera's position.
        cameraPos = glm::vec3(chopperPos.x, chopperPos.y + 10.0, chopperPos.z + 10.0);
        // Rotates the camera around the y-axis.
        float rotatedZ = ((cameraPos.z - chopperPos.z) * cos(horizontalAngle)) - ((chopperPos.x - cameraPos.x) * sin(horizontalAngle)) + chopperPos.z;
        float rotatedX = ((chopperPos.x - cameraPos.x) * cos(horizontalAngle)) - ((cameraPos.z - chopperPos.z) * sin(horizontalAngle)) + chopperPos.x;
        cameraPos = glm::vec3(rotatedX, cameraPos.y, rotatedZ);
        view = glm::lookAt(
            cameraPos,  //Position
            chopperPos, //LookAt
            cameraUp    //Up
        );

        // Resets the mouse's position to the center of the screen.
        glfwSetCursorPos(win, 640/2, 480/2);

        glm::vec3 rotatedChopperSpeed = updateChopperSpeed(win);
        updateChopperPos(rotatedChopperSpeed);
        checkBounds();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawHangar(view, projection);
        drawChopper(view, projection, bladesAngle);
        glfwSwapBuffers(win);
        glfwPollEvents();
        usleep(1000);
    }

    glfwTerminate();
    exit(EXIT_SUCCESS);

    return 0;
}
