#ifndef ANETWORK_H
#define ANETWORK_H

#include <QObject>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>

#include "src/modules/networkModules/inetwork.h"

class ANetwork : public QObject, public INetwork
{
    Q_OBJECT
public:
    ANetwork(QObject *parent = nullptr);
    ~ANetwork();
    bool isBusy();
    void setReplyTimeoutMs(int timeout);
signals:
    void getRequestResultSignal(QByteArray result);
    void getRequestTimeoutSignal();
    void getRequestErrorSignal();
public slots:
    void getRequestSlot(QUrl request);
private slots:
    void handleGetRequestSlot();
    void timeoutGetRequestSlot();
    void timeoutGetRequestIntervalSlot();
private:
    bool busy = false;
    bool requestPermission = true;
    int msReplyTimeout = 10000;
    int msRequestInterval = 1000;
    QNetworkAccessManager* manager;
    QNetworkReply* reply;
    QTimer* replyTimeoutTimer;
    QTimer* requestIntervalTimer;
};

#endif // ANETWORK_H
