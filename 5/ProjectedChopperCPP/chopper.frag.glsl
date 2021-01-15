#version 400

// This is the output variable of the fragment shader
// We configure this to be "output 0" for our fragment shader
// in the main program using glBindFragDataLocation
in vec3 interpolatedColor;
out vec4 fragColor;

void main(void) {
    fragColor = vec4(interpolatedColor, 1.0);
}
