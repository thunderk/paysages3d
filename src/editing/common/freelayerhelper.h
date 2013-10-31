#ifndef _EDITING_COMMON_FREELAYERHELPER_H_
#define	_EDITING_COMMON_FREELAYERHELPER_H_

#include <QObject>
#include "Layers.h"

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
    void setEditButton(QPushButton* button);

signals:
    void selectionChanged(int layer);
    void layersChanged();
    void tableUpdateNeeded();
    void editRequired(int layer);

public slots:
    void addLayer();
    void deleteLayer();
    void moveLayerDown();
    void moveLayerUp();
    void startEditing();
    void tableSelectionChanged(int row, int col);

private:
    QTableWidget* _table;
    Layers* _layers;
    int _selected;
    bool _reverse;
};

#endif

