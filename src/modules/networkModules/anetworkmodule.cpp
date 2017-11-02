#include "anetworkmodule.h"

#include "src/modules/amodule.h"
#include "src/modules/networkModules/inetworkmodule.h"

ANetworkModule::ANetworkModule(QObject *parent) : AModule(parent), INetworkModule()
{
    this->net = new ANetwork();
    connect(this->net, &ANetwork::getRequestResultSignal, this, &ANetworkModule::handleNetworkDataSlot, Qt::QueuedConnection);
    connect(this->net, &ANetwork::getRequestTimeoutSignal, this, &ANetworkModule::requestTimeoutSlot, Qt::QueuedConnection);
    connect(this->net, &ANetwork::getRequestErrorSignal, this, &ANetworkModule::requestErrorSlot, Qt::QueuedConnection);
}

ANetworkModule::~ANetworkModule()
{

}

void ANetworkModule::requestService(QString args)
{
    qDebug() << "router size ANetworkModule: " << router.size();
    for(int i = 0; i < router.size(); i++) {
        for(int j = 0; j < router[i].xprs.size(); j++) {
            if(router[i].xprs[j].exactMatch(args)) {
                requestService(args, i, j);
            }
        }
    }
}

void ANetworkModule::setNetwork(ANetwork *net)
{
    disconnect(this->net, &ANetwork::getRequestResultSignal, this, &ANetworkModule::handleNetworkDataSlot);
    disconnect(this->net, &ANetwork::getRequestTimeoutSignal, this, &ANetworkModule::requestTimeoutSlot);
    disconnect(this->net, &ANetwork::getRequestErrorSignal, this, &ANetworkModule::requestErrorSlot);
    this->net = net;
    connect(this->net, &ANetwork::getRequestResultSignal, this, &ANetworkModule::handleNetworkDataSlot, Qt::QueuedConnection);
    connect(this->net, &ANetwork::getRequestTimeoutSignal, this, &ANetworkModule::requestTimeoutSlot, Qt::QueuedConnection);
    connect(this->net, &ANetwork::getRequestErrorSignal, this, &ANetworkModule::requestErrorSlot, Qt::QueuedConnection);
}

void ANetworkModule::configureRouter(QVector<ANetworkModule::RegExpRouter> newrouter)
{
    router.clear();
    router = newrouter;
}
