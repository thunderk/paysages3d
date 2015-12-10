#include "RandomGenerator.h"

#include <chrono>
#include <random>

static RandomGenerator _RandomGeneratorDefault;
RandomGenerator &paysages::system::RandomGeneratorDefault = _RandomGeneratorDefault;

class RandomGenerator::RandomGeneratorPrivate {
  public:
    RandomGeneratorPrivate(unsigned int seed) : generator(seed) {
    }

    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution_double;
};

RandomGenerator::RandomGenerator(RandomGenerator::Seed seed) {
    if (not seed) {
        std::random_device true_random;
        if (true_random.entropy()) {
            seed = true_random();
        } else {
            seed = std::chrono::system_clock::now().time_since_epoch().count();
        }
    }
    data = new RandomGeneratorPrivate(seed);
}

RandomGenerator::~RandomGenerator() {
    delete data;
}

double RandomGenerator::genDouble() {
    return data->distribution_double(data->generator);
}
