#include "browser2ch.h"

#include <QObject>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "src/modules/networkModules/anetworkmodule.h"

Browser2ch::Browser2ch(QObject *parent) : ANetworkModule(parent)
{
    //The order is important
    //getPost board postNumber
//    patterns.append({
//        QVector<QRegExp>() << QRegExp("getPost (\\S.*) (\\S\\d*)"),
//        COMMANDS::GET_POST,
//        "https://2ch.hk/makaba/mobile.fcgi?task=get_post&board=%1&post=%2"
//    });

//    //getThread board pageNumber postNumber
//    patterns.append({
//        QVector<QRegExp>() << QRegExp("getThread (\\S.*) ([0-9]) ([0-9])"),
//        COMMANDS::GET_THREAD,
//        "https://2ch.hk/%1/%2.json"
//    });
}

void Browser2ch::requestService(QString args, int routerIndex, int xprIndex)
{
    router[routerIndex].xprs[xprIndex].indexIn(args);
    switch(router[routerIndex].cmd) {
    case COMMANDS::GET_THREAD:
    {
        threadNumber = router[routerIndex].xprs[xprIndex].cap(3).toInt();
        pageNumber = router[routerIndex].xprs[xprIndex].cap(2).toInt();
        board = router[routerIndex].xprs[xprIndex].cap(1);
        activeCommand = COMMANDS::GET_THREAD;
        this->net->getRequestSlot(router[routerIndex].linkTemplate.arg(board, pageNumber==0?"index":QString::number(pageNumber)));
        break;
    }
    case COMMANDS::GET_POST:
    {
        postNumber = router[routerIndex].xprs[xprIndex].cap(2).toInt();
        board = router[routerIndex].xprs[xprIndex].cap(1);
        activeCommand = COMMANDS::GET_POST;
        this->net->getRequestSlot(router[routerIndex].linkTemplate.arg(board, QString::number(postNumber)));
        break;
    }
    default: break;
    }
}

void Browser2ch::parseGetThreadCommandResult(QByteArray result)
{
    if(!result.isEmpty()) {
        QString resultString = "";
        QJsonDocument itemDoc = QJsonDocument::fromJson(result);
        QJsonObject rootObject = itemDoc.object();
        QJsonArray items = rootObject["threads"].toArray();
        QJsonObject item = items[threadNumber].toObject();
        resultString += item["thread_num"].toString() + "\n";
        resultString += item["posts"].toArray()[0].toObject()["comment"].toString().replace("&amp;","&").replace("&gt;",">").replace("&lt;","<").replace("<br>", "\n").remove(QRegExp("<[^>]*>")).left(200) + "...";
        requestServiceResultSignal(this, resultString);
    } else {
        requestServiceResultSignal(this, "Oh shit im sorry..");
    }
}

void Browser2ch::parseGetPostCommandResult(QByteArray result)
{
    if(!result.isEmpty()) {
        QString resultString = "";
        QJsonDocument itemDoc = QJsonDocument::fromJson(result);
        QJsonObject rootObject = itemDoc.array()[0].toObject();
        resultString += rootObject["comment"].toString().replace("&amp;","&").replace("&gt;",">").replace("&lt;","<").replace("<br>", "\n").remove(QRegExp("<[^>]*>")).left(500) + "...";
        requestServiceResultSignal(this, resultString);
    } else {
        requestServiceResultSignal(this, "Oh shit im sorry..");
    }
}

void Browser2ch::handleNetworkDataSlot(QByteArray data)
{
    switch(activeCommand) {
    case COMMANDS::GET_THREAD: {
        parseGetThreadCommandResult(data);
        break;
    }
    case COMMANDS::GET_POST: {
        parseGetPostCommandResult(data);
        break;
    }
    default: break;
    }
}

void Browser2ch::requestTimeoutSlot()
{
    requestServiceResultSignal(this, "Request timeout.");
}

void Browser2ch::requestErrorSlot()
{
    requestServiceResultSignal(this, "Oh shit im sorry.");
}

