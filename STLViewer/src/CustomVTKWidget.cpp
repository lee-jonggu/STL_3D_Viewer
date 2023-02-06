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

	mAmbient = new QPushButton("Ambient", this);
	mDiffuseButton = new QPushButton("Diffuse", this);
	mSpotPushButton = new QPushButton("SpotButton", this);


	greenLight = vtkSmartPointer<vtkLight>::New();

	mWViewer = new QWidget(this);
	mlayout = new QHBoxLayout();
	mlayout->addWidget(mAmbient);
	mlayout->addWidget(mDiffuseButton);
	mlayout->addWidget(mSpotPushButton);

	mWViewer->setLayout(mlayout);

	mWViewer->setStyleSheet("QWidget::horver{"
		"background-color:#fff;"
		"border: 10px;}");

	vtkOrientationMarkerWidget* vtkOrient = vtkOrientationMarkerWidget::New();
	vtkSmartPointer< vtkAxesActor> Axes = vtkSmartPointer< vtkAxesActor>::New();
	mLight = vtkSmartPointer<vtkLight>::New();
	colors = vtkSmartPointer<vtkNamedColors>::New();

	mColorDialog = new QColorDialog();

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

	connect(mAmbient, SIGNAL(clicked()), this, SLOT(on_AmbientButton_clicked()));
	connect(mDiffuseButton, SIGNAL(clicked()), this, SLOT(on_DiffusetButton_clicked()));
	connect(mSpotPushButton, SIGNAL(clicked()), this, SLOT(on_SpotPushButton_clicked()));
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

//void CustomVTKWidget::funcRander(vtkSmartPointer<vtkRenderer> Render)
//{
//
//}

void CustomVTKWidget::on_AmbientButton_clicked()
{
	qDebug() << "11111111111111";

	//mColorDialog->show();

	QColor color = mColorDialog->getColor();
	double r = color.toRgb().redF();
	double g = color.toRgb().greenF();
	double b = color.toRgb().blueF();
	
	mLight->SetLightTypeToSceneLight();
	//mLight->SetLightTypeToCameraLight();

	mLight->SetAmbientColor(0.1,0,0);
	qDebug() << "mLight" << mLight;
	mRenderer->AddLight(mLight);
	mInteractor->Start();
	
}

void CustomVTKWidget::on_DiffusetButton_clicked()
{
	QColor color = mColorDialog->getColor();
	double r = color.toRgb().redF();
	double g = color.toRgb().greenF();
	double b = color.toRgb().blueF();

	mLight->SetLightTypeToSceneLight();
	//mLight->SetLightTypeToCameraLight();

	mLight->SetDiffuseColor(r, g, b);
	mRenderer->AddLight(mLight);
	mInteractor->Start();
}

void CustomVTKWidget::on_SpotPushButton_clicked()
{
	qDebug() << "22222222222222222222";
	QColor color = mColorDialog->getColor();
	double r = color.toRgb().redF();
	double g = color.toRgb().greenF();
	double b = color.toRgb().blueF();

	mRenderer->ResetCamera();
	mRenderer->GetActiveCamera()->Azimuth(40.0);
	mRenderer->GetActiveCamera()->Elevation(30.0);

	greenLight->PositionalOn();
	greenLight->SetColor(r,g,b);
	greenLight->SetPosition(-4.0, 4.0, -1.0);
	greenLight->SetIntensity(0.5);

	mRenderer->AddLight(greenLight);

	mRenderer->SetBackground(colors->GetColor3d("RoyalBlue").GetData());
	mRenderer->GradientBackgroundOn();

	mInteractor->Initialize();
	mRenderWindow->Render();
	mInteractor->Start();
}