#version 400

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform vec3 lightPosition;
uniform vec3 viewerPosition;

in vec3 position;
in vec3 normal;
in vec2 uv;

out vec3 interpolatedNormal;
out vec3 interpolatedPosition;
out vec2 interpolatedUv;

void main(void) {
    mat4 modelViewMatrix = viewMatrix * modelMatrix;
    mat3 normalMatrix = transpose(inverse(mat3(modelViewMatrix)));

    gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0);

    interpolatedNormal = normalize(normalMatrix * normal);
    interpolatedPosition = (modelViewMatrix * vec4(position, 1.0)).xyz;
    interpolatedUv = uv;
}
