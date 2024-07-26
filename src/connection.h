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


private slots:
    void onBinaryMessage(const QByteArray &message);

private:
    void moveToNewThread();

    QPointer<QThread> connectionThread;
    QPointer<QWebSocket> m_socket;

signals:
    void disconnected();
};

#endif // CONNECTION_H
