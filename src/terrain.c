#include <stdlib.h>
#include <math.h>

#include "shared/types.h"
#include "shared/functions.h"
#include "shared/globals.h"
#include "shared/constants.h"

#include "water.h"

#define MAX_TEXTURES 10
#define MAX_MODIFIERS 50

typedef struct {
    Texture* tex;
    double subsurf_scale;
    Zone* zone;
    double border_scaling;
} TerrainTexture;

static NoiseGenerator* _noise_height;
static NoiseGenerator* _noise_texture_borders;
static double _max_height = 1.0;
static double _base_chunk_size = 1.0, _visible_chunk_size = 1.0;
static int _texture_count = 0;
static TerrainTexture _textures[MAX_TEXTURES];
static int _modifiers_count = 0;
static HeightModifier* _modifiers[MAX_MODIFIERS];

void terrainSave(FILE* f)
{
    noiseSave(_noise_height, f);
    noiseSave(_noise_texture_borders, f);

    toolsSaveDouble(f, _max_height);
    toolsSaveDouble(f, _base_chunk_size);
    toolsSaveDouble(f, _visible_chunk_size);

    /* TODO Textures */
    /* TODO Modifiers */
}

void terrainLoad(FILE* f)
{
    noiseLoad(_noise_height, f);
    noiseLoad(_noise_texture_borders, f);

    _max_height = toolsLoadDouble(f);
    _base_chunk_size = toolsLoadDouble(f);
    _visible_chunk_size = toolsLoadDouble(f);
}

void terrainInit()
{
    _noise_height = noiseCreateGenerator();
    _max_height = noiseGetMaxValue(_noise_height);

    _noise_texture_borders = noiseCreateGenerator();
    noiseGenerateBaseNoise(_noise_texture_borders, 100);
    noiseAddLevelsSimple(_noise_texture_borders, 10, 1.0, 1.0);
    noiseNormalizeHeight(_noise_texture_borders, 0.0, 1.0, 0);
}

NoiseGenerator* terrainGetNoiseGenerator()
{
    return _noise_height;
}

void terrainSetNoiseGenerator(NoiseGenerator* generator)
{
    noiseCopy(generator, _noise_height);
    _max_height = noiseGetMaxValue(_noise_height);
    /* FIXME Max height depends on modifiers*/
}

void terrainAddTexture(Texture* tex, double subsurf_scale, Zone* zone, double border_scaling)
{
    TerrainTexture ttex;
    if (_texture_count < MAX_TEXTURES)
    {
        ttex.tex = tex;
        ttex.subsurf_scale = subsurf_scale;
        ttex.zone = zone;
        ttex.border_scaling = border_scaling;

        _textures[_texture_count++] = ttex;
    }
}

void terrainAddModifier(HeightModifier* modifier)
{
    if (_modifiers_count < MAX_MODIFIERS)
    {
        _modifiers[_modifiers_count++] = modifier;
    }
}

static inline double _getHeight(double x, double z, double precision)
{
    Vector3 location;
    int i;
    
    location.x = x;
    location.y = noiseGet2DDetail(_noise_height, x, z, precision);
    location.z = z;
    
    for (i = 0; i < _modifiers_count; i++)
    {
        location = modifierApply(_modifiers[i], location);
    }
    
    return location.y;
}

static inline Vector3 _getPoint(double x, double z, double precision)
{
    Vector3 result;

    result.x = x;
    result.y = _getHeight(x, z, precision);
    result.z = z;

    return result;
}

double terrainGetShadow(Vector3 start, Vector3 direction)
{
    Vector3 inc_vector;
    double inc_value, inc_base, inc_factor, height, diff, light, smoothing, length, water;

    direction = v3Normalize(direction);
    inc_factor = (double)render_quality;
    inc_base = 1.0;
    inc_value = inc_base / inc_factor;
    smoothing = 0.03 * inc_factor;;

    water = waterGetLightFactor(start);

    light = 1.0;
    length = 0.0;
    do
    {
        inc_vector = v3Scale(direction, inc_value);
        length += v3Norm(inc_vector);
        start = v3Add(start, inc_vector);
        height = _getHeight(start.x, start.z, inc_value);
        diff = start.y - height;
        if (diff < 0.0)
        {
            light += diff / smoothing;
        }

        if (diff < inc_base / inc_factor)
        {
            inc_value = inc_base / inc_factor;
        }
        else if (diff > inc_base)
        {
            inc_value = inc_base;
        }
        else
        {
            inc_value = diff;
        }
    } while (light > 0.0 && length < 50.0 && start.y <= _max_height);

    light *= water;
    if (light < 0.0)
    {
        return 1.0;
    }
    else
    {
        return 1.0 - light;
    }
}

static inline Vector3 _getNormal(Vector3 point, double scale)
{
    Vector3 dpoint, ref, normal;

    ref.x = 0.0;
    ref.y = 0.0;

    dpoint = _getPoint(point.x - scale, point.z, scale * 0.3);
    ref.z = -1.0;
    normal = v3Normalize(v3Cross(ref, v3Sub(dpoint, point)));

    dpoint = _getPoint(point.x + scale, point.z, scale * 0.3);
    ref.z = 1.0;
    normal = v3Add(normal, v3Normalize(v3Cross(ref, v3Sub(dpoint, point))));

    ref.z = 0.0;

    dpoint = _getPoint(point.x, point.z - scale, scale * 0.3);
    ref.x = 1.0;
    normal = v3Add(normal, v3Normalize(v3Cross(ref, v3Sub(dpoint, point))));

    dpoint = _getPoint(point.x, point.z + scale, scale * 0.3);
    ref.x = -1.0;
    normal = v3Add(normal, v3Normalize(v3Cross(ref, v3Sub(dpoint, point))));

    return v3Normalize(normal);
}

