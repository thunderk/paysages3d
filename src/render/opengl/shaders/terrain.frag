uniform sampler2D groundTexture;
varying vec2 texcoord;

void main(void)
{
    gl_FragColor = texture2D(groundTexture, texcoord);

    gl_FragColor = applyAerialPerspective(gl_FragColor);

    gl_FragColor = applyToneMapping(gl_FragColor);

    gl_FragColor.a = distanceFadeout();
}
