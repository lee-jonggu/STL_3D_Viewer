#ifndef STLVIEWER_H
#define STLVIEWER_H

#include "TriMesh.h"
#include "CustomVTKWidget.h"

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
#include <QFileDialog>


QT_BEGIN_NAMESPACE
namespace Ui { class STLViewer; }
QT_END_NAMESPACE

class STLViewer : public QWidget
{
    Q_OBJECT

public:
    explicit STLViewer(QWidget *parent = nullptr);
    ~STLViewer();

protected:
    virtual void resizeEvent(QResizeEvent* event) override;
    TriMesh convertToMesh(vtkSmartPointer<vtkPolyData>);
    vtkSmartPointer<vtkPolyData> convertToPolyData(TriMesh);

private:
    Ui::STLViewer* ui;
    QSlider* m_Slider;
    QPushButton* m_Cutbutton;
    QPushButton* m_Openbutton;
    QPushButton* m_Colorbutton;
    QPushButton* m_Light;
    QPushButton* m_Sliderbutton; //sliderbutton
    QPushButton* m_HoleFilling;      
    QHBoxLayout* m_Hlayout;
    QVBoxLayout* m_Vlayout;
    QWidget* w;

    CustomVTKWidget* customVTKWidget;

    vtkSmartPointer<vtkActor> mActor;
    vtkSmartPointer<vtkRenderer> mRenderer;

    QColorDialog* mColorDialog; 

    vtkSmartPointer<vtkLight> mLight;

    vtkSmartPointer<vtkPolyData> mPolyData;

    void MakeMesh(std::vector<std::vector<TriMesh::VertexHandle> >, OpenMesh::Vec3d, TriMesh&);
    void CleanBoundary(TriMesh&);

private slots:
    void on_pushButton_clicked();
    void ClickedOpen();

    void SetColor(QColor);                          // Actor Color Change
    void SetOpacity(int);                           // Acotr Opacity Change
    void HoleFilling();                             // Filling hole
    void SetLight(); 
};

#endif // STLVIEWER_H
