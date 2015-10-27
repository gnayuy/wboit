attribute vec3 vPosition;
attribute vec4 vColor;

varying vec4 color;
varying float depth;

uniform mat4 modelview;
uniform mat4 projection;

void main()
{
    color = vColor;

    gl_Position = projection * modelview * vec4(vPosition,1.0);

    // eye coord
    depth = -(modelview * vec4(vPosition,1.0)).z;
}
