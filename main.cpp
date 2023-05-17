#include "calender_dialog.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CalenderDialog w;
    w.show();
    return QApplication::exec();
}
