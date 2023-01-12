#include "doctor.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Doctor w;
    w.show();
    return a.exec();
}
