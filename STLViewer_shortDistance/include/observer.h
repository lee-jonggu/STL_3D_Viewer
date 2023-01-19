#pragma once
#include <vtkActor.h>
#include <vtkPoints.h>

class Observer
{
public : 
	virtual void func(vtkSmartPointer<vtkActor>) = 0;
	//virtual void vertexPoints(vtkSmartPointer<vtkPoints>, int) = 0;
};