#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "editing_global.h"
#include <QMainWindow>
#include <QVector>

class BaseForm;
class FormRender;

namespace paysages {
namespace system {
class PackStream;
}
}

namespace Ui {
class MainWindow;
}

class MainWindow;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

    virtual bool event(QEvent* event);

    static inline MainWindow* instance()
    {
        return _instance;
    }
    static void guiSaveCallback(PackStream* stream, void* data);
    static void guiLoadCallback(PackStream* stream, void* data);

protected:

public slots:
    void refreshAll();

    void openTab(int position);

    void fileNew();
    void fileSave();
    void fileLoad();

    void showAboutDialog();

    void quickPreview();
    void finalRender();
    void showLastRender();

    void explore3D();

signals:
    void refreshed();

private:
    static MainWindow* _instance;
    Ui::MainWindow *ui;

    void guiSave(PackStream* stream);
    void guiLoad(PackStream* stream);

    QVector<BaseForm*> _forms;
    FormRender* _form_render;
};

#endif // MAINWINDOW_H
