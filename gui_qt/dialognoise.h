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

protected:
    virtual void closeEvent(QCloseEvent* e);

private slots:
    void cancelClicked();
    void revertClicked();
    void applyClicked();
    
private:
    NoiseGenerator* _base;
    NoiseGenerator* _current;
    NoiseLevel _current_level;
    Preview* previewLevel;
    Preview* previewTotal;
    QListWidget* levels;
};

#endif
