#ifndef BROWSER4CH_H
#define BROWSER4CH_H

#include <QObject>
#include <QVector>

#include "src/modules/networkModules/anetworkmodule.h"

class Browser4ch : public ANetworkModule
{
    Q_OBJECT
public:
    explicit Browser4ch(QObject *parent = nullptr);
    enum COMMANDS {
        GET_THREAD
    };
    void requestService(QString args, int routerIndex, int xprIndex);
private:
    COMMANDS activeCommand = GET_THREAD;
    int threadNumber = 0;
    int pageNumber = 0;
    QString board = "b";

    void parseGetThreadCommandResult(QByteArray result);
public slots: //make a protected
    void handleNetworkDataSlot(QByteArray data);
private slots:
    void requestTimeoutSlot();
    void requestErrorSlot();
};

#endif // BROWSER4CH_H
