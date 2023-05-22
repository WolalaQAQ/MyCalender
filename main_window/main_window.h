#ifndef CALENDERDIALOG_H
#define CALENDERDIALOG_H

#include <QDate>
#include <QDialog>
#include <QFile>
#include <QGraphicsSvgItem>
#include <QGraphicsView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QKeyEvent>
#include <QListWidgetItem>
#include <QMainWindow>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSvgRenderer>
#include <QTimer>
#include <QTreeWidgetItem>

#include "reminder.h"
#include "user.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;

    QList<Reminder> reminders;
    QList<User> users;

    int currentYear_ = QDate::currentDate().year();
    int currentMonth_ = QDate::currentDate().month();

    QString currentUsername_;

    void refreshMainWindow(int year, int month);
    void updateCalender(int year, int month);
    void updateUserStatusBar();
    void updateReminderTree();

    void saveReminders();
    void loadReminders();

    void saveUsers();
    void loadUsers();

    void keyPressEvent(QKeyEvent *event) override;

public slots:

private slots:
    void yearComboxChanged(const QString &year);
    void monthComboxChanged(const QString &month);

    void openReminderDialog();
    void showReminder(int row, int column);
    void editReminder(QTreeWidgetItem *item);
    void deleteReminder();
    void handleSearchInputChanged();
    void searchReminders(const QString &keyword, const QDateTime &dateTime);

    void updateWeather(const QString &icon, const QString &temp, const QString &text);

    void openLoginDialog(uint8_t isRegister);
    void signOut();
};

#endif// CALENDERDIALOG_H
