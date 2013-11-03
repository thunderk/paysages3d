#ifndef _PAYSAGES_NOISE_H_
#define _PAYSAGES_NOISE_H_

#include "rendering_global.h"

namespace paysages {
namespace system {class PackStream;}
}

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    NOISE_FUNCTION_SIMPLEX,
    NOISE_FUNCTION_PERLIN,
    NOISE_FUNCTION_NAIVE
} NoiseFunctionAlgorithm;

typedef struct
{
    NoiseFunctionAlgorithm algorithm;
    double ridge_factor; /* -0.5;0.5 */
    double curve_factor; /* -1.0;1.0 */
} NoiseFunction;

typedef struct
{
    double wavelength;
    double amplitude;
    double minvalue;
    double xoffset;
    double yoffset;
    double zoffset;
} NoiseLevel;

typedef struct NoiseGenerator NoiseGenerator;

RENDERINGSHARED_EXPORT void noiseInit();
RENDERINGSHARED_EXPORT void noiseQuit();
RENDERINGSHARED_EXPORT void noiseSave(PackStream* stream);
RENDERINGSHARED_EXPORT void noiseLoad(PackStream* stream);

RENDERINGSHARED_EXPORT NoiseGenerator* noiseCreateGenerator();
RENDERINGSHARED_EXPORT void noiseDeleteGenerator(NoiseGenerator* generator);
RENDERINGSHARED_EXPORT void noiseSaveGenerator(PackStream* stream, NoiseGenerator* generator);
RENDERINGSHARED_EXPORT void noiseLoadGenerator(PackStream* stream, NoiseGenerator* generator);
RENDERINGSHARED_EXPORT void noiseCopy(NoiseGenerator* source, NoiseGenerator* destination);
RENDERINGSHARED_EXPORT void noiseValidate(NoiseGenerator* generator);
RENDERINGSHARED_EXPORT void noiseRandomizeOffsets(NoiseGenerator* generator);
RENDERINGSHARED_EXPORT NoiseFunction noiseGetFunction(NoiseGenerator* generator);
RENDERINGSHARED_EXPORT void noiseSetCustomFunction(NoiseGenerator* generator, double (*func1d)(double x), double (*func2d)(double x, double y), double (*func3d)(double x, double y, double z));
RENDERINGSHARED_EXPORT void noiseSetFunction(NoiseGenerator* generator, NoiseFunction* function);
RENDERINGSHARED_EXPORT void noiseSetFunctionParams(NoiseGenerator* generator, NoiseFunctionAlgorithm algorithm, double ridge_factor, double curve_factor);
RENDERINGSHARED_EXPORT void noiseForceValue(NoiseGenerator* generator, double value);
RENDERINGSHARED_EXPORT void noiseGetRange(NoiseGenerator* generator, double* minvalue, double* maxvalue);
RENDERINGSHARED_EXPORT int noiseGetLevelCount(NoiseGenerator* generator);
RENDERINGSHARED_EXPORT void noiseClearLevels(NoiseGenerator* generator);
RENDERINGSHARED_EXPORT void noiseAddLevel(NoiseGenerator* generator, NoiseLevel level, int protect_offsets);
RENDERINGSHARED_EXPORT void noiseAddLevelSimple(NoiseGenerator* generator, double scaling, double minvalue, double maxvalue);
RENDERINGSHARED_EXPORT void noiseAddLevels(NoiseGenerator* generator, int level_count, NoiseLevel start_level, double scaling_factor, double amplitude_factor, double center_factor);
RENDERINGSHARED_EXPORT void noiseAddLevelsSimple(NoiseGenerator* generator, int level_count, double scaling, double minvalue, double maxvalue, double center_factor);
RENDERINGSHARED_EXPORT void noiseRemoveLevel(NoiseGenerator* generator, int level);
RENDERINGSHARED_EXPORT int noiseGetLevel(NoiseGenerator* generator, int level, NoiseLevel* params);
RENDERINGSHARED_EXPORT void noiseSetLevel(NoiseGenerator* generator, int index, NoiseLevel level, int protect_offsets);
RENDERINGSHARED_EXPORT void noiseSetLevelSimple(NoiseGenerator* generator, int index, double scaling, double minvalue, double maxvalue);
RENDERINGSHARED_EXPORT void noiseNormalizeAmplitude(NoiseGenerator* generator, double minvalue, double maxvalue, int adjust_scaling);
RENDERINGSHARED_EXPORT double noiseGet1DLevel(NoiseGenerator* generator, int level, double x);
RENDERINGSHARED_EXPORT double noiseGet1DTotal(NoiseGenerator* generator, double x);
RENDERINGSHARED_EXPORT double noiseGet1DDetail(NoiseGenerator* generator, double x, double detail);
RENDERINGSHARED_EXPORT double noiseGet2DLevel(NoiseGenerator* generator, int level, double x, double y);
RENDERINGSHARED_EXPORT double noiseGet2DTotal(NoiseGenerator* generator, double x, double y);
RENDERINGSHARED_EXPORT double noiseGet2DDetail(NoiseGenerator* generator, double x, double y, double detail);
RENDERINGSHARED_EXPORT double noiseGet3DLevel(NoiseGenerator* generator, int level, double x, double y, double z);
RENDERINGSHARED_EXPORT double noiseGet3DTotal(NoiseGenerator* generator, double x, double y, double z);
RENDERINGSHARED_EXPORT double noiseGet3DDetail(NoiseGenerator* generator, double x, double y, double z, double detail);

#ifdef __cplusplus
}
#endif

#endif
