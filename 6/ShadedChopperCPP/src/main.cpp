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
#include "shader_util.h"    // Utility methods to keep this file a bit shorter.

#include <iostream>


// --------------- Forward declarations ------------- //
GLuint createWall(glm::vec3 color);
GLuint createCube(glm::vec3 color);
GLuint createSphere(float radius, int widthSegments, int heightSegments, glm::vec3 color, int &faceCount);

// --- Load the shaders declared in glsl files in the project folder ---//
shader_prog gouraud("shaders/gouraud.vert.glsl", "shaders/gouraud.frag.glsl");
shader_prog phong("shaders/phong.vert.glsl", "shaders/phong.frag.glsl");
//active shader
shader_prog shader = gouraud;

GLuint cubeVAO, sphereVAO;
GLuint leftWallVAO, rightWallVAO, backWallVAO, ceilingVAO, floorVAO;
GLuint bodyVAO, bladeVAO;
int chopperBodyTriangleCount;

unsigned int sceneID = 0;
const int sceneCount = 2;

/**
 * Initializes the hangar walls
 */
void initWalls() {
    leftWallVAO = createWall(glm::vec3(0.66, 0.66, 0.66));
    rightWallVAO = createWall(glm::vec3(0.4, 0.4, 0.4));
    backWallVAO = createWall(glm::vec3(0.52, 0.52, 0.52));
    ceilingVAO = createWall(glm::vec3(0.33, 0.33, 0.33));
    floorVAO = createWall(glm::vec3(0.22, 0.22, 0.22));

}

/**
 * --Task--
 * Copy your chopper initialization from the previous task
 * Use createSphere for the chopper's body.
 */
void initChopper() {
    //cubeVAO = createCube(glm::vec3(1.0f, 0.0f, 0.0f));
    //sphereVAO = createSphere(1.0f, 38, 36, glm::vec3(1.0, 0.0, 0.0), chopperBodyTriangleCount);

    bodyVAO = createSphere(1.0f, 8, 6, glm::vec3(1.0, 0.0, 0.0), chopperBodyTriangleCount); //createCube(glm::vec3(0.4, 0.4, 0.4), glm::vec3(0.9, 0.9, 0.9));
    bladeVAO = createCube(glm::vec3(0.9, 0.2, 0.9));
}

/**
 * Creates a single colored wall.
 */
GLuint createWall(glm::vec3 color) {
    float s = 10.0;

    GLfloat vertices[] = {
                            -s, -s, 0.0,
                             s, -s, 0.0,
                             s,  s, 0.0,
                            -s,  s, 0.0
                        };
    GLfloat normals[] = {
                            0.0f, 0.0f, 1.0f,
                            0.0f, 0.0f, 1.0f,
                            0.0f, 0.0f, 1.0f,
                            0.0f, 0.0f, 1.0f,
                        };

    GLubyte indices[] = { 0, 1, 2, 0, 2, 3 };

    GLfloat colors[] = {
                            color.r, color.g, color.b,
                            color.r, color.g, color.b,
                            color.r, color.g, color.b,
                            color.r, color.g, color.b
                        };

    GLuint vertexArrayHandle;
    glGenVertexArrays(1, &vertexArrayHandle);
    glBindVertexArray(vertexArrayHandle);

    shader.attribute3fv("position", vertices, 12);
    shader.attribute3fv("color", colors, 12);
    shader.attribute3fv("normal", normals, 12);

    GLuint vboHandle;
    glGenBuffers(1, &vboHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLfloat)*12, indices, GL_STATIC_DRAW);
    glBindVertexArray(0);

    return vertexArrayHandle;
}

/**
 * Draws the hangar
 */
