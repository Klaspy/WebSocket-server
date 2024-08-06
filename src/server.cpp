#include "server.h"

Server::Server(QObject *parent) :
    QObject{parent},
    m_server{new QWebSocketServer("Web server", QWebSocketServer::NonSecureMode, this)}
{
    if (m_server->listen(QHostAddress::Any, 8080))
    {
        qInfo() << "Server started on port 8080";
    }
    else
    {
        qCritical() << "Cannot start server on port 8080. Error code:" << m_server->errorString();
        return;
    }

    connect(m_server, &QWebSocketServer::newConnection, this, &Server::onNewConnection);

    m_worker = new Worker();
    workerThread = new QThread(this);
    workerThread->start();
    m_worker->moveToThread(workerThread);
}

Server::~Server()
{
    if (!m_worker.isNull())
    {
        m_worker->deleteLater();
    }

    if (!workerThread.isNull())
    {
        workerThread->quit();
        while(workerThread->isRunning())
        {
            this->thread()->msleep(1);
            QCoreApplication::processEvents();
        }

        workerThread->deleteLater();
    }
}

void Server::onNewConnection()
{
    QPointer<Connection> connection = new Connection(m_server->nextPendingConnection(), this);

    connect(connection, &Connection::disconnected, this, &Server::removeConnection);
    connect(connection, &Connection::binaryMessage, m_worker, &Worker::processRequest);

    connects.append(connection);
}

void Server::removeConnection()
{
    QPointer<Connection> connection(qobject_cast<Connection*>(sender()));
    qDebug() << connects << connection;

    if (connection.isNull())
    {
        qCritical() << "can't delete null object";
    }
    else if (connects.contains(connection))
    {
        qInfo() << "delete connection on index" << connects.indexOf(connection);
        connects.removeAt(connects.indexOf(connection));
        connection->deleteLater();
    }
    else
    {
        qCritical() << "cannot find object to remove in list";
    }

    qDebug() << connects;
}
