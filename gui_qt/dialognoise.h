#ifndef _PAYSAGES_QT_DIALOGNOISE_H_
#define _PAYSAGES_QT_DIALOGNOISE_H_

#include <QDialog>
#include <QListWidget>
#include "preview.h"

#include "../lib_paysages/shared/types.h"

class DialogNoise : public QDialog
{
    Q_OBJECT
public:
    explicit DialogNoise(QWidget* parent, NoiseGenerator* noise);
    ~DialogNoise();
    
    static bool getNoise(QWidget* parent, NoiseGenerator* noise);

public slots:
    virtual void accept();
    void revert();

protected:
    virtual void closeEvent(QCloseEvent* e);

private:
    void revertToCurrent();

private slots:
    void addLevel();
    void removeLevel();
    void levelChanged(int row);
    void heightChanged(int value);
    void scalingChanged(int value);
    
private:
    NoiseGenerator* _base;
    NoiseGenerator* _current;
    int _current_level;
    NoiseLevel _current_level_params;
    Preview* previewLevel;
    Preview* previewTotal;
    QListWidget* levels;
    QSlider* slider_height;
    QSlider* slider_scaling;
};

#endif
