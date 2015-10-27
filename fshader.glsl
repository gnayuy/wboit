varying vec4 color;
varying float depth;

void main()
{
    float alpha = color.a;

    // the object lies between -40 and -60 z coordinates
    float weight = pow(alpha + 0.01, 4.0) + max(0.01, min(3000.0, 0.3 / (0.00001 + pow(abs(depth) / 200.0, 4.0))));

    // RGBA32F texture (accumulation), a synonym of gl_FragColor attached to GL_COLOR_ATTACHMENT0
    gl_FragData[0] = vec4(color.rgb * alpha * weight, alpha);

    // R32F texture (revealage), attached to GL_COLOR_ATTACHMENT1
    gl_FragData[1].r = alpha * weight;

}
