#ifndef _PAYSAGES_QT_DIALOGHEIGHTMAP_H_
#define _PAYSAGES_QT_DIALOGHEIGHTMAP_H_

#include <QLabel>
#include "tools.h"
#include "widgetheightmap.h"
#include "../lib_paysages/heightmap.h"
#include "../lib_paysages/terraincanvas.h"

class DialogHeightMap : public DialogWithPreview
{
    Q_OBJECT
public:
    explicit DialogHeightMap(QWidget* parent, HeightMap* heightmap, TerrainCanvas* canvas);
    static bool editHeightMap(QWidget* parent, HeightMap* heightmap, TerrainCanvas* canvas);

public slots:
    virtual void accept();
    void revert();
    
private slots:
    void angleHChanged(int value);
    void angleVChanged(int value);
    void brushModeChanged(int value);
    void brushSizeChanged(int value);
    void brushSmoothingChanged(int value);
    void brushStrengthChanged(int value);
    void loadFromFile();
    void resetToTerrain();
    void changeResolution();
    void updateResolutionLabel();

private:
    HeightMap* _value_original;
    HeightMap _value_modified;
    WidgetHeightMap* _3dview;
    QLabel* _resolution_label;
    TerrainCanvas* _canvas;
};

#endif
