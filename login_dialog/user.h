//
// Created by wolala on 23-5-23.
//

#ifndef MYCALENDER_USER_H
#define MYCALENDER_USER_H

#include <QString>

class User {
public:
    QString username;
    QString password;
    bool isAdmin = false;
};

#endif //MYCALENDER_USER_H
