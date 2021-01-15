#version 400

uniform vec3 lightPosition;
uniform vec3 viewerPosition;

in vec3 interpolatedColor;

// This is the output variable of the fragment shader, we assign the pixel color there
out vec4 fragColor;

void main(void) {
    fragColor = vec4(interpolatedColor, 1.0);
}
