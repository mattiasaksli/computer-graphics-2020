#include "geometry.h"

/**
 * This function creates a colored quad.
 * These quads have a position, color and normal for each vertex.
 * Used for hangar walls.
 */
GLuint createQuad(glm::vec3 color, shader_prog* shader) {
    GLuint vertexArrayHandle;
    GLuint arrayBufferHandle;

    glGenVertexArrays(1, &vertexArrayHandle);
    glBindVertexArray(vertexArrayHandle);
    glGenBuffers(1, &arrayBufferHandle);

    float s = 10.0;

    GLfloat vertexData[] = {
        //  X     Y     Z   Color   Normal
        -s,-s, 0.0f,  color.r, color.g, color.b, 0.0f, 0.0f, 1.0f,
         s,-s, 0.0f,  color.r, color.g, color.b, 0.0f, 0.0f, 1.0f,
        -s, s, 0.0f,  color.r, color.g, color.b, 0.0f, 0.0f, 1.0f,
         s,-s, 0.0f,  color.r, color.g, color.b, 0.0f, 0.0f, 1.0f,
         s, s, 0.0f,  color.r, color.g, color.b, 0.0f, 0.0f, 1.0f,
        -s, s, 0.0f,  color.r, color.g, color.b, 0.0f, 0.0f, 1.0f,
    };

    glBindBuffer(GL_ARRAY_BUFFER, arrayBufferHandle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

    GLuint loc = glGetAttribLocation(shader->getProg(), "position");
    if (loc < 0) throw (std::runtime_error(std::string("Location not found in shader program for variable ") + "position"));
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), (const GLvoid*)(0*sizeof(float)));

    loc = glGetAttribLocation(shader->getProg(), "color");
    if (loc < 0) throw (std::runtime_error(std::string("Location not found in shader program for variable ") + "color"));
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), (const GLvoid*)(3*sizeof(float)));

    loc = glGetAttribLocation(shader->getProg(), "normal");
    if (loc < 0) throw (std::runtime_error(std::string("Location not found in shader program for variable ") + "normal"));
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), (const GLvoid*)(6*sizeof(float)));

    glBindVertexArray(0);

    return vertexArrayHandle;
}

/**
 * This function creates a textured quad.
 * These quads have the position and UV for each vertex.
 * Used for the smoke particles.
 */
GLuint createTexturedQuad(shader_prog* shader) {
    GLuint vertexArrayHandle;
    GLuint arrayBufferHandle;

    glGenVertexArrays(1, &vertexArrayHandle);
    glBindVertexArray(vertexArrayHandle);
    glGenBuffers(1, &arrayBufferHandle);

    float s = 4.0;
    GLfloat vertexData[] = {
        //  X     Y     Z    U     V
        -s, -s, 0.0f,   0.0f, 0.0f,
         s, -s, 0.0f,   1.0f, 0.0f,
        -s, s, 0.0f,   0.0f, 1.0f,
         s, -s, 0.0f,   1.0f, 0.0f,
         s, s, 0.0f,   1.0f, 1.0f,
        -s, s, 0.0f,   0.0f, 1.0f,
    };

    glBindBuffer(GL_ARRAY_BUFFER, arrayBufferHandle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

    GLuint loc = glGetAttribLocation(shader->getProg(), "position");
    if (loc < 0) throw (std::runtime_error(std::string("Location not found in shader program for variable ") + "position"));
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (const GLvoid*)(0*sizeof(float)));

    loc = glGetAttribLocation(shader->getProg(), "uv");
    if (loc < 0) throw (std::runtime_error(std::string("Location not found in shader program for variable ") + "color"));
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (const GLvoid*)(3*sizeof(float)));

    glBindVertexArray(0);
    return vertexArrayHandle;
}

/**
 * This function will create the sphere geometry.
 * It uses the same algorithm as in Three.js SphereGeometry class:
 * http://threejs.org/docs/#Reference/Extras.Geometries/SphereGeometry
 */
GLuint createSphere(float radius, int widthSegments, int heightSegments, glm::vec3 color, int &chopperBodyTriangleCount, shader_prog* shader) {
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

    shader->use();
    shader->attributeVectorVec3("position", vertices);
    shader->attributeVectorVec3("color", colors);
    shader->attributeVectorVec3("normal", normals);

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
GLuint createCube(glm::vec3 color, shader_prog* shader) {
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

    GLuint loc = glGetAttribLocation(shader->getProg(), "position");
    if (loc < 0) throw (std::runtime_error(std::string("Location not found in shader program for variable ") + "position"));
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), (const GLvoid*)(0*sizeof(float)));


    loc = glGetAttribLocation(shader->getProg(), "color");
    if (loc < 0) throw (std::runtime_error(std::string("Location not found in shader program for variable ") + "color"));
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), (const GLvoid*)(3*sizeof(float)));

    loc = glGetAttribLocation(shader->getProg(), "normal");
    if (loc < 0) throw (std::runtime_error(std::string("Location not found in shader program for variable ") + "normal"));
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), (const GLvoid*)(6*sizeof(float)));

    glBindVertexArray(0);
    return vertexArrayHandle;
}
