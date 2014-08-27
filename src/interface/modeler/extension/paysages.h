#ifndef PAYSAGES_H
#define PAYSAGES_H

#include <QQuickItem>

class Paysages : public QQuickItem
{
    Q_OBJECT
    Q_DISABLE_COPY(Paysages)

public:
    Paysages(QQuickItem *parent = 0);
    ~Paysages();
};

#endif // PAYSAGES_H

