#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <stdexcept>
#include <GLEW/glew.h>      // OpenGL Extension Wrangler -
#include <GLFW/glfw3.h>     // Windows and input
#include <glm/glm.hpp>      // OpenGL math library
#include <vector>
#include "shader_util.h"

GLuint createQuad(glm::vec3 color, shader_prog* shader);
GLuint createTexturedQuad(shader_prog* shader);
GLuint createSphere(float radius, int widthSegments, int heightSegments, glm::vec3 color, int &chopperBodyTriangleCount, shader_prog* shader);
GLuint createCube(glm::vec3 color, shader_prog* shader);

#endif
