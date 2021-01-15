
#include <string>
#include <vector>
#include <GLEW/glew.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

class bezier {
private:
    std::vector<glm::vec3> points;
    float b3p0(float t, float p);
    float b3p1(float t, float p);
    float b3p2(float t, float p);
    float b3p3(float t, float p);
    float b3(float t, float p0, float p1, float p2, float p3);
public:
    bezier(std::vector<glm::vec3> points);
    std::vector<glm::vec3> getPoints(int count);
    glm::vec3 sample(float t);
};
