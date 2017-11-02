#ifndef WIKISEARCH_H
#define WIKISEARCH_H

#include <QObject>
#include <QVector>

#include "src/modules/networkModules/anetworkmodule.h"

class WikiSearch : public ANetworkModule
{
    Q_OBJECT
public:
    enum COMMANDS {
        WIKI_SEARCH
    };
    explicit WikiSearch(QObject *parent = nullptr);
    void requestService(QString args, int routerIndex, int xprIndex);
private:
    struct QueryParams {
        int resultsLimit;
        QString language;
        QString query;
    };

    COMMANDS activeCommand = COMMANDS::WIKI_SEARCH;
    QueryParams params = {1, "en", ""};

    void parseWikiSearchResult(QByteArray result);
public slots:
    void handleNetworkDataSlot(QByteArray data);
private slots:
    void requestTimeoutSlot();
    void requestErrorSlot();
};

#endif // WIKISEARCH_H
