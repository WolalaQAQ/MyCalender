//
// Created by wolala on 23-5-17.
//

#ifndef MYCALENDER_REMINDER_DIALOG_H
#define MYCALENDER_REMINDER_DIALOG_H

#include <QDialog>
#include <QDate>

QT_BEGIN_NAMESPACE
namespace Ui { class ReminderDialog; }
QT_END_NAMESPACE

class ReminderDialog : public QDialog
{
Q_OBJECT

public:
    explicit ReminderDialog(QWidget *parent = nullptr);
    ~ReminderDialog() override;

    [[nodiscard]] QDate getDate() const;
    [[nodiscard]] QString getContent() const;

    void setDate(const QDate &date);
    void setContent(const QString &content);

private:
    Ui::ReminderDialog *ui;
};

#endif //MYCALENDER_REMINDER_DIALOG_H
