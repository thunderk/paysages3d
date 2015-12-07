out vec4 final_color;

void main(void)
{
    final_color = getSkyColor(cameraLocation, unprojected - cameraLocation);
    final_color = applyToneMapping(final_color);
}
