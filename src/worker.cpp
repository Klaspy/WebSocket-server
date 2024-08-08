#include "worker.h"

Worker::Worker(QObject *parent)
    : QObject{parent}
{
    manager = new Manager(this);
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
                answersList.append(invalidParamsAnswer());
            }
            else if (!request.id.isUndefined())
            {
                Answer answer(true);
                auto objects = manager->getAll();

                QJsonArray result;
                for (int i = 0; i < objects.size(); i++)
                {
                    result.append(QJsonObject{
                        {"id", QString(objects.at(i).second.toHex())},
                        {"name", objects.at(i).first}
                    });
                }
                answer.result = result;
                answer.id = request.id;

                answersList.append(answer);
            }
        }
        else if (request.method == "addNew")
        {
            if (!request.paramsMap.contains("name") || request.paramsMap.value("name").type() != QVariant::String)
            {
                answersList.append(invalidParamsAnswer());
            }
            else
            {
                manager->addNew(request.paramsMap.value("name").toString());

                if (!request.id.isUndefined())
                {
                    Answer answer(true);
                    answer.result = "New object added";
                    answer.id = request.id;

                    answersList.append(answer);
                }
            }
        }
        else if (request.method == "removeObj")
        {
            if (!request.paramsMap.contains("id") || request.paramsMap.value("id").type() != QVariant::String)
            {
                answersList.append(invalidParamsAnswer());
            }
            else
            {
                if (manager->removeObject(QByteArray::fromHex(request.paramsMap.value("id").toString().toUtf8())))
                {
                    if (!request.id.isUndefined())
                    {
                        Answer answer(true);
                        answer.result = "Object #" + request.paramsMap.value("id").toString() + " deleted";
                        answer.id = request.id;

                        answersList.append(answer);
                    }
                }
                else
                {
                    if (!request.id.isUndefined())
                    {
                        Answer answer(true);
                        answer.result = "Cannot find object #" + request.paramsMap.value("id").toString();
                        answer.id = request.id;

                        answersList.append(answer);
                    }
                }
            }
        }
        else if (request.method == "getObjInfo")
        {
            if (!request.paramsMap.contains("id") || request.paramsMap.value("id").type() != QVariant::String)
            {
                answersList.append(invalidParamsAnswer());
            }
            else
            {
                if (!request.id.isUndefined())
                {
                    QVariantMap objInfo = manager->getObjectInfo(QByteArray::fromHex(request.paramsMap.value("id").toString().toUtf8()));

                    Answer answer(true);
                    answer.result = QJsonObject::fromVariantMap(objInfo);
                    answer.id = request.id;

                    answersList.append(answer);
                }
            }
        }
        else if (request.method == "renameObj")
        {
            if ((!request.paramsMap.contains("id")   || request.paramsMap.value("id").type()   != QVariant::String) &&
                (!request.paramsMap.contains("name") || request.paramsMap.value("name").type() != QVariant::String))
            {
                answersList.append(invalidParamsAnswer());
            }
            else
            {
                bool success = manager->renameObject(QByteArray::fromHex(request.paramsMap.value("id").toString().toUtf8()), request.paramsMap.value("name").toString());

                if (!request.id.isUndefined())
                {
                    if (success)
                    {
                        Answer answer(true);
                        answer.result = "Object #" + request.paramsMap.value("id").toString() + " renamed";
                        answer.id = request.id;

                        answersList.append(answer);
                    }
                    else
                    {
                        Answer answer(true);
                        answer.result = "Cannot find object #" + request.paramsMap.value("id").toString();
                        answer.id = request.id;

                        answersList.append(answer);
                    }
                }
            }
        }
        else if (request.method == "getCustomProperty")
        {
            if ((!request.paramsMap.contains("id")           || request.paramsMap.value("id").type()           != QVariant::String) &&
                (!request.paramsMap.contains("propertyName") || request.paramsMap.value("propertyName").type() != QVariant::String))
            {
                answersList.append(invalidParamsAnswer());
            }
            else
            {
                if (!request.id.isUndefined())
                {
                    Answer answer(true);
                    answer.result = QJsonValue::fromVariant(manager->getCustomProperty(QByteArray::fromHex(request.paramsMap.value("id").toString().toUtf8()),
                                                                                       request.paramsMap.value("propertyName").toString()));
                    answer.id = request.id;

                    answersList.append(answer);
                }
            }
        }
        else if (request.method == "setCustomProperty")
        {
            if ((!request.paramsMap.contains("id")           || request.paramsMap.value("id").type()           != QVariant::String) &&
                (!request.paramsMap.contains("propertyName") || request.paramsMap.value("propertyName").type() != QVariant::String) &&
                (!request.paramsMap.contains("value")        || !request.paramsMap.value("value").isValid()))
            {
                answersList.append(invalidParamsAnswer());
            }
            else
            {
                int result = manager->setCustomProperty(QByteArray::fromHex(request.paramsMap.value("id").toString().toUtf8()),
                                                        request.paramsMap.value("propertyName").toString(),
                                                        request.paramsMap.value("value"));

                if (!request.id.isUndefined())
                {
                    Answer answer(true);
                    answer.id = request.id;
                    switch (result)
                    {
                    case 0:
                    {
                        if (request.paramsMap.value("value").isNull())
                        {
                            answer.result = "Property removed";
                        }
                        {
                            answer.result = "Property changed";
                        }
                        break;
                    }
                    case -1:
                    {
                        answer.result = "Cannot find object #" + request.paramsMap.value("id").toString();
                        break;
                    }
                    case -2:
                    {
                        answer.result = "Cannot set reserved property";
                        break;
                    }
                    }

                    answersList.append(answer);
                }
            }
        }
        else if (request.method == "setParent")
        {
            if ((!request.paramsMap.contains("id")       ||  request.paramsMap.value("id").type()       != QVariant::String) &&
                (!request.paramsMap.contains("parentId") || (request.paramsMap.value("parentId").type() != QVariant::String && !request.paramsMap.value("parentId").isNull())))
            {
                answersList.append(invalidParamsAnswer());
            }
            else
            {
                int result = manager->setParent(QByteArray::fromHex(request.paramsMap.value("id").toString().toUtf8()),
                                                request.paramsMap.value("parentId"));

                if (!request.id.isUndefined())
                {
                    Answer answer(true);
                    answer.id = request.id;
                    switch (result)
                    {
                    case 0:
                    {
                        answer.result = "Parent changed";
                        break;
                    }
                    case 1:
                    {
                        answer.result = "Parent unset";
                        break;
                    }
                    case -1:
                    {
                        answer.result = "Cannot find object #" + request.paramsMap.value("id").toString();
                        break;
                    }
                    case -2:
                    {
                        answer.result = "Cannot find object #" + request.paramsMap.value("parentId").toString();
                        break;
                    }
                    case -3:
                    {
                        answer.result = "Cannot set set the parent of an object to itself";
                        break;
                    }
                    }

                    answersList.append(answer);
                }
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

Answer Worker::invalidParamsAnswer()
{
    Answer answer(false);
    answer.code = RpcErrors::InvalidParams;
    answer.messsage = "Invalid parameters";

    return answer;
}
