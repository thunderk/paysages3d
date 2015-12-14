#ifndef WATERMODELER_H
#define WATERMODELER_H

#include <QObject>

#include "modeler_global.h"

namespace paysages {
namespace modeler {

class WaterModeler : public QObject {
    Q_OBJECT
  public:
    WaterModeler(MainModelerWindow *ui);
    ~WaterModeler();

  public slots:
    void enableRendering(bool enable);

  private:
    MainModelerWindow *ui;
    IntPropertyBind *prop_model;
    FloatPropertyBind *prop_height;
    FloatPropertyBind *prop_reflexion;
};
}
}

#endif // WATERMODELER_H
