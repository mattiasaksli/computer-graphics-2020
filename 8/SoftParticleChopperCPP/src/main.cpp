// ---------------------------- Includes -------------------------- //
#include <stdlib.h>         // C++ standard library
#include <random>
#include <chrono>
#include <stdexcept>
#include <stack>            // We use the standard C++ stack implementation to create model matrix stacks
#include <vector>           // We use the standard C++ vector to store values in
#include <algorithm>        // Sorting is in there
#include <unistd.h>         // Threading
#include <stdio.h>          // Input/Output
#include <GLEW/glew.h>      // OpenGL Extension Wrangler -
#include <GLFW/glfw3.h>     // Windows and input
#include <glm/glm.hpp>      // OpenGL math library

#include "shader_util.h"
#include "texture_util.h"
#include "geometry.h"


float getRandom();

//For the particles, we need a texture, VAO and a vector (array) of positions
GLuint particleTextureHandle;
GLuint particleVAO;
std::vector<glm::vec3> particlePositions;

//We render our scene's depth into a texture (render target)
GLuint depthRenderTarget, depthFramebuffer;

//These will hold our hangar
GLuint leftWallVAO, rightWallVAO, backWallVAO, ceilingVAO, floorVAO;

GLuint bodyVAO, bladeVAO;
int chopperBodyTriangleCount;

// --- Load the shaders declared in glsl files in the project folder ---//
shader_prog defaultShader("shaders/default.vert.glsl", "shaders/default.frag.glsl");
shader_prog particleShader("shaders/particle.vert.glsl", "shaders/particle.frag.glsl");
shader_prog depthShader("shaders/depth.vert.glsl", "shaders/depth.frag.glsl");

float screenWidth = 800;
float screenHeight = 450;

/**
 * This function creates the hangar by assigning the correct VAO-s.
 * Different walls have different colors.
 */
void initHangar() {
    printf("Init walls\n");
    defaultShader.activate();
    leftWallVAO = createQuad(glm::vec3(0.66, 0.66, 0.66), &defaultShader);
    rightWallVAO = createQuad(glm::vec3(0.4, 0.4, 0.4), &defaultShader);
    backWallVAO = createQuad(glm::vec3(0.52, 0.52, 0.52), &defaultShader);
    ceilingVAO = createQuad(glm::vec3(0.33, 0.33, 0.33), &defaultShader);
    floorVAO = createQuad(glm::vec3(0.22, 0.22, 0.22), &defaultShader);
}

/**
 * Copy your chopper initialization from the previous tasks.
 */
void initChopper() {
    bodyVAO = createSphere(1.0f, 38, 36, glm::vec3(1.0, 0.0, 0.0), chopperBodyTriangleCount, &defaultShader);
    bladeVAO = createCube(glm::vec3(0.9, 0.2, 0.9), &defaultShader);
}

/**
 * This functions should initialize everything related to our particles.
 *
 * --Task--
 * Finish this function. Currently it generates one particle. We should have many.
 */
void initParticles() {
    particleShader.activate(); //Particles will use the particle shader
    particleTextureHandle = load_texture(GL_TEXTURE_2D, GL_RGBA, "data/smoke.png"); //Loading the correct texture, RGBA this time.
    particleShader.uniformTex2D("texture", particleTextureHandle);
    particleVAO = createTexturedQuad(&particleShader); //Particle consists of a quad.

    //We store the positions in this vector
    particlePositions = std::vector<glm::vec3>();

    /**
     * --Task--
     * Currently we add one position. Change this to add around 20 particles around the hangar.
     * Push all the values into the particlePositions vector
     * You can do a C++11 random here (with an uniform real distribution). Find out, how to do it.
     * NB! Don't try to prevent intersecting with hangar walls. Removing this visual artifact is part of the task.
     */

    for (int i = 0; i < 23; i++) {
        particlePositions.push_back(glm::vec3(getRandom(), -7.0f, getRandom()));
    }

    //This will sort our vector from back to front.
    std::sort(particlePositions.begin(), particlePositions.end(), [](const glm::vec3 &u, const glm::vec3 &v){ return u.z < v.z; });

    //Debug will show you z value of each particle.
    //Particles with the same depth can clip each other... :(
    for (int i = 0; i < particlePositions.size(); i++) {
        printf("%f\n", particlePositions.at(i).z);
    }
}

float getRandom()
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    static std::default_random_engine gen(seed);
    std::uniform_real_distribution<float> distr(-9, 9);
    return distr(gen);
}

/**
 * This function will create us another framebuffer and a texture to render the hangar depth info into.
 * Originates from here: http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
 */
