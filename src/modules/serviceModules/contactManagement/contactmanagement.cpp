#include "contactmanagement.h"

ContactManagement::ContactManagement(QObject *parent)
{

}

void ContactManagement::requestService(QString args, int routerIndex, int xprIndex)
{
    router[routerIndex].xprs[xprIndex].indexIn(args);
    switch(router[routerIndex].cmd) {
    case COMMANDS::LISTEN_CONFERENCE:
    {
        int confId = router[routerIndex].xprs[xprIndex].cap(1).toInt();
        if(bot->listenConference(confId)) {
            requestServiceResultSignal(this, "Listening " + bot->getConferenceTitle(confId));
        } else {
            requestServiceResultSignal(this, "Oh shit, cannot listen. Bad id?");
        }
        break;
    }
    case COMMANDS::LEAVE_CONFERENCE:
    {
        int confId = router[routerIndex].xprs[xprIndex].cap(1).toInt();
        QString title = bot->getConferenceTitle(confId);
        if(bot->leaveConference(confId)) {
            requestServiceResultSignal(this, "");
        } else {
            requestServiceResultSignal(this, "Oh shit, cannot leave " + title);
        }
        break;
    }
    case COMMANDS::LISTEN_FRIEND:
    {
        requestServiceResultSignal(this, "Done");
        break;
    }
    default: break;
    }
}

ContactManagement *ContactManagement::setBotInstance(AdoxaBot *bot)
{
    this->bot = bot;
    return this;
}
