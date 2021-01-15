// ---------------------------- Includes -------------------------- //
#include <stdlib.h>         // C++ standard library
#include <stdexcept>
#include <stack>            // We use the standard C++ stack implementation to create model matrix stacks
#include <unistd.h>         // Threading
#include <stdio.h>          // Input/Output
#include <GLEW/glew.h>      // OpenGL Extension Wrangler -
#include <GLFW/glfw3.h>     // Windows and input
#include <glm/glm.hpp>      // OpenGL math library

#include "shader_util.h"
#include "geometry_util.h"

// --------------- Forward declarations ------------- //
void drawHangar(shader_prog shader, glm::mat4 mMatrix);
void drawChopper(shader_prog shader, glm::mat4 mMatrix);

GLuint cubeVAO;
GLuint leftWallVAO, rightWallVAO, backWallVAO, ceilingVAO, floorVAO;
GLuint chopperVAO, firstBladeVAO, secondBladeVAO;

// --- Load the shaders declared in glsl files in the project folder ---//
shader_prog phongShader("shaders/phong.vert.glsl", "shaders/phong.frag.glsl");

glm::vec3 lightPosition = glm::vec3(0.0);
glm::vec3 viewerPosition = glm::vec3(2.0, 0.0, 18.0);

glm::mat4 mirrorMat;
float lookAtZ = 0.0;
bool renderReflectionCamera = false;

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

    glm::vec3 toView(glm::vec3 v) {

        return glm::vec3(view * glm::vec4(v, 1.0));
    }
};

Camera* mainCam;
Camera* reflCam;

void drawHangar(shader_prog shader) {
    drawHangar(shader, glm::mat4(1.0));
}

void drawHangar(shader_prog shader, glm::mat4 mMatrix) {
    glUseProgram(shader);
    std::stack<glm::mat4> ms;
    ms.push(mMatrix);

    ms.push(ms.top());
        ms.top() = glm::translate(ms.top(), glm::vec3(-10.0, 0.0, 0.0));
        ms.top() = glm::rotate(ms.top(), glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
        shader.uniformMatrix4fv("modelMatrix", ms.top());
        glBindVertexArray(leftWallVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    ms.pop();
    ms.push(ms.top());
        ms.top() = glm::translate(ms.top(), glm::vec3(10.0, 0.0, 0.0));
        ms.top() = glm::rotate(ms.top(), glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
        shader.uniformMatrix4fv("modelMatrix", ms.top());
        glBindVertexArray(leftWallVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    ms.pop();
    ms.push(ms.top());
        ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 10.0, 0.0));
        ms.top() = glm::rotate(ms.top(), glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
        shader.uniformMatrix4fv("modelMatrix", ms.top());
        glBindVertexArray(ceilingVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    ms.pop();
    ms.push(ms.top());
        ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 0.0, -10.0));
        shader.uniformMatrix4fv("modelMatrix", ms.top());
        glBindVertexArray(backWallVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    ms.pop();
}

void drawFloor(shader_prog shader) {
    glUseProgram(shader);
    glm::mat4 mModel = glm::mat4(1.0);
    mModel = glm::translate(mModel, glm::vec3(0.0, -10.0, 0.0));
    mModel = glm::rotate(mModel, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
    shader.uniformMatrix4fv("modelMatrix", mModel);
    glBindVertexArray(floorVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

/**
* Task: Copy chopper from previous task
*/
void drawChopper(shader_prog shader) {
    glUseProgram(shader);

    std::stack<glm::mat4> ms;
    ms.push(glm::mat4(1.0)); // World: Push an identity matrix to the bottom of stack

        ms.push(ms.top()); // Chopper
            ms.top() = glm::translate(ms.top(), glm::vec3(0.0, -2.0, -3.0));
            ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 2 * sin(glfwGetTime()) - 3.0, 0.0));
            ms.top() = glm::scale(ms.top(), glm::vec3(0.5, 0.5, 0.5));

            ms.push(ms.top()); // Body
                ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 1.0, 0.0));
                ms.top() = glm::rotate(ms.top(), glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
                ms.top() = glm::scale(ms.top(), glm::vec3(2.5, 2.0, 7.0));

                shader.uniformMatrix4fv("modelMatrix", ms.top());
                glBindVertexArray(chopperVAO);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            ms.pop();

            ms.push(ms.top()); // Blades
                ms.top() = glm::rotate(ms.top(), glm::radians((float)glfwGetTime() * 250), glm::vec3(0.0, 1.0, 0.0));
                ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 4.0, 0.0));

                ms.push(ms.top()); // Blade1
                    ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 0.1, 6.0));
                    ms.top() = glm::scale(ms.top(), glm::vec3(0.5, 0.2, 5.0));

                    shader.uniformMatrix4fv("modelMatrix", ms.top());
                    glBindVertexArray(firstBladeVAO);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                ms.pop();

                ms.push(ms.top()); // Blade2
                    ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 0.1, -6.0));
                    ms.top() = glm::rotate(ms.top(), glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
                    ms.top() = glm::scale(ms.top(), glm::vec3(0.5, 0.2, 5.0));

                    shader.uniformMatrix4fv("modelMatrix", ms.top());
                    glBindVertexArray(secondBladeVAO);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                ms.pop();
            ms.pop();
        ms.pop();
    ms.pop();
}

