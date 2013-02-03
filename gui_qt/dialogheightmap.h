#ifndef _PAYSAGES_QT_DIALOGHEIGHTMAP_H_
#define _PAYSAGES_QT_DIALOGHEIGHTMAP_H_

#include <QLabel>
#include "../lib_paysages/terrain/public.h"
#include "tools.h"
#include "widgetheightmap.h"

class DialogHeightMap : public DialogWithPreview
{
    Q_OBJECT
public:
    explicit DialogHeightMap(QWidget* parent, TerrainDefinition* terrain);
    static bool editHeightMap(QWidget* parent, TerrainDefinition* terrain);

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
    void heightmapChanged();
    //void loadFromFile();

private:
    TerrainDefinition* _value_original;
    TerrainDefinition* _value_modified;
    QLabel* _info_memory;
    WidgetHeightMap* _3dview;
};

#endif
