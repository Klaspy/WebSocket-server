#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <QObject>

class JsonParser : public QObject
{
    Q_OBJECT
public:
    explicit JsonParser(QObject *parent = nullptr);

signals:
};

#endif // JSONPARSER_H
