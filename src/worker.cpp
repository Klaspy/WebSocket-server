#include "worker.h"

Worker::Worker(QObject *parent)
    : QObject{parent}
{}

QByteArray Worker::processRequest(QByteArray request)
{
    QList<RpcRequest> parsedRequests = JsonParser::parseRequest(request);

    QList<int> validRequests;

    for (int i = 0; i < parsedRequests.size(); i++)
    {
        if (parsedRequests.at(i).parseError == RpcErrors::NoError)
        {
            validRequests.append(i);
        }
    }
}
