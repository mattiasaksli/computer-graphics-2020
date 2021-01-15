#version 400

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform vec3 lightPosition;
uniform vec3 viewerPosition;
uniform vec2 frustum;

in vec3 position;
in vec2 uv;

out vec3 interpolatedPosition;
out vec2 interpolatedUv;
out float smokeDepth;

void main(void) {
    mat4 modelViewMatrix = viewMatrix * modelMatrix;
    mat3 normalMatrix = transpose(inverse(mat3(modelViewMatrix)));

    vec4 posView = modelViewMatrix * vec4(position, 1.0);

    //If you use the normalized view space z, then this is the place to normalize it.
    smokeDepth = -posView.z;
    smokeDepth = (smokeDepth - frustum.x) / (frustum.y - frustum.x);

    interpolatedPosition = (modelViewMatrix * vec4(position, 1.0)).xyz;
    interpolatedUv = uv;

    gl_Position = projectionMatrix * posView;
}
