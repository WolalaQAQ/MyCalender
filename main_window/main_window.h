#ifndef CALENDERDIALOG_H
#define CALENDERDIALOG_H

#include <QMainWindow>
#include <QDialog>
#include <QDate>
#include <QTimer>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "reminder.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;
    QList<Reminder> reminders;
    int currentYear_ = QDate::currentDate().year();
    int currentMonth_ = QDate::currentDate().month();

public slots:
    void yearComboxChanged(const QString& year);
    void monthComboxChanged(const QString& month);
    void openReminderDialog();
    void showReminder(int row, int column);
    void editReminder(QListWidgetItem *item);

private:
    void refreshMainWindow(int year, int month);
    void updateCalender(int year, int month);
    void updateReminderList();
    void saveReminders();
    void loadReminders();

};

#endif // CALENDERDIALOG_H
