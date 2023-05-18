//
// Created by wolala on 23-5-18.
//

#ifndef MYCALENDER_WEATHER_H
#define MYCALENDER_WEATHER_H

#include <QObject>
#include <QNetworkAccessManager>

class WeatherService : public QObject {
Q_OBJECT
public:
    explicit WeatherService(QObject *parent = nullptr);

signals:
    void weatherUpdated(const QString &icon, const QString &temp, const QString &text);

private slots:
    void onWeatherDataReceived(QNetworkReply *reply);
    void onLocationDataReceived(QNetworkReply *reply);
    void sendWeatherRequest(double longitude, double latitude);

private:
    QNetworkAccessManager *location_manager;
    QNetworkAccessManager *weather_manager;
    void getWeatherData();
    void getLocationFromAmap();

};

#endif //MYCALENDER_WEATHER_H
