const float GROUND_OFFSET = 0.5;
const float Rg = 6360.0;
const float Rt = 6420.0;
const float RL = 6421.0;
const float exposure = 0.4;
const float ISun = 100.0;
const float AVERAGE_GROUND_REFLECTANCE = 0.1;
const float HR = 8.0;
const vec3 betaR = vec3(5.8e-3, 1.35e-2, 3.31e-2);
const float HM = 1.2;
const vec3 betaMSca = vec3(4e-3);
const vec3 betaMEx = vec3(4e-3 / 0.9);
const float mieG = 0.8;
const float SPHERE_SIZE = 20000.0;
const float WORLD_SCALING = 0.05;
const float SUN_DISTANCE = 149597870.0;
const float SUN_DISTANCE_SCALED = (SUN_DISTANCE / WORLD_SCALING);
const float SUN_RADIUS = 6.955e5;
const float SUN_RADIUS_SCALED = (SUN_RADIUS / WORLD_SCALING);
const float M_PI = 3.141592657;

const int RES_MU = 128;
const int RES_MU_S = 32;
const int RES_R = 32;
const int RES_NU = 8;

uniform float waterHeight;
uniform vec3 cameraLocation;
uniform vec3 sunDirection;
uniform vec4 sunColor;
const float sunRadius = 1.0; // TODO -> uniform

varying vec3 unprojected;

uniform sampler2D transmittanceTexture;
uniform sampler3D inscatterTexture;

vec4 texture4D(sampler3D tex, float r, float mu, float muS, float nu)
{
    if (r < Rg + 0.00000001) r = Rg + 0.00000001;
    float H = sqrt(Rt * Rt - Rg * Rg);
    float rho = sqrt(r * r - Rg * Rg);
    float rmu = r * mu;
    float delta = rmu * rmu - r * r + Rg * Rg;
    vec4 cst = (rmu < 0.0 && delta > 0.0) ? vec4(1.0, 0.0, 0.0, 0.5 - 0.5 / float(RES_MU)) : vec4(-1.0, H * H, H, 0.5 + 0.5 / float(RES_MU));
    float uR = 0.5 / float(RES_R) + rho / H * (1.0 - 1.0 / float(RES_R));
    float uMu = cst.a + (rmu * cst.r + sqrt(delta + cst.g)) / (rho + cst.b) * (0.5 - 1.0 / float(RES_MU));
    float uMuS = 0.5 / float(RES_MU_S) + (atan(max(muS, -0.1975) * tan(1.26 * 1.1)) / 1.1 + (1.0 - 0.26)) * 0.5 * (1.0 - 1.0 / float(RES_MU_S));

    float sr = 1.0 / float(RES_R);
    int br = int(floor(uR / sr));
    vec4 r1 = texture3D(tex, vec3(uMu, uMuS, float(br) * sr + nu * sr));
    vec4 r2 = texture3D(tex, vec3(uMu, uMuS, float(br + 1) * sr + nu * sr));
    return mix(r1, r2, (uR - float(br) * sr) / sr);
}

float _limit(float r, float mu)
{
    float dout = -r * mu + sqrt(r * r * (mu * mu - 1.0) + RL * RL);
    float delta2 = r * r * (mu * mu - 1.0) + Rg * Rg;
    if (delta2 >= 0.0)
    {
        float din = -r * mu - sqrt(delta2);
        if (din >= 0.0) {
            dout = min(dout, din);
        }
    }
    return dout;
}

vec2 _getTransmittanceUV(float r, float mu)
{
    if (r < Rg + 0.00000001) r = Rg + 0.00000001;
    float dr = (r - Rg) / (Rt - Rg);
    return vec2(atan((mu + 0.15) / (1.0 + 0.15) * tan(1.5)) / 1.5, sqrt(dr));
}

vec4 _transmittance(float r, float mu)
{
    vec2 uv = _getTransmittanceUV(r, mu);
    return texture2D(transmittanceTexture, uv);
}

vec4 _transmittanceWithShadow(float r, float mu)
{
    return mu < -sqrt(1.0 - (Rg / r) * (Rg / r)) ? vec4(0.0) : _transmittance(r, mu);
}

vec4 _sunTransmittance(vec3 v, vec3 s, float r, float mu, float radius)
{
    vec4 transmittance = r <= Rt ? _transmittanceWithShadow(r, mu) : vec4(1.0); /* T(x,xo) */
    float d = _limit(r, mu);
    radius *= (1.0 + 10.0 * d / Rt); /* Inflating due to lens effect near horizon */
    float isun = step(cos(radius * M_PI / 180.0), dot(v, s)) * ISun; /* Lsun */
    transmittance.r *= isun;
    transmittance.g *= isun;
    transmittance.b *= isun;
    transmittance.a = 1.0;
    return transmittance; /* Eq (9) */
}

