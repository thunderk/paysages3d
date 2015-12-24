uniform sampler2D simplexSampler;

vec3 noiseNormal2d(float[4] data, vec2 location, float detail)
{
    vec3 normal = vec3(0.0, 0.0, 0.0);
    float scaling = data[0];
    float height = data[1];
    float step_scaling = data[2];
    float step_height = data[3];
    while (height > detail)
    {
        // TODO offsets
        // TODO parametrized texture scaling (0.01)
        normal += texture(simplexSampler, location * 0.01 * scaling).xyz;
        scaling *= step_scaling;
        height *= step_height;
    }
    return normalize(normal);
}
