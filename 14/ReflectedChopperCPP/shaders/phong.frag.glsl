#version 400

uniform vec3 lightPosition;

in vec3 interpolatedColor;
in vec3 interpolatedNormal;
in vec3 interpolatedPosition;

void main(void) {
    vec3 color = interpolatedColor;
    vec3 viewerPosition = vec3(0.0);

    vec3 n = normalize(interpolatedNormal);
    vec3 l = normalize(lightPosition - interpolatedPosition);
    vec3 v = normalize(viewerPosition - interpolatedPosition);
    vec3 r = normalize(reflect(-v, n));

    vec3 colorOut = (0.1f + max(0.0f, dot(l, n))) * color + pow(max(0.0f, dot(l, r)), 1000.0f);

    gl_FragColor = vec4(colorOut, 1.0f);
}
