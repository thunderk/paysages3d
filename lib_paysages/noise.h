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
    double ridge_factor;
} NoiseFunction;

struct NoiseLevel
{
    double scaling;
    double height;
    double xoffset;
    double yoffset;
    double zoffset;
};
typedef struct NoiseLevel NoiseLevel;
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
NoiseFunction noiseGetFunction(NoiseGenerator* generator);
void noiseSetFunction(NoiseGenerator* generator, NoiseFunction* function);
void noiseSetFunctionParams(NoiseGenerator* generator, NoiseFunctionAlgorithm algorithm, double ridge_factor);
void noiseForceValue(NoiseGenerator* generator, double value);
double noiseGetMaxValue(NoiseGenerator* generator);
int noiseGetLevelCount(NoiseGenerator* generator);
void noiseClearLevels(NoiseGenerator* generator);
void noiseAddLevel(NoiseGenerator* generator, NoiseLevel level);
void noiseAddLevelSimple(NoiseGenerator* generator, double scaling, double height);
void noiseAddLevels(NoiseGenerator* generator, int level_count, NoiseLevel start_level, double scaling_factor, double height_factor, int randomize_offset);
void noiseAddLevelsSimple(NoiseGenerator* generator, int level_count, double scaling, double height);
void noiseRemoveLevel(NoiseGenerator* generator, int level);
int noiseGetLevel(NoiseGenerator* generator, int level, NoiseLevel* params);
void noiseSetLevel(NoiseGenerator* generator, int level, NoiseLevel params);
void noiseSetLevelSimple(NoiseGenerator* generator, int level, double scaling, double height);
void noiseNormalizeHeight(NoiseGenerator* generator, double min_height, double max_height, int adjust_scaling);
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
