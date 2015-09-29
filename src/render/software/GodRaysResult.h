#ifndef GODRAYSRESULT_H
#define GODRAYSRESULT_H

#include "software_global.h"

#include "Color.h"

namespace paysages {
namespace software {

/**
 * Result of god rays computation.
 */
class SOFTWARESHARED_EXPORT GodRaysResult
{
public:
    typedef struct {
        double penetration;
        double resistance;
        double boost;
    } GodRaysParams;

public:
    GodRaysResult() = default;
    GodRaysResult(double inside_length, double full_length);

    /**
     * Apply the result on a base color.
     */
    Color apply(const Color &raw, const Color &atmosphered, const GodRaysParams &params);

private:
    double inside_length;
    double full_length;
};

}
}

#endif // GODRAYSRESULT_H
