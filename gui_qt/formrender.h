#ifndef _PAYSAGES_QT_FORMRENDER_H_
#define _PAYSAGES_QT_FORMRENDER_H_

#include "baseform.h"

class FormRender : public BaseForm
{
    Q_OBJECT

public:
    explicit FormRender(QWidget *parent = 0);

private slots:
    void startRender();
    void showRender();
    void saveRender();

private:
    int _quality;
};

#endif // _PAYSAGES_QT_FORMRENDER_H_
