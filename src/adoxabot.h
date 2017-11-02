#ifndef ADOXABOT_H
#define ADOXABOT_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QUrl>
#include <QHash>
#include <QPair>

#include "tox/tox.h"
#include "src/modules/amodule.h"
#include "src/modules/networkModules/anetworkmodule.h"
#include "src/modules/interactiveModules/protectedControl/protectedcontrol.h"

class ToxCore;
class ANetwork;
class ANetworkModule;
class MessageParser;
class ProtectedControl;

class AdoxaBot : public QObject
{
    Q_OBJECT
public:
    explicit AdoxaBot(QObject *parent = nullptr);

    struct ProtectedAccess {
        bool protectedAccess;
    };

    struct ModuleName {
        QString moduleName;
    };

    struct Module : ProtectedAccess, ModuleName {
        AModule* module;
    };

    struct ModulesMap {
        QHash<AModule*,Module> modulesMap;
    };

    QString getToxId() const;
    void setControlKey(QString newKey);
    QString getControlKey() const;
    QVector<uint32_t> getConferenceIds() const;
    QString getConferenceTitle(uint32_t id) const;
    bool listenConference(uint32_t id);
    bool leaveConference(uint32_t id);
private:
    ToxCore* toxCore;
    ANetwork* network;
    MessageParser* messageParser;
    ModulesMap modules;

    uint32_t listenedConferenceId;

    QString controlKey;
    QRegExp controlKeyPattern;
    bool validateControlKey(QString &query);

    void createModule(QString moduleName, ANetworkModule *module, QHash<AModule *, Module> &moduleStorage, QVector<ANetworkModule::RegExpRouter> routes, bool protectedAccess = false);
    void createModule(QString moduleName, AModule *module, QHash<AModule*, AdoxaBot::Module> &moduleStorage, QVector<AModule::RegExpRouter> routes, bool protectedAccess = false);
    void requestModuleService();
signals:
    //////////////////To parser////////////////////////
    void conferenceMessageSignal(QString message);
    //////////////////To network///////////////////////
    void getRequestSignal(QUrl url);
public slots:
    void conferenceMessageSlot(uint32_t conference_number, uint32_t peer_number, TOX_MESSAGE_TYPE type, QByteArray message);
    void selfConnectionStatusChangedSlot(TOX_CONNECTION connection_status);
    void friendRequestSlot(const uint8_t *public_key, const uint8_t *data, size_t length);
    void friendMessageSlot(uint32_t friendId, TOX_MESSAGE_TYPE type, QByteArray message);
    void conferenceInviteSlot(uint32_t friendnumber, TOX_CONFERENCE_TYPE type, const uint8_t *cookie, size_t length);

    void moduleResultSlot(AModule *module, QString query);
    //////////////////From parser//////////////////////
    void parseMessageSlot(AModule *module, QString query);
};

#endif // ADOXABOT_H
