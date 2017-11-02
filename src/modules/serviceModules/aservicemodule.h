#ifndef ASERVICEMODULE_H
#define ASERVICEMODULE_H

#include "src/modules/amodule.h"
#include "src/modules/serviceModules/iservicemodule.h"

class AModule;
class IServiceModule;

class AServiceModule: public AModule, public IServiceModule
{
    Q_OBJECT
public:
    AServiceModule(QObject *parent = nullptr);
    ~AServiceModule();
    virtual void requestService(QString args, int routerIndex, int xprIndex) = 0;
};

#endif // ASERVICEMODULE_H
