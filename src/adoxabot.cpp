#include <QRegExp>
#include <QThread>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QByteArray>
#include <QString>
#include <QUrl>
#include <QVector>
#include <QHash>
#include <QHashIterator>
#include <QCryptographicHash>

#include <QDebug>

#include "tox/toxav.h"
#include "tox/tox.h"

#include "adoxabot.h"
#include "toxcore.h"
#include "network.h"
#include "messageparser.h"

#include "src/modules/imodule.h"
#include "src/modules/networkModules/anetwork.h"
#include "src/modules/networkModules/browser2ch/browser2ch.h"
#include "src/modules/networkModules/bitcoinRate/bitcoinrate.h"
#include "src/modules/networkModules/googleSearch/googlesearch.h"
#include "src/modules/networkModules/wikiSearch/wikisearch.h"
#include "src/modules/networkModules/weatherCondition/weathercondition.h"
#include "src/modules/networkModules/coinRate/coinrate.h"
#include "src/modules/networkModules/browser4ch/browser4ch.h"
#include "src/modules/interactiveModules/cryptoHash/cryptohash.h"
#include "src/modules/interactiveModules/protectedControl/protectedcontrol.h"
#include "src/modules/interactiveModules/botInfo/botinfo.h"
#include "src/modules/serviceModules/commandPrefix/commandprefix.h"
#include "src/modules/serviceModules/contactManagement/contactmanagement.h"

