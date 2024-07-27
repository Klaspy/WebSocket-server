#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>

enum RpcErrors
{
    ParseError = -32700,
    InvalidRequest = -32600,
    MethodNotFound = -32601,
    InvalidParams = -32602,

    NoError = 0
};

struct ErrorAnswer
{
    RpcErrors code;
    QString messsage;

    explicit ErrorAnswer(RpcErrors code, QString message)
    {
        this->code = code;
        this->messsage = message;
    }
};

struct RpcRequest
{
    QString method;
    QVariantMap paramsMap;
    QVariantList paramsList;
    QVariant id {0};

    RpcErrors parseError {RpcErrors::NoError};
    QString errorString;

    ErrorAnswer errorAnswer()
    {
        return ErrorAnswer(parseError, errorString);
    }
};

class JsonParser : public QObject
{
    Q_OBJECT
public:
    explicit JsonParser(QObject *parent = nullptr);

    static QList<RpcRequest> parseRequest(QByteArray request);

private:
    static RpcRequest parseSingleRequest(QJsonValue request);

signals:
};

#endif // JSONPARSER_H
