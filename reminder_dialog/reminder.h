//
// Created by wolala on 23-5-17.
//

#ifndef MYCALENDER_REMINDER_H
#define MYCALENDER_REMINDER_H

#include <QDate>
#include <QString>

class Reminder {
public:
    QDateTime dateTime;
    QString content;
    QString username;
};

#endif//MYCALENDER_REMINDER_H