AdoxaBot::AdoxaBot(QObject *parent) : QObject(parent)
{
    QThread* coreThread = new QThread();
    toxCore = new ToxCore();
    toxCore->moveToThread(coreThread);

    messageParser = new MessageParser();

    network = new ANetwork();

    controlKey = "643aec5a590257a2b2881021e86bf2a459b98793192993124a796237104377e1";
    controlKeyPattern = QRegExp("key ([a-f0-9]{64})");
    listenedConferenceId = 0;

    QHash<QString,QVector<ANetworkModule::RegExpRouter>> networkModulesConfig
    {
        {
            "GoogleSearch",
            QVector<ANetworkModule::RegExpRouter>() << ANetworkModule::RegExpRouter {
                GoogleSearch::GOOGLE_SEARCH,
                QVector<QRegExp>() << QRegExp("google (\\S.*)"),
                QString("https://www.googleapis.com/customsearch/v1?key=AIzaSyCCocbrnMGkjkIt0vznidmxjxAcKFqbtkk&cx=001354305087942324278:vtfb5njh_1q&q=%1&hl=ru&num=1")
            }
        },
        {
            "BitcoinRate",
            QVector<ANetworkModule::RegExpRouter>() << ANetworkModule::RegExpRouter {
                BitcoinRate::BITCOIN_RATE,
                QVector<QRegExp>() << QRegExp("bitcoinRate( \S.*)?"), // ( .*)? - for control key parsing
                QString("https://blockchain.info/ru/ticker")
            }
        },
        {
            "Browser2ch",
            QVector<ANetworkModule::RegExpRouter>() << ANetworkModule::RegExpRouter {
                Browser2ch::GET_POST,
                QVector<QRegExp>() << QRegExp("2ch getPost (\\S.*) (\\S\\d*)( \S.*)?"),
                QString("https://2ch.hk/makaba/mobile.fcgi?task=get_post&board=%1&post=%2")
            } <<  ANetworkModule::RegExpRouter {
                Browser2ch::GET_THREAD,
                QVector<QRegExp>() << QRegExp("2ch getThread (\\S.*) ([0-9]) ([0-9])( \S.*)?"),
                QString("https://2ch.hk/%1/%2.json")
            }
        },
        {
            "Browser4ch",
            QVector<ANetworkModule::RegExpRouter>() << ANetworkModule::RegExpRouter {
                Browser4ch::GET_THREAD,
                QVector<QRegExp>() << QRegExp("4ch getThread (\\S.*) ([0-9]) ([0-9])( \S.*)?") << QRegExp("4ch (\\S.*) ([0-9]) ([0-9])( \S.*)?"),
                QString("https://a.4cdn.org/%1/%2.json")
            }
        },
        {
            "CoinRate",
            QVector<ANetworkModule::RegExpRouter>() << ANetworkModule::RegExpRouter {
                CoinRate::COIN_RATE,
                QVector<QRegExp>() << QRegExp("coinRate (\\S.*)"),
                QString("https://api.coinmarketcap.com/v1/ticker/%1/")
            }
        },
        {
            "WikiSearch",
            QVector<ANetworkModule::RegExpRouter>() << ANetworkModule::RegExpRouter {
                WikiSearch::WIKI_SEARCH,
                QVector<QRegExp>() << QRegExp("wiki (\\S.*) (\\S.*) ([0-9]+)( \S.*)?") << QRegExp("wiki (\\S.*) ([0-9]+)( \S.*)?") << QRegExp("wiki (\\S.*) (\\S.*)( \S.*)?") << QRegExp("wiki (\\S.*)"),
                QString("https://%2.wikipedia.org/w/api.php?action=opensearch&search=%1&prop=info&format=json&inprop=url&limit=%3")
            }
        },
        {
            "WeatherCondition",
            QVector<ANetworkModule::RegExpRouter>() << ANetworkModule::RegExpRouter {
                WeatherCondition::CURRENT_WEATHER,
                QVector<QRegExp>() << QRegExp("weather (\\S.*)"),
                QString("http://api.openweathermap.org/data/2.5/weather?q=%1&appid=9d45016c60f4bfa839ae7d0b0c557765&units=%2&lang=%3")
            }
        }
    };

    QHash<QString,QVector<AModule::RegExpRouter>> interactiveModulesConfig
    {
        {
            "CryptoHash",
            QVector<AModule::RegExpRouter>() << AModule::RegExpRouter {
                CryptoHash::SHA256,
                QVector<QRegExp>() << QRegExp("sha256 ([1-9]) (\\S.*)")
            }
        },
        {
            "ProtectedControl",
            QVector<AModule::RegExpRouter>()/*<< AModule::RegExpRouter {
                ProtectedControl::CONTROL,
                QVector<QRegExp>() << QRegExp("control (\\S.*)")
            } << QVector<AModule::RegExpRouter>() << AModule::RegExpRouter {
                ProtectedControl::KEY_INFO,
                QVector<QRegExp>() << QRegExp("pkey")
            }*/ << QVector<AModule::RegExpRouter>() << AModule::RegExpRouter {
                ProtectedControl::SET_CONTROL_KEY,
                QVector<QRegExp>() << QRegExp("setControlKey ([a-f0-9]{64})( \S.*)?")
            } << QVector<AModule::RegExpRouter>() << AModule::RegExpRouter {
                ProtectedControl::GET_CONTROL_KEY,
                QVector<QRegExp>() << QRegExp("getControlKey( \S.*)?")
            }
        },
        {
            "BotInfo",
            QVector<AModule::RegExpRouter>() << AModule::RegExpRouter {
                BotInfo::HELP_INFO,
                QVector<QRegExp>() << QRegExp("help( \S.*)?")
            } << QVector<AModule::RegExpRouter>() << AModule::RegExpRouter {
                BotInfo::BOT_TOX_ID_INFO,
                QVector<QRegExp>() << QRegExp("id( \S.*)?")
            } << QVector<AModule::RegExpRouter>() << AModule::RegExpRouter {
                BotInfo::CONFERENCE_INFO,
                QVector<QRegExp>() << QRegExp("conferenceInfo( \S.*)?")
            }
        }
    };

    QHash<QString,QVector<AModule::RegExpRouter>> serviceModulesConfig
    {
        {
            "CommandPrefix",
            QVector<AModule::RegExpRouter>() << AModule::RegExpRouter {
                CommandPrefix::CHANGE_MAIN_PREFIX,
                QVector<QRegExp>() << QRegExp("setPrefix (\\S.*)")
            } << QVector<AModule::RegExpRouter>() << AModule::RegExpRouter {
                CommandPrefix::SET_PREFIX_DEFAULT,
                QVector<QRegExp>() << QRegExp("setPrefixDefault( \S.*)?")
            }
        },
        {
            "ContactManagement",
            QVector<AModule::RegExpRouter>() << AModule::RegExpRouter {
                ContactManagement::LISTEN_CONFERENCE,
                QVector<QRegExp>() << QRegExp("listenConference ([0-9]+)( \S.*)?")
            } << QVector<AModule::RegExpRouter>() << AModule::RegExpRouter {
                ContactManagement::LEAVE_CONFERENCE,
                QVector<QRegExp>() << QRegExp("leaveConference ([0-9]+)( \S.*)?")
            }
        }
    };

    createModule("GoogleSearch", new GoogleSearch(), modules.modulesMap, networkModulesConfig.value("GoogleSearch"));
    createModule("BitcoinRate", new BitcoinRate(), modules.modulesMap, networkModulesConfig.value("BitcoinRate"));
    createModule("Browser2ch", new Browser2ch(), modules.modulesMap, networkModulesConfig.value("Browser2ch"));
    createModule("Browser4ch", new Browser4ch(), modules.modulesMap, networkModulesConfig.value("Browser4ch"));
    createModule("CoinRate", new CoinRate(), modules.modulesMap, networkModulesConfig.value("CoinRate"));
    createModule("WikiSearch", new WikiSearch(), modules.modulesMap, networkModulesConfig.value("WikiSearch"));
    createModule("WeatherCondition", new WeatherCondition(), modules.modulesMap, networkModulesConfig.value("WeatherCondition"));

    createModule("CryptoHash", new CryptoHash(), modules.modulesMap, interactiveModulesConfig.value("CryptoHash"));
    createModule("ProtectedControl", (new ProtectedControl())->setBotInstance(this), modules.modulesMap, interactiveModulesConfig.value("ProtectedControl"));
    createModule("BotInfo", (new BotInfo())->setBotInstance(this), modules.modulesMap, interactiveModulesConfig.value("BotInfo"));

    createModule("CommandPrefix", (new CommandPrefix())->setParser(messageParser), modules.modulesMap, serviceModulesConfig.value("CommandPrefix"));
    createModule("ContactManagement", (new ContactManagement())->setBotInstance(this), modules.modulesMap, serviceModulesConfig.value("ContactManagement"));

    QHashIterator<AModule*,Module> iMod(modules.modulesMap);

    while(iMod.hasNext()) {
        iMod.next();
        int intSize = interactiveModulesConfig.value(iMod.value().moduleName).size();
        int netSize = networkModulesConfig.value(iMod.value().moduleName).size();
        int serSize = serviceModulesConfig.value(iMod.value().moduleName).size();

        for(int i = 0; i < intSize; i++) {
            messageParser->insertPattern(iMod.value().module, interactiveModulesConfig.value(iMod.value().moduleName).at(i).xprs);
        }

        for(int i = 0; i < netSize; i++) {
            messageParser->insertPattern(iMod.value().module, networkModulesConfig.value(iMod.value().moduleName).at(i).xprs);
        }

        for(int i = 0; i < serSize; i++) {
            messageParser->insertPattern(iMod.value().module, serviceModulesConfig.value(iMod.value().moduleName).at(i).xprs);
        }
    }

    toxCore->setNodes(QVector<ToxCore::ToxNode>() <<
                      ToxCore::ToxNode{"178.62.250.138", 33445, "788236D34978D1D5BD822F0A5BEBD2C53C64CC31CD3149350EE27D4D9A2F9B6B"} <<
                      ToxCore::ToxNode{"130.133.110.14", 33445, "461FA3776EF0FA655F1A05477DF1B3B614F7D6B124F7DB1DD4FE3C08B03B640F"} <<
                      ToxCore::ToxNode{"128.199.199.197",33445, "B05C8869DBB4EDDD308F43C1A974A20A725A36EACCA123862FDE9945BF9D3E09"} <<
                      ToxCore::ToxNode{"146.185.136.123",33445, "09993FAF174DFFDC515B398A2EFC5639C4E6D7B736FC864F89786B50EAF88C1A"} <<
                      ToxCore::ToxNode{"193.124.186.205",5228,  "9906D65F2A4751068A59D30505C5FC8AE1A95E0843AE9372EAFA3BAB6AC16C2C"} <<
                      ToxCore::ToxNode{"185.25.116.107", 33445, "DA4E4ED4B697F2E9B000EEFE3A34B554ACD3F45F5C96EAEA2516DD7FF9AF7B43"} <<
                      ToxCore::ToxNode{"5.189.176.217",  33445, "2B2137E094F743AC8BD44652C55F41DFACC502F125E99E4FE24D40537489E32F"} <<
                      ToxCore::ToxNode{"46.101.197.175", 443,   "CD133B521159541FB1D326DE9850F5E56A6C724B5B8E5EB5CD8D950408E95707"});

    connect(toxCore, &ToxCore::conferenceMessageSignal, this, &AdoxaBot::conferenceMessageSlot);
    connect(toxCore, &ToxCore::friendRequestSignal, this, &AdoxaBot::friendRequestSlot);
    connect(toxCore, &ToxCore::friendMessageSignal, this, &AdoxaBot::friendMessageSlot);
    connect(toxCore, &ToxCore::selfConnectionStatusChangedSignal, this, &AdoxaBot::selfConnectionStatusChangedSlot);
    connect(toxCore, &ToxCore::conferenceInviteSignal, this, &AdoxaBot::conferenceInviteSlot);

    QObject::connect(coreThread, &QThread::started, toxCore, [=]() {
        toxCore->initToxOptionsDefault();
        toxCore->makeTox();
        toxCore->bootstrapDht();
        toxCore->startToxCore();
        toxCore->setName("AdoxaBot");
    });

    connect(this, &AdoxaBot::conferenceMessageSignal, messageParser, &MessageParser::parseConferenceMessageSlot);
    connect(messageParser, SIGNAL(parseSignal(AModule*,QString)), this, SLOT(parseMessageSlot(AModule*,QString)));

    coreThread->start();
}

