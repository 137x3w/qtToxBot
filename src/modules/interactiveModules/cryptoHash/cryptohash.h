#ifndef CRYPTOHASH_H
#define CRYPTOHASH_H

#include <QObject>
#include <QVector>

#include "src/modules/interactiveModules/ainteractivemodule.h"

class CryptoHash : public AInteractiveModule
{
    Q_OBJECT
public:
    explicit CryptoHash(QObject *parent = nullptr);
    enum COMMANDS {
        SHA256
    };
    void requestService(QString args, int routerIndex, int xprIndex);
private:
    QString sha256(QString inputString);
};

#endif // CRYPTOHASH_H
