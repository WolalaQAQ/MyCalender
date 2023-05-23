//
// Created by wolala on 23-5-23.
//

#include "login_dialog.h"

LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent),
                                            ui(new Ui::LoginDialog) {
    ui->setupUi(this);
    connect(ui->eyeButton, &QToolButton::clicked, this, &LoginDialog::eyeChanged);
}

QString LoginDialog::getUsername() const {
    return ui->usernameLineEdit->text();
}

QString LoginDialog::getPassword() const {
    return ui->passwordLineEdit->text();
}

void LoginDialog::setUsername(const QString &username) {
    ui->usernameLineEdit->setText(username);
}

void LoginDialog::setPassword(const QString &password) {
    ui->passwordLineEdit->setText(password);
}

void LoginDialog::eyeChanged(bool checked) {
    if(ui->passwordLineEdit->echoMode() == QLineEdit::Password) {
        ui->eyeButton->setIcon(QIcon(":/eye/eye_icons/eye_o.png"));
        ui->passwordLineEdit->setEchoMode(QLineEdit::Normal);
    } else {
        ui->eyeButton->setIcon(QIcon(":/eye/eye_icons/eye_c.png"));
        ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
    }
}

LoginDialog::~LoginDialog() {
    delete ui;
}