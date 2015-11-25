varying vec2 texcoord;
uniform sampler2D impostorTexture;

void main(void)
{
    gl_FragColor = texture2D(impostorTexture, texcoord);
    float alpha = gl_FragColor.a;

    gl_FragColor = applyAerialPerspective(gl_FragColor);

    gl_FragColor = applyToneMapping(gl_FragColor);

    gl_FragColor = applyMouseTracking(unprojected, gl_FragColor);
    gl_FragColor.a = alpha;
}
