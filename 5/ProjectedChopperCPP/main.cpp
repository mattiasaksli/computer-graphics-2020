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
void changeSecondaryCameraMode(int mode);

// --- Load the shaders declared in glsl files in the project folder ---//
shader_prog shader("chopper.vert.glsl", "chopper.frag.glsl");

GLuint cubeVAO;
GLuint leftWallVAO, rightWallVAO, backWallVAO, ceilingVAO, floorVAO;
GLuint bodyVAO, bladeVAO;

float fov = 80.0f; //In degrees
float screenWidth = 800;
float screenHeight = 450;

/**
 * This struct holds the camera. Has both the projection and view matrices.
 */
struct Camera {
    glm::mat4 projection;
    glm::mat4 view;
    Camera(glm::mat4 in_projection, glm::mat4 in_view) {
        projection = in_projection;
        view = in_view;
    }
};
Camera* mainCamera;     //Pointer to our main (perspective) camera
Camera* secondaryCamera;//Pointer to our secondary (orthographic) camera


void initChopper() {
    bodyVAO = createCube(glm::vec3(0.4, 0.4, 0.4), glm::vec3(0.9, 0.9, 0.9));
    bladeVAO = createCube(glm::vec3(0.9, 0.2, 0.9), glm::vec3(0.5, 0.0, 0.5));
}


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

void drawChopper() {

    std::stack<glm::mat4> ms;
    ms.push(glm::mat4(1.0)); // World: Push an identity matrix to the bottom of stack

        ms.push(ms.top()); // Chopper
            //ms.top() = glm::translate(ms.top(), glm::vec3(0.0, -10.0, 0.0));

            ms.push(ms.top()); // Body
                //ms.top() = glm::rotate(ms.top(), glm::radians((float)glfwGetTime() * 20), glm::vec3(0.0, 1.0, 0.0));
                ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 1.0, 0.0));
                ms.top() = glm::rotate(ms.top(), glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
                ms.top() = glm::scale(ms.top(), glm::vec3(2.5, 2.0, 7.0));

                shader.uniformMatrix4fv("modelMatrix", ms.top());
                glBindVertexArray(bodyVAO);
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0);
            ms.pop();

            ms.push(ms.top()); // Blades
                ms.top() = glm::rotate(ms.top(), glm::radians((float)glfwGetTime() * 250), glm::vec3(0.0, 1.0, 0.0));
                ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 2.0, 0.0));

                ms.push(ms.top()); // Blade1
                    ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 0.1, 4.0));
                    ms.top() = glm::scale(ms.top(), glm::vec3(0.5, 0.2, 7.0));

                    shader.uniformMatrix4fv("modelMatrix", ms.top());
                    glBindVertexArray(bladeVAO);
                    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0);
                ms.pop();

                ms.push(ms.top()); // Blade2
                    ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 0.1, -4.0));
                    ms.top() = glm::rotate(ms.top(), glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
                    ms.top() = glm::scale(ms.top(), glm::vec3(0.5, 0.2, 7.0));

                    shader.uniformMatrix4fv("modelMatrix", ms.top());
                    glBindVertexArray(bladeVAO);
                    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0);
                ms.pop();
            ms.pop();
        ms.pop();
    ms.pop();
}

void initWalls() {
    leftWallVAO = createWall(glm::vec3(0.66, 0.66, 0.66));
    rightWallVAO = createWall(glm::vec3(0.4, 0.4, 0.4));
    backWallVAO = createWall(glm::vec3(0.52, 0.52, 0.52));
    ceilingVAO = createWall(glm::vec3(0.33, 0.33, 0.33));
    floorVAO = createWall(glm::vec3(0.22, 0.22, 0.22));
}

/**
 * This creates a single colored wall.
 */
