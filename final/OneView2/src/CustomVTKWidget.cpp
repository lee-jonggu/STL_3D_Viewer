#pragma once
#include "CustomVTKWidget.h"
#include <vtkGenericOpenGLRenderWindow.h>
#include "CustomInteractorStyle.h"
#include <QVTKInteractor.h>

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkFloatArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h> 
#include <vtkLight.h>
#include <QPushButton>
#include <QWidget>
#include <QVBoxLayout>


CustomVTKWidget::CustomVTKWidget()
{

}

CustomVTKWidget::CustomVTKWidget(QWidget* parent)
	: QVTKOpenGLNativeWidget(parent)
{    
	vtkOrientationMarkerWidget* vtkOrient = vtkOrientationMarkerWidget::New();
	vtkSmartPointer< vtkAxesActor> Axes = vtkSmartPointer< vtkAxesActor>::New(); 

	mRenderer = vtkSmartPointer<vtkRenderer>::New();
	mRenderer->SetBackground(0.2, 0.2, 0.2); // 배경색 지정
	mRenderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
	setRenderWindow(mRenderWindow);
	mRenderWindow->AddRenderer(mRenderer);
	customInteractorStyle = new CustomInteractorStyle;
	customInteractorStyle->addObserver(this);
	mInteractor = vtkSmartPointer<QVTKInteractor>::New();
	mRenderWindow->SetInteractor(mInteractor);
	mInteractor->Initialize();
	customInteractorStyle->SetCurrentRenderer(mRenderer); 
	mInteractor->SetInteractorStyle(customInteractorStyle);
	customInteractorStyle->Delete();
	mvtkInteractorAdapter = new QVTKInteractorAdapter(this);

	vtkOrient->SetOrientationMarker(Axes);
	vtkOrient->SetInteractor(mInteractor);
	vtkOrient->SetViewport(0.0, 0.0, 0.2, 0.2);
	vtkOrient->SetEnabled(1);
	vtkOrient->InteractiveOff();  
}

CustomVTKWidget::~CustomVTKWidget()
{

}

void CustomVTKWidget::AddActor(vtkSmartPointer<vtkActor> actor)
{
	mRenderer->AddActor(actor); 
	mRenderWindow->Render();
}

void CustomVTKWidget::GetPolyData(vtkSmartPointer<vtkPolyData> polyData)
{
	customInteractorStyle->GetPolyData(polyData);
}

void CustomVTKWidget::GetSphere(vtkSmartPointer<vtkSphereSource> sphere)
{
	customInteractorStyle->GetSphere(sphere); 
}

void CustomVTKWidget::AddSphere(vtkSmartPointer<vtkActor> sphere)
{
	mRenderer->AddActor(sphere);
}

void CustomVTKWidget::func(vtkSmartPointer<vtkActor> Actor)
{ 
	AddActor(Actor);
} 