#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QWebSocket>
#include <QThread>
#include <QPointer>


class Connection : public QObject
{
    Q_OBJECT
public:
    explicit Connection(QPointer<QWebSocket> socket, QObject *parent = nullptr);

public slots:
    void sendBinaryMessage(QByteArray message);

private slots:
    void onBinaryMessage(const QByteArray &message);

private:
    void moveToNewThread();

    QPointer<QWebSocket> m_socket;

signals:
    void disconnected();
    void binaryMessage(QByteArray message);
};

#endif // CONNECTION_H