GLuint createWall(glm::vec3 color) {
    float s = 10.0;

    GLfloat vertices[] = {
                            -s, -s, 0.0,
                             s, -s, 0.0,
                             s,  s, 0.0,
                            -s,  s, 0.0
                        };
    GLubyte indices[] = {
                            0, 1, 2,
                            0, 2, 3
                        };
    GLfloat colors[] = {
                            color[0], color[1], color[2],
                            color[0], color[1], color[2],
                            color[0], color[1], color[2],
                            color[0], color[1], color[2]
                        };

    GLuint vertexArrayHandle;
    glGenVertexArrays(1, &vertexArrayHandle);
    glBindVertexArray(vertexArrayHandle);

    shader.attribute3fv("position", vertices, 12);
    shader.attribute3fv("color", colors, 12);

    GLuint vboHandle;
    glGenBuffers(1, &vboHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLfloat)*12, indices, GL_STATIC_DRAW);

    return vertexArrayHandle;
}

/**
 * This functions draw the hangar walls
 */
void drawHangar() {
    std::stack<glm::mat4> ms;
    ms.push(glm::mat4(1.0)); //Push an identity matrix to the bottom of stack

    ms.push(ms.top()); //Left wall
        ms.top() = glm::rotate(ms.top(), glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
        ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 0.0, -10.0));
        shader.uniformMatrix4fv("modelMatrix", ms.top());
        glBindVertexArray(leftWallVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
    ms.pop();
    ms.push(ms.top()); //Right wall
        ms.top() = glm::rotate(ms.top(), glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
        ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 0.0, -10.0));
        shader.uniformMatrix4fv("modelMatrix", ms.top());
        glBindVertexArray(rightWallVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
    ms.pop();
    ms.push(ms.top()); //Floor
        ms.top() = glm::rotate(ms.top(), glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
        ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 0.0, -10.0));

        shader.uniformMatrix4fv("modelMatrix", ms.top());
        glBindVertexArray(floorVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
    ms.pop();
    ms.push(ms.top()); //Ceiling
        ms.top() = glm::rotate(ms.top(), glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
        ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 0.0, -10.0));
        shader.uniformMatrix4fv("modelMatrix", ms.top());
        glBindVertexArray(ceilingVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
    ms.pop();
    ms.push(ms.top()); //Back wall
        ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 0.0, -10.0));
        shader.uniformMatrix4fv("modelMatrix", ms.top());
        glBindVertexArray(backWallVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
    ms.pop();
}

/**
 * Draws the entire scene.
 */
void drawScene() {
    drawHangar();
    drawChopper();
}

// ---------------------------- Input -------------------------- //
// This method is called when keyboard event happens.
// Sets GLFW window should close flag to true, when escape key is pressed.
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (action == GLFW_REPEAT || action == GLFW_PRESS) {
        if (key == GLFW_KEY_LEFT) {
            /**
             * --Task--
             * Change the global fov, create a new perspective matrix, assign it to mainCamera->projection
             */
             mainCamera->projection = glm::perspective(glm::radians(++fov), screenWidth / screenHeight, 0.1f, 100.0f);
        }
        if (key == GLFW_KEY_RIGHT) {
            /**
             * --Task--
             * Change the global fov, create a new perspective matrix, assign it to mainCamera->projection
             */
             mainCamera->projection = glm::perspective(glm::radians(--fov), screenWidth / screenHeight, 0.1f, 100.0f);
        }
        printf("FOV: %f \n", fov);
    }

}

/**
 * --Task--
 * Implement this function. It should change the view matrix of the secondary camera.
 * Modes are 1, 2 and 3.
 */
