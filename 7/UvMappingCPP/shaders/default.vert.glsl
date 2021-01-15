#version 400

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform vec3 lightPosition;

in vec3 position;
in vec3 normal;
in vec3 uv;

void main(void) {
    mat4 modelViewMatrix = viewMatrix * modelMatrix;

    gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0);
}