void drawHangar() {
    std::stack<glm::mat4> ms;
    ms.push(glm::mat4(1.0)); //Push an identity matrix to the bottom of stack

    ms.push(ms.top());
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
 * This function will create the sphere geometry.
 * It uses the same algorithm as in Three.js SphereGeometry class:
 * http://threejs.org/docs/#Reference/Extras.Geometries/SphereGeometry
 */
GLuint createSphere(float radius, int widthSegments, int heightSegments, glm::vec3 color, int &chopperBodyTriangleCount) {
    GLuint vaoHandle;

    glGenVertexArrays(1, &vaoHandle);
    glBindVertexArray(vaoHandle);

	float phiStart = 0.0;
	float phiLength = glm::pi<float>() * 2.0;

	float thetaStart = 0.0;
	float thetaLength = glm::pi<float>();

	int x, y;
	float u, v;
	std::vector<glm::vec3> vertices = std::vector<glm::vec3>();
	std::vector<glm::vec3> normals = std::vector<glm::vec3>();
	std::vector<glm::vec3> colors = std::vector<glm::vec3>();
	std::vector<glm::u16vec3> faces = std::vector<glm::u16vec3>();
	std::vector<unsigned short> indices = std::vector<unsigned short>();

	for(y = 0; y <= heightSegments; y++) {
		for(x = 0; x <= widthSegments; x++) {
			u = (float)x / widthSegments;
			v = (float)y / heightSegments;

			glm::vec3 vertex = glm::vec3(
                -radius * glm::cos(phiStart + u * phiLength) * glm::sin(thetaStart + v * thetaLength),
                radius * glm::cos(thetaStart + v * thetaLength),
                radius * glm::sin(phiStart + u * phiLength) * glm::sin(thetaStart + v * thetaLength)
            );

			vertices.push_back(vertex);
			normals.push_back(glm::normalize(vertex));
			colors.push_back(color);
		}
	}

	int vIndex1, vIndex2, vIndex3, vIndex4;

	for(y = 0; y < heightSegments; y++) {
		for(x = 0; x < widthSegments; x++) {

            vIndex1 = (widthSegments+1) * y + x+1;
            vIndex2 = (widthSegments+1) * y + x;
            vIndex3 = (widthSegments+1) * (y+1) + x;
            vIndex4 = (widthSegments+1) * (y+1) + x + 1;

			glm::vec3 v1 = vertices.at(vIndex1);
			glm::vec3 v2 = vertices.at(vIndex2);
			glm::vec3 v3 = vertices.at(vIndex3);
			glm::vec3 v4 = vertices.at(vIndex4);

			if (glm::abs(v1.y) == radius) {
                faces.push_back(glm::u16vec3(vIndex1, vIndex3, vIndex4));
			} else if(glm::abs(v3.y) == radius) {
                faces.push_back(glm::u16vec3(vIndex1, vIndex2, vIndex3));
			} else {
                faces.push_back(glm::u16vec3(vIndex1, vIndex2, vIndex4));
                faces.push_back(glm::u16vec3(vIndex2, vIndex3, vIndex4));
			}
			chopperBodyTriangleCount += 3;
		}
		chopperBodyTriangleCount += 3;
	}

    shader.use();
    shader.attributeVectorVec3("position", vertices);
    shader.attributeVectorVec3("color", colors);
    shader.attributeVectorVec3("normal", normals);

    GLuint vboHandle;
    glGenBuffers(1, &vboHandle);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*faces.size()*3, &faces[0], GL_STATIC_DRAW);

    glBindVertexArray(0);

    return vaoHandle;
}

/**
 * This function creates a cube.
 * Here we use another approach to define the geometry: one VBO with a stride to hold different data.
 */
