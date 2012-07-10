#ifndef _PAYSAGES_QT_BASEFORMLAYER_H_
#define _PAYSAGES_QT_BASEFORMLAYER_H_

/* Base form, with automatic layer control */

#include "baseform.h"

class BaseFormLayer:public BaseForm
{
    Q_OBJECT

public:
    BaseFormLayer(QWidget* parent);

};

#endif
