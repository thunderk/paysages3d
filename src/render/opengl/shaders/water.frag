uniform vec4 waterMaterialColor;
uniform float waterMaterialReflection;
uniform float waterMaterialShininess;
uniform float waterMaterialHardness;
uniform float waterReflection;
out vec4 final_color;

void main(void)
{
    vec3 normal = noiseNormal2d(unprojected.xz, 0.001);

    final_color = applyLighting(unprojected, normal, waterMaterialColor, waterMaterialReflection, waterMaterialShininess, waterMaterialHardness);

    vec3 reflected = reflect(unprojected - cameraLocation, normal);
    reflected.y = max(reflected.y, 0.0);
    final_color += getSkyColor(unprojected, reflected) * waterReflection;

    final_color = applyAerialPerspective(final_color);

    final_color = applyToneMapping(final_color);

    final_color = applyMouseTracking(unprojected, final_color);

    final_color.a = distanceFadeout();
}
