#version 400

uniform vec3 lightPosition;

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

     vec3 r = reflect(-l, n);

     vec3 litColor = interpolatedColor * (0.1 + max(0.0, dot(n, l))) + pow(max(0.0, dot(v, r)), 200.0);

    /**
     * --Task--
     * Do the same Phong's lighting/reflection model calculation that you did in Gouraud vertex shader before.
     */

    fragColor = vec4(litColor, 1.0);

}
