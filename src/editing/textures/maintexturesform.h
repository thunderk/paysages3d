#ifndef MAINTEXTURESFORM_H
#define MAINTEXTURESFORM_H

#include <QWidget>

namespace Ui {
class MainTexturesForm;
}

class MainTexturesForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit MainTexturesForm(QWidget *parent = 0);
    ~MainTexturesForm();
    
private:
    Ui::MainTexturesForm *ui;
};

#endif // MAINTEXTURESFORM_H
