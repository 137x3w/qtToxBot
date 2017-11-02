#ifndef BOTINFO_H
#define BOTINFO_H

#include <QObject>
#include <QVector>

#include "src/modules/interactiveModules/ainteractivemodule.h"
#include "src/adoxabot.h"

class AdoxaBot;

class BotInfo : public AInteractiveModule
{
public:
    explicit BotInfo(QObject *parent = nullptr);
    enum COMMANDS {
        HELP_INFO,
        BOT_TOX_ID_INFO,
        CONTROL_KEY_INFO,
        CONFERENCE_INFO
    };
    void requestService(QString args, int routerIndex, int xprIndex);
    BotInfo* setBotInstance(AdoxaBot *bot);
private:
    AdoxaBot* bot;

    QString getHelpInfo();
    QString getToxId();
    QString getConferenceInfo();
};

#endif // BOTINFO_H
