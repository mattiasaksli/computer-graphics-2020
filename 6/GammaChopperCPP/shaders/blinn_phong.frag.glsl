#version 400

uniform vec3 lightPosition;
uniform float gamma = 2.2;

in vec3 interpolatedNormal;
in vec3 interpolatedPosition;
in vec3 interpolatedColor;

// This is the output variable of the fragment shader
// We configure this to be "output 0" for our fragment shader
// in the main program using glBindFragDataLocation
out vec4 fragColor;

void main(void) {
    vec3 vertexPosition = interpolatedPosition;

    vec3 n = normalize(interpolatedNormal);

    vec3 v = normalize(-vertexPosition);

    vec3 l = normalize(lightPosition - vertexPosition);

    vec3 h = (l + v) / length(l + v);

    vec3 gammaColor = pow(interpolatedColor, vec3(gamma, gamma, gamma));
    vec3 litColor = gammaColor * (0.1 + max(0.0, dot(n, l))) + pow(max(0.0, dot(n, h)), 200.0);
    litColor = pow(litColor, vec3(1.0 / gamma, 1.0 / gamma, 1.0 / gamma));

    fragColor = vec4(litColor, 1.0);
}
