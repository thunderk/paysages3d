#ifndef COLORPROFILE_H
#define COLORPROFILE_H

#include "basics_global.h"

#include "Color.h"

namespace paysages {
namespace basics {

class BASICSSHARED_EXPORT ColorProfile {
  public:
    typedef enum { TONE_MAPPING_UNCHARTED, TONE_MAPPING_REIHNARD, TONE_MAPPING_CLAMP } ToneMappingOperator;

  public:
    ColorProfile();
    ColorProfile(ToneMappingOperator tonemapper, double exposure);

    void setToneMapping(ToneMappingOperator tonemapper, double exposure);

    void save(PackStream *stream) const;
    void load(PackStream *stream);
    void copy(ColorProfile *destination) const;

    Color apply(const Color &pixel) const;

  private:
    ToneMappingOperator mapper;
    double exposure;
};
}
}

#endif // COLORPROFILE_H
