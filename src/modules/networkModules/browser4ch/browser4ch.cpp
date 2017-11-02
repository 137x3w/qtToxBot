#include "browser4ch.h"

#include <QObject>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "src/modules/networkModules/anetworkmodule.h"

Browser4ch::Browser4ch(QObject *parent) : ANetworkModule(parent)
{

//    patterns.append({
//        QVector<QRegExp>() << QRegExp("getThread (\\S.*) ([0-9]) ([0-9])") << QRegExp("(\\S.*) ([0-9]) ([0-9])"),
//        COMMANDS::GET_THREAD,
//        "https://a.4cdn.org/%1/%2.json"
//    });
}

void Browser4ch::requestService(QString args, int routerIndex, int xprIndex)
{
    router[routerIndex].xprs[xprIndex].indexIn(args);
    switch(router[routerIndex].cmd) {
    case COMMANDS::GET_THREAD:
    {
        threadNumber = router[routerIndex].xprs[xprIndex].cap(3).toInt();
        pageNumber = router[routerIndex].xprs[xprIndex].cap(2).toInt() + 1;
        board = router[routerIndex].xprs[xprIndex].cap(1);
        activeCommand = COMMANDS::GET_THREAD;
        this->net->getRequestSlot(router[routerIndex].linkTemplate.arg(board, QString::number(pageNumber)));
        break;
    }
    default: break;
    }
}

void Browser4ch::parseGetThreadCommandResult(QByteArray result)
{
    if(!result.isEmpty()) {
        QString resultString = "";
        QJsonDocument itemDoc = QJsonDocument::fromJson(result);
        QJsonObject rootObject = itemDoc.object();
        QJsonArray items = rootObject["threads"].toArray();
        QJsonObject item = items[threadNumber].toObject()["posts"].toArray()[0].toObject();
        resultString += QString::number(item["no"].toInt()) + "\n";
        resultString += item["com"].toString().replace("&amp;","&").replace("&gt;",">").replace("&lt;","<").replace("<br>", "\n").remove(QRegExp("<[^>]*>"));
        resultString = resultString.left(200) + "...";
        requestServiceResultSignal(this, resultString);
    } else {
        requestServiceResultSignal(this, "Oh shit im sorry..");
    }
}

void Browser4ch::handleNetworkDataSlot(QByteArray data)
{
    switch(activeCommand) {
    case COMMANDS::GET_THREAD: {
        parseGetThreadCommandResult(data);
        break;
    }
    default: break;
    }
}

void Browser4ch::requestTimeoutSlot()
{
    requestServiceResultSignal(this, "Request timeout.");
}

void Browser4ch::requestErrorSlot()
{
    requestServiceResultSignal(this, "Oh shit im sorry.");
}
