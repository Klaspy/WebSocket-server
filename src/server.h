#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QWebSocketServer>
#include <QWebSocket>

#include "connection.h"

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);

private slots:
    void onNewConnection();
    void removeConnection();

private:
    QList<QPointer<Connection>> connects;
    QWebSocketServer *m_server;
};

#endif // SERVER_H
