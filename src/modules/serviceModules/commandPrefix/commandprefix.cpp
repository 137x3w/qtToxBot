#include "commandprefix.h"

CommandPrefix::CommandPrefix(QObject *parent)
{

}

void CommandPrefix::requestService(QString args, int routerIndex, int xprIndex)
{
    router[routerIndex].xprs[xprIndex].indexIn(args);
    switch(router[routerIndex].cmd) {
    case COMMANDS::CHANGE_MAIN_PREFIX:
    {
        parser->setPrefix(router[routerIndex].xprs[xprIndex].cap(1));
        requestServiceResultSignal(this, "Done");
        break;
    }
    case COMMANDS::SET_PREFIX_DEFAULT:
    {
        parser->setPrefix("@");
        requestServiceResultSignal(this, "Done");
        break;
    }
    default: break;
    }
}

CommandPrefix *CommandPrefix::setParser(MessageParser *parser)
{
    this->parser = parser;
    return this;
}
