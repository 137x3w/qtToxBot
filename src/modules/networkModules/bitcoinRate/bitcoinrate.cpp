#include "bitcoinrate.h"

#include <QObject>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "src/modules/networkModules/anetworkmodule.h"

BitcoinRate::BitcoinRate(QObject *parent) : ANetworkModule(parent)
{
//    patterns.append({
//        QRegExp("bitcoinRate"),
//        COMMANDS::DEFAULT_COMMAND,
//        "https://blockchain.info/ru/ticker"
//    });
}

void BitcoinRate::requestService(QString args, int routerIndex, int xprIndex)
{
    switch(router[routerIndex].cmd) {
    case COMMANDS::BITCOIN_RATE:
    {
        this->net->getRequestSlot(router[routerIndex].linkTemplate);
        break;
    }
    default: break;
    }
}

void BitcoinRate::parseBitcoinRateResult(QByteArray result)
{
    if(!result.isEmpty()) {
        QString resultString = "";
        QJsonDocument itemDoc = QJsonDocument::fromJson(result);
        QJsonObject rootObject = itemDoc.object();
        resultString = QString::number(rootObject["USD"].toObject()["last"].toDouble()) + " USD";
        requestServiceResultSignal(this, resultString);
    } else {
        requestServiceResultSignal(this, "Oh shit im sorry..");
    }
}


void BitcoinRate::handleNetworkDataSlot(QByteArray data)
{
    switch(activeCommand) {
    case COMMANDS::BITCOIN_RATE: {
        parseBitcoinRateResult(data);
        break;
    }
    default: break;
    }
}

void BitcoinRate::requestTimeoutSlot()
{
    requestServiceResultSignal(this, "Request timeout.");
}

void BitcoinRate::requestErrorSlot()
{
    requestServiceResultSignal(this, "Oh shit im sorry.");
}
