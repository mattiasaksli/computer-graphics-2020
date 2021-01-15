// ---------------------------- Includes -------------------------- //
#include <stdlib.h>             // C++ standard library
#include <stack>                // We use the standard C++ stack implementation to create model matrix stacks
#include <unistd.h>             // Threading
#include <stdio.h>              // Input/Output
#include <random>               // Random number generation
#include <functional>           // For objects that follow a function syntax (callable objects)
#include <GLEW/glew.h>          // OpenGL Extension Wrangler -
#include <GLFW/glfw3.h>         // Windows and input
#include <glm/glm.hpp>          // OpenGL math library
#include <glm/gtx/rotate_vector.hpp>     // Vector rotation
#include "shader_util.h"        // Utility methods to keep this file a bit shorter.
#include "bezier.h"

#define POINTS_COUNT 16
#define SPLINE_POINTS_COUNT 100
// --------------- Forward declarations ------------- //

//These functions will generate our initial point sets
void generateRandomCurvePoints();
void generateTestCurvePoints();

//Spline manipulation functions
void createSpline(std::vector<glm::vec3> points);
std::vector<glm::vec3> smoothC1(std::vector<glm::vec3> points);
std::vector<glm::vec3> smoothC2(std::vector<glm::vec3> points);
std::vector<glm::vec3> scaleSpline(std::vector<glm::vec3> points);
std::vector<glm::vec3> sampleSpline(std::vector<glm::vec3> allPoints);
void updateSpline(std::vector<glm::vec3> points);

//We need VAO-s for the spline and spline points
GLuint splineVAO, splinePointsVAO;
//Also the VBO-s for the position coordinates, because we will change those
GLuint splinePositionVBO, splinePointsPositionVBO;

//Variables to hold the point sets
std::vector<glm::vec3> testPoints;
std::vector<glm::vec3> randomPoints;
std::vector<glm::vec3> currentPoints;

int currentSmoothness = 0;

//C++ random number generator for the random point set
std::default_random_engine generator;
std::uniform_int_distribution<int> distribution(-15, 15);
auto dice = std::bind(distribution, generator);

// --- Load the shaders declared in glsl files in the project folder ---//
shader_prog shader("bezier.vert.glsl", "bezier.frag.glsl");

/**
 * This function will generate the random point set and assign it to the randomPoints variable.
 */
void generateRandomCurvePoints() {
    float y;
    float previousY = 0.0f;

    randomPoints.clear();

    for (int i = 0; i < POINTS_COUNT; i++) {
        y = std::min(10.0, std::max(-10.0, (previousY + double(dice()))));
        randomPoints.push_back(glm::vec3(i * 2 - 20, y, 0));
        previousY = y;
    }

    return;
}

/**
 * This function generates the test set and assigns it to the testPoints variable
 */
void generateTestCurvePoints() {
    testPoints.clear();
    testPoints.push_back(glm::vec3(104, -236, 0)); //p0 = d_{-2}
    testPoints.push_back(glm::vec3( 69, -131, 0)); //p1 = d_{-1}
    testPoints.push_back(glm::vec3(100, -100, 0)); //Random point for C1
    testPoints.push_back(glm::vec3(206, -166, 0)); //p3 = d_{0}
    testPoints.push_back(glm::vec3(255, -150, 0)); //Random point for C0
    testPoints.push_back(glm::vec3(205, -100, 0)); //Random point for C1
    testPoints.push_back(glm::vec3(205,  -27, 0)); //p6 = d_{1}
    testPoints.push_back(glm::vec3(225,  -50, 0)); //Random point for C0
    testPoints.push_back(glm::vec3(250, -100, 0)); //Random point for C1
    testPoints.push_back(glm::vec3(343, -131, 0)); //p9 = d_{2}
    testPoints.push_back(glm::vec3(360, -150, 0)); //Random point for C0
    testPoints.push_back(glm::vec3(320, -200, 0)); //Random point for C1
    testPoints.push_back(glm::vec3(309, -235, 0)); //p12 = d_{3}
    testPoints.push_back(glm::vec3(275, -250, 0)); //Random point for C1
    testPoints.push_back(glm::vec3(240, -269, 0)); //p14 = d_{4}
    testPoints.push_back(glm::vec3(171, -198, 0)); //p15 = d_{5}

    for (int i = 0; i < testPoints.size(); i++) {
        testPoints[i] = 0.1f * testPoints[i] + glm::vec3(-20, 17, 0);
    }

    return;
}

/**
 * This will create our spline VAO-s and VBO-s.
 */
void createSpline() {
    glGenVertexArrays(1, &splineVAO);
    glGenVertexArrays(1, &splinePointsVAO);

    glBindVertexArray(splinePointsVAO);
    splinePointsPositionVBO = shader.attributeVectorVec3("position", currentPoints);

    std::vector<glm::vec3> samplePoints = sampleSpline(currentPoints);
    glBindVertexArray(splineVAO);
    splinePositionVBO = shader.attributeVectorVec3("position", samplePoints);
}

