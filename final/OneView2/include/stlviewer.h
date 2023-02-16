#ifndef STLVIEWER_H
#define STLVIEWER_H

#include "TriMesh.h"
#include "CustomVTKWidget.h"
#include "cutform.h"
#include "lightwidgetform.h"
#include "shadingform.h"

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
#include <QMessageBox>
#include <vtkWindowToImageFilter.h>
#include <vtkSTLWriter.h>
#include <vtkGlyph3D.h>
#include <vtkPolyDataMapper.h>


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
    CutForm *mCutform;
    LightWidgetForm *lightform;
    ShadingForm *shadingform;
    QSlider* m_Slider;
     
    QPushButton* m_HoleFilling;        

    CustomVTKWidget* customVTKWidget;

    vtkSmartPointer<vtkActor> mActor;
    vtkSmartPointer<vtkRenderer> mRenderer;

    QColorDialog* mColorDialog;  

    vtkSmartPointer<vtkPolyData> mPolyData;
    TriMesh mTriMesh;
    vtkSmartPointer<vtkPolyData> mMeshToPoly;

    void MakeMesh(std::vector<std::vector<TriMesh::VertexHandle> >, OpenMesh::Vec3d, TriMesh&);
    void AdvancingFrontMethod(TriMesh&);
    std::vector<std::vector<TriMesh::VertexHandle>> FindHoleVertex(TriMesh&);

private slots:   
    void SetOpacity(int);                           // Acotr Opacity Change
    void HoleFilling();                             // Filling hole 

    void on_CuttoolButton_clicked();
    void on_FillingtoolButton_clicked();
    void on_OpacitytoolButton_clicked();
    void on_LighttoolButton_clicked();
    void on_ShadingtoolButton_clicked();
    void on_ColortoolButton_clicked(QColor);
    void on_BoxtoolButton_clicked();
    void on_VertextoolButton_clicked();
    void on_WiretoolButton_clicked();
    void on_AxistoolButton_clicked();
    void on_OpentoolButton_clicked(); 
    void on_SavetoolButton_clicked();
};

#endif // STLVIEWER_H
