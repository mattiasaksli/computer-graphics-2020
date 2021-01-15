#version 400

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform vec3 lightPosition;

in vec3 position;
in vec3 color;
in vec3 normal;

out vec3 interpolatedNormal;
out vec3 interpolatedPosition;
out vec3 interpolatedColor;

void main(void) {
    mat4 modelViewMatrix = viewMatrix * modelMatrix;
    mat3 normalMatrix = transpose(inverse(mat3(modelViewMatrix)));

    interpolatedNormal = normalize(normalMatrix * normal);
    interpolatedPosition = (modelViewMatrix * vec4(position, 1.0)).xyz;
    interpolatedColor = color;

    gl_Position = projectionMatrix * vec4(interpolatedPosition, 1.0);
}