GLuint createCube(glm::vec3 color) {
    GLuint vertexArrayHandle;
    GLuint arrayBufferHandle;

    glGenVertexArrays(1, &vertexArrayHandle);
    glBindVertexArray(vertexArrayHandle);
    glGenBuffers(1, &arrayBufferHandle);

    GLfloat vertexData[] = {
        //  X     Y     Z      Red      Green    Blue       Normal
        // bottom
        -1.0f, -1.0f, -1.0f,   color.r, color.g, color.b,   0.0f, -1.0f, 0.0f,
         1.0f, -1.0f, -1.0f,   color.r, color.g, color.b,   0.0f, -1.0f, 0.0f,
        -1.0f, -1.0f,  1.0f,   color.r, color.g, color.b,   0.0f, -1.0f, 0.0f,
         1.0f, -1.0f, -1.0f,   color.r, color.g, color.b,   0.0f, -1.0f, 0.0f,
         1.0f, -1.0f,  1.0f,   color.r, color.g, color.b,   0.0f, -1.0f, 0.0f,
        -1.0f, -1.0f,  1.0f,   color.r, color.g, color.b,   0.0f, -1.0f, 0.0f,

        // top
        -1.0f, 1.0f, -1.0f,   color.r, color.g, color.b,   0.0f, 1.0f, 0.0f,
        -1.0f, 1.0f,  1.0f,   color.r, color.g, color.b,   0.0f, 1.0f, 0.0f,
         1.0f, 1.0f, -1.0f,   color.r, color.g, color.b,   0.0f, 1.0f, 0.0f,
         1.0f, 1.0f, -1.0f,   color.r, color.g, color.b,   0.0f, 1.0f, 0.0f,
        -1.0f, 1.0f,  1.0f,   color.r, color.g, color.b,   0.0f, 1.0f, 0.0f,
         1.0f, 1.0f,  1.0f,   color.r, color.g, color.b,   0.0f, 1.0f, 0.0f,

        // front
        -1.0f, -1.0f, 1.0f,   color.r, color.g, color.b,   0.0f, 0.0f, 1.0f,
         1.0f, -1.0f, 1.0f,   color.r, color.g, color.b,   0.0f, 0.0f, 1.0f,
        -1.0f,  1.0f, 1.0f,   color.r, color.g, color.b,   0.0f, 0.0f, 1.0f,
         1.0f, -1.0f, 1.0f,   color.r, color.g, color.b,   0.0f, 0.0f, 1.0f,
         1.0f,  1.0f, 1.0f,   color.r, color.g, color.b,   0.0f, 0.0f, 1.0f,
        -1.0f,  1.0f, 1.0f,   color.r, color.g, color.b,   0.0f, 0.0f, 1.0f,

        // back
        -1.0f, -1.0f, -1.0f,   color.r, color.g, color.b,   0.0f, 0.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,   color.r, color.g, color.b,   0.0f, 0.0f, -1.0f,
         1.0f, -1.0f, -1.0f,   color.r, color.g, color.b,   0.0f, 0.0f, -1.0f,
         1.0f, -1.0f, -1.0f,   color.r, color.g, color.b,   0.0f, 0.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,   color.r, color.g, color.b,   0.0f, 0.0f, -1.0f,
         1.0f,  1.0f, -1.0f,   color.r, color.g, color.b,   0.0f, 0.0f, -1.0f,

        // left
        -1.0f, -1.0f,  1.0f,   color.r, color.g, color.b,   -1.0f, 0.0f, 0.0f,
        -1.0f,  1.0f, -1.0f,   color.r, color.g, color.b,   -1.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,   color.r, color.g, color.b,   -1.0f, 0.0f, 0.0f,
        -1.0f, -1.0f,  1.0f,   color.r, color.g, color.b,   -1.0f, 0.0f, 0.0f,
        -1.0f,  1.0f,  1.0f,   color.r, color.g, color.b,   -1.0f, 0.0f, 0.0f,
        -1.0f,  1.0f, -1.0f,   color.r, color.g, color.b,   -1.0f, 0.0f, 0.0f,

        // right
         1.0f, -1.0f,  1.0f,   color.r, color.g, color.b,   1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, -1.0f,   color.r, color.g, color.b,   1.0f, 0.0f, 0.0f,
         1.0f,  1.0f, -1.0f,   color.r, color.g, color.b,   1.0f, 0.0f, 0.0f,
         1.0f, -1.0f,  1.0f,   color.r, color.g, color.b,   1.0f, 0.0f, 0.0f,
         1.0f,  1.0f, -1.0f,   color.r, color.g, color.b,   1.0f, 0.0f, 0.0f,
         1.0f,  1.0f,  1.0f,   color.r, color.g, color.b,   1.0f, 0.0f, 0.0f
    };

    glBindBuffer(GL_ARRAY_BUFFER, arrayBufferHandle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

    GLuint loc = glGetAttribLocation(shader.getProg(), "position");
    if (loc < 0) throw (std::runtime_error(std::string("Location not found in shader program for variable ") + "position"));
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), (const GLvoid*)(0*sizeof(float)));


    loc = glGetAttribLocation(shader.getProg(), "color");
    if (loc < 0) throw (std::runtime_error(std::string("Location not found in shader program for variable ") + "color"));
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), (const GLvoid*)(3*sizeof(float)));

    loc = glGetAttribLocation(shader.getProg(), "normal");
    if (loc < 0) throw (std::runtime_error(std::string("Location not found in shader program for variable ") + "normal"));
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), (const GLvoid*)(6*sizeof(float)));

    glBindVertexArray(0);
    return vertexArrayHandle;
}

/**
 * --Task--
 * Currently draws a cube and a sphere.
 * Copy the chopper's drawing from the previous task, but be sure to call the correct draw calls!
 */
