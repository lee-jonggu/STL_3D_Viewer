#pragma once
#include <QVTKOpenGLNativeWidget.h>
#include <QVTKInteractorAdapter.h>
#include <vtkSTLReader.h>
#include <vtkActor.h>

class CustomVTKWidget : public QVTKOpenGLNativeWidget 
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
};