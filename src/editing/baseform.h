#ifndef _PAYSAGES_QT_BASEFORM_H_
#define _PAYSAGES_QT_BASEFORM_H_

#include <QWidget>
#include <QStringList>
#include "basepreview.h"
#include "baseinput.h"
#include "dialoglayers.h"
#include "shared/types.h"
#include "tools/curve.h"
#include "tools/color.h"
#include "tools/lighting.h"

class QPushButton;
class QComboBox;

namespace paysages {
namespace system {
    class PackStream;
}
namespace basics {
    class NoiseGenerator;
}
namespace definition {
    class Layers;
}
}

class BaseForm : public QWidget
{
    Q_OBJECT

public:
    BaseForm(QWidget* parent, bool auto_apply = false, bool with_layers = false);
    void hideButtons();
    virtual void savePack(PackStream* stream);
    virtual void loadPack(PackStream* stream);

signals:
    void configApplied();

public slots:
    virtual void revertConfig();
    virtual void applyConfig();

protected slots:
    virtual void configChangeEvent();
    virtual void autoPresetSelected(int preset);

private slots:
    void rebuildLayerList();
    void layerAddClicked();
    void layerDelClicked();
    void layerUpClicked();
    void layerDownClicked();
    void layerRenameClicked();
    void layerListChanged();
    void presetChoiceClicked();

protected:
    void addPreview(BasePreview* preview, QString label);
    QPushButton* addButton(QString label);
    void addAutoPreset(QString label);
    BaseInput* addInput(BaseInput* input);
    BaseInput* addInputInt(QString label, int* value, int min, int max, int small_step, int large_step);
    BaseInput* addInputDouble(QString label, double* value, double min, double max, double small_step, double large_step);
    BaseInput* addInputBoolean(QString label, int* value);
    BaseInput* addInputColor(QString label, Color* value);
    BaseInput* addInputColorGradation(QString label, ColorGradation* value);
    BaseInput* addInputNoise(QString label, NoiseGenerator* value);
    BaseInput* addInputCurve(QString label, Curve* value, double xmin, double xmax, double ymin, double ymax, QString xlabel, QString ylabel);
    BaseInput* addInputMaterial(QString label, SurfaceMaterial* material);
    BaseInput* addInputEnum(QString label, int* value, const QStringList& values);
    BaseInput* addInputLayers(QString label, Layers* value, FormLayerBuilder form_builder);

    void updatePreviews();
    void disablePreviewsUpdate();

    int currentLayer();
    virtual QStringList getLayers();

    virtual void layerAddedEvent();
    virtual void layerDeletedEvent(int layer);
    virtual void layerMovedEvent(int layer, int new_position);
    virtual void layerRenamedEvent(int layer, QString new_name);
    virtual void layerSelectedEvent(int layer);

private:
    bool _auto_apply;
    QWidget* _form;
    QWidget* _form_labels;
    QWidget* _form_previews;
    QWidget* _form_controls;
    QWidget* _buttons;
    QPushButton* _button_apply;
    QPushButton* _button_revert;
    QPushButton* _button_preset;

    QWidget* _previews;
    QVector<BasePreview*> _previews_list;
    bool _auto_update_previews;

    QVector<BaseInput*> _inputs_list;

    QStringList _preset_list;

    bool _with_layers;
    QComboBox* _layer_list;
    int _layer_count;
    QStringList _layer_names;
    QPushButton* _layer_new;
    QPushButton* _layer_del;
    QPushButton* _layer_up;
    QPushButton* _layer_down;
    QPushButton* _layer_rename;
};

#endif
