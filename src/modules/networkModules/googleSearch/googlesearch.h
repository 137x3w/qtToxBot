#ifndef GOOGLESEARCH_H
#define GOOGLESEARCH_H

#include <QObject>
#include <QVector>

#include "src/modules/networkModules/anetworkmodule.h"

class GoogleSearch : public ANetworkModule
{
    Q_OBJECT
public:
    explicit GoogleSearch(QObject *parent = nullptr);
    enum COMMANDS {
        GOOGLE_SEARCH
    };
    void requestService(QString args, int routerIndex, int xprIndex);
private:
    COMMANDS activeCommand = COMMANDS::GOOGLE_SEARCH;
    QString searchString;
    void parseGoogleSearchResult(QByteArray result);
public slots:
    void handleNetworkDataSlot(QByteArray data);
private slots:
    void requestTimeoutSlot();
    void requestErrorSlot();
};

#endif // GOOGLESEARCH_H
