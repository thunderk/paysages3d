#ifndef _PAYSAGES_QT_DIALOGCOLORGRADATION_H_
#define _PAYSAGES_QT_DIALOGCOLORGRADATION_H_

#include <QDialog>
#include "baseform.h"

#include "../lib_paysages/color.h"

class DialogColorGradation : public QDialog
{
    Q_OBJECT
public:
    explicit DialogColorGradation(QWidget* parent, ColorGradation* gradation);
    ~DialogColorGradation();

    static bool getGradation(QWidget* parent, ColorGradation* gradation);

public slots:
    virtual void accept();
    void revert();

protected:
    virtual void closeEvent(QCloseEvent* e);

private:
    void revertToCurrent();

private:
    ColorGradation* _base;
    ColorGradation _current;
    BaseForm* _form;
};

#endif
