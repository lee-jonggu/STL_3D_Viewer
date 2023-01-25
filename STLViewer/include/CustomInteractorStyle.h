#pragma once

#include <QWidget> 
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <QVTKInteractor.h>
#include <vtkInteractorStyle.h> 
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkParallelCoordinatesInteractorStyle.h>
#include <vtkPolyData.h>
#include <vtkCellPicker.h>
#include <vtkIdTypeArray.h>
#include <vtkSelectionNode.h>
#include <vtkSelection.h>
#include <vtkExtractSelection.h>
#include <vtkNamedColors.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkTriangle.h> 
#include <vtkDecimatePro.h>
#include "observer.h"
#include "TriMesh.h"
#include <vtkDijkstraGraphGeodesicPath.h> 

class CustomInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
	CustomInteractorStyle();
	~CustomInteractorStyle();
	
	void GetPolyData(vtkSmartPointer<vtkPolyData>);
	void GetSphere(vtkSmartPointer<vtkSphereSource>);
	void GetActor(vtkSmartPointer<vtkActor>);
	

	void addObserver(Observer*);

protected:
	virtual void OnRightButtonDown() override;
	virtual void OnRightButtonUp() override;
	virtual void OnLeftButtonDown() override;
	virtual void OnLeftButtonUp() override;

	virtual void OnMouseWheelForward() override;
	virtual void OnMouseWheelBackward() override;

	TriMesh convertToMesh(vtkSmartPointer<vtkPolyData>);
	vtkSmartPointer<vtkPolyData> convertToPolyData(TriMesh);
	 
private:
	vtkSmartPointer<vtkPolyData> mPolyData;
	vtkSmartPointer<vtkSphereSource> mSphere;
	vtkSmartPointer<vtkActor> mActor;
	vtkSmartPointer<vtkActor> mVertexActor;
	vtkSmartPointer<vtkPoints> mVertex;
	vtkNew<vtkActor> mNeighborVertexActor; 
	std::vector<int> vertexId; 
	std::vector<int> mAllVertex; 
	std::vector<int> mCalVertex;
	int mVertexCount;
	//std::vector<TriMesh::VertexHandle> vertexHandleVector;
	OpenMesh::Vec3d startVertex;
	OpenMesh::Vec3d endVertex;
	bool flag;
	OpenMesh::Vec3d negihborVertex;


	Observer* mObserver;
};
