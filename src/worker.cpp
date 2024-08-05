#include "worker.h"

Worker::Worker(QObject *parent)
    : QObject{parent}
{
    methods.insert("test", QVariant());
}

QByteArray Worker::processRequest(QByteArray request)
{
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
        else if (!methods.contains(request.method))
        {
            Answer answer(false);
            answer.code = RpcErrors::InvalidRequest;
            answer.messsage = "Wrong method";

            answersList.append(answer);
        }
        else
        {
            if (request.method == "test")
            {
                if (request.paramsList.isEmpty())
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
        }
    }

    return JsonParser::parseAnswerJson(answersList);
}
