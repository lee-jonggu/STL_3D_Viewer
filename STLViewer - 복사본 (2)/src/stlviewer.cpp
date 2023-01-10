#include "stlviewer.h"
#include "./ui_stlviewer.h"
#include <QDebug>
#include <QFileDialog>
#include <QColorDialog>
#include <vtkPointData.h>



STLViewer::STLViewer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::STLViewer)
{
    ui->setupUi(this);

    connect(ui->actionOpen, SIGNAL(triggered(bool)), SLOT(ClickedOpen(bool))); 
    connect(ui->actionColor, &QAction::triggered, this, [this](bool) { emit SendActor(mActor); }); 
    connect(this, SIGNAL(SendActor(vtkSmartPointer<vtkActor>)), this, SLOT(ReceiveActor(vtkSmartPointer<vtkActor>)));
}

STLViewer::~STLViewer()
{
    delete ui;
}

void STLViewer::ClickedOpen(bool)
{
    qDebug("Triggered Open Button");
    std::string fileName = QFileDialog::getOpenFileName(nullptr,
        "STL Files", "/home", "STL Files (*.stl)").toStdString().c_str();
    
    // STL Reader
    vtkSmartPointer<vtkSTLReader> mSTLReader = vtkSmartPointer<vtkSTLReader>::New();
    mSTLReader->SetFileName(fileName.c_str());
    mSTLReader->Update();

    // Mapper
    vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
    polyData->DeepCopy(mSTLReader->GetOutput());
    vtkSmartPointer<vtkPolyDataMapper> mMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mMapper->SetInputData(polyData);

    // Actor
    mActor = vtkSmartPointer<vtkActor>::New(); 
    mActor->SetMapper(mMapper); 
    ui->openGLWidget->AddActor(mActor);
    qDebug() << mActor;
}

void STLViewer::ReceiveActor(vtkSmartPointer<vtkActor> Actor)
{
    qDebug() << Actor;
    QColor color = QColorDialog::getColor(Qt::green);
    
    double r = color.toRgb().redF();
    double g = color.toRgb().greenF();
    double b = color.toRgb().blueF();
    qDebug() << "\n" << r << g << b;
    Actor->GetProperty()->SetDiffuseColor(r, g, b);  
    Actor->Modified();

    ui->openGLWidget->GetRenderWindow()->Render();
}