#include "Timing.h"

#include <QTime>

static QTime EPOCH = QTime::currentTime();

unsigned long Timing::getRelativeTimeMs() {
    return EPOCH.msecsTo(QTime::currentTime());
}
