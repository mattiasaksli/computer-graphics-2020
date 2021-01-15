#version 400

uniform vec3 lightPosition;
uniform float gamma = 2.2;

/**
 * --Task--
 * Receive the texture uniform.
 * - Find out what type is it
 * - You can use any name for it, because there is only one texture at a time
 */
 uniform sampler2D colorTexture;

in vec3 interpolatedNormal;
in vec3 interpolatedPosition;
in vec2 interpolatedUv;


void main(void) {
    vec3 viewerPosition = vec3(0.0);

    /**
    * --Task--
    * Copy your Phong or Blinn lighting model here.
    * Use the diffuse and ambient color from the texture - sample it from the correct coordinates.
    */
    vec3 color = texture2D(colorTexture, interpolatedUv).rgb;

    vec3 vertexPosition = interpolatedPosition;

    vec3 n = interpolatedNormal;

    vec3 v = normalize(-vertexPosition);

    vec3 l = normalize(lightPosition - vertexPosition);

    vec3 h = (l + v) / length(l + v);

    vec3 gammaColor = pow(color, vec3(gamma, gamma, gamma));
    vec3 litColor = gammaColor * (0.1 + max(0.0, dot(n, l))) + pow(max(0.0, dot(n, h)), 200.0);
    litColor = pow(litColor, vec3(1.0 / gamma, 1.0 / gamma, 1.0 / gamma));

    gl_FragColor = vec4(litColor, 1.0);
}
