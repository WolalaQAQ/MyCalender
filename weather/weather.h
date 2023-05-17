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
    void getWeatherData();

signals:
    void weatherUpdated(const QString &temp, const QString &text);

private slots:
    void onWeatherDataReceived(QNetworkReply *reply);

private:
    QNetworkAccessManager *manager;
};

#endif //MYCALENDER_WEATHER_H