void initDepthRenderTarget() {
    glGenFramebuffers(1, &depthFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, depthFramebuffer);

    GLuint renderTexture;
    glGenTextures(1, &depthRenderTarget);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, depthRenderTarget);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, depthRenderTarget, 0);

    // Set the list of draw buffers.
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

    // Always check that our framebuffer is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("Something went wrong, when generating the depth render target!");
    }
}

/**
 * Drawing the hangar (each wall).
 */
void drawHangar(shader_prog* shader) {
    shader->activate();

    std::stack<glm::mat4> ms;
    ms.push(glm::mat4(1.0)); //Push an identity matrix to the bottom of stack

    ms.push(ms.top());
        ms.top() = glm::translate(ms.top(), glm::vec3(-10.0, 0.0, 0.0));
        ms.top() = glm::rotate(ms.top(), glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
        shader->uniformMatrix4fv("modelMatrix", ms.top());
        glBindVertexArray(leftWallVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    ms.pop();
    ms.push(ms.top());
        ms.top() = glm::translate(ms.top(), glm::vec3(10.0, 0.0, 0.0));
        ms.top() = glm::rotate(ms.top(), glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
        shader->uniformMatrix4fv("modelMatrix", ms.top());
        glBindVertexArray(leftWallVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    ms.pop();
    ms.push(ms.top());
        ms.top() = glm::translate(ms.top(), glm::vec3(0.0, -10.0, 0.0));
        ms.top() = glm::rotate(ms.top(), glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
        shader->uniformMatrix4fv("modelMatrix", ms.top());
        glBindVertexArray(floorVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    ms.pop();
    ms.push(ms.top());
        ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 10.0, 0.0));
        ms.top() = glm::rotate(ms.top(), glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
        shader->uniformMatrix4fv("modelMatrix", ms.top());
        glBindVertexArray(ceilingVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    ms.pop();
    ms.push(ms.top());
        ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 0.0, -10.0));
        shader->uniformMatrix4fv("modelMatrix", ms.top());
        glBindVertexArray(backWallVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    ms.pop();
}

/**
 * --Task--
 * You can copy your chopper drawing here.
 * Not necessary if you don't want to, but it can look cool.
 */
void drawChopper(shader_prog* shader) {
    shader->activate();
    //Draw chopper
    std::stack<glm::mat4> ms;
    ms.push(glm::mat4(1.0)); // World: Push an identity matrix to the bottom of stack

        ms.push(ms.top()); // Chopper
            ms.top() = glm::translate(ms.top(), glm::vec3(0.0, glm::sin((float)glfwGetTime() * 2.0) - 3.0, 0.0));
            ms.top() = glm::scale(ms.top(), glm::vec3(0.5, 0.5, 0.5));

            ms.push(ms.top()); // Body
                ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 1.0, 0.0));
                ms.top() = glm::rotate(ms.top(), glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
                ms.top() = glm::scale(ms.top(), glm::vec3(2.5, 2.0, 7.0));

                shader->uniformMatrix4fv("modelMatrix", ms.top());
                glBindVertexArray(bodyVAO);
                glDrawElements(GL_TRIANGLES, chopperBodyTriangleCount * 3, GL_UNSIGNED_SHORT, 0);
            ms.pop();

            ms.push(ms.top()); // Blades
                ms.top() = glm::rotate(ms.top(), glm::radians((float)glfwGetTime() * 250), glm::vec3(0.0, 1.0, 0.0));
                ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 4.0, 0.0));

                ms.push(ms.top()); // Blade1
                    ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 0.1, 6.0));
                    ms.top() = glm::scale(ms.top(), glm::vec3(0.5, 0.2, 5.0));

                    shader->uniformMatrix4fv("modelMatrix", ms.top());
                    glBindVertexArray(bladeVAO);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                ms.pop();

                ms.push(ms.top()); // Blade2
                    ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 0.1, -6.0));
                    ms.top() = glm::rotate(ms.top(), glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
                    ms.top() = glm::scale(ms.top(), glm::vec3(0.5, 0.2, 5.0));

                    shader->uniformMatrix4fv("modelMatrix", ms.top());
                    glBindVertexArray(bladeVAO);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                ms.pop();
            ms.pop();
        ms.pop();
    ms.pop();
}


/**
 * --Task--
 * Here we want to draw the particles. Finish this functions.
 */
