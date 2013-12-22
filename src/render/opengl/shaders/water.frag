void main(void)
{
    gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0);

    float yoffset = GROUND_OFFSET - waterHeight;
    vec3 camera = vec3(cameraLocation.x, max(cameraLocation.y + yoffset, 0.0), cameraLocation.z);
    vec3 location = vec3(unprojected.x, max(unprojected.y + yoffset, 0.0), unprojected.z);
    vec3 x = vec3(0.0, Rg + camera.y * WORLD_SCALING, 0.0);
    vec3 v = normalize(location - camera);
    vec3 s = normalize(sunDirection * SUN_DISTANCE_SCALED - x);

    if (v.y == 0.0)
    {
        v.y = -0.000001;
    }

    float r = length(x);
    float mu = dot(x, v) / r;
    float t = length(location - camera) * WORLD_SCALING;

    vec3 attenuation;
    vec3 inscattering = _getInscatterColor(x, t, v, s, r, mu, attenuation);

    gl_FragColor = gl_FragColor * vec4(attenuation, 0.0) + vec4(inscattering, 0.0);

    gl_FragColor = _toneMappingUncharted(gl_FragColor, 2.0);
}
