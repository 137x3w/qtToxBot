#ifndef BROWSER2CH_H
#define BROWSER2CH_H

#include <QObject>
#include <QVector>

#include "src/modules/networkModules/anetworkmodule.h"

class Browser2ch : public ANetworkModule
{
    Q_OBJECT
public:
    enum COMMANDS {
        GET_THREAD,
        GET_POST
    };
    explicit Browser2ch(QObject *parent = nullptr);
    void requestService(QString args, int routerIndex, int xprIndex);
private:
    COMMANDS activeCommand = GET_THREAD;
    int threadNumber = 0;
    int postNumber = 0;
    int pageNumber = 0;
    QString board = "b";

    void parseGetThreadCommandResult(QByteArray result);
    void parseGetPostCommandResult(QByteArray result);
public slots: //make a private
    void handleNetworkDataSlot(QByteArray data);
private slots:
    void requestTimeoutSlot();
    void requestErrorSlot();
};

#endif // BROWSER2CH_H
