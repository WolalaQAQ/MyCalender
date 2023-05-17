#ifndef CALENDERDIALOG_H
#define CALENDERDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class CalenderDialog; }
QT_END_NAMESPACE

class CalenderDialog : public QDialog {
Q_OBJECT

public:
    CalenderDialog(QWidget *parent = nullptr);

    ~CalenderDialog();

private:
    Ui::CalenderDialog *ui;
public slots:

    void yearComboxChanged(const QString& year);

    void monthComboxChanged(const QString& month);
private:
    void refreshCalender(int year, int month);
};

#endif // CALENDERDIALOG_H