/**
 * --Task--
 * Finish this function.
 * This function should update the corresponding VAO-s and VBO-s.
 */
void updateSpline(std::vector<glm::vec3> allPoints) {

    //Bind the splinePointsVAO, then bind the corresponding VBO
    //Use the glBufferData to send the updated control points
    glBindVertexArray(splinePointsVAO);
    glBindBuffer(GL_ARRAY_BUFFER, splinePointsPositionVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*allPoints.size()*3, &allPoints[0].x, GL_STATIC_DRAW);

    //After this we sample the new points for the spline
    std::vector<glm::vec3> samplePoints = sampleSpline(allPoints);

    //Now bind the splineVAO and the corresponding VBO
    //Use the glBufferData to send the updated spline points
    glBindVertexArray(splineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, splinePositionVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*samplePoints.size()*3, &samplePoints[0].x, GL_STATIC_DRAW);
}

/**
 * This function calls the right smoothing functions for generating correct control points for a smooth spline.
 */
std::vector<glm::vec3> smoothSpline(std::vector<glm::vec3> points) {
    std::vector<glm::vec3> smoothPoints;
    switch (currentSmoothness) {
        case 1:
            smoothPoints = smoothC1(points);
        break;
        case 2:
            smoothPoints= smoothC2(points);
        break;
        default:
            //C0 smoothing, just copy the points
            smoothPoints = std::vector<glm::vec3>();
            smoothPoints.clear();
            smoothPoints.insert(smoothPoints.end(), points.begin(), points.end());
        break;
    }

    return smoothPoints;
}

/**
 * This function creates a C0 smooth spline out of given control points.
 * Returns the points sampled along this spline.
 */
std::vector<glm::vec3> sampleSpline(std::vector<glm::vec3> allPoints) {
    std::vector<glm::vec3> points = std::vector<glm::vec3>();
    std::vector<glm::vec3> controlPoints = std::vector<glm::vec3>();
    for (int i = 0; i < allPoints.size(); i += 3) {
        controlPoints.clear();
        controlPoints.push_back(allPoints[i + 0]);
        controlPoints.push_back(allPoints[i + 1]);
        controlPoints.push_back(allPoints[i + 2]);
        controlPoints.push_back(allPoints[i + 3]);
        bezier spline = bezier(controlPoints);
        std::vector<glm::vec3> curvePoints = spline.getPoints(20);
        points.insert(points.end(), curvePoints.begin(), curvePoints.end());
    }

    return points;
}

/**
 * This function draws the spline.
 * It selects the correct VAO-s, assigns colors and calls the correct draw calls.
 */
void drawSpline() {
    glBindVertexArray(splinePointsVAO);
    shader.uniform3f("color", 1.0, 0.0, 0.0);
    glDrawArrays(GL_POINTS, 0, POINTS_COUNT);

    glBindVertexArray(splinePointsVAO);
    shader.uniform3f("color", 1.0, 0.0, 0.0);
    glDrawArrays(GL_LINE_STRIP, 0, POINTS_COUNT);

    glBindVertexArray(splineVAO);
    shader.uniform3f("color", 1.0, 1.0, 1.0);
    glDrawArrays(GL_LINE_STRIP, 0, SPLINE_POINTS_COUNT);
}

/**
 * --Task--
 * Finish this function.
 * This function should modify the control points so that the drawn spline would be C1 smooth.
 */
std::vector<glm::vec3> smoothC1(std::vector<glm::vec3> points) {
    std::vector<glm::vec3> smoothPoints = std::vector<glm::vec3>();
    smoothPoints.insert(smoothPoints.end(), points.begin(), points.end());

    //You should modify the shared control point.
    for (int i = 3; i < smoothPoints.size() - 3; i += 3) {
        smoothPoints[i] = 0.5f * (smoothPoints[i - 1] + smoothPoints[i + 1]);
    }

    return smoothPoints;
}

/**
 * --Task--
 * Finish this function.
 * This function should modify the control points so that the resulting spline is C2 smooth.
 * -Regard the initial points p3, p6, p9, ... as the tips of the triangles.
 * -For the start and end curves you can write the correct changes out.
 * -For the middle curves, create a loop that modifies the correct control points.
 */
