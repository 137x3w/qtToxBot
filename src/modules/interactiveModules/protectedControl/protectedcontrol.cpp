#include "protectedcontrol.h"

#include <QCryptographicHash>

#include <sodium.h>

#include "src/modules/interactiveModules/ainteractivemodule.h"

#include <QDebug>

ProtectedControl::ProtectedControl(QObject *parent) : AInteractiveModule(parent)
{
//    patterns.append({
//        QRegExp("control (\\S.*)+"),
//        COMMANDS::CONTROL
//    });

//    patterns.append({
//        QRegExp("pkey"),
//        COMMANDS::KEY_INFO
//    });

    crypto_box_keypair(keys.pKey, keys.sKey);
}

void ProtectedControl::requestService(QString args, int routerIndex, int xprIndex)
{
    router[routerIndex].xprs[xprIndex].indexIn(args);
    switch(router[routerIndex].cmd) {
    case COMMANDS::CONTROL:
    {
        QString decryptedMsg = decryptB64msg(router[routerIndex].xprs[xprIndex].cap(1));
        if(decryptedMsg.isEmpty()) {
            requestServiceResultSignal(this, "Oh shit...");
        } else {
            if(updateToken(decryptedMsg)) {
                QString res = getContainedMessage(decryptedMsg);
                requestServiceResultSignal(this, res.isEmpty() ? "Oh shit, invalid message." : res);
            } else {
                requestServiceResultSignal(this, "Oh shit, token is bad(");
            }
        }
        break;
    }
    case COMMANDS::KEY_INFO:
    {
        requestServiceResultSignal(this, QString(QByteArray::fromRawData((const char*)keys.pKey, crypto_box_PUBLICKEYBYTES).toBase64()));
        break;
    }
    case COMMANDS::SET_CONTROL_KEY:
    {
        bot->setControlKey(router[routerIndex].xprs[xprIndex].cap(1));
        requestServiceResultSignal(this, "New control key is: " + router[routerIndex].xprs[xprIndex].cap(1));
        break;
    }
    case COMMANDS::GET_CONTROL_KEY:
    {
        requestServiceResultSignal(this, bot->getControlKey());
        break;
    }
    default: break;
    }
}

ProtectedControl *ProtectedControl::setBotInstance(AdoxaBot *bot)
{
    this->bot = bot;
    return this;
}

QString ProtectedControl::decryptB64msg(QString b64emsg)
{
    QByteArray byteEmsg;
    byteEmsg = QByteArray::fromBase64(byteEmsg.append(b64emsg));
    unsigned char decrypted[(byteEmsg.size() == 0 ? 1 : byteEmsg.size())]{0};
    if (crypto_box_seal_open(decrypted, (const unsigned char*)byteEmsg.constData(), byteEmsg.size(), keys.pKey, keys.sKey) != 0) {
        return "";
    } else {
        return QString::fromUtf8((const char*)decrypted);
    }

}

bool ProtectedControl::updateToken(QString dmsg)
{
    if(regToken.exactMatch(dmsg)) {
        regToken.indexIn(dmsg);
        if(!QString::compare(regToken.cap(1), token, Qt::CaseInsensitive)) {
            token = QCryptographicHash::hash(regToken.cap(1).toUtf8(), QCryptographicHash::Sha224).toHex();
            qDebug() << token;
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

QString ProtectedControl::getContainedMessage(QString dmsg)
{
    if(regToken.exactMatch(dmsg)) {
        regToken.indexIn(dmsg);
        return regToken.cap(2);
    } else {
        return "";
    }
}
