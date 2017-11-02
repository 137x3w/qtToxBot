#include <QCoreApplication>
#include <QThread>
#include <QDebug>

#include "toxcore.h"
#include "tox/toxav.h"

#include "adoxabot.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    AdoxaBot* ab = new AdoxaBot();


    return a.exec();
}
