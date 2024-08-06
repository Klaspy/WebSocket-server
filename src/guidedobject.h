#ifndef GUIDEDOBJECT_H
#define GUIDEDOBJECT_H

#include <QObject>
#include <QDateTime>
#include <QVariantMap>

class GuidedObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString    name     READ name WRITE setName NOTIFY nameChanged FINAL)
    Q_PROPERTY(QString    createDT READ createDT                     CONSTANT FINAL)
    Q_PROPERTY(QByteArray id       READ id                           CONSTANT FINAL)

public:
    explicit GuidedObject(QObject *parent = nullptr);
    explicit GuidedObject(QString name, QObject *parent = nullptr);
    explicit GuidedObject(QString name, QString createDT, QByteArray id,  QObject *parent = nullptr);

    QString name() const;
    void setName(const QString &newName);

    QString createDT() const;

    QByteArray id() const;

    QVariantMap getCustomProperties();

    bool operator ==(const QByteArray &id) const;

private:
    QString m_name;
    QString m_createDT;
    QByteArray m_id;


signals:
    void nameChanged();
    void customPropertyChanged(QString propertyName, QVariant propertyValue);
};

#endif // GUIDEDOBJECT_H
