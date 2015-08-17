#ifndef WATERMODELER_H
#define WATERMODELER_H

#include "modeler_global.h"

#include <QObject>

namespace paysages {
namespace modeler {

class WaterModeler: public QObject
{
    Q_OBJECT
public:
    WaterModeler(MainModelerWindow *main);

public slots:
    void waterLevelChanged(double value);

private:
    FloatNode *propWaterHeight() const;
    MainModelerWindow *main;
};

}
}

#endif // WATERMODELER_H
