// ---------------------------- Includes -------------------------- //
#include <stdlib.h>         // C++ standard library
#include <stdexcept>
#include <stack>            // We use the standard C++ stack implementation to create model matrix stacks
#include <vector>
#include <unistd.h>         // Threading
#include <stdio.h>          // Input/Output
#include <GLEW/glew.h>      // OpenGL Extension Wrangler -
#include <GLFW/glfw3.h>     // Windows and input
#include <glm/glm.hpp>      // OpenGL math library

#include "shader_util.h"
#include "texture_util.h"

// --------------- Forward declarations ------------- //
GLuint createQuad(float width);

int textureIndex = 0; //We put texture handles in a vector and specify the current index
std::vector<GLuint> textureHandles = std::vector<GLuint>();

shader_prog shader("shaders/texture.vert.glsl", "shaders/texture.frag.glsl");
shader_prog defaultShader("shaders/default.vert.glsl", "shaders/default.frag.glsl");

GLuint quadVAO;
GLuint lightVAO;

glm::vec3 lightPosition = glm::vec3(1.0f);
glm::vec3 lightPositionCam = glm::vec3(1.0f);

/**
 * Creates a quad
 */
GLuint createQuad(float width, shader_prog* shader) {
    GLuint vertexArrayHandle;
    GLuint arrayBufferHandle;

    glGenVertexArrays(1, &vertexArrayHandle);
    glBindVertexArray(vertexArrayHandle);
    glGenBuffers(1, &arrayBufferHandle);

    GLfloat vertexData[] = {
        //X     Y       Z       U     V       Normal
        -width, -width, 0.0f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
         width, -width, 0.0f,   2.0f, 0.0f,   0.0f, 0.0f, 1.0f,
        -width,  width, 0.0f,   0.0f, 2.0f,   0.0f, 0.0f, 1.0f,
         width, -width, 0.0f,   2.0f, 0.0f,   0.0f, 0.0f, 1.0f,
         width,  width, 0.0f,   2.0f, 2.0f,   0.0f, 0.0f, 1.0f,
        -width,  width, 0.0f,   0.0f, 2.0f,   0.0f, 0.0f, 1.0f,
    };

    glBindBuffer(GL_ARRAY_BUFFER, arrayBufferHandle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

    GLuint loc = glGetAttribLocation(shader->getProg(), "position");
    if (loc < 0) throw (std::runtime_error(std::string("Location not found in shader program for variable ") + "position"));
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (const GLvoid*)(0*sizeof(float)));

    loc = glGetAttribLocation(shader->getProg(), "uv");
    if (loc < 0) throw (std::runtime_error(std::string("Location not found in shader program for variable ") + "uv"));
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (const GLvoid*)(3*sizeof(float)));

    loc = glGetAttribLocation(shader->getProg(), "normal");
    if (loc < 0) throw (std::runtime_error(std::string("Location not found in shader program for variable ") + "normal"));
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (const GLvoid*)(5*sizeof(float)));

    glBindVertexArray(0);
    return vertexArrayHandle;
}

/**
 * Draws the scene (quad and light source).
 */
void drawScene() {
    std::stack<glm::mat4> ms;
    ms.push(glm::mat4(1.0));

    ms.push(ms.top()); //Textured Quad
        shader.activate();
        shader.uniformMatrix4fv("modelMatrix", ms.top());

        //bind the texture
        glActiveTexture(GL_TEXTURE0); //We activate the first texture uniform in the shader
        glBindTexture(GL_TEXTURE_2D, textureHandles.at(textureIndex)); //Bind specific texture to it

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    ms.pop();

    ms.push(ms.top()); //Light Source
        ms.top() = glm::translate(ms.top(), lightPosition);
        defaultShader.activate();
        defaultShader.uniformMatrix4fv("modelMatrix", ms.top());
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    ms.pop();
}

// ---------------------------- Input -------------------------- //
// This method is called when keyboard event happens.
// Sets GLFW window should close flag to true, when escape key is pressed.
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        if (key == GLFW_KEY_RIGHT) {
            textureIndex = (textureIndex + 1 + textureHandles.size()) % textureHandles.size();
        }
        if (key == GLFW_KEY_LEFT) {
            textureIndex = (textureIndex - 1 + textureHandles.size()) % textureHandles.size();
        }
    }
}

// ---------------------------- Main -------------------------- //
int main(int argc, char *argv[]) {
    GLFWwindow *win;
    if (!glfwInit()) {
        exit (EXIT_FAILURE);
    }

    float screenWidth = 800;
    float screenHeight = 450;
    win = glfwCreateWindow(screenWidth, screenHeight, "UV Mapping", NULL, NULL);

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

    glfwSetKeyCallback(win, key_callback);

    glm::vec3 viewer = glm::vec3(0.0, 0.0, 1.2);
    glm::mat4 perspective = glm::perspective(glm::radians(80.0f), screenWidth / screenHeight, 0.1f, 100.f);
    glm::mat4 view = glm::lookAt(
        viewer, //Position
        glm::vec3(0.0, 0.0, 0.0),  //LookAt
        glm::vec3(0.0, 1.0, 0.0)   //Up
    );

    shader.use(); //Compile and send uniforms
    shader.uniformMatrix4fv("projectionMatrix", perspective);
    shader.uniformMatrix4fv("viewMatrix", view);

    defaultShader.use(); //Compile and send uniforms
    defaultShader.uniformMatrix4fv("projectionMatrix", perspective);
    defaultShader.uniformMatrix4fv("viewMatrix", view);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // -------------- Load textures ------------- //
    //Some options for the internaltFormat: GL_RGB, GL_RGBA, GL_SRGB, GL_SRGB_ALPHA (use last 2 if you want to do gamma correction)
    textureHandles.push_back(load_texture(GL_TEXTURE_2D, GL_RGB, "data/stripeTexture.png"));
    textureHandles.push_back(load_texture(GL_TEXTURE_2D, GL_RGBA, "data/sasuke256.png"));
    textureHandles.push_back(load_texture(GL_TEXTURE_2D, GL_RGB, "data/ut256.png"));
    textureHandles.push_back(load_texture(GL_TEXTURE_2D, GL_RGB, "data/brickTexture.jpg"));

    // -------------- Create objects ------------- //
    quadVAO = createQuad(1.0f, &shader);
    lightVAO = createQuad(0.02f, &defaultShader);

    while (!glfwWindowShouldClose(win)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightPosition.x = 0.8f * sin(glfwGetTime()); //Move our light on a trajectory
        lightPosition.y = 0.5f * cos(glfwGetTime());
        lightPosition.z = 0.1f * sin(glfwGetTime()) + 0.3;
        lightPositionCam = glm::vec3(view * glm::vec4(lightPosition, 1.0));
        shader.activate();
        shader.uniform3f("lightPosition", lightPositionCam.x, lightPositionCam.y, lightPositionCam.z);

        drawScene();
        glfwSwapBuffers(win);

        usleep(10000);
        glfwPollEvents();
    }

    glfwTerminate();
    exit(EXIT_SUCCESS);
    return 0;
}
