#ifndef INETWORKMODULE_H
#define INETWORKMODULE_H

#include <QObject>

#include "src/modules/imodule.h"
#include "src/modules/networkModules/anetwork.h"

class IModule;
class ANetwork;

class INetworkModule : public IModule
{
public:
    INetworkModule() {}
    ~INetworkModule() {}
    virtual void setNetwork(ANetwork* net) = 0;
public slots:
    virtual void handleNetworkDataSlot(QByteArray data) = 0;
};

#endif // INETWORKMODULE_H
