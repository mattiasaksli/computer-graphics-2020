#include "shader_util.h"

#include <stdexcept>
#include <stdio.h>
//#include <iostream>

GLuint createQuad(glm::vec3 color, float scale, shader_prog shader) {
    GLuint vertexArrayHandle;
    GLuint arrayBufferHandle;

    glGenVertexArrays(1, &vertexArrayHandle);
    glBindVertexArray(vertexArrayHandle);
    glGenBuffers(1, &arrayBufferHandle);

    GLfloat vertexData[] = {
        //  X     Y     Z      U     V       Normal               Color
        -scale,-scale, 0.0f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f,    color.r, color.g, color.b,
         scale,-scale, 0.0f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f,    color.r, color.g, color.b,
        -scale, scale, 0.0f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f,    color.r, color.g, color.b,
         scale,-scale, 0.0f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f,    color.r, color.g, color.b,
         scale, scale, 0.0f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f,    color.r, color.g, color.b,
        -scale, scale, 0.0f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f,    color.r, color.g, color.b,
    };

    glBindBuffer(GL_ARRAY_BUFFER, arrayBufferHandle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

    GLuint loc = glGetAttribLocation(shader.getProg(), "position");
    if (loc < 0) printf("WARNING: Location not found in shader program for variable %s.\n", "position");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 11*sizeof(float), (const GLvoid*)(0*sizeof(float)));

    loc = glGetAttribLocation(shader.getProg(), "uv");
    if (loc < 0) printf("WARNING: Location not found in shader program for variable %s.\n", "uv");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 11*sizeof(float), (const GLvoid*)(3*sizeof(float)));

    loc = glGetAttribLocation(shader.getProg(), "normal");
    if (loc < 0) printf("WARNING: Location not found in shader program for variable %s.\n", "normal");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 11*sizeof(float), (const GLvoid*)(5*sizeof(float)));

    loc = glGetAttribLocation(shader.getProg(), "color");
    if (loc < 0) printf("WARNING: Location not found in shader program for variable %s.\n", "color");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 11*sizeof(float), (const GLvoid*)(8*sizeof(float)));

    glBindVertexArray(0);
    return vertexArrayHandle;
}

GLuint createCube(glm::vec3 color, shader_prog shader) {
    GLuint vertexArrayHandle;
    GLuint arrayBufferHandle;

    glGenVertexArrays(1, &vertexArrayHandle);
    glBindVertexArray(vertexArrayHandle);
    glGenBuffers(1, &arrayBufferHandle);

    GLfloat vertexData[] = {
        //  X     Y     Z    R     G     B         Normal
        // bottom
        -1.0f,-1.0f,-1.0f,   color.r, color.g, color.b,   0.0f, -1.0f, 0.0f,
         1.0f,-1.0f,-1.0f,   color.r, color.g, color.b,   0.0f, -1.0f, 0.0f,
        -1.0f,-1.0f, 1.0f,   color.r, color.g, color.b,   0.0f, -1.0f, 0.0f,
         1.0f,-1.0f,-1.0f,   color.r, color.g, color.b,   0.0f, -1.0f, 0.0f,
         1.0f,-1.0f, 1.0f,   color.r, color.g, color.b,   0.0f, -1.0f, 0.0f,
        -1.0f,-1.0f, 1.0f,   color.r, color.g, color.b,   0.0f, -1.0f, 0.0f,

        // top
        -1.0f, 1.0f,-1.0f,   color.r, color.g, color.b,   0.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 1.0f,   color.r, color.g, color.b,   0.0f, 1.0f, 0.0f,
         1.0f, 1.0f,-1.0f,   color.r, color.g, color.b,   0.0f, 1.0f, 0.0f,
         1.0f, 1.0f,-1.0f,   color.r, color.g, color.b,   0.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 1.0f,   color.r, color.g, color.b,   0.0f, 1.0f, 0.0f,
         1.0f, 1.0f, 1.0f,   color.r, color.g, color.b,   0.0f, 1.0f, 0.0f,

        // front
        -1.0f,-1.0f, 1.0f,   color.r, color.g, color.b,   0.0f, 0.0f, 1.0f,
         1.0f,-1.0f, 1.0f,   color.r, color.g, color.b,   0.0f, 0.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,   color.r, color.g, color.b,   0.0f, 0.0f, 1.0f,
         1.0f,-1.0f, 1.0f,   color.r, color.g, color.b,   0.0f, 0.0f, 1.0f,
         1.0f, 1.0f, 1.0f,   color.r, color.g, color.b,   0.0f, 0.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,   color.r, color.g, color.b,   0.0f, 0.0f, 1.0f,

        // back
        -1.0f,-1.0f,-1.0f,   color.r, color.g, color.b,   0.0f, 0.0f, -1.0f,
        -1.0f, 1.0f,-1.0f,   color.r, color.g, color.b,   0.0f, 0.0f, -1.0f,
         1.0f,-1.0f,-1.0f,   color.r, color.g, color.b,   0.0f, 0.0f, -1.0f,
         1.0f,-1.0f,-1.0f,   color.r, color.g, color.b,   0.0f, 0.0f, -1.0f,
        -1.0f, 1.0f,-1.0f,   color.r, color.g, color.b,   0.0f, 0.0f, -1.0f,
         1.0f, 1.0f,-1.0f,   color.r, color.g, color.b,   0.0f, 0.0f, -1.0f,

        // left
        -1.0f,-1.0f, 1.0f,   color.r, color.g, color.b,   -1.0f, 0.0f, 0.0f,
        -1.0f, 1.0f,-1.0f,   color.r, color.g, color.b,   -1.0f, 0.0f, 0.0f,
        -1.0f,-1.0f,-1.0f,   color.r, color.g, color.b,   -1.0f, 0.0f, 0.0f,
        -1.0f,-1.0f, 1.0f,   color.r, color.g, color.b,   -1.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, 1.0f,   color.r, color.g, color.b,   -1.0f, 0.0f, 0.0f,
        -1.0f, 1.0f,-1.0f,   color.r, color.g, color.b,   -1.0f, 0.0f, 0.0f,

        // right
         1.0f,-1.0f, 1.0f,   color.r, color.g, color.b,   1.0f, 0.0f, 0.0f,
         1.0f,-1.0f,-1.0f,   color.r, color.g, color.b,   1.0f, 0.0f, 0.0f,
         1.0f, 1.0f,-1.0f,   color.r, color.g, color.b,   1.0f, 0.0f, 0.0f,
         1.0f,-1.0f, 1.0f,   color.r, color.g, color.b,   1.0f, 0.0f, 0.0f,
         1.0f, 1.0f,-1.0f,   color.r, color.g, color.b,   1.0f, 0.0f, 0.0f,
         1.0f, 1.0f, 1.0f,   color.r, color.g, color.b,   1.0f, 0.0f, 0.0f
    };

    glBindBuffer(GL_ARRAY_BUFFER, arrayBufferHandle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

    GLuint loc = glGetAttribLocation(shader.getProg(), "position");
    if (loc < 0) printf("WARNING: Location not found in shader program for variable %s.\n", "position");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), (const GLvoid*)(0*sizeof(float)));


    loc = glGetAttribLocation(shader.getProg(), "color");
    if (loc < 0) printf("WARNING: Location not found in shader program for variable %s.\n", "color");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), (const GLvoid*)(3*sizeof(float)));

    loc = glGetAttribLocation(shader.getProg(), "normal");
    if (loc < 0) printf("WARNING: Location not found in shader program for variable %s.\n", "normal");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), (const GLvoid*)(6*sizeof(float)));

    glBindVertexArray(0);
    return vertexArrayHandle;
}
