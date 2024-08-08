#include <QCoreApplication>
#include "src/server.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Server *server = new Server(&a);
    Q_UNUSED(server)

    return a.exec();
}
