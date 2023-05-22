//
// Created by wolala on 23-5-17.
//

#ifndef MYCALENDER_REMINDER_DIALOG_H
#define MYCALENDER_REMINDER_DIALOG_H

#include <QDate>
#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
    class ReminderDialog;
}
QT_END_NAMESPACE

class ReminderDialog : public QDialog {
    Q_OBJECT

public:
    explicit ReminderDialog(QWidget *parent = nullptr);
    ~ReminderDialog() override;

    [[nodiscard]] QDateTime getDate() const;
    [[nodiscard]] QString getContent() const;

    void setDate(const QDateTime &date);
    void setContent(const QString &content);

private:
    Ui::ReminderDialog *ui;
};

#endif//MYCALENDER_REMINDER_DIALOG_H
