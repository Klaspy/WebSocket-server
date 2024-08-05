#include "jsonparser.h"

JsonParser::JsonParser(QObject *parent)
    : QObject{parent}
{}

QList<RpcRequest> JsonParser::parseRequest(QByteArray request)
{
    QJsonDocument requestDoc;
    QJsonParseError parseError;
    QList<RpcRequest> parsedRequests;
    requestDoc = QJsonDocument::fromJson(request, &parseError);

    if (parseError.error != QJsonParseError::NoError)
    {
        RpcRequest errorReq;
        errorReq.parseError = RpcErrors::ParseError;
        errorReq.errorString = "JSON parse erorr";

        parsedRequests.append(errorReq);
    }
    else
    {
        if (requestDoc.isArray())
        {
            foreach(QJsonValue request, requestDoc.array())
            {
                parsedRequests.append(parseSingleRequest(request));
            }
        }
        else
        {
            parsedRequests.append(parseSingleRequest(requestDoc.object()));
        }
    }

    return parsedRequests;
}

QByteArray JsonParser::parseAnswerJson(QList<Answer> answersList)
{
    QJsonArray answerArray;
    foreach (Answer answer, answersList)
    {
        QJsonObject answerJson
            {
                {"jsonrpc", "2.0"}
            };

        if (answer.error)
        {
            answerJson.insert("error", QJsonObject
                              {
                                  {"code", answer.code},
                                  {"message", answer.messsage}
                              });
            answerJson.insert("id", QJsonValue::Null);
        }
        else if (!answer.id.isUndefined())
        {
            answerJson.insert("result", answer.result);
            answerJson.insert("id",     answer.id);
        }

        answerArray.append(answerJson);
    }

    if (answerArray.size() > 1)
    {
        return QJsonDocument(answerArray).toJson();
    }
    else if (answerArray.size() > 0)
    {
        return QJsonDocument(answerArray.first().toObject()).toJson();
    }
    else
    {
        return QByteArray();
    }
}

RpcRequest JsonParser::parseSingleRequest(QJsonValue request)
{
    if (!request.isObject() || request.toObject().isEmpty())
    {
        RpcRequest errorReq;
        errorReq.parseError = RpcErrors::InvalidRequest;
        errorReq.errorString = "Invalid request";

        return errorReq;
    }
    else
    {
        RpcRequest req;
        QJsonObject reqObject = request.toObject();

        if (reqObject.value("jsonrpc").toString() == "2.0")
        {
            req.method = reqObject.value("method").toString();
            req.id     = reqObject.value("id");

            if (reqObject.value("params").isArray())
            {
                req.paramsList = reqObject.value("params").toArray().toVariantList();
            }
            else if (reqObject.value("params").isObject())
            {
                req.paramsMap = reqObject.value("params").toObject().toVariantMap();
            }
        }
        else
        {
            req.parseError = RpcErrors::InvalidRequest;
            req.errorString = "Invalid request";
        }

        return req;
    }
}
