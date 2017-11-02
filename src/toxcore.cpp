#include "toxcore.h"

#include "tox/tox.h"
#include "tox/toxav.h"

#include <QDebug>
#include <QFile>
#include <QTimer>

ToxCore::ToxCore(QObject *parent): QObject(parent)
{
    qRegisterMetaType<TOX_CONNECTION>("TOX_CONNECTION");
    qRegisterMetaType<TOX_CONFERENCE_TYPE>("TOX_CONFERENCE_TYPE");
    qRegisterMetaType<TOX_MESSAGE_TYPE>("TOX_MESSAGE_TYPE");
    qRegisterMetaType<size_t>("size_t");
    qRegisterMetaType<uint8_t>("uint8_t");
    qRegisterMetaType<const uint8_t*>("const uint8_t*");
    qRegisterMetaType<uint32_t>("uint32_t");
    qRegisterMetaType<const uint32_t*>("const uint32_t*");
}

void ToxCore::initToxOptionsDefault()
{
    tox_options_default(&toxOptions);
}

void ToxCore::setToxOptions(Tox_Options options)
{
    toxOptions = options;
}

bool ToxCore::makeTox()
{
    TOX_ERR_NEW toxErr;
    tox = tox_new(&toxOptions, &toxErr);

    if (toxErr != TOX_ERR_NEW_OK) {
        qDebug() << "Warning: tox_new failed with error: " << toxErr;
        return false;
    }
    maked = true;
    return true;
}

bool ToxCore::isMaked()
{
    return maked;
}

void ToxCore::killTox()
{
    tox_kill(tox);
    maked = false;
}


bool ToxCore::saveData(QString path)
{
    if(path.isEmpty()) {
        path = "saved_data";
    }

    FILE *fp = fopen(path.toUtf8().data(), "wb");

    if (fp == NULL) {
        qDebug() <<  "Warning: cannot open file.";
        return false;
    }

    size_t data_len = tox_get_savedata_size(tox);
    char *data = (char*)malloc(data_len);

    if (data == NULL) {
        qDebug() << "Warning: cannot malloc memory.";
        return false;
    }

    tox_get_savedata(tox, (uint8_t *)data);

    if (fwrite(data, data_len, 1, fp) != 1) {
        free(data);
        fclose(fp);
        qDebug() << "Warning: cannot save tox data.";
        return false;
    }

    free(data);
    fclose(fp);
    return true;
}

bool ToxCore::loadData(QString path)
{
    FILE *fp = fopen(path.toUtf8().data(), "rb");

    qint64 fsize = QFile(path).size();

    if (fsize == 0) {
        fclose(fp);
        qDebug() << "Warning: cannot find file.";
        return false;
    }

    char data[fsize];

    if (fread(data, sizeof(data), 1, fp) != 1) {
        fclose(fp);
        qDebug() << "Warning: cannot read file.";
        return false;
    }

    toxOptions.savedata_type = TOX_SAVEDATA_TYPE_TOX_SAVE;
    toxOptions.savedata_data = (uint8_t *) data;
    toxOptions.savedata_length = fsize;

    fclose(fp);
    return true;
}

void ToxCore::setNodes(QVector<ToxCore::ToxNode> nodes)
{
    this->nodes.clear();
    this->nodes = nodes;
}

void ToxCore::addNode(ToxCore::ToxNode node)
{
    nodes.push_back(node);
}

void ToxCore::clearNodes()
{
    nodes.clear();
}

void ToxCore::startToxCore()
{
    toxTimer = new QTimer(this);
    toxTimer->setSingleShot(true);
    connect(toxTimer, &QTimer::timeout, this, &ToxCore::process);

    tox_callback_friend_request(tox, friendRequestCallback);
    tox_callback_conference_message(tox, conferenceMessageCallback);
    tox_callback_friend_message(tox, friendMessageCallback);
    tox_callback_self_connection_status(tox, selfConnectionStatusChangedCallback);
    tox_callback_conference_invite(tox, conferenceInviteCallback);

    process();
}

