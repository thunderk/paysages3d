uniform vec4 waterColor;
uniform sampler2D simplexSampler;

void main(void)
{
    //gl_FragColor = waterColor;
    gl_FragColor = texture2D(simplexSampler, unprojected.xz * 0.01);

    gl_FragColor = applyAerialPerspective(gl_FragColor);

    gl_FragColor = applyToneMapping(gl_FragColor);

    gl_FragColor.a = distanceFadeout();
}
