#ifndef PROTECTEDCONTROL_H
#define PROTECTEDCONTROL_H

#include <QObject>
#include <QVector>
#include <sodium.h>

#include "src/modules/interactiveModules/ainteractivemodule.h"
#include "src/adoxabot.h"

class AdoxaBot;

class ProtectedControl : public AInteractiveModule
{
    Q_OBJECT
public:
    explicit ProtectedControl(QObject *parent = nullptr);
    enum COMMANDS {
        CONTROL, //parse encrypted message, see the token and doing encrypted command.
        KEY_INFO, //freezed
        SET_CONTROL_KEY,
        GET_CONTROL_KEY
    };
    void requestService(QString args, int routerIndex, int xprIndex);
    ProtectedControl* setBotInstance(AdoxaBot* bot);
private:
    struct Keys {
        unsigned char pKey[crypto_box_PUBLICKEYBYTES];
        unsigned char sKey[crypto_box_SECRETKEYBYTES];
    };
    Keys keys = {{0}, {0}};

    QString token = "06c9f71496e24dec6acc44895648cf9ec40b5cebb7bc4858a3c69f25";
    QRegExp regToken = QRegExp("token ([a-f0-9]{56}) (\\S.*)");
    AdoxaBot *bot;
    QString decryptB64msg(QString b64emsg);
    bool updateToken(QString dmsg);
    QString getContainedMessage(QString dmsg);
};


#endif // PROTECTEDCONTROL_H
