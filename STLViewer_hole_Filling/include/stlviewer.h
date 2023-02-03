#ifndef STLVIEWER_H
#define STLVIEWER_H

#include "CustomVTKWidget.h"
#include "TriMesh.h"

#include <QMainWindow>
#include <vtkSTLReader.h>
#include <vtkMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <QColorDialog.h>
#include <vtkPolyData.h>
#include <vtkAxesActor.h> 
#include <vtkTransform.h> 
#include <QSlider>
#include <QResizeEvent>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QWidget>
#include <vtkGenericOpenGLRenderWindow.h> 
#include <vtkCellPicker.h>
#include <vtkNamedColors.h> 

QT_BEGIN_NAMESPACE
namespace Ui { class STLViewer; }
QT_END_NAMESPACE

class STLViewer : public QMainWindow
{
    Q_OBJECT

public:
    STLViewer(QWidget *parent = nullptr);
    ~STLViewer();

protected:
    virtual void resizeEvent(QResizeEvent* event) override;
    TriMesh convertToMesh(vtkSmartPointer<vtkPolyData>);

private:
    Ui::STLViewer *ui;

    CustomVTKWidget* customVTKWidget;

    vtkSmartPointer<vtkActor> mActor; 
    vtkSmartPointer<vtkPolyData> mPolyData;
    
    QColorDialog* mColorDialog;   
    QSlider* mSlider;  

    QPushButton* mHoleFilling;
    QHBoxLayout* mHLayout;
    QWidget* mWidget;

signals:

private slots:
    void ClickedOpen(bool);                         // Menu -> Open 
    void SetColor(QColor);                          // Actor Color Change
    void SetOpacity(int);                           // Acotr Opacity Change 

    void HoleFilling();

};
#endif // STLVIEWER_H
