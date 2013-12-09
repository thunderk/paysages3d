#ifndef DIALOGTERRAINPAINTING_H
#define DIALOGTERRAINPAINTING_H

#include "desktop_global.h"

#include <QDialog>
#include "paintingbrush.h"

namespace Ui {
class DialogTerrainPainting;
}

class DialogTerrainPainting : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogTerrainPainting(QWidget *parent, TerrainDefinition* terrain);
    ~DialogTerrainPainting();

protected:
    virtual void keyReleaseEvent(QKeyEvent* event);
    virtual void wheelEvent(QWheelEvent* event);

public slots:
    void brushConfigChanged();
    void heightmapChanged();
    virtual void accept();
    void revert();

private:
    QString getHelpText();

    Ui::DialogTerrainPainting *ui;

    TerrainDefinition* _terrain_modified;
    TerrainDefinition* _terrain_original;
    PaintingBrush _brush;
};

#endif // DIALOGTERRAINPAINTING_H
