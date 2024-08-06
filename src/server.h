#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QCoreApplication>

#include "connection.h"
#include "worker.h"

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);
    ~Server();

private slots:
    void onNewConnection();
    void removeConnection();

private:
    QList<QPointer<Connection>> connects;
    QWebSocketServer *m_server;
    QPointer<Worker> m_worker;
    QPointer<QThread> workerThread;
};

#endif // SERVER_H
