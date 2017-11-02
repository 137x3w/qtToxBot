#ifndef AINTERACTIVEMODULE_H
#define AINTERACTIVEMODULE_H

#include "src/modules/amodule.h"

class AInteractiveModule : public AModule
{
    Q_OBJECT
public:
    AInteractiveModule(QObject *parent = nullptr);
    virtual void requestService(QString args, int routerIndex, int xprIndex) = 0;
};

#endif // AINTERACTIVEMODULE_H
