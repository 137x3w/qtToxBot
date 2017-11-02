#include "weathercondition.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>

WeatherCondition::WeatherCondition(QObject *parent)
{

}

void WeatherCondition::requestService(QString args, int routerIndex, int xprIndex)
{
    router[routerIndex].xprs[xprIndex].indexIn(args);
    switch(router[routerIndex].cmd) {
    case COMMANDS::CURRENT_WEATHER:
    {
        cityName = router[routerIndex].xprs[xprIndex].cap(1);
        this->net->getRequestSlot(router[routerIndex].linkTemplate.arg(cityName).arg(units).arg(language));
        break;
    }
    default: break;
    }
}

void WeatherCondition::parseWeatherConditionResult(QByteArray result)
{
    if(!result.isEmpty()) {
        QString resultString = "";
        QJsonDocument itemDoc = QJsonDocument::fromJson(result);
        QJsonObject rootObject = itemDoc.object();
        resultString += "Temperature: ";
        resultString += QString::number(rootObject["main"].toObject()["temp"].toDouble());
        resultString += "C°. ";
        resultString += "Pressure: ";
        resultString += QString::number(rootObject["main"].toObject()["pressure"].toDouble());
        resultString += ". ";
        resultString += "Humidity: ";
        resultString += QString::number(rootObject["main"].toObject()["humidity"].toDouble());
        resultString += "%. ";
        resultString += "Max temperature: ";
        resultString += QString::number(rootObject["main"].toObject()["temp_max"].toDouble());
        resultString += "C°. ";
        resultString += "Min temperature: ";
        resultString += QString::number(rootObject["main"].toObject()["temp_min"].toDouble());
        resultString += "C°. ";
        resultString += "Wind: ";
        resultString += QString::number(rootObject["wind"].toObject()["speed"].toDouble());
        resultString += "m/s, ";
        resultString += QString::number(rootObject["wind"].toObject()["deg"].toDouble());
        resultString += " degrees. ";
        resultString += "Clouds: ";
        resultString += QString::number(rootObject["clouds"].toObject()["all"].toDouble());
        resultString += "%, ";
        resultString += rootObject["weather"].toArray()[0].toObject()["description"].toString();
        resultString += ". Last update: " + QString::number(QDateTime::currentDateTimeUtc().toTime_t() - rootObject["dt"].toInt());
        resultString += " secs ago.";
        requestServiceResultSignal(this, resultString);
    } else {
        requestServiceResultSignal(this, "Oh shit im sorry..");
    }
}

void WeatherCondition::handleNetworkDataSlot(QByteArray data)
{
    switch(activeCommand) {
    case COMMANDS::CURRENT_WEATHER: {
        parseWeatherConditionResult(data);
        break;
    }
    default: break;
    }
}

void WeatherCondition::requestTimeoutSlot()
{
    requestServiceResultSignal(this, "Request timeout.");
}

void WeatherCondition::requestErrorSlot()
{
    requestServiceResultSignal(this, "Oh shit im sorry.");
}
