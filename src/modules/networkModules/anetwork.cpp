#include "anetwork.h"

#include <QTimer>

ANetwork::ANetwork(QObject *parent) : QObject(parent), INetwork()
{
    manager = new QNetworkAccessManager();
    replyTimeoutTimer = new QTimer();
    requestIntervalTimer = new QTimer();

    replyTimeoutTimer->setSingleShot(true);
    requestIntervalTimer->setSingleShot(true);

    connect(replyTimeoutTimer, &QTimer::timeout, this, &ANetwork::timeoutGetRequestSlot);
    connect(requestIntervalTimer, &QTimer::timeout, this, &ANetwork::timeoutGetRequestIntervalSlot);
}

ANetwork::~ANetwork()
{
    if(isBusy()) {
        reply->abort();
        reply->deleteLater();
    }
    replyTimeoutTimer->stop();
    requestIntervalTimer->stop();
    replyTimeoutTimer->deleteLater();
    requestIntervalTimer->deleteLater();
    delete manager;
}

bool ANetwork::isBusy()
{
    return busy;
}

void ANetwork::setReplyTimeoutMs(int timeout)
{
    this->msReplyTimeout = timeout;
}

void ANetwork::getRequestSlot(QUrl request)
{
    if(!isBusy() && requestPermission) {
        busy = true;
        requestPermission = false;

        replyTimeoutTimer->start(msReplyTimeout);
        requestIntervalTimer->start(msRequestInterval);

        reply = manager->get(QNetworkRequest(request));
        connect(reply, &QNetworkReply::finished, this, &ANetwork::handleGetRequestSlot);
        qDebug() << request.toString() << " requested";
    }
}

void ANetwork::handleGetRequestSlot()
{
    qDebug() << "handle...";
    replyTimeoutTimer->stop();
    if(!reply->error()) {
        qDebug() << "handled and emit";
        getRequestResultSignal(reply->readAll());
    } else {
        qDebug() << "reply error";
        getRequestErrorSignal();
        reply->abort();
    }
    reply->deleteLater();
    reply = Q_NULLPTR;
    busy = false;
}

void ANetwork::timeoutGetRequestSlot()
{
    if(isBusy()) {
        reply->abort();
        reply->deleteLater();
        reply = Q_NULLPTR;
        getRequestTimeoutSignal();
    }
}

void ANetwork::timeoutGetRequestIntervalSlot()
{
    requestPermission = true;
}