void drawChopper(shader_prog shader, glm::mat4 model) {
    glUseProgram(shader);
    model = glm::translate(model, glm::vec3(0.0, 3*sin(glfwGetTime())-3.0, 0.0));
    shader.uniformMatrix4fv("modelMatrix", model);
    glBindVertexArray(chopperVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void initHangar() {
    printf("Init walls");
    glUseProgram(phongShader);
    leftWallVAO = createQuad(glm::vec3(0.66, 0.66, 0.66), 10.0f,phongShader);
    rightWallVAO = createQuad(glm::vec3(0.4, 0.4, 0.4), 10.0f,phongShader);
    backWallVAO = createQuad(glm::vec3(0.52, 0.52, 0.52), 10.0f,phongShader);
    ceilingVAO = createQuad(glm::vec3(0.33, 0.33, 0.33), 10.0f,phongShader);
    floorVAO = createQuad(glm::vec3(0.22, 0.22, 0.22), 10.0f,phongShader);
}

/**
    Task: Complete drawScene()
*/
void drawScene() {

    // Make keys change camera's lookAt (maybe position too, maybe change camera's too).

    /**
        1. Draw a stencil for the reflective area.
            a. Enable stencil test
            b. Disable writing into depth and color buffers with glColorMask and glDepthMask
            c. Set stencil function to always fail
            d. Set stencil operation to write 1 into stencil buffer on fail
            e. Draw the floor with the mainCam (update the view matrix). Since other buffers are masked, it will be drawn only into stencil buffer.

            Reference:
            * Stencil:  http://en.wikibooks.org/wiki/OpenGL_Programming/Stencil_buffer
            * color mask: https://www.khronos.org/opengles/sdk/docs/man/xhtml/glColorMask.xml
            * depth mask: https://www.khronos.org/opengles/sdk/docs/man/xhtml/glDepthMask.xml


        2. Draw reflected scene into the buffer area with floor stencil
            a. Unmask writing into depth and color buffers
            b. Set stencil function to pass, when equal to 1
            c. Enable front face culling and disable back face culling.
            d. Transform the lightPosition into reflCam space and update the uniform.
            e. Draw the hangar and the 'chopper' with the reflCam (update the view matrix).
            f. Enable back face culling and disable front face culling.
            g. Disable stencil test

            Reference:
            * Matrix transformation: http://glm.g-truc.net/0.9.2/api/a00245.html
            * Culling: https://www.opengl.org/wiki/Face_Culling

        3. Draw the floor as semi transparent object. To do this, we can tell OpenGL that
           everything following should be drawn as transparent, without changing the material
           value for the floor.
            a. Enable blending
            b. Set the alpha value for blending. This can be done with glBlendColor().
            c. Set blend function source value to GL_Constant_Alpha. Source is the object
               we are currently drawing.
            d. Set blend function destination value to GL_One_Minus_Constant_Alpha. Destination
               value is the color value that is already in the buffer.
            e. Transform the lightPosition into mainCam space and update the uniform.
            f. Draw the floor with the mainCam (update the view matrix).
            g. Disable blending.

            Reference:
            * https://www.opengl.org/wiki/Blending
            * https://www.opengl.org/sdk/docs/man/html/glBlendFunc.xhtml
            * https://www.opengl.org/sdk/docs/man/html/glBlendColor.xhtml

        4. Draw the rest of the scene as usual.
    */
    if (renderReflectionCamera) {
        glCullFace(GL_FRONT);
        phongShader.uniform3v("lightPosition", reflCam->toView(lightPosition));
        phongShader.uniformMatrix4fv("viewMatrix", reflCam->view);
        drawHangar(phongShader);
        drawChopper(phongShader);
        //glCullFace(GL_BACK);
    }
    else {
        // --- 1 ---
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_NEVER, 1, 0xFF);
        glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glDepthMask(GL_FALSE);

        phongShader.uniformMatrix4fv("viewMatrix", mainCam->view);
        drawFloor(phongShader);

        // --- 2 ---
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glDepthMask(GL_TRUE);
        glCullFace(GL_FRONT);
        glStencilFunc(GL_EQUAL, 1, 0xFF);

        phongShader.uniform3v("lightPosition", reflCam->toView(lightPosition));
        phongShader.uniformMatrix4fv("viewMatrix", reflCam->view);
        drawHangar(phongShader);
        drawChopper(phongShader);

        glCullFace(GL_BACK);

        glDisable(GL_STENCIL_TEST);

        // --- 3 ---
        glEnable(GL_BLEND);
        glBlendColor(0.0, 0.0, 0.0, 0.8);
        glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);

        phongShader.uniform3v("lightPosition", mainCam->toView(lightPosition));
        phongShader.uniformMatrix4fv("viewMatrix", mainCam->view);

        drawFloor(phongShader);

        glDisable(GL_BLEND);

        // --- 4 ---
        drawHangar(phongShader);
        drawChopper(phongShader);
    }
}