QString AdoxaBot::getToxId() const
{
    return toxCore->getSelfAddress();
}

void AdoxaBot::setControlKey(QString newKey)
{
    controlKey = newKey;
    qDebug() << "new control key " << newKey;
}

QString AdoxaBot::getControlKey() const
{
    return controlKey;
}

QVector<uint32_t> AdoxaBot::getConferenceIds() const
{
    return toxCore->conferenceGetChatlist();
}

QString AdoxaBot::getConferenceTitle(uint32_t id) const
{
    return toxCore->conferenceGetTitle(id);
}

bool AdoxaBot::listenConference(uint32_t id)
{
    //add conference validation
    if(getConferenceIds().length() > id) {
        listenedConferenceId = id;
        return true;
    } else return false;
}

bool AdoxaBot::leaveConference(uint32_t id)
{
    //add conference validation
    if(getConferenceIds().length() > id) {
        if(toxCore->conferenceDelete(id)) {
            //add a listen conference handle
            return true;
        }
    }
    return false;
}

bool AdoxaBot::validateControlKey(QString &query)
{
    if(controlKeyPattern.indexIn(query) != -1) {
        QString newKey = controlKeyPattern.cap(0).toLower();
        newKey = controlKeyPattern.cap(1);
        QString queryKey = QCryptographicHash::hash(newKey.toUtf8(), QCryptographicHash::Sha256).toHex();
        if(!QString::compare(queryKey, controlKey, Qt::CaseInsensitive)) {
            controlKey = newKey;
            query = query.left(controlKeyPattern.pos(0));
            return true;
        }
    }
    return false;
}

