#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QMetaObject>

#include "jsonparser.h"
#include "connection.h"

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = nullptr);

public slots:
    void processRequest(QByteArray request);

signals:

};

#endif // WORKER_H
