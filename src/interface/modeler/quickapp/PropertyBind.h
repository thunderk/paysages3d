#ifndef PROPERTYBIND_H
#define PROPERTYBIND_H

#include <QObject>

class PropertyBind : public QObject
{
    Q_OBJECT
public:
    explicit PropertyBind(QObject *parent = 0);

signals:

public slots:
};

#endif // PROPERTYBIND_H
