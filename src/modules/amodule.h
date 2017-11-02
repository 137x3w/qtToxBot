#ifndef AMODULE_H
#define AMODULE_H

#include <QHash>
#include <QString>
#include <QRegExp>
#include <QVector>

#include "src/modules/imodule.h"

class IModule;

class AModule : public QObject, public IModule
{
    Q_OBJECT
public:
    AModule(QObject *parent = nullptr);
    ~AModule();
    struct RegExpRouter
    {
        int cmd;
        QVector<QRegExp> xprs;
    };
    enum COMMANDS {};
    virtual void requestService(QString args);
    virtual void requestService(QString args, int routerIndex, int xprIndex) = 0;
    virtual void configureRouter(QVector<AModule::RegExpRouter> newrouter);
protected:
    COMMANDS activeCommand;
    QVector<AModule::RegExpRouter> router;
signals:
//    void requestServiceResultSignal(QString moduleName, QString preparedStr);
    void requestServiceResultSignal(AModule* moduleName, QString preparedStr);
};

#endif // AMODULE_H
