void main(void)
{
    gl_FragColor = getSkyColor(cameraLocation, unprojected - cameraLocation);
    gl_FragColor = applyToneMapping(gl_FragColor);
}
