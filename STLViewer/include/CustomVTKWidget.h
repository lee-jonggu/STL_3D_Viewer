#pragma once
#include <QVTKOpenGLNativeWidget.h>
#include <QVTKInteractorAdapter.h>
#include <vtkSTLReader.h>
#include <vtkActor.h>
#include "CustomInteractorStyle.h"
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include "observer.h"
#include <vtkOrientationMarkerWidget.h>
#include <vtkAxesActor.h>
#include <QDebug>
#include <vtkLight.h>
#include <QPushButton>
#include <QWidget>
#include <QHBoxLayout>
#include <QColorDialog>


class CustomVTKWidget : public QVTKOpenGLNativeWidget , public Observer
{ 
    Q_OBJECT

public:
    CustomVTKWidget();
	CustomVTKWidget(QWidget* parent);
	~CustomVTKWidget();

    // Renderer Add Actor
    void AddActor(vtkSmartPointer<vtkActor>);

    void GetPolyData(vtkSmartPointer<vtkPolyData>);
    void GetSphere(vtkSmartPointer<vtkSphereSource>);
    void AddSphere(vtkSmartPointer<vtkActor>);


protected:
    // VTK Renderer
    vtkSmartPointer<vtkRenderer> mRenderer; 
    // VTK Render Window
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> mRenderWindow; 
    vtkSmartPointer<QVTKInteractor> mInteractor;
    QVTKInteractorAdapter* mvtkInteractorAdapter; 
    
    virtual void func(vtkSmartPointer<vtkActor>);
   // virtual void funcRander(vtkSmartPointer<vtkRenderer>);


private slots:
    void on_AmbientButton_clicked();
    void on_DiffusetButton_clicked();
    void on_SpotPushButton_clicked();

private: 
    vtkSmartPointer<vtkLight> greenLight;//
    vtkSmartPointer<vtkNamedColors> colors; //

    vtkSmartPointer<vtkLight> mLight;
    QPushButton* mAmbient;
    QPushButton* mDiffuseButton;
    QPushButton* mSpotPushButton;
    QWidget* mWViewer;
    QHBoxLayout* mlayout;
    QColorDialog* mColorDialog;
    CustomInteractorStyle* customInteractorStyle;
};