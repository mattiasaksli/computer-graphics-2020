#version 400

uniform vec2 screenSize;
uniform vec3 lightPosition;
uniform sampler2D texture;
uniform sampler2D depthTexture;

in vec3 interpolatedNormal;
in vec3 interpolatedPosition;
in vec2 interpolatedUv;
in float smokeDepth;

void main(void) {

   	/**
   	 * --Task--
   	 * First get the same linear z value you got from the depth shader.
   	 */

    /** Then, in order to calculate soft shadow values follow these steps:
     *		1. Get background depth value from the depthTexture.
     *         You can use gl_FragCoord.xy that gives you screen space x and y.
     *
     *		2. Get the color from smoke texture
     *
     *		3. Subtract smoke depth from background depth and use this difference value to change the smoke texture's alpha
     *		   You could also multiply the result by a constant (like 10) to make it more visible.
     *         If you do that, you might want to take min(1.0, depthDiff * 10), otherwise the particles become too solid.
     *
     *		3. Pass sampled color and result alpha values to gl_FragColor
     */

    float sceneDepth = texture2D(depthTexture, gl_FragCoord.xy / screenSize).r;

    float depthDiff = sceneDepth - smokeDepth;
    depthDiff = min(1.0, depthDiff * 10);

    vec4 smokeColor = texture2D(texture, interpolatedUv);
    gl_FragColor = vec4(smokeColor.xyz, smokeColor.a * depthDiff);
}
