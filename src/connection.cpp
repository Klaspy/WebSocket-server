#include "connection.h"

Connection::Connection(QPointer<QWebSocket> socket, QObject *parent) :
    QObject  {parent},
    m_socket {socket}
{
    qDebug() << m_socket << m_socket->state();
    connect(m_socket, &QWebSocket::disconnected,          this, &Connection::disconnected);
    connect(m_socket, &QWebSocket::binaryMessageReceived, this, &Connection::onBinaryMessage);

    moveToNewThread();
}

void Connection::moveToNewThread()
{
    if (!connectionThread.isNull())
    {
        connectionThread->deleteLater();
    }

    connectionThread = new QThread();
    connectionThread->start();
    this->moveToThread(connectionThread);
    connect(this, &Connection::destroyed, connectionThread, &QThread::deleteLater);
}

void Connection::onBinaryMessage(const QByteArray &message)
{
    qDebug() << "messsage" << message;
}