// ---------------------------- Input -------------------------- //
// This method is called when keyboard event happens.
// Sets GLFW window should close flag to true, when escape key is pressed.
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (key == GLFW_KEY_UP && lookAtZ < 15.0) {
        lookAtZ += 0.5;
        }
    if (key == GLFW_KEY_DOWN && lookAtZ > -15.0) {
        lookAtZ -= 0.5;
    }
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        renderReflectionCamera = !renderReflectionCamera;
    }
}

// ---------------------------- Main -------------------------- //
int main(int argc, char *argv[]) {
    GLFWwindow *win;
    if (!glfwInit()) {
        exit (EXIT_FAILURE);
    }
    // -------------- Create window ------------- //
    float screenWidth = 800;
    float screenHeight = 450;
    win = glfwCreateWindow(screenWidth, screenHeight, "Reflected Chopper", NULL, NULL);

    if (!win) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // -------------- Set up OpenGL context ------------- //
    glfwMakeContextCurrent(win);

    // -------------- Init glew ------------- //
    glewExperimental = GL_TRUE;
    GLenum status = glewInit();
    if(status != GLEW_OK) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(status));
    }

    // -------------- Check OpenGL version ------------- //
    const GLubyte* renderer = glGetString (GL_RENDERER);
    const GLubyte* version = glGetString (GL_VERSION);
    printf ("Renderer: %s\n", renderer);
    printf ("OpenGL version supported %s\n", version);

    // -------------- Set up input callback function ------------- //
    glfwSetKeyCallback(win, key_callback);

    // -------------- Set up shader ------------- //
    phongShader.use();

    // -------------- Set up the cameras ------------- //
    mainCam = new Camera(
        glm::perspective(glm::radians(80.0f), screenWidth / screenHeight, 0.1f, 100.f),
        glm::lookAt(
            viewerPosition, //Position
            glm::vec3(0.0, -5.0, lookAtZ),  //LookAt
            glm::vec3(0.0, 1.0, 0.0)   //Up
        )
    );

    mirrorMat = glm::scale(glm::mat4(1.0), glm::vec3(-1.0, 1.0, 1.0));
    reflCam = new Camera(
        glm::perspective(glm::radians(80.0f), screenWidth / screenHeight, 0.1f, 100.f),
        mirrorMat * glm::lookAt(
            glm::vec3(viewerPosition.x, -20, viewerPosition.z), //Position
            glm::vec3(0.0, -15.0, lookAtZ),  //LookAt
            glm::vec3(0.0, -1.0, 0.0)   //Up
        )
    );

    phongShader.uniformMatrix4fv("projectionMatrix", mainCam->projection);

    // -------------- Set light position ------------- //
    glm::vec3 light;
    glm::vec3 light1 = glm::vec3(5.5, 1.5, 0.0);
    glm::vec3 light2 = glm::vec3(-2.0, 0.0, 0.2);
    float speed = 0.5f;

    // -------------- few more settings ------------- //
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // -------------- Create objects ------------- //
    initHangar();
    chopperVAO = createCube(glm::vec3(0.16, 0.16, 0.66), phongShader);
    firstBladeVAO = createCube(glm::vec3(0.66, 0.66, 0.66), phongShader);
    secondBladeVAO = createCube(glm::vec3(0.66, 0.66, 0.66), phongShader);

    // -------------- Event loop ------------- //
    while (!glfwWindowShouldClose(win)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        lightPosition = (float)sin(glfwGetTime() * (float)speed) * light1 + light2;

        mainCam->view = glm::lookAt(
            viewerPosition, //Position
            glm::vec3(0.0, -5.0, lookAtZ),  //LookAt
            glm::vec3(0.0, 1.0, 0.0)   //Up
        );

        reflCam->view = mirrorMat * glm::lookAt(
            glm::vec3(viewerPosition.x, -20, viewerPosition.z), //Position
            glm::vec3(0.0, -15.0, lookAtZ),  //LookAt
            glm::vec3(0.0, -1.0, 0.0)   //Up
        );

        drawScene();

        glfwSwapBuffers(win);
        usleep(10000);
        glfwPollEvents();
    }

    glfwTerminate();
    exit(EXIT_SUCCESS);
    return 0;
}
