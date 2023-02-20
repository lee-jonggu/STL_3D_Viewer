#pragma once
#include <vtkActor.h>
class Observer
{
public : 
	virtual void func(vtkSmartPointer<vtkActor>) = 0;
	virtual void addEdge(vtkSmartPointer<vtkActor>) = 0;
	virtual void removeEdge() = 0;
	virtual void lineActor(vtkSmartPointer<vtkActor>) = 0;
	virtual void addBox(vtkSmartPointer<vtkActor>) = 0;
	virtual void removeBox() = 0;
};