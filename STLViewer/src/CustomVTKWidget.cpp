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



CustomVTKWidget::CustomVTKWidget()
{
	
}

CustomVTKWidget::CustomVTKWidget(QWidget* parent)
	: QVTKOpenGLNativeWidget(parent)
{
	vtkNew<vtkNamedColors> colors;
	vtkSmartPointer<vtkOrientationMarkerWidget> mOrientationMarkerWidget = vtkOrientationMarkerWidget::New();

	mRenderer = vtkSmartPointer<vtkRenderer>::New();
	mRenderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();



	setRenderWindow(mRenderWindow);
	mRenderWindow->AddRenderer(mRenderer);
	customInteractorStyle = new CustomInteractorStyle;
	customInteractorStyle->addObserver(this);
	mInteractor = vtkSmartPointer<QVTKInteractor>::New();
	mRenderWindow->SetInteractor(mInteractor);
	mInteractor->Initialize();
	customInteractorStyle->SetCurrentRenderer(mRenderer);
	mRenderer->SetBackground(0.5, 0.5, 0.5);
	mInteractor->SetInteractorStyle(customInteractorStyle);
	customInteractorStyle->Delete();

	mvtkInteractorAdapter = new QVTKInteractorAdapter(this);

	vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();

	mOrientationMarkerWidget->SetOrientationMarker(axes);
	mOrientationMarkerWidget->SetInteractor(mInteractor);
	mOrientationMarkerWidget->SetViewport(0.0, 0.0, 0.2, 0.2);
	mOrientationMarkerWidget->SetEnabled(1);  
	mOrientationMarkerWidget->InteractiveOff();
	  
	//mRenderer->ResetCamera();
}

CustomVTKWidget::~CustomVTKWidget()
{

}

void CustomVTKWidget::AddActor(vtkSmartPointer<vtkActor> actor)
{ 
	//vtkNew<vtkTransform> transform;
	////transform->Translate(actor->GetCenter()[0], actor->GetCenter()[1], actor->GetCenter()[2]);
	//transform->Translate(0, 0, 0);
	//double l[3];
	//l[0] = (actor->GetBounds()[1] - actor->GetBounds()[0]) * 1.5;
	//l[1] = (actor->GetBounds()[3] - actor->GetBounds()[2]) * 1.5;
	//l[2] = (actor->GetBounds()[5] - actor->GetBounds()[4]) * 1.5;
	//axes->SetTotalLength(l);
	//axes->SetUserTransform(transform); 
 //   auto cen = actor->GetCenter(); 
	// 
	//mRenderer->GetActiveCamera()->SetPosition(100.0, -200.0, 130.0);  
	//mRenderer->GetActiveCamera()->SetViewUp(0, 1, 0);
	//mRenderer->GetActiveCamera()->SetFocalPoint(0.0, 0.0, 0.0);
	//mRenderer->GetActiveCamera()->Elevation(50); 

	mRenderer->AddActor(actor);  
	mRenderer->GetRenderWindow()->Render();
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

