#pragma once
#include "CustomInteractorStyle.h"
#include "observer.h"

#include <QVTKOpenGLNativeWidget.h>
#include <QVTKInteractorAdapter.h>
#include <vtkSTLReader.h>
#include <vtkActor.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkSmartPointer.h> 
#include <vtkAxesActor.h> 
#include <vtkTransform.h> 
#include <vtkOrientationMarkerWidget.h> 

class CustomVTKWidget : public QVTKOpenGLNativeWidget , public Observer
{
public:
    CustomVTKWidget();
	CustomVTKWidget(QWidget* parent);
	~CustomVTKWidget();

    // Renderer Add Actor
    void AddActor(vtkSmartPointer<vtkActor>);


protected:
    // VTK Renderer
    vtkSmartPointer<vtkRenderer> mRenderer; 

    // VTK Render Window
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> mRenderWindow; 
    vtkSmartPointer<QVTKInteractor> mInteractor;
    QVTKInteractorAdapter* mvtkInteractorAdapter; 

    virtual void func(vtkSmartPointer<vtkActor>);

private slots: 

private:  
    CustomInteractorStyle* customInteractorStyle;  
};