#version 400

in vec3 interpolatedColor;
out vec4 fragColor;

void main(void) {
    fragColor = vec4(interpolatedColor, 1.0);
}
