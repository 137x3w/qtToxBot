#include "messageparser.h"

#include <QRegExp>
#include <QString>

#include <QDebug>

MessageParser::MessageParser(QObject *parent) : QObject(parent)
{
    globalPattern.setPattern("@(\\S.*)");
}

void MessageParser::setPrefix(QString prefix)
{
    globalPattern.setPattern(QRegExp::escape(prefix) + QString("(\\S.*)"));
}

void MessageParser::setPatterns(QVector<MessageParser::ModuleRouter> routers)
{
    moduleRouters = routers;
}

void MessageParser::insertPattern(AModule *module, QVector<QRegExp> patterns)
{
    ModuleRouter mr;
    mr.module = module;
    mr.patterns = patterns;
    moduleRouters.append(mr);
}

void MessageParser::clearAllPatterns()
{
    moduleRouters.clear();
}

void MessageParser::parseConferenceMessageSlot(QString message)
{
    if(!globalPattern.exactMatch(message)) {
        return;
    } else {
        globalPattern.indexIn(message);
        QString command = globalPattern.cap(1);
        qDebug() << "parse: " << message;

        for(int i = 0; i < moduleRouters.size(); i++) {
            for(int j = 0; j < moduleRouters.at(i).patterns.size(); j++) {
                if(moduleRouters.at(i).patterns.at(j).exactMatch(command)) {
                    parseSignal(moduleRouters.at(i).module, command);
                }
            }
        }
    }
}
