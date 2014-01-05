uniform vec4 waterColor;
uniform sampler2D simplexSampler;
uniform float waterReflection;

vec4 applyLighting(vec3 location, vec3 normal, vec4 color, float shininess)
{
    // TEMP phong lighting implementation for testing
    vec3 N = normalize(normal);
    vec3 L = sunDirection;
    vec3 E = normalize(cameraLocation - location);
    vec3 R = normalize(-reflect(L, N));

    //calculate Ambient Term:
    vec4 Iamb = vec4(0.1, 0.1, 0.1, 1.0);

    //calculate Diffuse Term:
    vec4 Idiff = vec4(3.0, 3.0, 3.0, 1.0) * color * max(dot(N, L), 0.0);

    // calculate Specular Term:
    vec4 Ispec = vec4(3.0, 3.0, 3.0, 1.0) * pow(max(dot(R,E),0.0),0.3*shininess);

    // write Total Color:
    return Iamb + Idiff + Ispec;
}

void main(void)
{
    vec3 normal = vec3(0.0, 0.0, 0.0);
    for (float scaling = 1.0; scaling < 400.0; scaling *= 1.5)
    {
        normal += texture2D(simplexSampler, unprojected.xz * 0.01 * scaling).xyz;
    }
    normal = normalize(normal);

    gl_FragColor = applyLighting(unprojected, normal, waterColor, 100.0);

    gl_FragColor += getSkyColor(unprojected, reflect(unprojected - cameraLocation, normal)) * waterReflection;

    gl_FragColor = applyAerialPerspective(gl_FragColor);

    gl_FragColor = applyToneMapping(gl_FragColor);

    gl_FragColor.a = distanceFadeout();
}
