#include <QCoreApplication>
#include "src/server.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Server *server = new Server;

    return a.exec();
}
