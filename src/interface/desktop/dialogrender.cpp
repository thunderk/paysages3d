#include "dialogrender.h"

#include <cmath>
#include <QVBoxLayout>
#include <QImage>
#include <QColor>
#include <QPainter>
#include <QMessageBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QListWidget>
#include <QPushButton>
#include <QMutex>
#include <QThread>
#include <QScrollArea>
#include <QLabel>
#include <QProgressBar>
#include <QComboBox>
#include "tools.h"

#include "Scenery.h"
#include "ColorProfile.h"
#include "SoftwareCanvasRenderer.h"
#include "WidgetPreviewCanvas.h"

class RenderThread:public QThread
{
public:
    RenderThread(DialogRender* dialog, SoftwareCanvasRenderer* renderer):QThread()
    {
        _dialog = dialog;
        _renderer = renderer;
    }
    void run()
    {
        _renderer->render();
        _dialog->tellRenderEnded();
    }
private:
    DialogRender* _dialog;
    SoftwareCanvasRenderer* _renderer;
};

DialogRender::DialogRender(QWidget *parent, SoftwareCanvasRenderer* renderer):
    QDialog(parent, Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint)
{
    pixbuf_lock = new QMutex();
    pixbuf = new QImage(1, 1, QImage::Format_ARGB32);
    _render_thread = NULL;
    canvas_renderer = renderer;

    setModal(true);
    setWindowTitle(tr("Paysages 3D - Render"));
    setLayout(new QVBoxLayout());

    canvas_preview = new WidgetPreviewCanvas(this);
    canvas_preview->setCanvas(canvas_renderer->getCanvas());
    layout()->addWidget(canvas_preview);

    // Status bar
    _info = new QWidget(this);
    _info->setLayout(new QHBoxLayout());
    layout()->addWidget(_info);

    _timer = new QLabel(QString("0:00.00"), _info);
    _info->layout()->addWidget(_timer);

    _progress = new QProgressBar(_info);
    _progress->setMaximumHeight(12);
    _progress->setMinimum(0);
    _progress->setMaximum(1000);
    _progress->setValue(0);
    _info->layout()->addWidget(_progress);

    // Action bar
    _actions = new QWidget(this);
    _actions->setLayout(new QHBoxLayout());
    layout()->addWidget(_actions);

    _actions->layout()->addWidget(new QLabel(tr("Tone-mapping: "), _actions));
    _tonemapping_control = new QComboBox(_actions);
    _tonemapping_control->addItems(QStringList(tr("Uncharted")) << tr("Reinhard"));
    _actions->layout()->addWidget(_tonemapping_control);

    _actions->layout()->addWidget(new QLabel(tr("Exposure: "), _actions));
    _actions->hide();
    _exposure_control = new QSlider(Qt::Horizontal, _actions);
    _exposure_control->setMinimumWidth(200);
    _exposure_control->setRange(0, 1000);
    _exposure_control->setValue(200);
    _actions->layout()->addWidget(_exposure_control);

    _save_button = new QPushButton(QIcon(getDataPath("images/save.png")), tr("Save picture"), _actions);
    _actions->layout()->addWidget(_save_button);

    // Connections
    connect(this, SIGNAL(progressChanged(double)), this, SLOT(applyProgress(double)));
    connect(this, SIGNAL(renderEnded()), this, SLOT(applyRenderEnded()));
    connect(_save_button, SIGNAL(clicked()), this, SLOT(saveRender()));
    connect(_tonemapping_control, SIGNAL(currentIndexChanged(int)), this, SLOT(toneMappingChanged()));
    connect(_exposure_control, SIGNAL(valueChanged(int)), this, SLOT(toneMappingChanged()));

    toneMappingChanged();
}

DialogRender::~DialogRender()
{
    if (_render_thread)
    {
        canvas_renderer->interrupt();
        _render_thread->wait();

        delete _render_thread;
    }
    delete pixbuf;
    delete pixbuf_lock;
}

void DialogRender::tellRenderEnded()
{
    emit renderEnded();
}

void DialogRender::startRender()
{
    _started = time(NULL);

    _render_thread = new RenderThread(this, canvas_renderer);
    _render_thread->start();

    startTimer(100);

    exec();
}

void DialogRender::applyRenderEnded()
{
    _info->hide();
    _actions->show();
}

void DialogRender::saveRender()
{
    QString filepath;

    filepath = QFileDialog::getSaveFileName(this, tr("Paysages 3D - Choose a filename to save the last render"), QString(), tr("Images (*.png *.jpg)"));
    if (!filepath.isNull())
    {
        if (!filepath.toLower().endsWith(".jpg") && !filepath.toLower().endsWith(".jpeg") && !filepath.toLower().endsWith(".png"))
        {
            filepath = filepath.append(".png");
        }
        std::string filepathstr = filepath.toStdString();
        /*if (canvas_renderer->render_area->saveToFile((char*)filepathstr.c_str()))
        {
            QMessageBox::information(this, "Message", QString(tr("The picture %1 has been saved.")).arg(filepath));
        }
        else*/
        {
            QMessageBox::critical(this, "Message", QString(tr("Can't write to file : %1")).arg(filepath));
        }
    }
}

void DialogRender::toneMappingChanged()
{
    ColorProfile profile((ColorProfile::ToneMappingOperator)_tonemapping_control->currentIndex(), ((double)_exposure_control->value()) * 0.01);
    canvas_preview->setToneMapping(profile);
}

void DialogRender::loadLastRender()
{
    renderEnded();
    toneMappingChanged();

    exec();
}

void DialogRender::timerEvent(QTimerEvent *)
{
    double diff = difftime(time(NULL), _started);
    int hours = (int)floor(diff / 3600.0);
    int minutes = (int)floor((diff - 3600.0 * hours) / 60.0);
    int seconds = (int)floor(diff - 3600.0 * hours - 60.0 * minutes);
    _timer->setText(tr("%1:%2.%3").arg(hours).arg(minutes, 2, 10, QLatin1Char('0')).arg(seconds, 2, 10, QLatin1Char('0')));

    _progress->setValue((int)(canvas_renderer->getProgress() * 1000.0));
    _progress->update();
}

