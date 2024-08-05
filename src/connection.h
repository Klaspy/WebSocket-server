#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QWebSocket>
#include <QThread>
#include <QPointer>

#include "worker.h"

class Connection : public QObject
{
    Q_OBJECT
public:
    explicit Connection(QPointer<QWebSocket> socket, Worker* worker, QObject *parent = nullptr);


private slots:
    void onBinaryMessage(const QByteArray &message);

private:
    void moveToNewThread();

    QPointer<QWebSocket> m_socket;
    QPointer<Worker> m_worker;

signals:
    void disconnected();
};

#endif // CONNECTION_H