void AdoxaBot::createModule(QString moduleName, ANetworkModule *module, QHash<AModule*, AdoxaBot::Module> &moduleStorage, QVector<ANetworkModule::RegExpRouter> routes, bool protectedAccess)
{
    module->setNetwork(network);
    module->configureRouter(routes);
    AdoxaBot::Module net;
    net.protectedAccess = protectedAccess;
    net.module = module;
    net.moduleName = moduleName;
    moduleStorage.insert(module, net);
}

void AdoxaBot::createModule(QString moduleName, AModule *module, QHash<AModule*, AdoxaBot::Module> &moduleStorage, QVector<AModule::RegExpRouter> routes, bool protectedAccess)
{
    module->configureRouter(routes);
    AdoxaBot::Module mod;
    mod.protectedAccess = protectedAccess;
    mod.module = module;
    mod.moduleName = moduleName;
    moduleStorage.insert(module, mod);
}

void AdoxaBot::conferenceMessageSlot(uint32_t conference_number, uint32_t peer_number, TOX_MESSAGE_TYPE type, QByteArray message)
{
    if(conference_number == listenedConferenceId) {
        QString messageStr = QString::fromUtf8(message);
        qDebug() << "new conf mess: " << messageStr;
        conferenceMessageSignal(messageStr);
    }
}

