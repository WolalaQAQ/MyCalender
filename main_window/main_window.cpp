#include "main_window.h"
#include "ui_main_window.h"
#include "reminder_dialog.h"
#include "weather.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent),
        ui(new Ui::MainWindow) {
    ui->setupUi(this);
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
    // 显示当前的时间
    auto currentDateTime = QDateTime::currentDateTime();
    QString currentTimeString = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");
    ui->currentTime->setText(currentTimeString);
    // 将搜索日程提醒的QDateTimeEdit设置为当前时间
    ui->searchDateTimeEdit->setDateTime(currentDateTime);
    // 创建定时器
    auto *timer = new QTimer();
    // 每秒更新一次时间
    QObject::connect(timer, &QTimer::timeout, [&]() {
        QDateTime currentDateTime = QDateTime::currentDateTime();
        QString currentTimeString = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");
        ui->currentTime->setText(currentTimeString);
    });
    // 启动定时器
    timer->start(1000);
    // 设置年份范围为当前时间的前 100 年至当前时间的后 10 年
    for (int year = currentYear_ - 2022; year <= currentYear_ + 10000-2023; year++) {
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

void MainWindow::yearComboxChanged(const QString& year) {
    refreshMainWindow(year.toInt(), ui->monthCombox->currentText().toInt());
}

void MainWindow::monthComboxChanged(const QString& month) {
    refreshMainWindow(ui->yearCombox->currentText().toInt(), month.toInt());
}

void MainWindow::openReminderDialog() {
    ReminderDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        Reminder reminder;
        reminder.dateTime = dialog.getDate();
        reminder.content = dialog.getContent();
        reminders.append(reminder);
        refreshMainWindow(currentYear_, currentMonth_);
        saveReminders();
    }
}

void MainWindow::showReminder(int row, int column) {
    if(ui->tableWidget->item(row, column) == nullptr) {
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
    for (auto& selectedItem : selectedItems) {
        int index = ui->reminderTree->indexOfTopLevelItem(selectedItem);
        indexesToDelete.prepend(index);
    }
    for (auto index : indexesToDelete) {
        reminders.removeAt(index);
    }
    refreshMainWindow(currentYear_, currentMonth_);
    saveReminders();
}

void MainWindow::handleSearchInputChanged() {
    if(ui->searchLineEdit) {
        searchReminders(ui->searchLineEdit->text(), ui->searchDateTimeEdit->dateTime());
    }
    else if(ui->searchDateTimeEdit) {
        searchReminders(ui->searchLineEdit->text(), ui->searchDateTimeEdit->dateTime());
    }
}

void MainWindow::searchReminders(const QString &keyword, const QDateTime &dateTime) {
    ui->reminderTree->clear();
    for (const Reminder &reminder : reminders) {
        if ( reminder.content.contains(keyword, Qt::CaseInsensitive) && reminder.dateTime.date().day() == dateTime.date().day() ) {
            auto item = new QTreeWidgetItem(ui->reminderTree);
            item->setText(0, reminder.dateTime.toString("yyyy-MM-dd hh:mm"));
            item->setText(1, reminder.content);
        }
    }
}

void MainWindow::updateWeather(const QString &icon, const QString &temp, const QString &text) {
    auto scene = new QGraphicsScene();
    ui->weatherIconView->setScene(scene);
    auto svgItem = new QGraphicsSvgItem("./icons/" + icon + "-fill.svg");
    scene->addItem(svgItem);
    ui->weatherIconView->fitInView(svgItem);
    ui->weatherInfo->setText(temp + " ℃" + " " + text);
}

void MainWindow::refreshMainWindow(int year, int month) {
    updateCalender(year, month);
    updateReminderTree();
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
        int row = (firstDayOfWeek + day - 1) / 7; // 计算行号
        int column = (firstDayOfWeek + day - 1) % 7; // 计算列号

        auto *item = new QTableWidgetItem(QString::number(day));
        // 检查该日期是否有日程
        QDate date(year, month, day);
        for (const Reminder &reminder : reminders) {
            if (reminder.dateTime.date() == date) {
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
            QTableWidgetItem *item = ui->tableWidget->item(row, col);
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

void MainWindow::updateReminderTree() {
    ui->reminderTree->clear();
    for (const Reminder &reminder : reminders) {
        auto item = new QTreeWidgetItem(ui->reminderTree);
        item->setText(0, reminder.dateTime.toString("yyyy-MM-dd hh:mm"));
        item->setText(1, reminder.content);
    }
    ui->reminderTree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->reminderTree->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
}

void MainWindow::saveReminders() {
    QJsonArray reminderArray;
    for (const Reminder &reminder : reminders) {
        QJsonObject reminderObject;
        reminderObject["dateTime"] = reminder.dateTime.toString(Qt::ISODate);
        reminderObject["content"] = reminder.content;
        reminderArray.append(reminderObject);
    }
    QJsonObject jsonObject;
    jsonObject["reminders"] = reminderArray;
    QJsonDocument jsonDocument(jsonObject);
    QFile file("reminders.json");
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
        return;
    }
    file.write(jsonDocument.toJson());
}

void MainWindow::loadReminders() {
    QFile file("reminders.json");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open save file. Creating a new one.");
        saveReminders();
        return;
    }
    QByteArray saveData = file.readAll();
    QJsonDocument jsonDocument(QJsonDocument::fromJson(saveData));
    QJsonObject jsonObject = jsonDocument.object();
    QJsonArray reminderArray = jsonObject["reminders"].toArray();
    for (auto && i : reminderArray) {
        QJsonObject reminderObject = i.toObject();
        Reminder reminder;
        reminder.dateTime = QDateTime::fromString(reminderObject["dateTime"].toString(), Qt::ISODate);
        reminder.content = reminderObject["content"].toString();
        reminders.append(reminder);
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Delete) {
        deleteReminder();
    }
}
