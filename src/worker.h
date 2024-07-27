#ifndef WORKER_H
#define WORKER_H

#include <QObject>

#include "jsonparser.h"

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = nullptr);

    QByteArray processRequest(QByteArray request);

private:

signals:
};

#endif // WORKER_H
