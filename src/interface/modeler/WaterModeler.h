#pragma once

#include "modeler_global.h"

#include "BaseModelerTool.h"
#include <QObject>

namespace paysages {
namespace modeler {

class WaterModeler : public QObject, public BaseModelerTool {
    Q_OBJECT
  public:
    WaterModeler(MainModelerWindow *ui);

  public slots:
    void enableRendering(bool enable);
};
}
}
