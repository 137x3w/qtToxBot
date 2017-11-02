#ifndef TOXCORE_H
#define TOXCORE_H

#include <QString>
#include <QObject>
#include <QVector>
#include <QByteArray>

#include "tox/tox.h"

class QTimer;

class ToxCore: public QObject
{
    Q_OBJECT

public:
    explicit ToxCore(QObject *parent = nullptr);

    struct ToxNode {
        const char *ip;
        uint16_t    port;
        const char *key;
    };

    void initToxOptionsDefault();
    void setToxOptions(Tox_Options options);
    bool makeTox();
    bool isMaked();
    void killTox();
    bool saveData(QString path);
    bool loadData(QString path);
    void setNodes(QVector<ToxNode> nodes);
    void addNode(ToxNode node);
    void clearNodes();
    void startToxCore();
    bool bootstrapDht();
    void printInfo();
    QString getSelfAddress();
    bool setName(QString name);
    bool sendConferenceMessage(uint32_t conference_number, TOX_MESSAGE_TYPE type, QString message);
    bool sendPrivateMessage(uint32_t conference_number, TOX_MESSAGE_TYPE type, QString message);

    uint32_t friendAddNorequest(const uint8_t *public_key);
    bool deleteFriend(uint32_t friend_number);
    bool getFriendName(uint32_t friendnumber, uint8_t *name);
    uint32_t getFriendNameSize(uint32_t friendnumber);
    uint32_t conferenceJoin(uint32_t friendnumber, const uint8_t *cookie, size_t length);
    int avGroupchatJoin(int32_t friendnumber, const uint8_t *data, uint16_t length, void (*audio_callback)(void *, int, int, const int16_t *, unsigned int, uint8_t, unsigned int, void *), void *userdata);
    bool conferenceDelete(uint32_t conference_number);
    size_t conferenceGetChatlistSize();
    QVector<uint32_t> conferenceGetChatlist();
    size_t conferenceGetTitleSize(uint32_t conference_number);
    QString conferenceGetTitle(uint32_t conference_number);
private slots:
    void process();
signals:
    void friendRequestSignal(const uint8_t *public_key, const uint8_t *data, size_t length);
    void conferenceMessageSignal(uint32_t conference_number, uint32_t peer_number, TOX_MESSAGE_TYPE type, QByteArray message);
    void friendMessageSignal(uint32_t friendId, TOX_MESSAGE_TYPE type, QByteArray message);
    void selfConnectionStatusChangedSignal(TOX_CONNECTION connection_status);
    void conferenceInviteSignal(uint32_t friendnumber, TOX_CONFERENCE_TYPE type, const uint8_t *cookie, size_t length);
private:
    Tox* tox;
    Tox_Options toxOptions;
    QThread* toxThread;
    QTimer* toxTimer;
    bool maked = false;
    QVector<ToxNode> nodes;

    static void conferenceMessageCallback(Tox* tox, uint32_t groupId, uint32_t peerId, TOX_MESSAGE_TYPE type, const uint8_t* cMessage, size_t length, void* vCore);
    static void friendRequestCallback(Tox* tox, const uint8_t* cUserId, const uint8_t* cMessage, size_t cMessageSize, void* core);
    static void selfConnectionStatusChangedCallback(Tox* tox, TOX_CONNECTION connection_status, void* core);
    static void conferenceInviteCallback(Tox *tox, uint32_t friendnumber, TOX_CONFERENCE_TYPE type, const uint8_t *cookie, size_t length, void *core);
    static void friendMessageCallback(Tox* tox, uint32_t friendId, TOX_MESSAGE_TYPE type, const uint8_t* cMessage, size_t cMessageSize, void* core);
};

#endif // TOXCORE_H
