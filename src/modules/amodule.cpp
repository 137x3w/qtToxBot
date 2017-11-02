#include "amodule.h"

#include <QDebug>

AModule::AModule(QObject *parent) : QObject(parent), IModule()
{

}

AModule::~AModule()
{

}

void AModule::requestService(QString args)
{
    qDebug() << "router size AModule: " << router.size();
    for(int i = 0; i < router.size(); i++) {
        for(int j = 0; j < router[i].xprs.size(); j++) {
            if(router[i].xprs[j].exactMatch(args)) {
                requestService(args, i, j);
            }
        }
    }
}

void AModule::configureRouter(QVector<AModule::RegExpRouter> newrouter)
{
    router.clear();
    router = newrouter;
}