void changeSecondaryCameraMode(int mode) {

    if (1 == mode) {
        // Look from the side
        secondaryCamera->view = glm::lookAt(
            glm::vec3(0.0, 0.0, 15.0), //Position
            glm::vec3(0.0, 0.0, 0.0),  //LookAt
            glm::vec3(0.0, 1.0, 0.0)   //Up
        );
    } else if (2 == mode) {
        //Look from the top
        secondaryCamera->view = glm::lookAt(
            glm::vec3(0.0, 15.0, 0.0), //Position
            glm::vec3(0.0, 0.0, 0.0),  //LookAt
            glm::vec3(0.0, 0.0, -1.0)   //Up
        );
    } else {
        //Look from the left
        secondaryCamera->view = glm::lookAt(
            glm::vec3(-15.0, 0.0, 0.0), //Position
            glm::vec3(0.0, 0.0, 0.0),  //LookAt
            glm::vec3(0.0, 1.0, 0.0)   //Up
        );
    }
}

// ---------------------------- Main -------------------------- //
int main(int argc, char *argv[]) {
    GLFWwindow *win;

    if (!glfwInit()) {
        exit (EXIT_FAILURE);
    }

    win = glfwCreateWindow(screenWidth, screenHeight, "Hello GLFW!", NULL, NULL);

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

    //Here we create the initial main camera
    mainCamera = new Camera(
        glm::perspective(glm::radians(fov), screenWidth / screenHeight, 0.1f, 100.f),
        glm::lookAt(
            glm::vec3(0.0, 7.0, 15.0), //Position
            glm::vec3(0.0, 0.0, 0.0),  //LookAt
            glm::vec3(0.0, 1.0, 0.0)   //Up
        )
    );

    //Here we create the initial secondary camera
    float halfSize = 10.0f; //You may want to change this so that your chopper fits
    secondaryCamera = new Camera(
        glm::ortho(-halfSize, halfSize, -halfSize, halfSize, 0.1f, 100.f),
        glm::lookAt(
            glm::vec3(0.0, 0.0, 15.0), //Position
            glm::vec3(0.0, 0.0, 0.0),  //LookAt
            glm::vec3(0.0, 1.0, 0.0)   //Up
        )
    );

    initChopper();
    initWalls();

    glEnable(GL_CULL_FACE); //Enable back-face culling
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST); //Enable depth testing
    glEnable(GL_SCISSOR_TEST); //Enable scissor testing

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    while (!glfwWindowShouldClose(win)) {

        /**
         * --Task--
         * Rendering flow would be the following:
         * 1) Set the viewport with glViewport
         * 2) Set the scissor area with glScissor
         * 3) Clear the viewport with glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
         * 4) Send the correct projection and view matrices to the shader as uniforms:
         *     - eg: shader.uniformMatrix4fv("projectionMatrix", ...)
         * 5) Call drawScene()
         *
         * Do it for both the main and secondary camera.
         */

        //Currently we are just drawing the main camera. Remove / refactor those lines.
        glViewport(0, 0, screenWidth, screenHeight);
        glScissor(0, 0, screenWidth, screenHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.uniformMatrix4fv("projectionMatrix", mainCamera->projection);
        shader.uniformMatrix4fv("viewMatrix", mainCamera->view);
        drawScene();

        int secondaryCameraWidth = 150;
        int secondaryCameraHeight = 150;
        glViewport(screenWidth - secondaryCameraWidth, 0, secondaryCameraWidth, secondaryCameraHeight);
        glScissor(screenWidth - secondaryCameraWidth, 0, secondaryCameraWidth, secondaryCameraHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.uniformMatrix4fv("projectionMatrix", secondaryCamera->projection);
        shader.uniformMatrix4fv("viewMatrix", secondaryCamera->view);
        drawScene();


        //This calls the changeSecondaryCameraMode with a different parameter every 3 seconds.
        float timeSwitch = fmod(glfwGetTime(), 9.0f);
        if (timeSwitch < 3) {
            changeSecondaryCameraMode(0);
        } else if (timeSwitch < 6) {
            changeSecondaryCameraMode(1);
        } else {
            changeSecondaryCameraMode(2);
        }

        glfwSwapBuffers(win);
        usleep(1000);
        glfwPollEvents();
    }

    delete mainCamera;
    delete secondaryCamera;

    glfwTerminate();
    exit(EXIT_SUCCESS);

    return 0;
}