bool ToxCore::bootstrapDht()
{
    for (int i = 0; i < nodes.size(); i++) {
        TOX_ERR_BOOTSTRAP err;
        tox_bootstrap(tox, nodes[i].ip, nodes[i].port, (uint8_t *)QByteArray::fromHex(nodes[i].key).constData(), &err);

        if (err != TOX_ERR_BOOTSTRAP_OK) {
            qDebug() << "Warning: failed to bootstrap DHT via." << nodes[i].ip << " " << nodes[i].port << " " << err;
            return false;
        }
    }
    return true;
}

void ToxCore::printInfo()
{
    char address[TOX_ADDRESS_SIZE] = {0};
    tox_self_get_address(tox, (uint8_t *) address);

    char name[TOX_MAX_NAME_LENGTH] = {0};
    size_t len = tox_self_get_name_size(tox);
    tox_self_get_name(tox, (uint8_t *) name);
    name[len] = '\0';

    qDebug() << " Adress: " << QByteArray::fromRawData(address, TOX_ADDRESS_SIZE).toHex();
    qDebug() << " Name: " << QString::fromUtf8(name);
}

QString ToxCore::getSelfAddress()
{
    char address[TOX_ADDRESS_SIZE] = {0};
    tox_self_get_address(tox, (uint8_t *) address);
    return QString(QByteArray::fromRawData(address, TOX_ADDRESS_SIZE).toHex());
}

bool ToxCore::setName(QString name)
{
    TOX_ERR_SET_INFO err;
    QByteArray byteName = name.toUtf8();
    tox_self_set_name(tox, (const uint8_t*)byteName.data(), byteName.length(), &err);

    if(err != TOX_ERR_SET_INFO_OK) {
        qDebug() << "Warning: failed to set name. " << name << " " << err;
        return false;
    }
    return true;
}

bool ToxCore::sendConferenceMessage(uint32_t conference_number, TOX_MESSAGE_TYPE type, QString message)
{
    TOX_ERR_CONFERENCE_SEND_MESSAGE err;
    QByteArray byteMes = message.toUtf8();
    uint8_t* mes = (uint8_t*)byteMes.data();
    tox_conference_send_message(tox, conference_number, type, mes, byteMes.length(), &err);

    if(err != TOX_ERR_CONFERENCE_SEND_MESSAGE_OK) {
        qDebug() << "Warning: failed to send conference message. " << message << " " << err;
        return false;
    }
    return true;
}

void ToxCore::process()
{
    tox_iterate(tox, this);
    toxTimer->start(tox_iteration_interval(tox));
}

uint32_t ToxCore::friendAddNorequest(const uint8_t *public_key)
{
    TOX_ERR_FRIEND_ADD err;
    uint32_t num = tox_friend_add_norequest(tox, public_key, &err);

    if (err != TOX_ERR_FRIEND_ADD_OK) {
        qDebug() << "Friend add norquest error. " << err;
        return -1;
    }
    return num;
}

bool ToxCore::deleteFriend(uint32_t friend_number)
{
    TOX_ERR_FRIEND_DELETE err;
    tox_friend_delete(tox, friend_number, &err);

    if(err != TOX_ERR_FRIEND_DELETE_OK) {
        qDebug() << "Friend delete error. " << err;
        return false;
    }
    return true;
}

bool ToxCore::getFriendName(uint32_t friendnumber, uint8_t *name)
{
    TOX_ERR_FRIEND_QUERY err;
    tox_friend_get_name(tox, friendnumber, name, &err);

    if(err != TOX_ERR_FRIEND_QUERY_OK) {
        qDebug() << "Get friend name error. " << err;
        return false;
    }
    return true;
}

uint32_t ToxCore::getFriendNameSize(uint32_t friendnumber)
{
    TOX_ERR_FRIEND_QUERY err;
    size_t size = tox_friend_get_name_size(tox, friendnumber, &err);

    if(err != TOX_ERR_FRIEND_QUERY_OK) {
        qDebug() << "Get friend name size error. " << err;
        return -1;
    }
    return size;
}

uint32_t ToxCore::conferenceJoin(uint32_t friendnumber, const uint8_t *cookie, size_t length)
{
    TOX_ERR_CONFERENCE_JOIN err;
    uint32_t cNumber = tox_conference_join(tox, friendnumber, cookie, length, &err);

    if(err != TOX_ERR_CONFERENCE_JOIN_OK) {
        qDebug() << "Conference join error. " << err;
        return -1;
    }

    return cNumber;
}

