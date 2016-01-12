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
        // TODO parametrized texture scaling
        float factor;
        if (height * step_height <= detail) {
            factor = (height - detail) / (height - height * step_height);
        } else {
            factor = 1.0;
        }
        normal += (texture(simplexSampler, location * scaling / 15.0).xyz * 2.0 - 1.0) * factor;
        scaling *= step_scaling;
        height *= step_height;
    }
    return normalize(normal);
}
