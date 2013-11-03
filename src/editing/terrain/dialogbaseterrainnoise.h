#ifndef DIALOGBASETERRAINNOISE_H
#define DIALOGBASETERRAINNOISE_H

#include "editing_global.h"
#include <QDialog>

namespace paysages {
namespace basics {
    class NoiseGenerator;
}
}

namespace Ui {
    class DialogBaseTerrainNoise;
}

class DialogBaseTerrainNoise : public QDialog
{
    Q_OBJECT

public:
    explicit DialogBaseTerrainNoise(QWidget *parent = 0);
    ~DialogBaseTerrainNoise();

    void setNoise(NoiseGenerator* noise);

    static int editNoise(QWidget* parent, NoiseGenerator* noise);

private:
    Ui::DialogBaseTerrainNoise *ui;
    NoiseGenerator* _original;
    NoiseGenerator* _modified;
};

#endif // DIALOGBASETERRAINNOISE_H
