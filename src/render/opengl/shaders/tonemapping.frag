float _uncharted2Tonemap(float x)
{
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;

    return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

vec4 _toneMappingUncharted(vec4 color, float exposure)
{
    float W = 11.2;
    float white_scale = 1.0 / _uncharted2Tonemap(W);
    vec4 result;

    result.r = pow(_uncharted2Tonemap(color.r * exposure) * white_scale, 1.0 / 2.2);
    result.g = pow(_uncharted2Tonemap(color.g * exposure) * white_scale, 1.0 / 2.2);
    result.b = pow(_uncharted2Tonemap(color.b * exposure) * white_scale, 1.0 / 2.2);
    result.a = 1.0;

    return result;
}
