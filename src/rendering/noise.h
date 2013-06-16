#ifndef _PAYSAGES_NOISE_H_
#define _PAYSAGES_NOISE_H_

#include "tools/pack.h"

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

void noiseInit();
void noiseQuit();
void noiseSave(PackStream* stream);
void noiseLoad(PackStream* stream);

NoiseGenerator* noiseCreateGenerator();
void noiseDeleteGenerator(NoiseGenerator* generator);
void noiseSaveGenerator(PackStream* stream, NoiseGenerator* generator);
void noiseLoadGenerator(PackStream* stream, NoiseGenerator* generator);
void noiseCopy(NoiseGenerator* source, NoiseGenerator* destination);
void noiseValidate(NoiseGenerator* generator);
void noiseRandomizeOffsets(NoiseGenerator* generator);
NoiseFunction noiseGetFunction(NoiseGenerator* generator);
void noiseSetCustomFunction(NoiseGenerator* generator, double (*func1d)(double x), double (*func2d)(double x, double y), double (*func3d)(double x, double y, double z));
void noiseSetFunction(NoiseGenerator* generator, NoiseFunction* function);
void noiseSetFunctionParams(NoiseGenerator* generator, NoiseFunctionAlgorithm algorithm, double ridge_factor, double curve_factor);
void noiseForceValue(NoiseGenerator* generator, double value);
void noiseGetRange(NoiseGenerator* generator, double* minvalue, double* maxvalue);
int noiseGetLevelCount(NoiseGenerator* generator);
void noiseClearLevels(NoiseGenerator* generator);
void noiseAddLevel(NoiseGenerator* generator, NoiseLevel level, int protect_offsets);
void noiseAddLevelSimple(NoiseGenerator* generator, double scaling, double minvalue, double maxvalue);
void noiseAddLevels(NoiseGenerator* generator, int level_count, NoiseLevel start_level, double scaling_factor, double amplitude_factor, double center_factor);
void noiseAddLevelsSimple(NoiseGenerator* generator, int level_count, double scaling, double minvalue, double maxvalue, double center_factor);
void noiseRemoveLevel(NoiseGenerator* generator, int level);
int noiseGetLevel(NoiseGenerator* generator, int level, NoiseLevel* params);
void noiseSetLevel(NoiseGenerator* generator, int index, NoiseLevel level, int protect_offsets);
void noiseSetLevelSimple(NoiseGenerator* generator, int index, double scaling, double minvalue, double maxvalue);
void noiseNormalizeAmplitude(NoiseGenerator* generator, double minvalue, double maxvalue, int adjust_scaling);
double noiseGet1DLevel(NoiseGenerator* generator, int level, double x);
double noiseGet1DTotal(NoiseGenerator* generator, double x);
double noiseGet1DDetail(NoiseGenerator* generator, double x, double detail);
double noiseGet2DLevel(NoiseGenerator* generator, int level, double x, double y);
double noiseGet2DTotal(NoiseGenerator* generator, double x, double y);
double noiseGet2DDetail(NoiseGenerator* generator, double x, double y, double detail);
double noiseGet3DLevel(NoiseGenerator* generator, int level, double x, double y, double z);
double noiseGet3DTotal(NoiseGenerator* generator, double x, double y, double z);
double noiseGet3DDetail(NoiseGenerator* generator, double x, double y, double z, double detail);

#ifdef __cplusplus
}
#endif

#endif