float phaseFunctionR(float mu) {
    return (3.0 / (16.0 * M_PI)) * (1.0 + mu * mu);
}

float phaseFunctionM(float mu) {
    return 1.5 * 1.0 / (4.0 * M_PI) * (1.0 - mieG*mieG) * pow(1.0 + (mieG*mieG) - 2.0*mieG*mu, -3.0/2.0) * (1.0 + mu * mu) / (2.0 + mieG*mieG);
}

float opticalDepth(float H, float r, float mu, float d) {
    float a = sqrt((0.5/H)*r);
    vec2 a01 = a*vec2(mu, mu + d / r);
    vec2 a01s = sign(a01);
    vec2 a01sq = a01*a01;
    float x = a01s.y > a01s.x ? exp(a01sq.x) : 0.0;
    vec2 y = a01s / (2.3193*abs(a01) + sqrt(1.52*a01sq + 4.0)) * vec2(1.0, exp(-d/H*(d/(2.0*r)+mu)));
    return sqrt((6.2831*H)*r) * exp((Rg-r)/H) * (x + dot(y, vec2(1.0, -1.0)));
}

vec3 analyticTransmittance(float r, float mu, float d) {
    return exp(- betaR * opticalDepth(HR, r, mu, d) - betaMEx * opticalDepth(HM, r, mu, d));
}

vec3 getMie(vec4 rayMie) { // rayMie.rgb=C*, rayMie.w=Cm,r
    return rayMie.rgb * rayMie.w / max(rayMie.r, 1e-4) * (betaR.r / betaR);
}

vec3 _getInscatterColor(inout vec3 x, inout float t, vec3 v, vec3 s, out float r, out float mu, out vec3 attenuation) {
    vec3 result;
    r = length(x);
    mu = dot(x, v) / r;
    float d = -r * mu - sqrt(r * r * (mu * mu - 1.0) + Rt * Rt);
    if (d > 0.0) { // if x in space and ray intersects atmosphere
        // move x to nearest intersection of ray with top atmosphere boundary
        x += d * v;
        t -= d;
        mu = (r * mu + d) / Rt;
        r = Rt;
    }
    if (r <= Rt) { // if ray intersects atmosphere
        float nu = dot(v, s);
        float muS = dot(x, s) / r;
        float phaseR = phaseFunctionR(nu);
        float phaseM = phaseFunctionM(nu);
        vec4 inscatter = max(texture4D(inscatterTexture, r, mu, muS, nu), 0.0);
        if (t > 0.0) {
            vec3 x0 = x + t * v;
            float r0 = length(x0);
            float rMu0 = dot(x0, v);
            float mu0 = rMu0 / r0;
            float muS0 = dot(x0, s) / r0;
            // avoids imprecision problems in transmittance computations based on textures
            attenuation = analyticTransmittance(r, mu, t);
            if (r0 > Rg + 0.001) {
                // computes S[L]-T(x,x0)S[L]|x0
                inscatter = max(inscatter - attenuation.rgbr * texture4D(inscatterTexture, r0, mu0, muS0, nu), 0.0);
                // avoids imprecision problems near horizon by interpolating between two points above and below horizon
                const float EPS = 0.02;
                float muHoriz = -sqrt(1.0 - (Rg / r) * (Rg / r));
                if (abs(mu - muHoriz) < EPS) {
                    float a = ((mu - muHoriz) + EPS) / (2.0 * EPS);

                    mu = muHoriz - EPS;
                    r0 = sqrt(r * r + t * t + 2.0 * r * t * mu);
                    mu0 = (r * mu + t) / r0;
                    vec4 inScatter0 = texture4D(inscatterTexture, r, mu, muS, nu);
                    vec4 inScatter1 = texture4D(inscatterTexture, r0, mu0, muS0, nu);
                    vec4 inScatterA = max(inScatter0 - attenuation.rgbr * inScatter1, 0.0);

                    mu = muHoriz + EPS;
                    r0 = sqrt(r * r + t * t + 2.0 * r * t * mu);
                    mu0 = (r * mu + t) / r0;
                    inScatter0 = texture4D(inscatterTexture, r, mu, muS, nu);
                    inScatter1 = texture4D(inscatterTexture, r0, mu0, muS0, nu);
                    vec4 inScatterB = max(inScatter0 - attenuation.rgbr * inScatter1, 0.0);

                    inscatter = mix(inScatterA, inScatterB, a);
                }
            }
        }
        // avoids imprecision problems in Mie scattering when sun is below horizon
        inscatter.w *= smoothstep(0.00, 0.02, muS);
        result = max(inscatter.rgb * phaseR + getMie(inscatter) * phaseM, 0.0);
    } else { // x in space and ray looking in space
        result = vec3(0.0);
    }
    return result * ISun;
}

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

    gl_FragColor = _toneMappingUncharted(sunTransmittance + vec4(inscattering, 0.0), 2.0);
}
