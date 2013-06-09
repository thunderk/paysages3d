#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include "rendering/tools/pack.h"

class BaseForm;
class FormRender;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

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
    void finalRender();
    void showLastRender();

    void explore3D();

private:
    Ui::MainWindow *ui;

    void guiSave(PackStream* stream);
    void guiLoad(PackStream* stream);

    QVector<BaseForm*> _forms;
    FormRender* _form_render;
};

#endif // MAINWINDOW_H
