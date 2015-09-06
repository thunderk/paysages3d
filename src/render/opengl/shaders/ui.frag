uniform vec3 mouseProjection;

vec4 applyMouseTracking(vec3 location, vec4 color)
{
    float dist = length(location - mouseProjection);
    if (dist > 2.0)
    {
        return color;
    }
    else
    {
        return mix(color, vec4(1.0, 0.0, 0.0, 1.0), 1.0 - dist / 2.0);
    }
}
