#include "cryptohash.h"

#include <QCryptographicHash>

#include "src/modules/interactiveModules/ainteractivemodule.h"

CryptoHash::CryptoHash(QObject *parent) : AInteractiveModule(parent)
{
//    patterns.append({
//        QRegExp("sha256 (.*)+"),
//        COMMANDS::SHA256
//    });
}

void CryptoHash::requestService(QString args, int routerIndex, int xprIndex)
{
    router[routerIndex].xprs[xprIndex].indexIn(args);
    switch(router[routerIndex].cmd) {
    case COMMANDS::SHA256:
    {
        int hashCount = router[routerIndex].xprs[xprIndex].cap(1).toInt();
        QString result = router[routerIndex].xprs[xprIndex].cap(2);
        QString temp = result;
        for(int i = 0; i < hashCount; i++) {
            temp = sha256(temp);
            result = result + "\n" + temp;
        }
        requestServiceResultSignal(this, result);
        break;
    }
    default: break;
    }
}

QString CryptoHash::sha256(QString inputString)
{
    return QString(QCryptographicHash::hash(inputString.toUtf8(), QCryptographicHash::Sha256).toHex());
}

