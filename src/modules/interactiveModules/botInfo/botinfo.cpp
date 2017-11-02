#include "botinfo.h"

BotInfo::BotInfo(QObject *parent) : AInteractiveModule(parent)
{

}

void BotInfo::requestService(QString args, int routerIndex, int xprIndex)
{
    router[routerIndex].xprs[xprIndex].indexIn(args);
    switch(router[routerIndex].cmd) {
    case COMMANDS::HELP_INFO:
    {
        requestServiceResultSignal(this, getHelpInfo());
        break;
    }
    case COMMANDS::BOT_TOX_ID_INFO:
    {
        requestServiceResultSignal(this, "My tox id is: " + getToxId());
        break;
    }
    case CONTROL_KEY_INFO:
    {
        break;
    }
    case CONFERENCE_INFO:
    {
        requestServiceResultSignal(this, getConferenceInfo());
    }
    default: break;
    }
}

BotInfo *BotInfo::setBotInstance(AdoxaBot *bot)
{
    this->bot = bot;
    return this;
}

QString BotInfo::getHelpInfo()
{
    QString info = "Help info ⚠\n";
    info += "@google [search query] - google search\n";

    info += "@wiki [search query] [lang(ru, en..)] [maximum results count(1-...)] - search in wiki\n";
    info += "@wiki [search query] [maximum results count(1-...)] - search in english wiki\n";
    info += "@wiki [search query] [lang(ru, en..)] - one result search\n";
    info += "@wiki [search query] - one result search in english wiki\n";

    info += "@bitcoinRate - current bitcoin rate from blockchain.info\n";
    info += "@coinRate [full coin name] - coin rate from coinmarketcap\n";

    info += "@weather [city name(en ru..)] - return weather in city \"city name\"\n";

    info += "@2ch getPost [board] [post number] - return post from 2ch.hk/\"board\" with number \"post number\"\n";
    info += "@2ch getThread [board] [page number(0-9)] [post index(0-9)] - return thread from 2ch.hk/\"board\" by page \"page number\" and \"post index\" on page\n";
    info += "@4ch [board_code(b..)] [page number(0-9)] [index(0-9)] - return post from 4chan.org/\"board\" by page \"page number\" and \"post index\" on page\n";

    info += "@sha256 [count(1-9)] [string] - return \"count\" sha256 hashes from \"string\"\n";
    info += "@setPrefix [new prefix] - change command prefix \"@\" to \"new prefix\"\n";
    info += "@setPrefixDefault - change command prefix to \"@\"\n";

    info += "@id - show my tox id\n";

    info += "@getControlKey - show sha256 hash of control key\n";
    info += "@setControlKey [64 symbol hex string] - set new control key\n";

    return info;
}

QString BotInfo::getToxId()
{
    return bot->getToxId();
}

QString BotInfo::getConferenceInfo()
{
    QVector<uint32_t> ids = bot->getConferenceIds();
    QString result = "";
    for(int i = 0; i < ids.size(); i++) {
        qDebug() << "id: " << ids.at(i);
        qDebug() << "title: " << bot->getConferenceTitle(ids.at(i));
        result = result + "id: " + QString::number(ids.at(i)) + " title: " + bot->getConferenceTitle(ids.at(i)) + "\n";
    }

    return result;
}
