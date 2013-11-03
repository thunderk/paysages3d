#ifndef RANDOMGENERATOR_H
#define RANDOMGENERATOR_H

#include "system_global.h"
#include <cstdlib>

namespace paysages {
namespace system {

class RandomGenerator
{
public:
    RandomGenerator();

    static inline double random()
    {
        return ((double)rand()) / (double)RAND_MAX;
    }
};

}
}

#endif // RANDOMGENERATOR_H
