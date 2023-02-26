#ifndef STLVIEWER_H
#define STLVIEWER_H

#include "TriMesh.h"
#include "CustomVTKWidget.h"
#include "cutform.h"
#include "lightwidgetform.h"
#include "shadingform.h"
#include "occlusion.h"
#include "filingform.h"
#include "opacity.h"

#include <QMainWindow>
#include <iostream>
#include <string>
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
#include <vtkFeatureEdges.h>
#include <vtkSSAOPass.h>
#include <vtkRenderStepsPass.h>
#include <vtkPolyDataAlgorithm.h>
#include <vtkLight.h>
#include <vtkLightActor.h>
#include <vtkNamedColors.h>
#include <vtkOutlineFilter.h>
#include <vtkPolyDataNormals.h>
#include <vtkPNGReader.h>
#include <vtkTextureMapToSphere.h>
#include <vtkTexture.h>
#include <vtkCamera.h>
#include <vtkSSAOPass.h>
#include <vtkRenderStepsPass.h>
#include <vtkPNGWriter.h> 

class LightWidgetForm;
class ShadingForm;
class FilingForm;
class Opacity;


QT_BEGIN_NAMESPACE
namespace Ui { class STLViewer; }
QT_END_NAMESPACE

class STLViewer : public QWidget
{
    Q_OBJECT

public: 
    STLViewer(QWidget* parent);
    ~STLViewer();

protected: 
    TriMesh convertToMesh(vtkSmartPointer<vtkPolyData>);
    vtkSmartPointer<vtkPolyData> convertToPolyData(TriMesh);
    void closeEvent(QCloseEvent*) override;

private:
    Ui::STLViewer* ui;

    std::string mFileName;

    CutForm *mCutform;
    LightWidgetForm *mLightform;
    ShadingForm *mShadingform;
    CutForm* mCutForm;
    Occlusion* mOcclusion;
    FilingForm* mFillingForm;
    Opacity* mOpacity;

    QSlider* mSlider;
     
    QPushButton* m_HoleFilling;        

    CustomVTKWidget* customVTKWidget;
    
    CustomInteractorStyle* customInteractorStyle;

    vtkSmartPointer<vtkSTLReader> mSTLReader;
    vtkSmartPointer<vtkPolyData> mPolyData;
    vtkSmartPointer<vtkActor> mActor;
    vtkSmartPointer<vtkRenderer> mRenderer;
    vtkSmartPointer<QVTKInteractor> mInteractor;
    vtkSmartPointer<vtkSSAOPass> mSsaoPass;
    vtkSmartPointer<vtkProperty> mOriginProp;
    vtkSmartPointer<vtkTexture> mTexture;
    vtkSmartPointer<vtkLight> mLight;
    vtkSmartPointer<vtkLightActor> mLightActor;
    vtkSmartPointer<vtkActor> mOutlineActor;
    vtkSmartPointer<vtkPolyDataMapper> mMapper;
    vtkSmartPointer<vtkPolyDataNormals> mNormalGenerator;


    QColorDialog* mColorDialog;   

    TriMesh mTriMesh;
    vtkSmartPointer<vtkPolyData> mMeshToPoly;

    void MakeMesh(std::vector<std::vector<TriMesh::VertexHandle> >, OpenMesh::Vec3d, TriMesh&);
    void AdvancingFrontMethod(TriMesh&, std::vector<std::vector<TriMesh::VertexHandle> > );
    std::vector<std::vector<TriMesh::VertexHandle>> FindHoleVertex(TriMesh&);

    bool edgeFlag;
    bool boxFlag;

private slots:    

    void on_CuttoolButton_clicked();  
    void on_LighttoolButton_clicked();
    void on_ShadingtoolButton_clicked();
    void on_ColortoolButton_clicked(QColor);
    void on_BoxtoolButton_clicked();
    void on_VertextoolButton_clicked();
    void on_WiretoolButton_clicked();
    void on_AxistoolButton_clicked();
    void on_OpentoolButton_clicked(); 
    void on_SavetoolButton_clicked(); 
    void on_OcclusiontoolButton_clicked();
    void on_FillingtoolButton_clicked(); 
    void on_OpacitytoolButton_clicked();

    void SetLightAmbientChange(int);
    void setBoundary();

    //light
    void setLightAmbientChange(int);
    void setLightDiffuseChange(int);
    void setLightSpecularChange(int);
    void setSpotChange();
    void setLightXMove();
    void setLightYMove();
    void setLightZMove();
    void setIntensityChange(int);
    void ambientColorChange();
    void diffuseColorChange();
    void specularColorChange(); 
    void setSpotOn();
    void setSpotOff();

    //occlusion
    void setAmbientRadiusChange(int);
    void setAmbientBiasChange(int);
    void setAmbientKernelSizeChange(int);
    void setAmbientBlurOnChange();
    void setAmbientBlurOffChange();

    //shading
    void setFlatChange();
    void setGouraudChange();
    void setPhongChange();
    void setTexture();

    // filling
    void meshButtonClicked();
    void advanceButtonClicked();

    //opacity
    void on_OpacityValuChange(int);

signals:
    void sendToStyle();
};

#endif // STLVIEWER_H
