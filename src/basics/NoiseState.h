#ifndef NOISESTATE_H
#define NOISESTATE_H

#include "basics_global.h"

#include <vector>

namespace paysages {
namespace basics {

/*!
 * Fractal noise state, that can be saved to a file.
 *
 * This state contains the noise offsets for noise layers.
 */
class BASICSSHARED_EXPORT NoiseState {
  public:
    typedef struct {
        double x;
        double y;
        double z;
    } NoiseOffset;

  public:
    NoiseState();

    void save(PackStream *stream) const;
    void load(PackStream *stream);
    void copy(NoiseState *destination) const;

    void randomizeOffsets(RandomGenerator &random = RandomGeneratorDefault);
    void resetOffsets(double x = 0.0, double y = 0.0, double z = 0.0);

    void setLevel(int level, double x, double y, double z);
    void setLevelCount(int level_count);

  private:
    std::vector<NoiseOffset> level_offsets;

    friend class NoiseGenerator;
    friend class FractalNoise;
};
}
}

#endif // NOISESTATE_H
