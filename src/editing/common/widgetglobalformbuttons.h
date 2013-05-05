#ifndef WIDGETGLOBALFORMBUTTONS_H
#define WIDGETGLOBALFORMBUTTONS_H

#include <QWidget>

namespace Ui {
class WidgetGlobalFormButtons;
}

class WidgetGlobalFormButtons : public QWidget
{
    Q_OBJECT
    
public:
    explicit WidgetGlobalFormButtons(QWidget *parent = 0);
    ~WidgetGlobalFormButtons();

signals:
    void okClicked();
    void revertClicked();
    void cancelClicked();
    
private:
    Ui::WidgetGlobalFormButtons *ui;
};

#endif // WIDGETGLOBALFORMBUTTONS_H
