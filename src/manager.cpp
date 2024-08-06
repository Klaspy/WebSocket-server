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

Manager::~Manager()
{
    JsonParser::saveGuidedObjects(objects);
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
}

bool Manager::removeObject(QByteArray id)
{
    for (int i = 0; i < objects.size(); i++)
    {
        if (objects.at(i) == id)
        {
            objects.removeAt(i);
            return true;
        }
    }

    return false;
}

QVariantMap Manager::getObjectInfo(QByteArray id)
{
    foreach (GuidedObject *object, objects)
    {
        if (object == id)
        {
            QVariantMap result;
            result.insert("id", object->id());
            result.insert("name", object->name());
            result.insert("create date time", object->createDT());
            result.insert(object->getCustomProperties());
            return result;
        }
    }

    return QVariantMap();
}

bool Manager::renameObject(QByteArray id, QString name)
{
    foreach (GuidedObject *object, objects)
    {
        if (object == id)
        {
            object->setName(name);
            return true;
        }
    }

    return false;
}

QVariant Manager::getCustomProperty(QByteArray id, QString propertyName)
{
    if (propertyName == "name" || propertyName == "create date time" || propertyName == "id" || propertyName == "parent")
    {
        return QVariant::Invalid;
    }

    foreach (GuidedObject *object, objects)
    {
        if (object == id)
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
    if (propertyName == "name" || propertyName == "create date time" || propertyName == "id" || propertyName == "parent")
    {
        return -2;
    }

    foreach (GuidedObject *object, objects)
    {
        if (object == id)
        {
            object->setProperty(propertyName.toUtf8(), value);
            return 0;
        }
    }

    return -1;
}

int Manager::setParent(QByteArray objectId, QByteArray parentId)
{
    if (objectId == parentId)
    {
        return -3;
    }
    GuidedObject *obj {nullptr}, *parent {nullptr};
    foreach (GuidedObject *object, objects)
    {
        if (object == objectId)
        {
            obj = object;
        }
        else if (object == parentId)
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
    else if (parent == nullptr)
    {
        return -2;
    }
    else
    {
        obj->setParent(parent);
        return 0;
    }
}
