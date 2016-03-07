#version 120

uniform sampler2D accumTexture; // sum(rgb * a, a)
uniform sampler2D revealageTexture; // prod(1 - a)

varying vec2 vTexcoord;

void main(void)
{
    vec4 accum = texture2D(accumTexture, vTexcoord);
    float alpha = accum.a;
    accum.a = texture2D(revealageTexture, vTexcoord).r; // GL_RED
    if (alpha >= 1.0) {
        discard;
    }
    gl_FragColor = vec4(accum.rgb / clamp(accum.a, 1e-4, 5e4), alpha);
}
