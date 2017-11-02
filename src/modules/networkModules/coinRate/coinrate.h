#ifndef COINRATE_H
#define COINRATE_H

#include <QObject>

#include "src/modules/networkModules/anetworkmodule.h"

class CoinRate : public ANetworkModule
{
    Q_OBJECT
public:
    explicit CoinRate(QObject *parent = nullptr);
    enum COMMANDS {
        COIN_RATE
    };
    void requestService(QString args, int routerIndex, int xprIndex);
private:
    COMMANDS activeCommand = COMMANDS::COIN_RATE;
    QString coinName;
    void parseCoinRateResult(QByteArray result);
public slots:
    void handleNetworkDataSlot(QByteArray data);
private slots:
    void requestTimeoutSlot();
    void requestErrorSlot();
};

#endif // COINRATE_H
