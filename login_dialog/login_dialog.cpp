//
// Created by wolala on 23-5-23.
//

#include "login_dialog.h"

LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent),
                                            ui(new Ui::LoginDialog) {
    ui->setupUi(this);
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

LoginDialog::~LoginDialog() {
    delete ui;
}