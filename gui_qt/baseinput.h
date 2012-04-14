#ifndef _PAYSAGES_QT_BASEINPUT_H_
#define _PAYSAGES_QT_BASEINPUT_H_

#include <QWidget>
#include <QLabel>

class BaseInput:public QObject
{
    Q_OBJECT

public:
    BaseInput(QWidget* form, QString label);
    inline QWidget* label() {return _label;}
    inline QWidget* preview() {return _preview;}
    inline QWidget* control() {return _control;}

public slots:
    virtual void updatePreview();
    virtual void revert();
    virtual void applyValue();

signals:
    void valueChanged();

protected:
    QLabel* _label;
    QWidget* _preview;
    QWidget* _control;
};

#endif