void drawParticles(shader_prog* shader) {
    std::stack<glm::mat4> ms;
    shader->activate();

    /**
     * 1. Iterate over the vector of particle positions, for each position:
     * 2. Rotate different particles (multiply with the corresponding glm::rotate())
     * 2.1 For all the odd particles, rotate them counter-clockwise
     * 2.2 For all the even particles, rotate them clockwise
     * 3. Send the model matrix
     * 4. Call glBindVertexArray to fetch the geometry from the corresponding VAO
     * 5. Call the correct draw call (glDrawArrays).
     */
    for (int i = 0; i < particlePositions.size(); i++) {
        ms.push(glm::mat4(1.0));
            ms.top() = glm::translate(ms.top(), particlePositions.at(i));
            if (i % 2 == 0) {
                ms.top() = glm::rotate(ms.top(), glm::radians((float)glfwGetTime() * -15), glm::vec3(0.0, 0.0, 1.0));
            }
            else {
                ms.top() = glm::rotate(ms.top(), glm::radians((float)glfwGetTime() * 15), glm::vec3(0.0, 0.0, 1.0));
            }
            shader->uniformMatrix4fv("modelMatrix", ms.top());
            glBindVertexArray(particleVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        ms.pop();
    }
}

/**
 * Draws the scene.
 * Hangar with the default shader, particles with the particle shader.
 */
void drawScene() {
    drawHangar(&defaultShader);
    drawChopper(&defaultShader);
    drawParticles(&particleShader);
}


// ---------------------------- Main -------------------------- //
int main(int argc, char *argv[]) {
    GLFWwindow *win;
    if (!glfwInit()) {
        exit (EXIT_FAILURE);
    }

    win = glfwCreateWindow(screenWidth, screenHeight, "Soft Particle Chopper", NULL, NULL);
    glfwMakeContextCurrent(win);

    glewExperimental = GL_TRUE;
    glewInit();

    if (!win) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(win);
    glewExperimental = GL_TRUE;

    GLenum status = glewInit();
    if(status != GLEW_OK) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(status));
    }

    const GLubyte* renderer = glGetString (GL_RENDERER);
    const GLubyte* version = glGetString (GL_VERSION);
    printf ("Renderer: %s\n", renderer);
    printf ("OpenGL version supported %s\n", version);


    //We compile all 3 of our shades
    defaultShader.use();
    particleShader.use();
    depthShader.use();

    //We initialize our stuff
    initHangar();
    initChopper();
    initParticles();

    //Also the render target texture
    initDepthRenderTarget();

    float near = 1.0f;
    float far = 30.0f;

    glm::vec3 viewer = glm::vec3(0.0, 0.0, 15.0);
    glm::mat4 perspective = glm::perspective(glm::radians(80.0f), screenWidth / screenHeight, near, far);
    glm::mat4 view = glm::lookAt(
        viewer, //Position
        glm::vec3(0.0, 0.0, 0.0),  //LookAt
        glm::vec3(0.0, 1.0, 0.0)   //Up
    );

    //Send the view and projection matrices to all 3 shaders.
    defaultShader.activate();
    defaultShader.uniformMatrix4fv("projectionMatrix", perspective);
    defaultShader.uniformMatrix4fv("viewMatrix", view);

    particleShader.activate();
    particleShader.uniformMatrix4fv("projectionMatrix", perspective);
    particleShader.uniformMatrix4fv("viewMatrix", view);
    particleShader.uniformVec2("frustum", glm::vec2(near, far));
    particleShader.uniformVec2("screenSize", glm::vec2(screenWidth, screenHeight));

    depthShader.activate();
    depthShader.uniformMatrix4fv("projectionMatrix", perspective);
    depthShader.uniformMatrix4fv("viewMatrix", view);
    depthShader.uniformVec2("frustum", glm::vec2(near, far));

    glm::vec3 lightPosition;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    /**
     * --Task--
     * Enable the conventional alpha-blending!
     * Look up functions glEnable and glBlendFunc
     */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // -------------- Create objects ------------- //
    printf("Starting rendering loop...");
    while (!glfwWindowShouldClose(win)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightPosition.x = 1.8f * sin(glfwGetTime()); //Move our light on a trajectory
        lightPosition.y = 1.5f * cos(glfwGetTime());
        lightPosition.z = 1.1f * sin(glfwGetTime()) + 0.3;
        defaultShader.activate();
        defaultShader.uniformVec3("lightPosition", glm::vec3(view * glm::vec4(lightPosition, 1.0)));

        //Set rendering to a texture
        glBindFramebuffer(GL_FRAMEBUFFER, depthFramebuffer);

        //Draw the hangar with the depthShader
        drawHangar(&depthShader);

        //Reset to normal rendering
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //Activate our previous render target
        particleShader.activate();

        //Get the location for the "depthTexture" variable, bind new data to that location
        glUniform1i(glGetUniformLocation(particleShader.getProg(), "depthTexture"), 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthRenderTarget);

        //Draw the scene normally
        drawScene();

        glfwSwapBuffers(win);

        usleep(100);
        glfwPollEvents();
    }

    glfwTerminate();
    exit(EXIT_SUCCESS);
    return 0;
}
