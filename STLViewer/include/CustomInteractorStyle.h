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
#include <unordered_map> 
#include <queue> 

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

	void Dijkstra3D(TriMesh, OpenMesh::VertexHandle, OpenMesh::VertexHandle);

private:
	vtkSmartPointer<vtkPolyData> mPolyData;
	vtkSmartPointer<vtkSphereSource> mSphere;
	vtkSmartPointer<vtkActor> mActor;
	vtkSmartPointer<vtkActor> mVertexActor;
	vtkSmartPointer<vtkPoints> mVertex;
	vtkNew<vtkActor> mNeighborVertexActor;
	std::vector<OpenMesh::VertexHandle> visited_vertices;
	std::vector<OpenMesh::VertexHandle> vertices_between;
	OpenMesh::VertexHandle start_vertex;
	OpenMesh::VertexHandle end_vertex;
	OpenMesh::VertexHandle current_vertex;
	std::vector<int> vertexId;
	std::vector<int> mAllVertex;
	std::vector<int> mCalVertex;
	std::vector<OpenMesh::VertexHandle> mNeighborVertex;
	int mVertexCount;
	//std::vector<TriMesh::VertexHandle> vertexHandleVector;
	OpenMesh::Vec3d startVertex;
	OpenMesh::Vec3d endVertex;
	bool flag;
	OpenMesh::Vec3d negihborVertex;
	int mMinDiffVertexId;                                // 가장 가까운 버텍스 ID
	double mVertexDistance;                              // 현재 버텍스에서 가장 가까운 버텍스 간의 거리


	Observer* mObserver;
};