#version 400

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

in vec3 position;

void main(void) {
    gl_PointSize = 4.0; //This specifies the size of the points
    gl_Position = projectionMatrix * viewMatrix * vec4(position, 1.0);
}
