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

QList<QPointer<GuidedObject>> JsonParser::getGuidedObjects()
{
    QFile file(QCoreApplication::applicationDirPath() + "/autosave.json");
    file.open(QIODevice::ReadOnly);
    QJsonArray objectsArray = QJsonDocument::fromJson(file.readAll()).array();
    file.close();

    QList<QPointer<GuidedObject>> objects;
    QList<QByteArray> parents;

    foreach(QJsonValue objectVal, objectsArray)
    {
        QJsonObject obj = objectVal.toObject();
        GuidedObject *object = new GuidedObject(obj.value("name").toString(), obj.value("create date time").toString(),
                                                QByteArray::fromHex(obj.value("id").toString().toUtf8()));
        parents.append(obj.value("parent").toString().toUtf8());
        obj.remove("name");
        obj.remove("create date time");
        obj.remove("id");
        obj.remove("parent");
        foreach (QString key, obj.keys())
        {
            object->setProperty(key.toUtf8(), obj.value(key).toVariant());
        }

        objects.append(object);
    }

    for (int i = 0; i < objects.size(); i++)
    {
        if (parents.at(i) != "")
        {
            foreach (GuidedObject *object, objects)
            {
                if (object == parents.at(i))
                {
                    objects[i]->setParent(object);
                }
            }
        }
    }

    return objects;
}

void JsonParser::saveGuidedObjects(QList<QPointer<GuidedObject>> objects)
{
    QJsonArray objectsArray;

    foreach (GuidedObject *object, objects)
    {
        if (object == nullptr)
        {
            continue;
        }
        QJsonObject obj;
        obj.insert("name", object->name());
        obj.insert("create date time", object->createDT());
        obj.insert("id", QString(object->id().toHex()));
        GuidedObject *parent = qobject_cast<GuidedObject*>(object->parent());
        if (parent != nullptr)
        {
            obj.insert("parent", QString(parent->id().toHex()));
        }

        foreach (const QByteArray &name, object->dynamicPropertyNames())
        {
            obj.insert(QString(name), QJsonValue::fromVariant(object->property(name)));
        }

        objectsArray.append(obj);
    }

    QFile file(QCoreApplication::applicationDirPath() + "/autosave.json");
    file.open(QIODevice::WriteOnly);
    file.write(QJsonDocument(objectsArray).toJson(QJsonDocument::Indented));
    file.close();
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
