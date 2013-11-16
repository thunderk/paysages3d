#include "freelayerhelper.h"

#include <QTableWidget>
#include <QPushButton>

FreeLayerHelper::FreeLayerHelper(Layers* layers, bool reverse)
{
    _table = NULL;
    _selected = -1;
    _layers = layers;
    _reverse = reverse;
}

FreeLayerHelper::~FreeLayerHelper()
{
}

void FreeLayerHelper::refreshLayers()
{
    if (_table)
    {
        int selected = _selected;

        emit tableUpdateNeeded();

        int n = _layers->count();
        if (n == 0)
        {
            _selected = -1;
        }

        if (selected < 0)
        {
            _table->clearSelection();
            emit selectionChanged(-1);
        }
        else
        {
            if (selected >= n)
            {
                selected = n - 1;
            }

            _table->setCurrentCell(selected, 0);
        }
    }
}

void FreeLayerHelper::setLayerTable(QTableWidget* table)
{
    _table = table;

    connect(table, SIGNAL(currentCellChanged(int, int, int, int)), this, SLOT(tableSelectionChanged(int, int)));

    refreshLayers();
}

void FreeLayerHelper::setAddButton(QPushButton* button)
{
    connect(button, SIGNAL(clicked()), this, SLOT(addLayer()));
}

void FreeLayerHelper::setDelButton(QPushButton* button)
{
    connect(button, SIGNAL(clicked()), this, SLOT(deleteLayer()));
}

void FreeLayerHelper::setDownButton(QPushButton* button)
{
    if (_reverse)
    {
        connect(button, SIGNAL(clicked()), this, SLOT(moveLayerDown()));
    }
    else
    {
        connect(button, SIGNAL(clicked()), this, SLOT(moveLayerUp()));
    }
}

void FreeLayerHelper::setUpButton(QPushButton* button)
{
    if (_reverse)
    {
        connect(button, SIGNAL(clicked()), this, SLOT(moveLayerUp()));
    }
    else
    {
        connect(button, SIGNAL(clicked()), this, SLOT(moveLayerDown()));
    }
}

void FreeLayerHelper::setEditButton(QPushButton* button)
{
    connect(button, SIGNAL(clicked()), this, SLOT(startEditing()));
}

void FreeLayerHelper::addLayer()
{
    _selected = _layers->addLayer(NULL);
    emit(layersChanged());
    refreshLayers();
}

void FreeLayerHelper::deleteLayer()
{
    if (_selected >= 0)
    {
        _layers->removeLayer(_selected);
        emit(layersChanged());
        refreshLayers();
    }
}

void FreeLayerHelper::moveLayerDown()
{
    if (_selected > 0)
    {
        _layers->moveLayer(_selected, _selected - 1);
        _selected--;
        emit(layersChanged());
        refreshLayers();
    }
}

void FreeLayerHelper::moveLayerUp()
{
    if (_selected >= 0 && _selected < _layers->count() - 1)
    {
        _layers->moveLayer(_selected, _selected + 1);
        _selected++;
        emit(layersChanged());
        refreshLayers();
    }
}

void FreeLayerHelper::startEditing()
{
    if (_selected >= 0 && _selected < _layers->count())
    {
        emit(editRequired(_selected));
    }
}

void FreeLayerHelper::tableSelectionChanged(int row, int)
{
    int n = _layers->count();

    if (n == 0 or row >= n)
    {
        _selected = -1;
    }
    else
    {
        _selected = _reverse ? n - 1 - row : row;
    }

    emit(selectionChanged(_selected));
}
