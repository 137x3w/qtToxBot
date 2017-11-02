#ifndef INETWORK_H
#define INETWORK_H

#include <QObject>
#include <QUrl>
#include <QByteArray>

class INetwork
{
public:
    INetwork() {}
    ~INetwork() {}
    virtual bool isBusy() = 0;
signals:
    void getRequestResultSignal(QByteArray);
    void getRequestTimeoutSignal();
    void getRequestErrorSignal();
public:
    virtual void getRequestSlot(QUrl) = 0;
private:
    virtual void handleGetRequestSlot() = 0;
};

#endif // INETWORK_H
