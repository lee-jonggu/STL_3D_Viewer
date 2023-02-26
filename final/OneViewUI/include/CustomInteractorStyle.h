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
#include "observer.h"
#include "TriMesh.h"
#include <vtkDijkstraGraphGeodesicPath.h>
#include <iostream>
#include <vector>
#include <queue>
#include <vtkRenderStepsPass.h>
#include <vtkOutlineGlowPass.h>


using namespace std;

class CustomInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
	CustomInteractorStyle();
	~CustomInteractorStyle();

	void GetPolyData(vtkSmartPointer<vtkPolyData>);
	void GetSphere(vtkSmartPointer<vtkSphereSource>);
	void GetActor(vtkSmartPointer<vtkActor>);
	void pathButton();
	void autoConnect(); 
	void MeshCutting(TriMesh& triMesh, int faceId);
	void addObserver(Observer*);
	std::vector<int> dijkstra(int startIdx, int endIdx, const TriMesh& triMesh);
	vtkNew<vtkActor> GetSphere(vtkStdString color, double x, double y, double z);

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
	Observer* mObserver;
	bool pathButtonStatus;
	bool SelectDeleteAll;
	bool NotFirst;
	bool CompleteCurve;
	bool DeleteComplete;
	bool start;
	int pickCount;
	int firstVertex;
	vector<int> curve;
	vector<int> findCurveId;
	vector<int> vertexIdxs;
	TriMesh mTriMesh;
};
