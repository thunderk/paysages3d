uniform float exposure;

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

/*vec4 applyToneMapping(vec4 color)
{
    float e = mix(exposure, exposure * 3.0, clamp(-sunDirection.y * 10.0, 0.0, 1.0));
    return vec4(((color * e) / (1.0 + color * e)).rgb, 1.0);
}*/

vec4 applyToneMapping(vec4 color)
{
    float e = mix(exposure, exposure * 3.0, clamp(-sunDirection.y * 10.0, 0.0, 1.0));
    float W = 11.2;
    float white_scale = 1.0 / _uncharted2Tonemap(W);
    vec4 result;

    result.r = pow(_uncharted2Tonemap(color.r * e) * white_scale, 1.0 / 2.2);
    result.g = pow(_uncharted2Tonemap(color.g * e) * white_scale, 1.0 / 2.2);
    result.b = pow(_uncharted2Tonemap(color.b * e) * white_scale, 1.0 / 2.2);
    result.a = 1.0;

    return result;
}
