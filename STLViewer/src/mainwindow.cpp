#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "doctor.h"
#include "stlviewer.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
   // this->setStyleSheet("background-color:#003458;");
    QToolBar tool;
    this->setStyleSheet("QToolBar{ background-color:#666666;}"
                        "QToolButton:hover {background: #5d7180; border: 1px groove #293843;}"
                       );

    this->resize(1920,1080);
    doctor = new Doctor(this);
    QMdiSubWindow *doctorW = ui->mdiArea->addSubWindow(doctor);

    doctor->setWindowTitle(tr("doctor"));
    doctor->setWindowFlags(Qt::FramelessWindowHint);


    stlviewer = new STLViewer(this);
    stlviewer->setWindowTitle("StlViewer");
    stlviewer->setWindowFlags(Qt::FramelessWindowHint);
    stlviewer->showMaximized();

    ui->mdiArea->addSubWindow(stlviewer);
    ui->mdiArea->setActiveSubWindow(doctorW);
    ui->mdiArea->setWindowFlags(Qt::FramelessWindowHint);
    ui->mdiArea->showMaximized();
//    doctor->loadDB();
    doctor->showMaximized();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionDoctor_triggered()
{
    if(doctor != nullptr)
    {
        doctor->setFocus();
    }
}

void MainWindow::on_actionStlViewer_triggered()
{
    if(stlviewer != nullptr)
    {
        stlviewer->setFocus();
    }
}
