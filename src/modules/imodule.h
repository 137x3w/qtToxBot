#ifndef IMODULE_H
#define IMODULE_H

#include <QObject>

class IModule
{
public:
    IModule() {}
    ~IModule() {}
    virtual void requestService(QString args, int routerIndex, int xprIndex) = 0;
};

#endif // IMODULE_H
