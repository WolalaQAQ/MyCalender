//
// Created by wolala on 23-5-17.
//

#include "reminder_dialog.h"
#include "ui_reminder_dialog.h"

ReminderDialog::ReminderDialog(QWidget *parent) : QDialog(parent),
                                                  ui(new Ui::ReminderDialog) {
    ui->setupUi(this);
    // 将初始时间设置当前时间
    auto currentDateTime = QDateTime::currentDateTime();
    ui->dateTimeEdit->setDateTime(currentDateTime);
}

ReminderDialog::~ReminderDialog() {
    delete ui;
}

QDateTime ReminderDialog::getDate() const {
    return ui->dateTimeEdit->dateTime();
}

QString ReminderDialog::getContent() const {
    return ui->lineEdit->text();
}

void ReminderDialog::setDate(const QDateTime &date) {
    ui->dateTimeEdit->setDateTime(date);
}

void ReminderDialog::setContent(const QString &content) {
    ui->lineEdit->setText(content);
}