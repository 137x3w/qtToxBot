#ifndef COMMANDPREFIX_H
#define COMMANDPREFIX_H

#include "src/modules/serviceModules/aservicemodule.h"
#include "src/adoxabot.h"
#include "src/messageparser.h"

class CommandPrefix : public AServiceModule
{
public:
    enum COMMANDS {
        CHANGE_MAIN_PREFIX,
        SET_PREFIX_DEFAULT
    };
    explicit CommandPrefix(QObject *parent = nullptr);
    void requestService(QString args, int routerIndex, int xprIndex);
    CommandPrefix* setParser(MessageParser* parser);
private:
    MessageParser* parser;
};

#endif // COMMANDPREFIX_H
