#include "googlesearch.h"

#include <QObject>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

GoogleSearch::GoogleSearch(QObject *parent) : ANetworkModule(parent)
{
//    patterns.append({
//        QRegExp("(\\S.*)+"),
//        COMMANDS::SEARCH,
//        "https://www.googleapis.com/customsearch/v1?key=AIzaSyCCocbrnMGkjkIt0vznidmxjxAcKFqbtkk&cx=001354305087942324278:vtfb5njh_1q&q=%1&hl=ru&num=1"
//    });
}

void GoogleSearch::requestService(QString args, int routerIndex, int xprIndex)
{
    router[routerIndex].xprs[xprIndex].indexIn(args);
    switch(router[routerIndex].cmd) {
    case COMMANDS::GOOGLE_SEARCH:
    {
        searchString = router[routerIndex].xprs[xprIndex].cap(1);
        this->net->getRequestSlot(router[routerIndex].linkTemplate.arg(searchString));
        break;
    }
    default: break;
    }
}

void GoogleSearch::parseGoogleSearchResult(QByteArray result)
{
    if(!result.isEmpty()) {
        QString resultString = "";
        QJsonDocument itemDoc = QJsonDocument::fromJson(result);
        QJsonObject rootObject = itemDoc.object();
        QJsonArray items = rootObject["items"].toArray();
        QJsonObject item = items[0].toObject();
        resultString = item["title"].toString() + "\n" + item["link"].toString() + "\n" + item["snippet"].toString();
        requestServiceResultSignal(this, resultString);
    } else {
        requestServiceResultSignal(this, "Oh shit im sorry..");
    }
}

void GoogleSearch::handleNetworkDataSlot(QByteArray data)
{
    switch(activeCommand) {
    case COMMANDS::GOOGLE_SEARCH: {
        parseGoogleSearchResult(data);
        break;
    }
    default: break;
    }
}

void GoogleSearch::requestTimeoutSlot()
{
    requestServiceResultSignal(this, "Request timeout.");
}

void GoogleSearch::requestErrorSlot()
{
    requestServiceResultSignal(this, "Oh shit im sorry.");
}
