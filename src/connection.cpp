#include "connection.h"

Connection::Connection(QPointer<QWebSocket> socket, Worker *worker, QObject *parent) :
    QObject  {parent},
    m_socket {socket}
{
    qDebug() << m_socket << m_socket->state();

    m_worker = worker;

    connect(m_socket, &QWebSocket::disconnected,          this, &Connection::disconnected);
    connect(m_socket, &QWebSocket::binaryMessageReceived, this, &Connection::onBinaryMessage);
}

void Connection::onBinaryMessage(const QByteArray &message)
{
    qDebug() << "messsage" << message;

    m_socket->sendBinaryMessage(m_worker->processRequest(message));
}
