#include "connection.h"

Connection::Connection(QPointer<QWebSocket> socket, QObject *parent) :
    QObject  {parent},
    m_socket {socket}
{
    qDebug() << m_socket << m_socket->state();

    connect(m_socket, &QWebSocket::disconnected,          this, &Connection::disconnected);
    connect(m_socket, &QWebSocket::binaryMessageReceived, this, &Connection::onBinaryMessage);
}

void Connection::sendBinaryMessage(QByteArray message)
{
    if (m_socket->state() == QAbstractSocket::ConnectedState)
    {
        m_socket->sendBinaryMessage(message);
    }
}

void Connection::onBinaryMessage(const QByteArray &message)
{
    qDebug() << "messsage" << message;

    emit binaryMessage(message);
}
