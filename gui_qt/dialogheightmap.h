#ifndef _PAYSAGES_QT_DIALOGHEIGHTMAP_H_
#define _PAYSAGES_QT_DIALOGHEIGHTMAP_H_

#include "tools.h"
#include "widgetheightmap.h"
#include "../lib_paysages/heightmap.h"

class DialogHeightMap : public DialogWithPreview
{
    Q_OBJECT
public:
    explicit DialogHeightMap(QWidget* parent, HeightMap* heightmap);
    static bool editHeightMap(QWidget* parent, HeightMap* heightmap);

public slots:
    virtual void accept();
    void revert();

private:
    HeightMap* _value_original;
    HeightMap _value_modified;
    WidgetHeightMap* _3dview;
};

#endif
