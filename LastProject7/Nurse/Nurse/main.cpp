#include "nurse.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Nurse w;
    w.show();
    return a.exec();
}
