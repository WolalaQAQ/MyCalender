//
// Created by wolala on 23-5-18.
//

#include "weather.h"

#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>
#include <iostream>

WeatherService::WeatherService(QObject *parent) : QObject(parent) {
    weather_manager = new QNetworkAccessManager(this);
    location_manager = new QNetworkAccessManager(this);
    getWeatherData();
}

void WeatherService::getWeatherData() {
    getLocationFromAmap();
}

void WeatherService::getLocationFromAmap() {
    QUrl url("https://restapi.amap.com/v3/ip");
    QUrlQuery query;
    query.addQueryItem("key", "21885260af9f75a5f21ef5f59540973d");
    url.setQuery(query);
    qDebug() << "Location Request: " << url;
    QNetworkRequest request(url);
    QNetworkReply *reply = location_manager->get(request);
    QObject::connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            onLocationDataReceived(reply);
            qDebug() << "Location Response";
        } else {
            qDebug() << "Request Error: " << reply->errorString();
        }
        reply->deleteLater();
    });
}

void WeatherService::onLocationDataReceived(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Error: " << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    // 保存JSON数据
    QFile file("location.json");
    file.open(QIODevice::WriteOnly);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    QJsonObject obj = doc.object();

    QString status = obj["status"].toString();
    if (status != "1") {
        qDebug() << "API error: " << status;
        reply->deleteLater();
        return;
    }

    QString province = obj["province"].toString();
    QString city = obj["city"].toString();
    QString district = obj["district"].toString();
    QString adcode = obj["adcode"].toString();
    QString rectangle = obj["rectangle"].toString();

    qDebug() << "province: " << province;
    qDebug() << "city: " << city;
    qDebug() << "district: " << district;
    qDebug() << "adcode: " << adcode;
    qDebug() << "rectangle: " << rectangle;

    QStringList list = rectangle.split(";");
    QStringList leftTop = list[0].split(",");
    QStringList rightBottom = list[1].split(",");

    double longitude = (leftTop[0].toDouble() + rightBottom[0].toDouble()) / 2;
    double latitude = (leftTop[1].toDouble() + rightBottom[1].toDouble()) / 2;

    qDebug() << "longitude: " << longitude;
    qDebug() << "latitude: " << latitude;

    sendWeatherRequest(longitude, latitude);

    reply->deleteLater();
}

void WeatherService::sendWeatherRequest(const double longitude, const double latitude) {
    // 创建URL
    QUrl url("https://devapi.qweather.com/v7/weather/now");
    // 创建URL参数
    QUrlQuery query;
    // 保留小数点后两位
    query.addQueryItem("location",
                       QString::number(longitude, 'f', 2) + "," + QString::number(latitude, 'f', 2));
    query.addQueryItem("key", "e95a587b7fb64668a7b36576f42a7cae");
    url.setQuery(query);
    qDebug() << "Weather Request: " << url;
    // 创建网络请求
    QNetworkRequest request(url);
    // 发送GET请求
    QNetworkReply *reply = weather_manager->get(request);
    // 连接请求完成信号
    QObject::connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            // 读取响应数据
            onWeatherDataReceived(reply);
            // 处理响应数据
            qDebug() << "Weather Response";
        } else {
            // 处理请求错误
            qDebug() << "Request Error: " << reply->errorString();
        }
        // 清理资源
        reply->deleteLater();
    });
}

void WeatherService::onWeatherDataReceived(QNetworkReply *reply) {
    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    // 保存JSON数据
    QFile file("weather.json");
    file.open(QIODevice::WriteOnly);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    QJsonObject obj = doc.object();

    QString code = obj["code"].toString();
    if (code != "200") {
        qDebug() << "API error: " << code;
        reply->deleteLater();
        return;
    }

    QJsonObject now = obj["now"].toObject();
    QString icon = now["icon"].toString();
    QString temp = now["temp"].toString();
    QString text = now["text"].toString();

    emit weatherUpdated(icon, temp, text);

    reply->deleteLater();
}