int ToxCore::avGroupchatJoin(int32_t friendnumber, const uint8_t *data, uint16_t length, void (*audio_callback)(void *, int, int, const int16_t *, unsigned int, uint8_t, unsigned int, void *), void *userdata)
{
    return toxav_join_av_groupchat(tox, friendnumber, data, length, NULL, NULL);
}

bool ToxCore::conferenceDelete(uint32_t conference_number)
{
    TOX_ERR_CONFERENCE_DELETE err;
    tox_conference_delete(tox, conference_number, &err);

    if(err != TOX_ERR_CONFERENCE_DELETE_OK) {
        qDebug() << "Conference delete error.";
        return false;
    }

    return true;
}

size_t ToxCore::conferenceGetChatlistSize()
{
    return tox_conference_get_chatlist_size(tox);
}

QVector<uint32_t> ToxCore::conferenceGetChatlist()
{
    QVector<uint32_t> list;
    size_t confsSize = conferenceGetChatlistSize();
    if(!confsSize) {return list;}

    uint32_t chatlist[confsSize] = {0};
    tox_conference_get_chatlist(tox, chatlist);
    for(int i = 0; i < confsSize; i++) {
        list.append(chatlist[i]);
    }
    return list;
}

size_t ToxCore::conferenceGetTitleSize(uint32_t conference_number)
{
    TOX_ERR_CONFERENCE_TITLE err;
    size_t confTitleSize = tox_conference_get_title_size(tox, conference_number, &err);

    switch (err) {
    case TOX_ERR_CONFERENCE_TITLE_INVALID_LENGTH:
    {
        qDebug() << "Conference invalid title size.";
        return 0;
    }
    }
    if(err != TOX_ERR_CONFERENCE_TITLE_OK) {
        qDebug() << "Conference get title size error.";
        return -1;
    }

    return confTitleSize;
}

QString ToxCore::conferenceGetTitle(uint32_t conference_number)
{
    size_t titleSize = conferenceGetTitleSize(conference_number);
    if(titleSize <= 0) {return "";}
    uint8_t title[titleSize] = {0};
    TOX_ERR_CONFERENCE_TITLE err;
    tox_conference_get_title(tox, conference_number, title, &err);
    if(err != TOX_ERR_CONFERENCE_TITLE_OK) {
        qDebug() << "Conference get title error.";
        return "";
    }

    return QString::fromUtf8((const char*)title, titleSize);
}

//Callbacks

void ToxCore::friendRequestCallback(Tox *tox, const uint8_t *cUserId, const uint8_t *cMessage, size_t cMessageSize, void *core)
{
    static_cast<ToxCore*>(core)->friendRequestSignal(cUserId, cMessage, cMessageSize);
}

void ToxCore::conferenceMessageCallback(Tox *tox, uint32_t groupId, uint32_t peerId, TOX_MESSAGE_TYPE type, const uint8_t *cMessage, size_t length, void *vCore)
{
    static_cast<ToxCore*>(vCore)->conferenceMessageSignal(groupId, peerId, type, QByteArray(reinterpret_cast<const char*>(cMessage), length));
}

void ToxCore::friendMessageCallback(Tox *tox, uint32_t friendId, TOX_MESSAGE_TYPE type, const uint8_t *cMessage, size_t cMessageSize, void *core)
{
    static_cast<ToxCore*>(core)->friendMessageSignal(friendId, type, QByteArray(reinterpret_cast<const char*>(cMessage), cMessageSize));
}

void ToxCore::selfConnectionStatusChangedCallback(Tox *tox, TOX_CONNECTION connection_status, void *core)
{
    static_cast<ToxCore*>(core)->selfConnectionStatusChangedSignal(connection_status);
}

void ToxCore::conferenceInviteCallback(Tox *tox, uint32_t friendnumber, TOX_CONFERENCE_TYPE type, const uint8_t *cookie, size_t length, void *core)
{
    static_cast<ToxCore*>(core)->conferenceInviteSignal(friendnumber, type, cookie, length);
}

