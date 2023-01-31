#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "doctor.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->resize(1920,1080);

    doctor = new Doctor(ui->widget);

    doctor->loadDB();
    doctor->loadSTL();
}

MainWindow::~MainWindow()
{
    delete ui;
}

