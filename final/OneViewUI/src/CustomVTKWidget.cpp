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
	mColorDialog = new QColorDialog(this);
	mColorDialog->hide();

	connect(mColorDialog, SIGNAL(currentColorChanged(QColor)), this, SLOT(on_ColortoolButton_clicked(QColor)));

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

	mVtkOrient = vtkOrientationMarkerWidget::New();
	vtkSmartPointer< vtkAxesActor> Axes = vtkSmartPointer< vtkAxesActor>::New();
	mVtkOrient->SetOrientationMarker(Axes); 
	mVtkOrient->SetInteractor(mInteractor);
	mVtkOrient->SetViewport(0.0, 0.0, 0.2, 0.2);
	mVtkOrient->SetEnabled(0);
	mVtkOrient->InteractiveOff();
	mVtkOrient->GlobalWarningDisplayOff(); 
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

void CustomVTKWidget::addEdge(vtkSmartPointer<vtkActor> edge)
{
	mRenderer->AddActor(edge); 
	edgeActor.push_back(edge);
	mRenderWindow->Render();
}

void CustomVTKWidget::addBox(vtkSmartPointer<vtkActor> box)
{
	mRenderer->AddActor(box);
	boxActor.push_back(box);
	mRenderWindow->Render();
}

void CustomVTKWidget::func(vtkSmartPointer<vtkActor> Actor)
{ 
	AddActor(Actor);
}

void CustomVTKWidget::showAxis()
{
	if (mVtkOrient->GetEnabled() == 1)
	{ 
		mVtkOrient->SetEnabled(0); 
		mRenderWindow->Render();
	}
	else
	{ 
		mVtkOrient->SetEnabled(1); 
		mRenderWindow->Render();
	}
}  

void CustomVTKWidget::removeEdge()
{
	for (auto actor : edgeActor)
	{
		mRenderer->RemoveActor(actor);
	}
	edgeActor.clear();
	mRenderWindow->Render();
}

void CustomVTKWidget::removeBox()
{
	for (auto actor : boxActor)
	{
		mRenderer->RemoveActor(actor);
	}
	boxActor.clear();
	mRenderWindow->Render();
}

void CustomVTKWidget::lineActor(vtkSmartPointer<vtkActor> Actor)
{
	AddActor(Actor);
	lineActorVector.push_back(Actor);
}

void CustomVTKWidget::PathButtonClicked()
{
	customInteractorStyle->PathButtonStatus();
}

void CustomVTKWidget::ConnectButtonClicked()
{
	customInteractorStyle->AutoConnection();
}

void CustomVTKWidget::HideButtonClicked()
{
	for (auto actor : lineActorVector)
	{
		if (actor->GetVisibility() == false)
		{
			actor->SetVisibility(true);
		}
		else
		{
			actor->SetVisibility(false);
		}
	}
	mRenderWindow->Render();
}

void CustomVTKWidget::ColorButtonClicked()
{
	mColorDialog->show();
}

void CustomVTKWidget::on_ColortoolButton_clicked(QColor color)
{
	double red = color.toRgb().redF();
	double green = color.toRgb().greenF();
	double blue = color.toRgb().blueF();
	for (auto actor : lineActorVector)
	{
		actor->GetProperty()->SetDiffuseColor(red, green, blue);
	}
	mRenderWindow->Render();
}
