#include "manager.h"

Manager::Manager(QObject *parent)
    : QObject{parent}
{
    objects = JsonParser::getGuidedObjects();

    for (int i = 0; i < objects.size(); i++)
    {
        if (objects.at(i)->parent() == nullptr)
        {
            objects[i]->setParent(this);
        }
    }
}

QList<QPair<QString, QByteArray>> Manager::getAll()
{
    QList<QPair<QString, QByteArray>> result;
    foreach (const GuidedObject *object, objects)
    {
        result.append({object->name(), object->id()});
    }

    return result;
}

void Manager::addNew(QString name)
{
    objects.append(new GuidedObject(name, this));
    JsonParser::saveGuidedObjects(objects);
}

bool Manager::removeObject(QByteArray id)
{
    bool success {false};
    for (int i = 0; i < objects.size(); i++)
    {
        if (objects.at(i)->id() == id)
        {
            delete objects.at(i);
            success = true;
            break;
        }
    }

    for (int i = 0; i < objects.size();)
    {
        if (!objects.at(i).isNull())
        {
            i++;
        }
        else
        {
            objects.removeAt(i);
        }
    }
    JsonParser::saveGuidedObjects(objects);
    return success;
}

QVariantMap Manager::getObjectInfo(QByteArray id)
{
    foreach (GuidedObject *object, objects)
    {
        if (object->id() == id)
        {
            QVariantMap result;
            result.insert("id", object->id().toHex());
            result.insert("name", object->name());
            result.insert("create date time", object->createDT());
            result.insert(object->getCustomProperties());
            GuidedObject *parent = qobject_cast<GuidedObject*>(object->parent());
            if (parent == nullptr)
            {
                result.insert("parent", QJsonValue::Null);
            }
            else
            {
                result.insert("parent", parent->id().toHex());
            }
            return result;
        }
    }

    return QVariantMap();
}

bool Manager::renameObject(QByteArray id, QString name)
{
    foreach (GuidedObject *object, objects)
    {
        if (object->id() == id)
        {
            object->setName(name);
            JsonParser::saveGuidedObjects(objects);
            return true;
        }
    }

    return false;
}

QVariant Manager::getCustomProperty(QByteArray id, QString propertyName)
{
    if (propertyName == "name" || propertyName == "create date time" || propertyName == "id" || propertyName == "parent" || propertyName == "")
    {
        return QVariant::Invalid;
    }

    foreach (GuidedObject *object, objects)
    {
        if (object->id() == id)
        {
            if (object->dynamicPropertyNames().contains(propertyName.toUtf8()))
            {
                return object->property(propertyName.toUtf8());
            }
            else
            {
                return QVariant::Invalid;
            }
        }
    }

    return QVariant::Invalid;
}

int Manager::setCustomProperty(QByteArray id, QString propertyName, QVariant value)
{
    if (propertyName == "name" || propertyName == "create date time" || propertyName == "id" || propertyName == "parent" || propertyName == "")
    {
        return -2;
    }

    if (value.isNull()) value = QVariant::Invalid;
    foreach (GuidedObject *object, objects)
    {
        if (object->id() == id)
        {
            object->setProperty(propertyName.toUtf8(), value);
            JsonParser::saveGuidedObjects(objects);
            return 0;
        }
    }

    return -1;
}

int Manager::setParent(QByteArray objectId, QVariant parentId_)
{
    QByteArray parentId = !parentId_.isNull() ? QByteArray::fromHex(parentId_.toString().toUtf8()) : "";
    if (objectId == parentId_)
    {
        return -3;
    }
    GuidedObject *obj {nullptr}, *parent {nullptr};
    foreach (GuidedObject *object, objects)
    {
        if (object->id() == objectId)
        {
            obj = object;
        }
        else if (!parentId_.isNull() && object->id() == parentId)
        {
            parent = object;
        }

        if (obj != nullptr && parent != nullptr)
        {
            break;
        }
    }

    if (obj == nullptr)
    {
        return -1;
    }
    else if (!parentId_.isNull() && parent == nullptr)
    {
        return -2;
    }
    else
    {
        if (parentId_.isNull())
        {
            obj->setParent(this);
        }
        else
        {
            obj->setParent(parent);
        }
        JsonParser::saveGuidedObjects(objects);
        return parentId_.isNull() ? 1 : 0;
    }
}
