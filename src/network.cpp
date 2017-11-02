#include "network.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkProxy>
#include <QUrl>

#include <QDebug>

Network::Network(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager();
}

bool Network::isRequested()
{
    return requested;
}

void Network::getRequestSlot(QUrl url)
{
    if(!requested) {
        requested = true;
        QNetworkRequest request(url);
        reply = manager->get(request);
        connect(reply, SIGNAL(finished()), this, SLOT(handleGetRequestSlot()));
        qDebug() << url.toString() << " requested";
    }
}

void Network::handleGetRequestSlot()
{
    qDebug() << "handle...";
    if (!reply->error()) {
        qDebug() << "handled and emit";
        getRequestResultSignal(reply->readAll());
        reply->deleteLater();
        reply = Q_NULLPTR;
        requested = false;
    } else {
        qDebug() << "search error";
    }
}
