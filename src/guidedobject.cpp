#include "guidedobject.h"

GuidedObject::GuidedObject(QObject *parent)
    : QObject{parent}
{
    m_createDT = QDateTime::currentDateTime().toString();
    m_id = QByteArray::fromHex(QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch(), 16).toUtf8());
}

GuidedObject::GuidedObject(QString name, QObject *parent)
    : QObject{parent}
{
    m_name = name;
    m_createDT = QDateTime::currentDateTime().toString();
    m_id = QByteArray::fromHex(QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch(), 16).toUtf8());
}

GuidedObject::GuidedObject(QString name, QString createDT, QByteArray id, QObject *parent)
    : QObject{parent}
{
    m_name = name;
    m_createDT = createDT;
    m_id = id;
}

QString GuidedObject::name() const
{
    return m_name;
}

void GuidedObject::setName(const QString &newName)
{
    if (m_name == newName)
        return;
    m_name = newName;
    emit nameChanged();
}

QString GuidedObject::createDT() const
{
    return m_createDT;
}

QByteArray GuidedObject::id() const
{
    return m_id;
}

QVariantMap GuidedObject::getCustomProperties()
{
    QVariantMap customPropertiesMap;

    foreach (QByteArray name, dynamicPropertyNames())
    {
        customPropertiesMap.insert(name, property(name));
    }

    return customPropertiesMap;
}
