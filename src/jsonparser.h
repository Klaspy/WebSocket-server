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

struct Answer
{
    QJsonValue id;
    QJsonValue result;

    bool error {false};
    RpcErrors code;
    QString messsage;

    Answer() {}

    Answer(bool isError)
    {
        error = !isError;
    }
};

struct RpcRequest
{
    QString method;
    QVariantMap paramsMap;
    QVariantList paramsList;
    QJsonValue id {0};

    RpcErrors parseError {RpcErrors::NoError};
    QString errorString;
};

class JsonParser : public QObject
{
    Q_OBJECT
public:
    explicit JsonParser(QObject *parent = nullptr);

    static QList<RpcRequest> parseRequest(QByteArray request);
    static QByteArray parseAnswerJson(QList<Answer> answersList);

private:
    static RpcRequest parseSingleRequest(QJsonValue request);

signals:
};

#endif // JSONPARSER_H
