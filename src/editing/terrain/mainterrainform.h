#ifndef MAINTERRAINFORM_H
#define MAINTERRAINFORM_H

#include <QWidget>

namespace Ui {
class MainTerrainForm;
}

class MainTerrainForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit MainTerrainForm(QWidget *parent = 0);
    ~MainTerrainForm();
    
private:
    Ui::MainTerrainForm *ui;
};

#endif // MAINTERRAINFORM_H
