#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H

#include "definition_global.h"

namespace paysages {
namespace definition {

/**
 * Time manager, handling the inexorable passing of time.
 */
class DEFINITIONSHARED_EXPORT TimeManager {
  public:
    TimeManager();

    /**
     * Alter a scenery to simulate the passing of *amount* of time.
     *
     * A 1.0 amount is a full day.
     */
    void moveForward(Scenery *scenery, double amount);

    /**
     * Set the wind factor in each direction.
     */
    void setWind(double wind_x, double wind_z);

  private:
    double wind_x;
    double wind_z;
};
}
}

#endif // TIMEMANAGER_H
