#ifndef ISERVICEMODULE_H
#define ISERVICEMODULE_H

#include "src/modules/imodule.h"

class IModule;

class IServiceModule : public IModule
{
public:
    IServiceModule() {}
    ~IServiceModule() {}
};

#endif // ISERVICEMODULE_H
