#ifndef CONTACTMANAGEMENT_H
#define CONTACTMANAGEMENT_H

#include "src/modules/serviceModules/aservicemodule.h"
#include "src/adoxabot.h"
#include "src/messageparser.h"

class ContactManagement : public AServiceModule
{
public:
    enum COMMANDS {
        LISTEN_CONFERENCE,
        LISTEN_FRIEND,
        LEAVE_CONFERENCE
    };
    explicit ContactManagement(QObject *parent = nullptr);
    void requestService(QString args, int routerIndex, int xprIndex);
    ContactManagement *setBotInstance(AdoxaBot *bot);
private:
    AdoxaBot* bot;

};

#endif // CONTACTMANAGEMENT_H
