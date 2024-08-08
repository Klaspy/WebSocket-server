#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>

#include "guidedobject.h"
#include "jsonparser.h"

class Manager : public QObject
{
    Q_OBJECT
public:
    explicit Manager(QObject *parent = nullptr);

    QList<QPair<QString, QByteArray>> getAll();

    void addNew(QString name);
    bool removeObject(QByteArray id);

    QVariantMap getObjectInfo(QByteArray id);
    bool        renameObject(QByteArray id, QString name);
    QVariant    getCustomProperty(QByteArray id, QString propertyName);
    int         setCustomProperty(QByteArray id, QString propertyName, QVariant value);
    int         setParent(QByteArray objectId, QVariant parentId_);

private:
    QList<QPointer<GuidedObject>> objects;

    void autoSave();

signals:
};

#endif // MANAGER_H