static Color _getColor(Vector3 point, double precision)
{
    Vector3 normal;
    Color color, tex_color;
    int i;
    double shadowed, coverage, value;

    shadowed = terrainGetShadow(point, sun_direction_inv);

    /* Apply textures and subsurface lighting */
    color = COLOR_GREEN;
    for (i = 0; i < _texture_count; i++)
    {
        /* TODO Don't recalculate normals for same precision */
        /* TODO Don't compute textures that will be totally covered */
        normal = _getNormal(point, precision * _textures[i].subsurf_scale);

        coverage = zoneGetValue(_textures[i].zone, point, normal);
        if (coverage > 0.0)
        {
            if (coverage < 1.0)
            {
                value = noiseGet2DTotal(_noise_texture_borders, point.x / _textures[i].border_scaling, point.z / _textures[i].border_scaling);
                if (value < coverage)
                {
                    /* TODO Make smoothness precision-dependant */
                    value = (coverage - value) / 0.1;
                    coverage = (value > 1.0) ? 1.0 : value;
                }
                else
                {
                    coverage = 0.0;
                }
            }
            if (coverage > 0.0)
            {
                tex_color = textureApply(_textures[i].tex, point, normal);
                tex_color = lightingApply(point, normal, shadowed, tex_color, 0.1, 0.1);
                tex_color.a = coverage;
                colorMask(&color, &tex_color);
            }
        }
    }

    color = fogApplyToLocation(point, color);
    //color = cloudsApplySegmentResult(color, camera_location, point);

    return color;
}

int terrainProjectRay(Vector3 start, Vector3 direction, Vector3* hit_point, Color* hit_color)
{
    Vector3 inc_vector;
    double inc_value, inc_base, inc_factor, height, diff, length;

    direction = v3Normalize(direction);
    inc_factor = (double)render_quality;
    inc_base = 1.0;
    inc_value = inc_base / inc_factor;

    length = 0.0;
    do
    {
        inc_vector = v3Scale(direction, inc_value);
        length += v3Norm(inc_vector);
        start = v3Add(start, inc_vector);
        height = _getHeight(start.x, start.z, inc_value);
        diff = start.y - height;
        if (diff < 0.0)
        {
            start.y = height;
            *hit_point = start;
            *hit_color = _getColor(start, inc_value);
            return 1;
        }

        if (diff < inc_base / inc_factor)
        {
            inc_value = inc_base / inc_factor;
        }
        else if (diff > inc_base)
        {
            inc_value = inc_base;
        }
        else
        {
            inc_value = diff;
        }
    } while (length < 50.0 && start.y <= _max_height);

    return 0;
}

static int _postProcessFragment(RenderFragment* fragment)
{
    Vector3 point;
    double precision;

    point = fragment->vertex.location;
    precision = renderGetPrecision(point);

    point = _getPoint(point.x, point.z, precision);

    fragment->vertex.color = _getColor(point, precision);

    return 1;
}

static Vertex _getFirstPassVertex(double x, double z, double precision)
{
    Vertex result;
    double value;

    result.location = _getPoint(x, z, 0.0);
    value = sin(x) * sin(x) * cos(z) * cos(z);
    result.color.r = value;
    result.color.g = value;
    result.color.b = value;
    result.color.a = 1.0;
    result.normal.x = result.normal.y = result.normal.z = 0.0;
    result.callback = _postProcessFragment;

    return result;
}

static void _renderQuad(double x, double z, double size)
{
    Vertex v1, v2, v3, v4;

    v1 = _getFirstPassVertex(x, z, size);
    v2 = _getFirstPassVertex(x, z + size, size);
    v3 = _getFirstPassVertex(x + size, z + size, size);
    v4 = _getFirstPassVertex(x + size, z, size);
    renderPushQuad(&v1, &v2, &v3, &v4);
}

double terrainGetHeight(double x, double z)
{
    return _getHeight(x, z, 0.01 / (double)render_quality);
}

double terrainGetHeightNormalized(double x, double z)
{
    return 0.5 + _getHeight(x, z, 0.01 / (double)render_quality) / (_max_height * 2.0);
}

Color terrainGetColor(double x, double z, double precision)
{
    return _getColor(_getPoint(x, z, precision), precision);
}

void terrainSetChunkSize(double min_size, double visible_size)
{
    _base_chunk_size = min_size;
    _visible_chunk_size = visible_size;
}

void terrainRender(RenderProgressCallback callback)
{
    int chunk_factor, chunk_count, i;
    double cx = camera_location.x;
    double cz = camera_location.z;
    double radius_int, radius_ext, chunk_size;

    chunk_factor = 1;
    chunk_count = 2;
    radius_int = 0.0;
    radius_ext = _base_chunk_size;
    chunk_size = _base_chunk_size;

    while (radius_ext < 1000.0)
    {
        if (!callback(radius_ext / 1000.0))
        {
            return;
        }
        
        for (i = 0; i < chunk_count - 1; i++)
        {
            _renderQuad(cx - radius_ext + chunk_size * i, cz - radius_ext, chunk_size);
            _renderQuad(cx + radius_int, cz - radius_ext + chunk_size * i, chunk_size);
            _renderQuad(cx + radius_int - chunk_size * i, cz + radius_int, chunk_size);
            _renderQuad(cx - radius_ext, cz + radius_int - chunk_size * i, chunk_size);
        }

        if (chunk_count % 64 == 0 && chunk_size / radius_int < _visible_chunk_size)
        {
            chunk_count /= 2;
            chunk_factor *= 2;
            /* TODO Fill in gaps with triangles */
        }
        chunk_count += 2;
        chunk_size = _base_chunk_size * chunk_factor;
        radius_int = radius_ext;
        radius_ext += chunk_size;
    }
}
