#ifndef _PAYSAGES_NOISE_H_
#define _PAYSAGES_NOISE_H_

#include "pack.h"

#ifdef __cplusplus
extern "C" {
#endif

struct NoiseLevel
{
    float scaling;
    float height;
    float xoffset;
    float yoffset;
    float zoffset;
};
typedef struct NoiseLevel NoiseLevel;
typedef struct NoiseGenerator NoiseGenerator;

void noiseInit();
void noiseQuit();
void noiseSave(PackStream* stream);
void noiseLoad(PackStream* stream);

NoiseGenerator* noiseCreateGenerator();
void noiseDeleteGenerator(NoiseGenerator* generator);
void noiseSaveGenerator(PackStream* stream, NoiseGenerator* perlin);
void noiseLoadGenerator(PackStream* stream, NoiseGenerator* perlin);
void noiseCopy(NoiseGenerator* source, NoiseGenerator* destination);
void noiseGenerateBaseNoise(NoiseGenerator* generator, int size);
int noiseGetBaseSize(NoiseGenerator* generator);
float noiseGetMaxValue(NoiseGenerator* generator);
int noiseGetLevelCount(NoiseGenerator* generator);
void noiseClearLevels(NoiseGenerator* generator);
void noiseAddLevel(NoiseGenerator* generator, NoiseLevel level);
void noiseAddLevelSimple(NoiseGenerator* generator, float scaling, float height);
void noiseAddLevels(NoiseGenerator* generator, int level_count, NoiseLevel start_level, float scaling_factor, float height_factor, int randomize_offset);
void noiseAddLevelsSimple(NoiseGenerator* generator, int level_count, float scaling, float height);
void noiseRemoveLevel(NoiseGenerator* generator, int level);
int noiseGetLevel(NoiseGenerator* generator, int level, NoiseLevel* params);
void noiseSetLevel(NoiseGenerator* generator, int level, NoiseLevel params);
void noiseSetLevelSimple(NoiseGenerator* generator, int level, float scaling, float height);
void noiseNormalizeHeight(NoiseGenerator* generator, float min_height, float max_height, int adjust_scaling);
float noiseGet1DLevel(NoiseGenerator* generator, int level, float x);
float noiseGet1DTotal(NoiseGenerator* generator, float x);
float noiseGet1DDetail(NoiseGenerator* generator, float x, float detail);
float noiseGet2DLevel(NoiseGenerator* generator, int level, float x, float y);
float noiseGet2DTotal(NoiseGenerator* generator, float x, float y);
float noiseGet2DDetail(NoiseGenerator* generator, float x, float y, float detail);
float noiseGet3DLevel(NoiseGenerator* generator, int level, float x, float y, float z);
float noiseGet3DTotal(NoiseGenerator* generator, float x, float y, float z);
float noiseGet3DDetail(NoiseGenerator* generator, float x, float y, float z, float detail);

#ifdef __cplusplus
}
#endif

#endif
