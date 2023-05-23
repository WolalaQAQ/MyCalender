#include "main_window.h"
#include "login_dialog.h"
#include "reminder_dialog.h"
#include "ui_main_window.h"
#include "weather.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow) {
    ui->setupUi(this);
    loadUsers();
    loadReminders();
    setFocusPolicy(Qt::StrongFocus);
    auto weather = new WeatherService();
    // 添加日程提醒的QAction
    connect(ui->openReminderDialogAction, &QAction::triggered, this, &MainWindow::openReminderDialog);
    // 添加日程提醒的QTableWidget
    connect(ui->tableWidget, &QTableWidget::cellClicked, this, &MainWindow::showReminder);
    // 修改日程提醒的QListWidget
    connect(ui->reminderTree, &QTreeWidget::itemDoubleClicked, this, &MainWindow::editReminder);
    // 搜索日程提醒的QLineEdit
    connect(ui->searchLineEdit, &QLineEdit::textChanged, this, &MainWindow::handleSearchInputChanged);
    // 搜索日程提醒的QDateTimeEdit
    connect(ui->searchDateTimeEdit, &QDateTimeEdit::dateTimeChanged, this, &MainWindow::handleSearchInputChanged);
    // 获取到天气的信号
    connect(weather, &WeatherService::weatherUpdated, this, &MainWindow::updateWeather);
    // 登录的QAction
    connect(ui->loginAction, &QAction::triggered, [this]() {
        openLoginDialog(0);
    });
    // 注册的QAction
    connect(ui->registerAction, &QAction::triggered, [this]() {
        openLoginDialog(1);
    });
    // 登出的QAction
    connect(ui->signoutAction, &QAction::triggered, this, &MainWindow::signOut);
    // 显示当前的时间
    auto currentDateTime = QDateTime::currentDateTime();
    QString currentTimeString = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");
    ui->currentTime->setText(currentTimeString);
    // 将搜索日程提醒的QDateTimeEdit设置为当前时间
    ui->searchDateTimeEdit->setDateTime(currentDateTime);
    // 创建定时器
    auto timer = new QTimer();
    // 每秒更新一次时间
    QObject::connect(timer, &QTimer::timeout, [&]() {
        QDateTime currentDateTime = QDateTime::currentDateTime();
        QString currentTimeString = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");
        ui->currentTime->setText(currentTimeString);
        updateUserStatusBar();
    });
    // 启动定时器
    timer->start(1000);
    // 设置年份范围为当前时间的前 100 年至当前时间的后 10 年
    for (int year = 1; year <= 10000; ++year) {
        ui->yearCombox->addItem(QString::number(year));
    }
    ui->yearCombox->setCurrentText(QString::number(currentYear_));

    for (int month = 1; month <= 12; ++month) {
        ui->monthCombox->addItem(QString::number(month));
    }
    ui->monthCombox->setCurrentText(QString::number(currentMonth_));

    connect(ui->yearCombox, &QComboBox::currentTextChanged, this, &MainWindow::yearComboxChanged);
    connect(ui->monthCombox, &QComboBox::currentTextChanged, this, &MainWindow::monthComboxChanged);

    refreshMainWindow(currentYear_, currentMonth_);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::yearComboxChanged(const QString &year) {
    refreshMainWindow(year.toInt(), ui->monthCombox->currentText().toInt());
}

void MainWindow::monthComboxChanged(const QString &month) {
    refreshMainWindow(ui->yearCombox->currentText().toInt(), month.toInt());
}

void MainWindow::openReminderDialog() {
    if (currentUsername_.isEmpty()) {
        QMessageBox::warning(this, "Warning", "请先登录");
        return;
    }
    ReminderDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        Reminder reminder;
        reminder.dateTime = dialog.getDate();
        reminder.content = dialog.getContent();
        reminder.username = currentUsername_;
        reminders.append(reminder);
        refreshMainWindow(currentYear_, currentMonth_);
        saveReminders();
    }
}

void MainWindow::showReminder(int row, int column) {
    if (ui->tableWidget->item(row, column) == nullptr) {
        return;
    }
    QDate date(ui->yearCombox->currentText().toInt(),
               ui->monthCombox->currentText().toInt(),
               ui->tableWidget->item(row, column)->text().toInt());
    QTime time(0, 0, 0);
    QDateTime dateTime(date, time);
    ui->searchDateTimeEdit->setDateTime(dateTime);
}

