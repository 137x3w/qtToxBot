#include "coinexchange.h"

#include <QCryptographicHash>

#include <sodium.h>

#include "src/modules/interactiveModules/ainteractivemodule.h"

CoinExchange::CoinExchange(QObject *parent) : AInteractiveModule(parent)
{

}

void CoinExchange::requestService(QString args, int routerIndex, int xprIndex)
{
    router[routerIndex].xprs[xprIndex].indexIn(args);
    switch(router[routerIndex].cmd) {

    }
}

ProtectedControl *CoinExchange::setBotInstance(AdoxaBot *bot)
{
    this->bot = bot;
    return this;
}
