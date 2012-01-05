#ifndef _GUI_QT_BASEINPUT_H_
#define _GUI_QT_BASEINPUT_H_

#include <QWidget>
#include <QSlider>

class BaseInput:public QObject
{
    Q_OBJECT

public:
    BaseInput(QWidget* form, QString label);
    inline QWidget* label() {return _label;}
    inline QWidget* preview() {return _preview;}
    inline QWidget* control() {return _control;}

public slots:
    virtual void revert() = 0;

protected slots:
    virtual void applyValue();

signals:
    void valueChanged();

protected:
    QWidget* _label;
    QWidget* _preview;
    QWidget* _control;
};

#endif // _GUI_QT_BASEINPUT_H_
