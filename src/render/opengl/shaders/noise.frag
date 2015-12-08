uniform float noiseInitScaling;
uniform float noiseInitHeight;
uniform float noiseStepScaling;
uniform float noiseStepHeight;
uniform sampler2D simplexSampler;

vec3 noiseNormal2d(vec2 location, float detail)
{
    vec3 normal = vec3(0.0, 0.0, 0.0);
    for (float scaling = 1.0; scaling < 400.0; scaling *= 1.5)
    {
        normal += texture(simplexSampler, location * 0.01 * scaling).xyz;
    }
    return normalize(normal);
}
