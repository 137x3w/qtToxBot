#include "coinrate.h"

#include <QObject>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "src/modules/networkModules/anetworkmodule.h"

CoinRate::CoinRate(QObject *parent) : ANetworkModule(parent)
{
//    patterns.append({
//        QRegExp("(\\S.*)+"),
//        COMMANDS::NO_COMMAND,
//        "https://api.coinmarketcap.com/v1/ticker/%1/"
//    });
}

void CoinRate::requestService(QString args, int routerIndex, int xprIndex)
{
    router[routerIndex].xprs[xprIndex].indexIn(args);
    switch(router[routerIndex].cmd) {
    case COMMANDS::COIN_RATE:
    {
        coinName = router[routerIndex].xprs[xprIndex].cap(1);
        this->net->getRequestSlot(router[routerIndex].linkTemplate.arg(coinName));
        break;
    }
    default: break;
    }
}

void CoinRate::parseCoinRateResult(QByteArray result)
{
    if(!result.isEmpty()) {
        QString resultString = "";
        QJsonDocument itemDoc = QJsonDocument::fromJson(result);
        QJsonObject rootObject = itemDoc.array()[0].toObject();
        resultString += rootObject["price_usd"].toString() + " USD";
        requestServiceResultSignal(this, resultString);
    } else {
        requestServiceResultSignal(this, "Oh shit im sorry..");
    }
}

void CoinRate::handleNetworkDataSlot(QByteArray data)
{
    switch(activeCommand) {
    case COMMANDS::COIN_RATE: {
        parseCoinRateResult(data);
        break;
    }
    default: break;
    }
}

void CoinRate::requestTimeoutSlot()
{
    requestServiceResultSignal(this, "Request timeout.");
}

void CoinRate::requestErrorSlot()
{
    requestServiceResultSignal(this, "Oh shit im sorry.");
}


