uniform float viewDistance;

float distanceFadeout()
{
    vec3 camera = vec3(cameraLocation.x, 0.0, cameraLocation.z);
    vec3 location = vec3(unprojected.x, 0.0, unprojected.z);
    return mix(1.0, 0.0, clamp((length(location - camera) - viewDistance * 0.8) / (viewDistance * 0.2), 0.0, 1.0));
}
