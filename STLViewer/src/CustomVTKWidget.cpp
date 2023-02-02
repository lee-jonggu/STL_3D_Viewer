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
	  
	mRenderer->ResetCamera(); 
}

CustomVTKWidget::~CustomVTKWidget()
{

}

void CustomVTKWidget::AddActor(vtkSmartPointer<vtkActor> actor)
{ 
	mRenderer->AddActor(actor);  
	mRenderer->GetRenderWindow()->Render();
}


void CustomVTKWidget::func(vtkSmartPointer<vtkActor> Actor)
{
	AddActor(Actor);
}
