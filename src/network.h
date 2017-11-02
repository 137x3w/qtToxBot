#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QByteArray>
#include <QString>

class Network : public QObject
{
    Q_OBJECT
public:
    explicit Network(QObject *parent = nullptr);
    bool isRequested();
private:
    QNetworkAccessManager* manager;
    QNetworkReply* reply;
    bool requested = false;
signals:
    void getRequestResultSignal(QByteArray result);
public slots:
    void getRequestSlot(QUrl url);
private slots:
    void handleGetRequestSlot();
};

#endif // NETWORK_H
