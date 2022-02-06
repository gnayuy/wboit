#version 120

attribute vec3 vPosition;
attribute vec4 vColor;

varying vec4 color;
varying float depth;

uniform mat4 modelview;
uniform mat4 projection;

void main()
{
    color = vColor;

    vec4 eyePosition = modelview * vec4(vPosition, 1.0);

    // homogeneous space coordinates
    gl_Position = projection * eyePosition;

    // depth n eye/camera space coordinates
    depth = -(eyePosition.z);
}
