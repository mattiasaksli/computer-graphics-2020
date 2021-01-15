#version 400

in float depth;

void main(void) {
    /**
     * --Task--
     * Currently it renders the non-linear z values.
     * Make it render linear z-values, for that there are 2 options:
     * 1) Inverse the transformation done by the projection matrix.
     * 2) Interpolate a normalized camera space z from the vertex shader.
     * Choose one of those. :)
     */

    //gl_FragColor = vec4(gl_FragCoord.z, gl_FragCoord.z, gl_FragCoord.z, 1.0);
    gl_FragColor = vec4(vec3(depth), 1.0);
}
