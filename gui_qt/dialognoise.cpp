#include "dialognoise.h"

#include <QVBoxLayout>
#include <QImage>
#include <QLabel>
#include <QColor>
#include <QPainter>
#include <QSlider>
#include <QScrollArea>
#include <QPushButton>

/**************** Previews ****************/
class PreviewLevel:public BasePreview
{
public:
    PreviewLevel(QWidget* parent, NoiseGenerator* noise): BasePreview(parent)
    {
        _noise_original = noise;
        _noise_preview = noiseCreateGenerator();
        _level = -1;
        
        configScaling(0.05, 2.0, 0.05, 2.0);
    }

    void setLevel(int row)
    {
        _level = row;
        redraw();
    }
protected:
    void updateData()
    {
        noiseCopy(_noise_original, _noise_preview);
    }
    QColor getColor(double x, double y)
    {
        if ((_level >= 0) && (y > noiseGet1DLevel(_noise_preview, _level, x)))
        {
            return QColor(255, 255, 255);
        }
        else
        {
            return QColor(0, 0, 0);
        }
    }
private:
    NoiseGenerator* _noise_original;
    NoiseGenerator* _noise_preview;
    int _level;
};

class PreviewTotal:public BasePreview
{
public:
    PreviewTotal(QWidget* parent, NoiseGenerator* noise): BasePreview(parent)
    {
        _noise_original = noise;
        _noise_preview = noiseCreateGenerator();
        
        configScaling(0.05, 2.0, 0.03, 2.0);
    }
protected:
    void updateData()
    {
        noiseCopy(_noise_original, _noise_preview);
    }
    QColor getColor(double x, double y)
    {
        if (y > noiseGet1DTotal(_noise_preview, x))
        {
            return QColor(255, 255, 255);
        }
        else
        {
            return QColor(0, 0, 0);
        }
    }
private:
    NoiseGenerator* _noise_original;
    NoiseGenerator* _noise_preview;
};

/**************** Dialog form ****************/
DialogNoise::DialogNoise(QWidget *parent, NoiseGenerator* value):
    DialogWithPreview(parent)
{
    QWidget* previews;
    QWidget* form;
    QWidget* buttons;
    QPushButton* button;
    QLabel* label;

    _base = value;
    _current = noiseCreateGenerator();

    setLayout(new QHBoxLayout());

    previews = new QWidget(this);
    previews->setLayout(new QVBoxLayout());
    layout()->addWidget(previews);
    layout()->setAlignment(previews, Qt::AlignTop);

    previewLevel = new PreviewLevel(previews, _current);
    label = new QLabel(tr("Level preview"));
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    previews->layout()->addWidget(label);
    previews->layout()->addWidget(previewLevel);
    
    previewTotal = new PreviewTotal(previews, _current);
    label = new QLabel(tr("Total preview"));
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    previews->layout()->addWidget(label);
    previews->layout()->addWidget(previewTotal);

    form = new QWidget(this);
    form->setLayout(new QVBoxLayout());
    layout()->addWidget(form);

    form->layout()->addWidget(new QLabel(tr("Noise components")));
    levels = new QListWidget(form);
    form->layout()->addWidget(levels);
    QObject::connect(levels, SIGNAL(currentRowChanged(int)), this, SLOT(levelChanged(int)));

    buttons = new QWidget(form);
    buttons->setLayout(new QHBoxLayout());
    form->layout()->addWidget(buttons);

    button = new QPushButton(tr("Add component"), buttons);
    buttons->layout()->addWidget(button);
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(addLevel()));

    button = new QPushButton(tr("Remove component"), buttons);
    buttons->layout()->addWidget(button);
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(removeLevel()));

    form->layout()->addWidget(new QLabel(tr("Component height")));
    slider_height = new QSlider(form);
    slider_height->setOrientation(Qt::Horizontal);
    slider_height->setMinimumWidth(150);
    //slider_height->setMaximumWidth(400);
    slider_height->setMinimum(0);
    slider_height->setMaximum(1000);
    slider_height->setTickInterval(100);
    slider_height->setTickPosition(QSlider::TicksBelow);
    form->layout()->addWidget(slider_height);
    QObject::connect(slider_height, SIGNAL(valueChanged(int)), this, SLOT(heightChanged(int)));

    form->layout()->addWidget(new QLabel(tr("Component scaling")));
    slider_scaling = new QSlider(form);
    slider_scaling->setOrientation(Qt::Horizontal);
    slider_scaling->setMinimumWidth(150);
    //slider_scaling->setMaximumWidth(400);
    slider_scaling->setMinimum(1);
    slider_scaling->setMaximum(1000);
    slider_scaling->setTickInterval(100);
    slider_scaling->setTickPosition(QSlider::TicksBelow);
    form->layout()->addWidget(slider_scaling);
    QObject::connect(slider_scaling, SIGNAL(valueChanged(int)), this, SLOT(scalingChanged(int)));

    buttons = new QWidget(form);
    buttons->setLayout(new QHBoxLayout());
    form->layout()->addWidget(buttons);

    button = new QPushButton(tr("Cancel"), buttons);
    button->setIcon(QIcon("images/cancel.png"));
    buttons->layout()->addWidget(button);
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(reject()));

    button = new QPushButton(tr("Revert"), buttons);
    button->setIcon(QIcon("images/revert.png"));
    buttons->layout()->addWidget(button);
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(revert()));

    button = new QPushButton(tr("Validate"), buttons);
    button->setIcon(QIcon("images/apply.png"));
    buttons->layout()->addWidget(button);
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(accept()));

    setWindowTitle(tr("Paysages 3D - Noise editor"));

    revert();
}

