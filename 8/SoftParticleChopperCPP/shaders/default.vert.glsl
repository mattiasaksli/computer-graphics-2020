#version 400

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform vec3 lightPosition;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;

out vec3 interpolatedNormal;
out vec3 interpolatedPosition;
out vec3 interpolatedColor;

void main(void) {
    mat4 modelViewMatrix = viewMatrix * modelMatrix;
    mat3 normalMatrix = transpose(inverse(mat3(modelViewMatrix)));

    gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0);

    interpolatedNormal = normalize(normalMatrix * normal);
    interpolatedPosition = (modelViewMatrix * vec4(position, 1.0)).xyz;
    interpolatedColor = color;
}
