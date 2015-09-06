uniform vec4 waterColor;
uniform float waterReflection;

void main(void)
{
    vec3 normal = noiseNormal2d(unprojected.xz, 0.001);

    gl_FragColor = applyLighting(unprojected, normal, waterColor, 16.0);

    vec3 reflected = reflect(unprojected - cameraLocation, normal);
    reflected.y = max(reflected.y, 0.0);
    gl_FragColor += getSkyColor(unprojected, reflected) * waterReflection;

    gl_FragColor = applyAerialPerspective(gl_FragColor);

    gl_FragColor = applyToneMapping(gl_FragColor);

    gl_FragColor = applyMouseTracking(unprojected, gl_FragColor);

    gl_FragColor.a = distanceFadeout();
}
