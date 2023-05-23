//
// Created by wolala on 23-5-23.
//

#ifndef MYCALENDER_LOGIN_DIALOG_H
#define MYCALENDER_LOGIN_DIALOG_H

#include <QDialog>
#include <QToolButton>

#include "ui_login_dialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class LoginDialog;
}
QT_END_NAMESPACE

class LoginDialog : public QDialog {
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog() override;

    [[nodiscard]] QString getUsername() const;
    [[nodiscard]] QString getPassword() const;

    void setUsername(const QString &username);
    void setPassword(const QString &password);

private:
    Ui::LoginDialog *ui;

public slots:

private slots:
    void eyeChanged(bool checked);
};

#endif//MYCALENDER_LOGIN_DIALOG_H