DialogNoise::~DialogNoise()
{
    delete previewLevel;
    delete previewTotal;

    noiseDeleteGenerator(_current);
}

bool DialogNoise::getNoise(QWidget* parent, NoiseGenerator* noise)
{
    int result;

    DialogNoise* dialog = new DialogNoise(parent, noise);
    result = dialog->exec();

    delete dialog;

    return (result != 0) ? true : false;
}

void DialogNoise::closeEvent(QCloseEvent* e)
{
    reject();
}

void DialogNoise::accept()
{
    noiseCopy(_current, _base);
    QDialog::accept();
}

void DialogNoise::revert()
{
    noiseCopy(_base, _current);

    revertToCurrent();
}

void DialogNoise::revertToCurrent()
{
    int i, n;
    int selected;
    
    selected = levels->currentRow();

    levels->clear();
    n = noiseGetLevelCount(_current);
    for (i = 0; i < n; i++)
    {
        levels->addItem(QString(tr("Component %1")).arg(i + 1));
    }
    
    if (n > 0)
    {
        if (selected < 0)
        {
            selected = 0;
        }
        if (selected >= n)
        {
            selected = n - 1;
        }
        levels->setCurrentRow(selected);
    }

    previewLevel->redraw();
    previewTotal->redraw();
}

void DialogNoise::addLevel()
{
    NoiseLevel level;
    
    level.height = 0.1;
    level.scaling = 0.1;
    level.xoffset = 0.0;
    level.yoffset = 0.0;
    level.zoffset = 0.0;
    noiseAddLevel(_current, level);

    revertToCurrent();
    
    levels->setCurrentRow(levels->count() - 1);
}

void DialogNoise::removeLevel()
{
    int row;
    
    row = levels->currentRow();
    
    noiseRemoveLevel(_current, _current_level);

    revertToCurrent();
    
    if (row >= levels->count())
    {
        row--;
    }
    levels->setCurrentRow(row);
}

void DialogNoise::levelChanged(int row)
{
    if (noiseGetLevel(_current, row, &_current_level_params))
    {
        _current_level = row;
        ((PreviewLevel*)previewLevel)->setLevel(row);

        slider_height->setValue(_current_level_params.height * 1000.0);
        slider_scaling->setValue(_current_level_params.scaling * 1000.0);
    }
    // TODO else ...
}

void DialogNoise::heightChanged(int value)
{
    _current_level_params.height = ((double)value) / 1000.0;
    noiseSetLevel(_current, _current_level, _current_level_params);
    previewLevel->redraw();
    previewTotal->redraw();
}

void DialogNoise::scalingChanged(int value)
{
    _current_level_params.scaling = ((double)value) / 1000.0;
    noiseSetLevel(_current, _current_level, _current_level_params);
    previewLevel->redraw();
    previewTotal->redraw();
}
