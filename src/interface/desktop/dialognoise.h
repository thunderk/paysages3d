#ifndef _PAYSAGES_QT_DIALOGNOISE_H_
#define _PAYSAGES_QT_DIALOGNOISE_H_

#include "desktop_global.h"

#include <QListWidget>
#include <QComboBox>
#include "tools.h"
#include "NoiseGenerator.h"

class DialogNoise : public DialogWithPreview
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
    void functionChanged();
    void levelChanged(int row);
    void heightChanged(int value);
    void scalingChanged(int value);

private:
    NoiseGenerator* _base;
    NoiseGenerator* _current;
    int _current_level;
    NoiseLevel _current_level_params;
    BasePreview* previewLevel;
    BasePreview* previewTotal;
    QComboBox* function_algo;
    QSlider* function_ridge;
    QListWidget* levels;
    QSlider* slider_height;
    QSlider* slider_scaling;
};

#endif
