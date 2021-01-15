#version 400

//uniform mat4 projectionMatrix;
//uniform mat4 viewMatrix;
//uniform mat4 modelMatrix;
uniform mat4 clipMatrix;

in vec3 position;
in vec3 color;
out vec3 interpolatedColor;

void main(void) {
    interpolatedColor = color;
    //gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
    gl_Position = clipMatrix * vec4(position, 1.0);
}
