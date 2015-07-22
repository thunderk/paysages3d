#ifndef ATMOSPHEREMODELER_H
#define ATMOSPHEREMODELER_H

#include "modeler_global.h"

#include <QObject>

namespace paysages {
namespace modeler {

class AtmosphereModeler : public QObject
{
    Q_OBJECT
public:
    AtmosphereModeler(MainModelerWindow *main);

public slots:
    void daytimeChanged(double value);

private:
    MainModelerWindow *main;
};

}
}

#endif // ATMOSPHEREMODELER_H
