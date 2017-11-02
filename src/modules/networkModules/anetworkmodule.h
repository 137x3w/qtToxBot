#ifndef ANETWORKMODULE_H
#define ANETWORKMODULE_H

#include <QObject>

#include "src/modules/amodule.h"
#include "src/modules/networkModules/inetworkmodule.h"

class ANetwork;
class INetworkModule;

class ANetworkModule : public AModule, public INetworkModule
{
    Q_OBJECT
public:
    explicit ANetworkModule(QObject *parent = nullptr);
    ~ANetworkModule();
    struct RegExpRouter
    {
        int cmd;
        QVector<QRegExp> xprs;
        QString linkTemplate;
    };
    virtual void requestService(QString args);
    virtual void requestService(QString args, int routerIndex, int xprIndex) = 0;
    virtual void setNetwork(ANetwork *net);
    virtual void configureRouter(QVector<ANetworkModule::RegExpRouter> newrouter);
protected:
    QVector<ANetworkModule::RegExpRouter> router;
    ANetwork* net;
public slots:
    virtual void handleNetworkDataSlot(QByteArray data) = 0;
private slots:
    virtual void requestTimeoutSlot() = 0;
    virtual void requestErrorSlot() = 0;
signals:
    void requestServiceResultSignal(AModule* moduleName, QString preparedStr);
};

#endif // ANETWORKMODULE_H
