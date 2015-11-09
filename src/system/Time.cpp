#include "Time.h"

#include <QTime>

static QTime EPOCH = QTime::currentTime();

unsigned long Time::getRelativeTimeMs() {
    return EPOCH.msecsTo(QTime::currentTime());
}
