#ifndef MESSAGEPARSER_H
#define MESSAGEPARSER_H

#include <QObject>
#include <QHash>
#include <QVector>
#include <QRegExp>
#include <QString>

#include "src/modules/amodule.h"
#include "src/modules/networkModules/anetworkmodule.h"

class MessageParser : public QObject
{
    Q_OBJECT
public:
    explicit MessageParser(QObject *parent = nullptr);

    struct ModuleRouter {
        AModule* module;
        QVector<QRegExp> patterns;
    };

    void setPrefix(QString prefix);
    void setPatterns(QVector<ModuleRouter> routers);
    void insertPattern(AModule *module, QVector<QRegExp> patterns);
    void clearAllPatterns();
private:
    QRegExp globalPattern;
    QVector<ModuleRouter> moduleRouters;
signals:
    //
    void parseSignal(AModule* module, QString query);
//    void parseSignal(ANetworkModule* module, QString query);
public slots:
    void parseConferenceMessageSlot(QString message);
};

#endif // MESSAGEPARSER_H