void MainWindow::editReminder(QTreeWidgetItem *item) {
    if (currentUsername_.isEmpty()) {
        QMessageBox::warning(this, "Warning", "请先登录!");
        return;
    }
    int index = ui->reminderTree->indexOfTopLevelItem(item);
    Reminder &reminder = reminders[index];
    ReminderDialog dialog(this);
    dialog.setDate(reminder.dateTime);
    dialog.setContent(reminder.content);
    if (dialog.exec() == QDialog::Accepted) {
        reminder.dateTime = dialog.getDate();
        reminder.content = dialog.getContent();
        refreshMainWindow(currentYear_, currentMonth_);
        saveReminders();
    }
}

void MainWindow::deleteReminder() {
    auto selectedItems = ui->reminderTree->selectedItems();
    qDebug() << "Selected Items Size : " << selectedItems.size();
    if (selectedItems.empty()) {
        return;
    }
    QList<int> indexesToDelete;
    for (auto &selectedItem: selectedItems) {
        int index = ui->reminderTree->indexOfTopLevelItem(selectedItem);
        indexesToDelete.prepend(index);
    }
    for (auto index: indexesToDelete) {
        reminders.removeAt(index);
    }
    refreshMainWindow(currentYear_, currentMonth_);
    saveReminders();
}

void MainWindow::handleSearchInputChanged() {
    if (ui->searchLineEdit) {
        searchReminders(ui->searchLineEdit->text(), ui->searchDateTimeEdit->dateTime());
    } else if (ui->searchDateTimeEdit) {
        searchReminders(ui->searchLineEdit->text(), ui->searchDateTimeEdit->dateTime());
    }
}

void MainWindow::searchReminders(const QString &keyword, const QDateTime &dateTime) {
    ui->reminderTree->clear();
    for (const auto &reminder: reminders) {
        if (reminder.content.contains(keyword, Qt::CaseInsensitive) && reminder.dateTime.date().day() == dateTime.date().day() && reminder.username == currentUsername_) {
            auto item = new QTreeWidgetItem(ui->reminderTree);
            item->setText(0, reminder.dateTime.toString("yyyy-MM-dd hh:mm"));
            item->setText(1, reminder.content);
        }
    }
}

void MainWindow::updateWeather(const QString &icon, const QString &temp, const QString &text) {
    auto scene = new QGraphicsScene();
    ui->weatherIconView->setScene(scene);
    auto svgItem = new QGraphicsSvgItem(":/weather/weather_icons/" + icon + "-fill.svg");
    scene->addItem(svgItem);
    ui->weatherIconView->fitInView(svgItem);
    ui->weatherInfo->setText(temp + " ℃" + " " + text);
}

void MainWindow::refreshMainWindow(int year, int month) {
    updateCalender(year, month);
    updateUserStatusBar();
    updateReminderTree();
}

void MainWindow::openLoginDialog(uint8_t isRegister) {
    if (!currentUsername_.isEmpty()) {
        QMessageBox::warning(this, "Warning", "请先退出当前账号！");
        return;
    }
    LoginDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        User user;
        user.username = dialog.getUsername();
        user.password = dialog.getPassword();
        if (user.username.isEmpty() && user.username.isEmpty()) {
            QMessageBox::warning(this, "Warning", "用户名或密码不能为空！");
            return;
        }
        switch (isRegister) {
            case 0:
                for (const auto &u: users) {
                    if (u.username == user.username && u.password == user.password) {
                        QMessageBox::information(this, "Information", "登录成功！");
                        currentUsername_ = user.username;
                        refreshMainWindow(currentYear_, currentMonth_);
                        return;
                    }
                }
                QMessageBox::warning(this, "Warning", "用户名或密码错误！");
                break;
            case 1:
                for (const auto &u: users) {
                    if (u.username == user.username) {
                        QMessageBox::warning(this, "Warning", "用户名已存在！");
                        return;
                    } else if (u.username.contains(" ")) {
                        QMessageBox::warning(this, "Warning", "用户名不能包含空格！");
                        return;
                    }
                }
                users.append(user);
                saveUsers();
                break;
            default:
                break;
        }
    }
}

void MainWindow::signOut() {
    if (currentUsername_.isEmpty()) {
        QMessageBox::warning(this, "Warning", "请先登录！");
        return;
    }
    currentUsername_.clear();
    QMessageBox::information(this, "Information", "退出成功！");
    refreshMainWindow(currentYear_, currentMonth_);
}

