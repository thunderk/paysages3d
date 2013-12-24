uniform vec4 waterColor;

void main(void)
{
    gl_FragColor = waterColor;

    gl_FragColor = applyAerialPerspective(gl_FragColor);

    gl_FragColor = applyToneMapping(gl_FragColor);

    gl_FragColor.a = distanceFadeout();
}
