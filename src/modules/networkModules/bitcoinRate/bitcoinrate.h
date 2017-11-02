#ifndef BITCOINRATE_H
#define BITCOINRATE_H

#include <QObject>

#include "src/modules/networkModules/anetworkmodule.h"

class BitcoinRate : public ANetworkModule
{
    Q_OBJECT
public:
    explicit BitcoinRate(QObject *parent = nullptr);
    enum COMMANDS {
        BITCOIN_RATE
    };
    void requestService(QString args, int routerIndex, int xprIndex);
private:
    COMMANDS activeCommand = BITCOIN_RATE;
    void parseBitcoinRateResult(QByteArray result);
public slots:
    void handleNetworkDataSlot(QByteArray data);
private slots:
    virtual void requestTimeoutSlot();
    virtual void requestErrorSlot();
};

#endif // BITCOINRATE_H
