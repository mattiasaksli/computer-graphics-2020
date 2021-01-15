/**
 * This is ported from Three.js library
 * https://threejsdoc.appspot.com/doc/three.js/src.source/extras/core/Shape.js.html
 */
#include <stdio.h>
#include "bezier.h"


float bezier::b3p0(float t, float p ) {
		float k = 1 - t;
		return k * k * k * p;
};

float bezier::b3p1(float t, float p) {
		float k = 1 - t;
		return 3 * k * k * t * p;
};

float bezier::b3p2(float t, float p) {
		float k = 1 - t;
		return 3 * k * t * t * p;
};

float bezier::b3p3(float t, float p) {

		return t * t * t * p;
};

float bezier::b3(float t, float p0, float p1, float p2, float p3) {

		return b3p0(t, p0) + b3p1(t, p1) + b3p2(t, p2) + b3p3(t, p3);
};

bezier::bezier(std::vector<glm::vec3> points) {
    if (points.size() != 4) {
        printf("Cubic Bezier will need 4 control points, you have: %d!\n", points.size());
    }
    this->points = std::vector<glm::vec3>(points);
};

std::vector<glm::vec3> bezier::getPoints(int count) {
    float step = 1.0 / float(count);
    std::vector<glm::vec3> samples = std::vector<glm::vec3>();
    for (float t = 0.0; t <= 1.0; t += step) {
        samples.push_back(this->sample(t));
    }

    return samples;
}

glm::vec3 bezier::sample(float t) {
    glm::vec3 point = glm::vec3(
      this->b3(t, this->points[0].x, this->points[1].x, this->points[2].x, this->points[3].x),
      this->b3(t, this->points[0].y, this->points[1].y, this->points[2].y, this->points[3].y),
      this->b3(t, this->points[0].z, this->points[1].z, this->points[2].z, this->points[3].z)
    );

	return point;
}
