in vec2 texcoord;
uniform sampler2D impostorTexture;
out vec4 final_color;

void main(void)
{
    final_color = texture(impostorTexture, texcoord);
    float alpha = final_color.a;

    final_color = applyAerialPerspective(final_color);

    final_color = applyToneMapping(final_color);

    final_color = applyMouseTracking(unprojected, final_color);
    final_color.a = alpha;
}
