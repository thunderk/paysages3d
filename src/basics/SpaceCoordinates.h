#ifndef SPACECOORDINATES_H
#define SPACECOORDINATES_H

#include "basics_global.h"

#include "Vector3.h"

namespace paysages {
namespace basics {

/*!
 * \brief Space 3D coordinates.
 *
 * These coordinates allow to work on very large scenes, regardless of the 'double' type precision.
 */
class SpaceCoordinates:public Vector3
{
/*public:
    SpaceCoordinates();*/
};

}
}

#endif // SPACECOORDINATES_H
