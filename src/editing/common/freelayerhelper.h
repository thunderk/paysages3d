#ifndef _EDITING_COMMON_FREELAYERHELPER_H_
#define	_EDITING_COMMON_FREELAYERHELPER_H_

#include <QObject>
#include "rendering/layers.h"

class QTableWidget;
class QPushButton;

class FreeLayerHelper : public QObject {
    Q_OBJECT

public:
    FreeLayerHelper(Layers* layers, bool reverse=false);
    ~FreeLayerHelper();

    void refreshLayers();
    void setLayerTable(QTableWidget* table);
    void setAddButton(QPushButton* button);
    void setDelButton(QPushButton* button);
    void setDownButton(QPushButton* button);
    void setUpButton(QPushButton* button);

signals:
    void selectionChanged(int layer);
    void layersChanged();
    void tableUpdateNeeded();

public slots:
    void addLayer();
    void deleteLayer();
    void moveLayerDown();
    void moveLayerUp();
    void tableSelectionChanged(int row, int col);

private:
    QTableWidget* _table;
    Layers* _layers;
    int _selected;
    bool _reverse;
};

#endif

