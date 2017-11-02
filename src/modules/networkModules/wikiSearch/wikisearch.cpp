#include "wikisearch.h"

#include <QObject>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "src/modules/networkModules/anetworkmodule.h"

WikiSearch::WikiSearch(QObject *parent) : ANetworkModule(parent)
{
    //wiki query lang limit
//    patterns.append({
//        QVector<QRegExp>() << QRegExp("(\\S.*) (\\S.*) ([0-9]+)") << QRegExp("(\\S.*) ([0-9]+)") << QRegExp("(\\S.*) (\\S.*)") << QRegExp("(\\S.*)"),
//        COMMANDS::SEARCH,
//        "https://%2.wikipedia.org/w/api.php?action=opensearch&search=%1&prop=info&format=json&inprop=url&limit=%3"
    //    });
}

void WikiSearch::requestService(QString args, int routerIndex, int xprIndex)
{
    router[routerIndex].xprs[xprIndex].indexIn(args);
    switch(router[routerIndex].cmd) {
    case COMMANDS::WIKI_SEARCH:
    {
        qDebug() << "cap 1: " << router[routerIndex].xprs[xprIndex].cap(1);
        qDebug() << "cap 2: " << router[routerIndex].xprs[xprIndex].cap(2);
        qDebug() << "cap 3: " << router[routerIndex].xprs[xprIndex].cap(3);
        //add a handle nosearch
        params.query = router[routerIndex].xprs[xprIndex].cap(1).isEmpty() ? "nosearch" : router[routerIndex].xprs[xprIndex].cap(1);
        params.resultsLimit = router[routerIndex].xprs[xprIndex].cap(3).isEmpty() ? 1 : router[routerIndex].xprs[xprIndex].cap(3).toInt();
        params.language = "en";
        if(router[routerIndex].xprs[xprIndex].cap(2).toInt()) {
            params.resultsLimit = router[routerIndex].xprs[xprIndex].cap(2).toInt();
        } else {
            params.language = router[routerIndex].xprs[xprIndex].cap(2).isEmpty() ? "en" : router[routerIndex].xprs[xprIndex].cap(2);
        }
        activeCommand = COMMANDS::WIKI_SEARCH;
        this->net->getRequestSlot(router[routerIndex].linkTemplate.arg(params.query, params.language, QString::number(params.resultsLimit)));
        break;
    }
    default: break;
    }
}

void WikiSearch::parseWikiSearchResult(QByteArray result)
{
    if(!result.isEmpty()) {
        QString resultString = "";
        QJsonDocument itemDoc = QJsonDocument::fromJson(result);
        QJsonArray rootObject = itemDoc.array();

        QJsonArray titles = rootObject[1].toArray();
        for(int i = 0; i < titles.size(); i++) {
            resultString += titles[i].toString() + "\n";
        }

        resultString += rootObject[2].toArray()[0].toString() + "\n";
        resultString += rootObject[3].toArray()[0].toString();

        requestServiceResultSignal(this, resultString);
    } else {
        requestServiceResultSignal(this, "Oh shit im sorry..");
    }
}

void WikiSearch::handleNetworkDataSlot(QByteArray data)
{
    switch(activeCommand) {
    case COMMANDS::WIKI_SEARCH: {
        parseWikiSearchResult(data);
        break;
    }
    default: break;
    }
}

void WikiSearch::requestTimeoutSlot()
{
    requestServiceResultSignal(this, "Request timeout.");
}

void WikiSearch::requestErrorSlot()
{
    requestServiceResultSignal(this, "Oh shit im sorry.");
}