void drawChopper() {
    std::stack<glm::mat4> ms;
    ms.push(glm::mat4(1.0)); // World: Push an identity matrix to the bottom of stack

        ms.push(ms.top()); // Chopper
            ms.top() = glm::translate(ms.top(), glm::vec3(0.0, -2.0, 0.0));
            ms.top() = glm::scale(ms.top(), glm::vec3(0.5, 0.5, 0.5));

            ms.push(ms.top()); // Body
                //ms.top() = glm::rotate(ms.top(), glm::radians((float)glfwGetTime() * 20), glm::vec3(0.0, 1.0, 0.0));
                ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 1.0, 0.0));
                ms.top() = glm::rotate(ms.top(), glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
                ms.top() = glm::scale(ms.top(), glm::vec3(2.5, 2.0, 7.0));

                shader.uniformMatrix4fv("modelMatrix", ms.top());
                glBindVertexArray(bodyVAO);
                glDrawElements(GL_TRIANGLES, chopperBodyTriangleCount * 3, GL_UNSIGNED_SHORT, 0);
            ms.pop();

            ms.push(ms.top()); // Blades
                ms.top() = glm::rotate(ms.top(), glm::radians((float)glfwGetTime() * 250), glm::vec3(0.0, 1.0, 0.0));
                ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 4.0, 0.0));

                ms.push(ms.top()); // Blade1
                    ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 0.1, 6.0));
                    ms.top() = glm::scale(ms.top(), glm::vec3(0.5, 0.2, 5.0));

                    shader.uniformMatrix4fv("modelMatrix", ms.top());
                    glBindVertexArray(bladeVAO);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                ms.pop();

                ms.push(ms.top()); // Blade2
                    ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 0.1, -6.0));
                    ms.top() = glm::rotate(ms.top(), glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
                    ms.top() = glm::scale(ms.top(), glm::vec3(0.5, 0.2, 5.0));

                    shader.uniformMatrix4fv("modelMatrix", ms.top());
                    glBindVertexArray(bladeVAO);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                ms.pop();
            ms.pop();
        ms.pop();
    ms.pop();
}

void changeShader(int shaderID) {
    switch(shaderID) {
        case 0:
            shader = gouraud;
            printf("Gouraud shading\n");
            break;
        case 1:
            shader = phong;
            printf("Phong shading\n");
            break;
    }
    shader.use();
}

// ---------------------------- Input -------------------------- //
// This method is called when keyboard event happens.
// Sets GLFW window should close flag to true, when escape key is pressed.
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (key== GLFW_KEY_LEFT && action == GLFW_PRESS) {
        sceneID = --sceneID % sceneCount;
        changeShader(sceneID);
    }
    if (key== GLFW_KEY_RIGHT && action == GLFW_PRESS) {
        sceneID = ++sceneID % sceneCount;
        changeShader(sceneID);
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
    win = glfwCreateWindow(screenWidth, screenHeight, "Shaders", NULL, NULL);

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

    changeShader(sceneID);
    shader.use();

    glm::vec3 viewer = glm::vec3(0.0, 4.0, 10.0);

    glm::mat4 perspective = glm::perspective(glm::radians(80.0f), screenWidth / screenHeight, 0.1f, 100.f);
    glm::mat4 view = glm::lookAt(
        viewer, //Position
        glm::vec3(0.0, -2.0, 0.0),  //LookAt
        glm::vec3(0.0, 1.0, 0.0)   //Up
    );

    initChopper();
    initWalls();

    glm::vec3 light = glm::vec3(1.0, 1.0, 1.0);
    glfwGetTime();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

    while (!glfwWindowShouldClose(win)) {
        light.x = 6.0f * sin(glfwGetTime()); //Move our light on a trajectory
        light.y = 5.0f * sin(glfwGetTime());
        light.z = 6.0f * cos(glfwGetTime());
        light = glm::vec3(view * glm::vec4(light, 1.0));
        shader.uniform3f("lightPosition", light.x, light.y, light.z);

        shader.uniformMatrix4fv("projectionMatrix", perspective);
        shader.uniformMatrix4fv("viewMatrix", view);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawHangar();
        drawChopper();
        glfwSwapBuffers(win);
        usleep(10000);
        glfwPollEvents();
    }

    glfwTerminate();
    exit(EXIT_SUCCESS);
    return 0;
}
