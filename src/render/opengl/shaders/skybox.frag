void main(void)
{
    float yoffset = GROUND_OFFSET - waterHeight;
    vec3 camera = vec3(cameraLocation.x, max(cameraLocation.y + yoffset, 0.0), cameraLocation.z);
    vec3 location = vec3(unprojected.x, max(unprojected.y + yoffset, 0.0), unprojected.z);
    vec3 x = vec3(0.0, Rg + camera.y * WORLD_SCALING, 0.0);
    vec3 v = normalize(location - camera);
    vec3 s = normalize(sunDirection * SUN_DISTANCE_SCALED - x);

    float r = length(x);
    float mu = dot(x, v) / r;
    float t = -r * mu - sqrt(r * r * (mu * mu - 1.0) + Rg * Rg);

    vec4 sunTransmittance = _sunTransmittance(v, s, r, mu, sunRadius);
    vec3 attenuation;
    vec3 inscattering = _getInscatterColor(x, t, v, s, r, mu, attenuation);

    gl_FragColor = applyToneMapping(sunTransmittance + vec4(inscattering, 0.0));
}
