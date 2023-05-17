#include "main_window.h"
#include "ui_main_window.h"
#include "reminder_dialog.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent),
        ui(new Ui::MainWindow) {
    ui->setupUi(this);
    loadReminders();
    // 添加日程提醒的QAction
    connect(ui->openReminderDialogAction, &QAction::triggered, this, &MainWindow::openReminderDialog);
    // 添加日程提醒的QTableWidget
    connect(ui->tableWidget, &QTableWidget::cellDoubleClicked, this, &MainWindow::showReminder);
    // 修改日程提醒的QListWidget
    connect(ui->reminderList, &QListWidget::itemDoubleClicked, this, &MainWindow::editReminder);
    // 显示当前的时间
    auto currentDateTime = QDateTime::currentDateTime();
    QString currentTimeString = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");
    ui->currentTime->setText(currentTimeString);
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

    connect(ui->yearCombox, SIGNAL(currentTextChanged(QString)), this, SLOT(yearComboxChanged(QString)));
    connect(ui->monthCombox, SIGNAL(currentTextChanged(QString)), this, SLOT(monthComboxChanged(QString)));

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

void MainWindow::refreshMainWindow(int year, int month) {
    updateCalender(year, month);
    updateReminderList();
}

void MainWindow::openReminderDialog() {
    ReminderDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        Reminder reminder;
        reminder.date = dialog.getDate();
        reminder.content = dialog.getContent();
        reminders.append(reminder);
        refreshMainWindow(currentYear_, currentMonth_);
        saveReminders();
    }
}

void MainWindow::showReminder(int row, int column) {
    QDate date(ui->yearCombox->currentText().toInt(),
               ui->monthCombox->currentText().toInt(),
               ui->tableWidget->item(row, column)->text().toInt());
    // 创建一个列表来存储当日的所有日程
    QStringList reminderList;
    for (const Reminder &reminder : reminders) {
        if (reminder.date == date) {
            reminderList.append(reminder.content);
        }
    }
    // 如果有日程，显示一个消息框列出所有日程
    if (!reminderList.isEmpty()) {
        QMessageBox::information(this, tr("Reminders"), reminderList.join("\n"));
    }
}

void MainWindow::editReminder(QListWidgetItem *item) {
    int index = ui->reminderList->row(item);
    Reminder &reminder = reminders[index];
    ReminderDialog dialog(this);
    dialog.setDate(reminder.date);
    dialog.setContent(reminder.content);
    if (dialog.exec() == QDialog::Accepted) {
        reminder.date = dialog.getDate();
        reminder.content = dialog.getContent();
        refreshMainWindow(currentYear_, currentMonth_);
        saveReminders();
    }
}

void MainWindow::updateCalender(int year, int month) {
    // 设置行、列数量
    ui->tableWidget->setRowCount(6);
    ui->tableWidget->setColumnCount(7);
    ui->tableWidget->clearContents();
    // 添加星期标题行
    QStringList headerLabels;
    headerLabels << tr("星期一") << tr("星期二")
                 << tr("星期三") << tr("星期四") << tr("星期五")
                 << tr("星期六") << tr("星期日");
    ui->tableWidget->setHorizontalHeaderLabels(headerLabels);
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
            if (reminder.date == date) {
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
    ui->tableWidget->verticalHeader()->setVisible(false);
}

void MainWindow::updateReminderList() {
    ui->reminderList->clear();
    for (const Reminder &reminder : reminders) {
        ui->reminderList->addItem(reminder.date.toString("yyyy-MM-dd") + ": " + reminder.content);
    }
}

void MainWindow::saveReminders() {
    QJsonArray reminderArray;
    for (const Reminder &reminder : reminders) {
        QJsonObject reminderObject;
        reminderObject["date"] = reminder.date.toString(Qt::ISODate);
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
        reminder.date = QDate::fromString(reminderObject["date"].toString(), Qt::ISODate);
        reminder.content = reminderObject["content"].toString();
        reminders.append(reminder);
    }
}