void MainWindow::updateCalender(int year, int month) {
    // 清空表格
    ui->tableWidget->clearContents();
    // 获取当前月份的第一天
    QDate firstDayOfMonth(year, month, 1);
    // 获取当前月份的最后一天
    QDate lastDayOfMonth(year, month,
                         firstDayOfMonth.daysInMonth());
    // 获取当前月份的第一天是星期几，用于确定第一行的起始位置
    int firstDayOfWeek = firstDayOfMonth.dayOfWeek() - 1;
    // 循环添加每一天的单元格
    for (int day = 1; day <= lastDayOfMonth.day(); ++day) {
        int row = (firstDayOfWeek + day - 1) / 7;   // 计算行号
        int column = (firstDayOfWeek + day - 1) % 7;// 计算列号

        auto item = new QTableWidgetItem(QString::number(day));
        // 检查该日期是否有日程
        QDate date(year, month, day);
        for (const auto &reminder: reminders) {
            if (reminder.dateTime.date() == date && reminder.username == currentUsername_) {
                // 如果有日程，将单元格的背景色设置为不同的颜色
                item->setBackground(Qt::darkCyan);
                break;
            }
        }
        ui->tableWidget->setItem(row, column, item);
    }
    // 设置单元格大小
    int cellHeight = ui->tableWidget->height() / 6;
    for (int i = 0; i < ui->tableWidget->rowCount(); ++i) {
        ui->tableWidget->setRowHeight(i, cellHeight);
    }
    int cellWidth = ui->tableWidget->width() / 7;
    for (int i = 0; i < ui->tableWidget->columnCount(); ++i) {
        ui->tableWidget->setColumnWidth(i, cellWidth);
    }
    // 设置单元格文本居中对齐
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        for (int col = 0; col < ui->tableWidget->columnCount(); ++col) {
            auto item = ui->tableWidget->item(row, col);
            if (item) {
                item->setTextAlignment(Qt::AlignCenter);
            }
        }
    }
    //拉伸每一列，使表格水平方向没有空隙
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //拉伸每一行，使表格垂直方向没有空隙
    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // 设置表格边距和间距为0
    ui->tableWidget->setContentsMargins(0, 0, 0, 0);
}

void MainWindow::updateUserStatusBar() {
    if (currentUsername_.isEmpty()) {
        ui->statusbar->showMessage("未登录");
    } else {
        ui->statusbar->showMessage("当前用户：" + currentUsername_);
    }
}

void MainWindow::updateReminderTree() {
    ui->reminderTree->clear();
    for (const auto &reminder: reminders) {
        if (reminder.username != currentUsername_) {
            continue;
        }
        auto item = new QTreeWidgetItem(ui->reminderTree);
        item->setText(0, reminder.dateTime.toString("yyyy-MM-dd hh:mm"));
        item->setText(1, reminder.content);
    }
    ui->reminderTree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->reminderTree->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
}

void MainWindow::saveReminders() {
    // 使用SQLite数据库
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("reminders.db");
    if (!db.open()) {
        qWarning("Couldn't open save file.");
        return;
    }
    QSqlQuery query;
    query.exec("DROP TABLE reminders");
    query.exec("CREATE TABLE reminders ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "dateTime TEXT,"
               "content TEXT,"
               "username TEXT)");
    for (const auto &reminder: reminders) {
        query.prepare("INSERT INTO reminders (dateTime, content, username) "
                      "VALUES (:dateTime, :content, :username)");
        query.bindValue(":dateTime", reminder.dateTime.toString(Qt::ISODate));
        query.bindValue(":content", reminder.content);
        query.bindValue(":username", reminder.username);
        query.exec();
    }
    db.close();
}

void MainWindow::loadReminders() {
    // 使用SQLite数据库
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("reminders.db");
    if (!db.open()) {
        qWarning("Couldn't open saved reminders file. Creating a new one.");
        saveReminders();
        return;
    }
    QSqlQuery query;
    query.exec("SELECT * FROM reminders");
    while (query.next()) {
        Reminder reminder;
        reminder.dateTime = QDateTime::fromString(query.value("dateTime").toString(), Qt::ISODate);
        reminder.content = query.value("content").toString();
        reminder.username = query.value("username").toString();
        reminders.append(reminder);
    }
    db.close();
}

void MainWindow::saveUsers() {
    // 使用SQLite数据库
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("users.db");
    if (!db.open()) {
        qWarning("Couldn't open save file.");
        return;
    }
    QSqlQuery query;
    query.exec("DROP TABLE users");
    query.exec("CREATE TABLE users ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "username TEXT,"
               "password TEXT)");
    for (const auto &user: users) {
        query.prepare("INSERT INTO users (username, password) "
                      "VALUES (:username, :password)");
        query.bindValue(":username", user.username);
        query.bindValue(":password", user.password);
        query.exec();
    }
    db.close();
}

void MainWindow::loadUsers() {
    // 使用SQLite数据库
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("users.db");
    if (!db.open()) {
        qWarning("Couldn't open save users file. Creating a new one.");
        saveUsers();
        return;
    }
    QSqlQuery query;
    query.exec("SELECT * FROM users");
    while (query.next()) {
        User user;
        user.username = query.value("username").toString();
        user.password = query.value("password").toString();
        users.append(user);
    }
    db.close();
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Delete) {
        deleteReminder();
    }
}