void AdoxaBot::selfConnectionStatusChangedSlot(TOX_CONNECTION connection_status)
{
    toxCore->printInfo();

    switch (connection_status) {
        case TOX_CONNECTION_NONE:
            qDebug() << "Connection to Tox network has been lost";
            break;

        case TOX_CONNECTION_TCP:
            qDebug() << "Connection to Tox network is weak (using TCP)";
            break;

        case TOX_CONNECTION_UDP:
            qDebug() << "Connection to Tox network is strong (using UDP)";
            break;
    }
}

void AdoxaBot::friendRequestSlot(const uint8_t *public_key, const uint8_t *data, size_t length)
{
    toxCore->friendAddNorequest(public_key);
}

void AdoxaBot::friendMessageSlot(uint32_t friendId, TOX_MESSAGE_TYPE type, QByteArray message)
{

}

void AdoxaBot::conferenceInviteSlot(uint32_t friendnumber, TOX_CONFERENCE_TYPE type, const uint8_t *cookie, size_t length)
{
    char name[TOX_MAX_NAME_LENGTH] = {0};
    int groupnum = -1;
    toxCore->getFriendName(friendnumber, (uint8_t *) name);
    size_t len = toxCore->getFriendNameSize(friendnumber);
    name[(len <= 0 ? 1 : len)] = '\0';

    QByteArray confCookie = QByteArray::fromRawData((char*)cookie, length);
    qDebug() << "Conference cookie: " << confCookie.toHex();
    qDebug() << "Cookie length: " << length;

//    if(toxCore->conferenceGetChatlistSize()) {
//        QVector<uint32_t> confList = toxCore->conferenceGetChatlist();
//        for(int i = 0; i < confList.size(); i++) {
//            if(!toxCore->conferenceDelete(confList.at(i))) {
//                qDebug() << "Error delete conference " << confList.at(i);
//            }
//        }
//    }

    if (type == TOX_CONFERENCE_TYPE_TEXT) {
        groupnum = toxCore->conferenceJoin(friendnumber, cookie, length);
    } else if (type == TOX_CONFERENCE_TYPE_AV) {
        groupnum = toxCore->avGroupchatJoin(friendnumber, cookie, length, NULL, NULL);
        if (groupnum == -1) {
            qDebug() << " Invite from " << name << " (av) failed.";
            return;
        }
    }
    return;
}

void AdoxaBot::moduleResultSlot(AModule *module, QString query)
{
    if(query.compare("")) {
        toxCore->sendConferenceMessage(listenedConferenceId, TOX_MESSAGE_TYPE_NORMAL, query);
    }
    QObject::disconnect(module, SIGNAL(requestServiceResultSignal(AModule *,QString)), this, SLOT(moduleResultSlot(AModule *,QString)));
}

void AdoxaBot::parseMessageSlot(AModule *module, QString query)
{
    auto moduleRequestF = [=] () {
        QObject::connect(module, SIGNAL(requestServiceResultSignal(AModule *,QString)), this, SLOT(moduleResultSlot(AModule *,QString)));
        module->requestService(query);
    };

    if(modules.modulesMap.value(module).protectedAccess) {
        if(this->validateControlKey(query)) {
            moduleRequestF();
        } else {
            toxCore->sendConferenceMessage(listenedConferenceId, TOX_MESSAGE_TYPE_NORMAL, "This module is protected. Specify the access key.");
        }
        return;
    }
    moduleRequestF();
}
