#pragma once

#include "system_global.h"

namespace paysages {
namespace system {

class SYSTEMSHARED_EXPORT RandomGenerator {
  public:
    typedef unsigned long Seed;
    class RandomGeneratorPrivate;

  public:
    RandomGenerator(Seed seed = 0);
    ~RandomGenerator();

    inline Seed getSeed() const {
        return seed;
    }

    /**
     * Generate a 0.0-1.0 random double value.
     */
    double genDouble();

  private:
    Seed seed;
    RandomGeneratorPrivate *data;
};
}
}
