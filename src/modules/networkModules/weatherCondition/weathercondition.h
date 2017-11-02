#ifndef WEATHERCONDITION_H
#define WEATHERCONDITION_H

#include <QObject>
#include <QVector>

#include "src/modules/networkModules/anetworkmodule.h"

class WeatherCondition : public ANetworkModule
{
    Q_OBJECT
public:
    explicit WeatherCondition(QObject *parent = nullptr);
    enum COMMANDS {
        CURRENT_WEATHER
    };
    void requestService(QString args, int routerIndex, int xprIndex);
private:
    COMMANDS activeCommand = COMMANDS::CURRENT_WEATHER;
    QString language = "en";
    QString units = "metric";
    QString cityName = "Saint Petersburg";
    void parseWeatherConditionResult(QByteArray result);
public slots:
    void handleNetworkDataSlot(QByteArray data);
private slots:
    void requestTimeoutSlot();
    void requestErrorSlot();
};

#endif // WEATHERCONDITION_H
