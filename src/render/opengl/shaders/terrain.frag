uniform sampler2D groundTexture;
in vec2 texcoord;
out vec4 final_color;

void main(void)
{
    final_color = vec4(texture(groundTexture, texcoord).rgb * 5.0, 1.0);

    final_color = applyAerialPerspective(final_color);

    final_color = applyToneMapping(final_color);

    final_color = applyMouseTracking(unprojected, final_color);

    final_color.a = distanceFadeout();
}
