#include <QDate>
#include <QTimer>
#include "calender_dialog.h"
#include "ui_calender_dialog.h"

CalenderDialog::CalenderDialog(QWidget *parent)
        : QDialog(parent), ui(new Ui::CalenderDialog) {
    ui->setupUi(this);
    this->setWindowTitle(QString::fromUtf8("日历小程序"));
    //显示当前的时间
    // 设置标签字体和对齐方式
    QFont font("Arial", 18);
    ui->currentTime->setFont(font);
    ui->currentTime->setAlignment(Qt::AlignLeft);
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString currentTimeString = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");
    ui->currentTime->setText(currentTimeString);
    // 创建定时器
    auto *timer = new QTimer();

    // 每秒更新一次时间
    QObject::connect(timer, &QTimer::timeout, [&]() {
        QDateTime currentDateTime = QDateTime::currentDateTime();
        QString currentTimeString = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");
//        QString currentTimeString = currentDateTime.toString("yyyy-MM-dd");
        ui->currentTime->setText(currentTimeString);
    });

    // 启动定时器
    timer->start(1000);



    //
    // 设置年份范围为当前时间的前 100 年至当前时间的后 10 年
    int currentYear = QDate::currentDate().year();
    for (int year = currentYear - 2022; year <= currentYear + 10000-2023; year++) {
        ui->yearCombox->addItem(QString::number(year));
    }
    ui->yearCombox->setCurrentText(QString::number(currentYear));
//    设置年份可以编辑
//    ui->yearCombox->setEditable(true);
    int currentMonth = QDate::currentDate().month();
    for (int month = 1; month <= 12; ++month) {
        ui->monthCombox->addItem(QString::number(month));
    }
    ui->monthCombox->setCurrentText(QString::number(currentMonth));
//    设置月份份可以编辑
//    ui->monthCombox->setEditable(true);
    connect(ui->yearCombox, SIGNAL(currentTextChanged(QString)), this, SLOT(yearComboxChanged(QString)));
    connect(ui->monthCombox, SIGNAL(currentTextChanged(QString)), this, SLOT(monthComboxChanged(QString)));
    refreshCalender(currentYear, currentMonth);
}

CalenderDialog::~CalenderDialog() {
    delete ui;
}

void CalenderDialog::refreshCalender(int year, int month) {
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

void CalenderDialog::yearComboxChanged(const QString& year) {
    refreshCalender(year.toInt(), ui->monthCombox->currentText().toInt());
}

void CalenderDialog::monthComboxChanged(const QString& month) {
    refreshCalender(ui->yearCombox->currentText().toInt(), month.toInt());
}



