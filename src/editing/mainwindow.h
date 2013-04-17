#ifndef _PAYSAGES_QT_MAINWINDOW_H_
#define _PAYSAGES_QT_MAINWINDOW_H_

#include <QMainWindow>
#include "formrender.h"
#include "rendering/tools/pack.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    virtual bool event(QEvent* event);

    static void guiSaveCallback(PackStream* stream, void* data);
    static void guiLoadCallback(PackStream* stream, void* data);

public slots:
    void refreshAll();

    void fileNew();
    void fileSave();
    void fileLoad();

    void showAboutDialog();

    void quickPreview();
    void explore3D();

private:
    void guiSave(PackStream* stream);
    void guiLoad(PackStream* stream);

    QVector<BaseForm*> _forms;
    FormRender* _form_render;
};

#endif
