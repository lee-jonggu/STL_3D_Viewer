#include "doctor.h"
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    Doctor w;
//    w.show();
    MainWindow mainWindow;
    mainWindow.show();

    return a.exec();
}
