#include "worker.h"

Worker::Worker(QObject *parent)
    : QObject{parent}
{
}

void Worker::processRequest(QByteArray request)
{
    QPointer<Connection> conn = qobject_cast<Connection*>(sender());
    if (conn.isNull())
    {
        return;
    }

    QList<RpcRequest> parsedRequests = JsonParser::parseRequest(request);

    QList<Answer> answersList;

    foreach (RpcRequest request, parsedRequests)
    {
        if (request.parseError != RpcErrors::NoError)
        {
            Answer answer(false);
            answer.code = request.parseError;
            answer.messsage = request.errorString;

            answersList.append(answer);
        }
        else if (request.method == "getAll")
        {
            if (!request.paramsList.isEmpty() && !request.paramsMap.isEmpty())
            {
                Answer answer(false);
                answer.code = RpcErrors::InvalidParams;
                answer.messsage = "Invalid parameters";

                answersList.append(answer);
            }

            else if (!request.id.isNull())
            {
                Answer answer(true);
                answer.result = QJsonObject
                    {
                        {"something", "Офигеть, он смог"}
                    };

                answer.id = request.id;

                answersList.append(answer);
            }
        }
        else
        {
            Answer answer(false);
            answer.code = RpcErrors::InvalidRequest;
            answer.messsage = "Wrong method";

            answersList.append(answer);
        }
    }

    if (!answersList.isEmpty())
    {
        QMetaObject::invokeMethod(sender(), "sendBinaryMessage", Qt::QueuedConnection,
                                  Q_ARG(QByteArray, JsonParser::parseAnswerJson(answersList)));
    }
}
