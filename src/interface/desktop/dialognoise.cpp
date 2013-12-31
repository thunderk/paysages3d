#include "dialognoise.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QImage>
#include <QLabel>
#include <QColor>
#include <QPainter>
#include <QSlider>
#include <QScrollArea>
#include <QPushButton>
#include <cmath>
#include "BasePreview.h"

/**************** Previews ****************/
class PreviewLevel:public BasePreview
{
public:
    PreviewLevel(QWidget* parent, NoiseGenerator* noise): BasePreview(parent)
    {
        _noise_original = noise;
        _noise_preview = new NoiseGenerator();
        _level = -1;

        configScaling(0.15, 6.0, 0.09, 6.0);
    }

    void setLevel(int row)
    {
        _level = row;
        redraw();
    }
protected:
    void updateData()
    {
        _noise_original->copy(_noise_preview);
    }
    Color getColor(double x, double y)
    {
        if ((_level >= 0) && (-y > _noise_preview->get1DLevel(_level, x)))
        {
            return COLOR_WHITE;
        }
        else
        {
            return COLOR_BLACK;
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
        _noise_preview = new NoiseGenerator();

        configScaling(0.15, 6.0, 0.09, 6.0);
    }
protected:
    void updateData()
    {
        _noise_original->copy(_noise_preview);
    }
    Color getColor(double x, double y)
    {
        if (-y > _noise_preview->get1DTotal(x))
        {
            return COLOR_WHITE;
        }
        else
        {
            return COLOR_BLACK;
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
    QWidget* function;
    QWidget* previews;
    QWidget* form;
    QWidget* buttons;
    QPushButton* button;
    QLabel* label;

    _base = value;
    _current = new NoiseGenerator();

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

    function = new QWidget(form);
    function->setLayout(new QHBoxLayout());
    form->layout()->addWidget(function);

    function->layout()->addWidget(new QLabel(tr("Noise function")));
    function_algo = new QComboBox(function);
    function_algo->addItems(QStringList(tr("Simplex (best)")) << tr("Perlin (fast)") << tr("Naive (slow)"));
    function->layout()->addWidget(function_algo);
    function->layout()->addWidget(new QLabel(tr("Ridge factor")));
    function_ridge = new QSlider(Qt::Horizontal, function);
    function_ridge->setRange(-10, 10);
    function_ridge->setTickInterval(10);
    function_ridge->setTickPosition(QSlider::TicksBelow);
    function_ridge->setMinimumWidth(150);
    function->layout()->addWidget(function_ridge);
    QObject::connect(function_algo, SIGNAL(currentIndexChanged(int)), this, SLOT(functionChanged()));
    QObject::connect(function_ridge, SIGNAL(valueChanged(int)), this, SLOT(functionChanged()));

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
    button->setIcon(QIcon(getDataPath("images/cancel.png")));
    buttons->layout()->addWidget(button);
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(reject()));

    button = new QPushButton(tr("Revert"), buttons);
    button->setIcon(QIcon(getDataPath("images/revert.png")));
    buttons->layout()->addWidget(button);
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(revert()));

    button = new QPushButton(tr("Validate"), buttons);
    button->setIcon(QIcon(getDataPath("images/apply.png")));
    buttons->layout()->addWidget(button);
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(accept()));

    setWindowTitle(tr("Paysages 3D - Noise editor"));

    revert();
}

DialogNoise::~DialogNoise()
{
    delete previewLevel;
    delete previewTotal;

    delete _current;
}

bool DialogNoise::getNoise(QWidget* parent, NoiseGenerator* noise)
{
    int result;

    DialogNoise* dialog = new DialogNoise(parent, noise);
    result = dialog->exec();

    delete dialog;

    return (result != 0) ? true : false;
}

void DialogNoise::closeEvent(QCloseEvent*)
{
    reject();
}

void DialogNoise::accept()
{
    _current->copy(_base);
    QDialog::accept();
}

void DialogNoise::revert()
{
    _base->copy(_current);

    revertToCurrent();
}

void DialogNoise::revertToCurrent()
{
    int i, n;
    int selected;
    NoiseGenerator::NoiseFunction function;

    selected = levels->currentRow();

    levels->clear();
    n = _current->getLevelCount();
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

    function = _current->getFunction();
    function_algo->setCurrentIndex((int)function.algorithm);
    function_ridge->setValue(round(function.ridge_factor * 20.0));

    previewLevel->redraw();
    previewTotal->redraw();
}

void DialogNoise::addLevel()
{
    NoiseGenerator::NoiseLevel level;

    level.amplitude = 0.1;
    level.wavelength = 0.1;

    _current->addLevel(level);

    revertToCurrent();

    levels->setCurrentRow(levels->count() - 1);
}

void DialogNoise::removeLevel()
{
    int row;

    row = levels->currentRow();

    _current->removeLevel(_current_level);

    revertToCurrent();

    if (row >= levels->count())
    {
        row--;
    }
    levels->setCurrentRow(row);
}

void DialogNoise::functionChanged()
{
    NoiseGenerator::NoiseFunction function;

    function.algorithm = (NoiseGenerator::NoiseFunctionAlgorithm)function_algo->currentIndex();
    function.ridge_factor = (double)function_ridge->value() * 0.05;

    _current->setFunction(&function);
    _current->validate();

    previewLevel->redraw();
    previewTotal->redraw();
}

void DialogNoise::levelChanged(int row)
{
    if (_current->getLevel(row, &_current_level_params))
    {
        _current_level = row;
        ((PreviewLevel*)previewLevel)->setLevel(row);

        slider_height->setValue(_current_level_params.amplitude * 1000.0);
        slider_scaling->setValue(_current_level_params.wavelength * 1000.0);
    }
    // TODO else ...
}

void DialogNoise::heightChanged(int value)
{
    _current_level_params.amplitude = ((double)value) / 1000.0;
    _current->setLevel(_current_level, _current_level_params);
    previewLevel->redraw();
    previewTotal->redraw();
}

void DialogNoise::scalingChanged(int value)
{
    _current_level_params.wavelength = ((double)value) / 1000.0;
    _current->setLevel(_current_level, _current_level_params);
    previewLevel->redraw();
    previewTotal->redraw();
}