std::vector<glm::vec3> smoothC2(std::vector<glm::vec3> points) {
    std::vector<glm::vec3> smoothPoints = std::vector<glm::vec3>();
    smoothPoints.insert(smoothPoints.end(), points.begin(), points.end());

    int len = points.size();
    //This may help: https://www.rose-hulman.edu/~finn/CCLI/Notes/day18.pdf

	//Modify points[2] and points[len - 3]
	smoothPoints[2] = 0.5f * (smoothPoints[1] + smoothPoints[3]);
	smoothPoints[len - 3] = 0.5f * (smoothPoints[len - 4] + smoothPoints[len - 2]);

	//Loop through the points that belong to the middle segments.
	for(int i = 4; i < len - 3; i += 3) {
        //Modify the correct points inside the loop
        glm::vec3 tip1 = smoothPoints[i - 1];
        glm::vec3 tip2 = smoothPoints[i + 2];

        smoothPoints[i] = 0.667f * tip1 + 0.333f * tip2;
        smoothPoints[i + 1] = 0.333f * tip1 + 0.667f * tip2;
	}

	//Loop through again and modify the points p3, p6, ... similarly as in the C1 case.
	for (int i = 3; i < smoothPoints.size() - 3; i += 3) {
        smoothPoints[i] = 0.5f * (smoothPoints[i - 1] + smoothPoints[i + 1]);
    }

    return smoothPoints;
}

/**
 * --Task--
 * Finish this function.
 * This function should scale all the control points in either X or Y (or both) axis in time (different scale each frame).
 * Do it such that the change in scale is smooth.
 */
std::vector<glm::vec3> scaleSpline(std::vector<glm::vec3> points) {
    std::vector<glm::vec3> scaledPoints;
    scaledPoints.insert(scaledPoints.end(), points.begin(), points.end());

    //Loop through the points and scale them
    //You can create a matrix, if you wish...
    //But you can also use just copy the values and multiply one coordinate with a scale factor
    //Hint: glfwGetTime() will give you the current time
    for (int i = 0; i < scaledPoints.size(); i++) {
        scaledPoints[i].x *= glm::sin((float)glfwGetTime()) * 0.5f + 1.0f;
        scaledPoints[i].y *= glm::sin((float)glfwGetTime()) * 0.5f + 1.0f;
    }

    //return scaledPoints;
    return scaledPoints; //Return the scaledPoints instead of this
}

// ---------------------------- Input -------------------------- //
// This method is called when keyboard event happens.
// Sets GLFW window should close flag to true, when escape key is pressed.
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch(key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GL_TRUE);
            break;
            case GLFW_KEY_UP: //On the "up" key we regenerate the random set
                printf("Random\n");
                generateRandomCurvePoints();
                currentPoints = randomPoints;
            break;
            case GLFW_KEY_DOWN: //On the "down" key we assign the test set
                printf("Test Set\n");
                currentPoints = testPoints;
            break;
            case GLFW_KEY_LEFT: //"Left" key decreases the smoothness
                currentSmoothness = std::max(0, currentSmoothness - 1);
                printf("C%d Smooth\n", currentSmoothness);
            break;
            case GLFW_KEY_RIGHT: //"Right" key increases the smoothness
                currentSmoothness = std::min(currentSmoothness + 1, 2);
                printf("C%d Smooth\n", currentSmoothness);
            break;
        }
    }
}

// ---------------------------- Main -------------------------- //
int main(int argc, char *argv[]) {
    GLFWwindow *win;

    if (!glfwInit()) {
        exit (EXIT_FAILURE);
    }

    win = glfwCreateWindow(640, 480, "Bezier Spline!", NULL, NULL);

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

    const GLubyte* renderer = glGetString (GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString (GL_VERSION); // version as a string
    printf ("Renderer: %s\n", renderer);
    printf ("OpenGL version supported %s\n", version);

    glfwSetKeyCallback(win, key_callback);

    shader.use();

    //First generate the sets
    generateTestCurvePoints();
    generateRandomCurvePoints();
    currentPoints = randomPoints; //Assign random set as the initial set

    //Then generate the spline (VAO-s, VBO-s).
    createSpline();

    //Projection and view, we don't need a model matrix here
    glm::mat4 projection = glm::perspective(glm::radians(80.0f), 4.0f / 3.0f, 0.1f, 100.f);
    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0, 0.0, 30.0), //Position
        glm::vec3(0.0, 0.0, 0.0),  //LookAt
        glm::vec3(0.0, 1.0, 0.0)   //Up
    );
    shader.uniformMatrix4fv("projectionMatrix", projection);
    shader.uniformMatrix4fv("viewMatrix", view);


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE); //Because we are drawing points, we need to enable this
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    while (!glfwWindowShouldClose(win)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //We take the currentPoints set of control points, then:
        //-Pass it through smoothing
        //-Pass the result through rotation
        //-Pass the result to the spline data update method
        updateSpline(scaleSpline(smoothSpline(currentPoints)));

        //Then we draw the spline
        drawSpline();

        glfwSwapBuffers(win);
        glfwPollEvents();
        usleep(1000);
    }

    glfwTerminate();
    exit(EXIT_SUCCESS);
    return 0;
}
