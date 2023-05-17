//
// Created by wolala on 23-5-18.
//

#include "weather.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QUrlQuery>
#include <QDebug>
#include <QGeoPositionInfo>
#include <QGeoPositionInfoSource>

WeatherService::WeatherService(QObject *parent) : QObject(parent) {
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &WeatherService::onWeatherDataReceived);
}

void WeatherService::getWeatherData() {
    QUrl url("https://devapi.qweather.com/v7/weather/now");
    QUrlQuery query;
    auto location = QGeoPositionInfoSource::createDefaultSource(this);
    auto longitude = location->lastKnownPosition().coordinate().longitude();
    auto latitude = location->lastKnownPosition().coordinate().latitude();
    query.addQueryItem("key", "e95a587b7fb64668a7b36576f42a7cae");
    // 精度精确到小数点后两位
    query.addQueryItem("location",
                       QString::number(longitude, 'f', 2) + "," +
                       QString::number(latitude, 'f', 2));
    url.setQuery(query);

    QNetworkRequest request(url);
    manager->get(request);
}

void WeatherService::onWeatherDataReceived(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Error: " << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject obj = doc.object();

    QString code = obj["code"].toString();
    if (code != "200") {
        qDebug() << "API error: " << code;
        reply->deleteLater();
        return;
    }

    QJsonObject now = obj["now"].toObject();
    QString temp = now["temp"].toString();
    QString text = now["text"].toString();

    emit weatherUpdated(temp, text);

    reply->deleteLater();
}
