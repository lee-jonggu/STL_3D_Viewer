#include "stlviewer.h"
#include "ui_stlviewer.h"
#include "CustomInteractorStyle.h"
#include <QDebug> 
#include <TriMesh.h>
#include <algorithm> 

const int PI = 3.141592;
const double thresholdA = 85.0;
const double thresholdB = 135.0;
const double alpha = 0.45;

class Occlusion;

STLViewer::STLViewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::STLViewer)
{  
    ui->setupUi(this);
    mPolyData = vtkSmartPointer<vtkPolyData>::New();
    mColorDialog = new QColorDialog();   
    mFileName = "";
    //QPushButton ColortoolButton = QPushButton("Color", this);

    connect(ui->ColortoolButton, &QPushButton::clicked, this, [this](bool) { if (mActor == nullptr) return; mColorDialog->show(); });
    connect(mColorDialog, SIGNAL(currentColorChanged(QColor)), this, SLOT(on_ColortoolButton_clicked(QColor))); 

    edgeFlag = false;
    boxFlag = false;
    connect(ui->BoundarytoolButton, SIGNAL(clicked()), this, SLOT(setBoundary())); 

    mRenderer = ui->openGLWidget->GetInteractor()->GetInteractorStyle()->GetCurrentRenderer();
    mInteractor = ui->openGLWidget->GetInteractor();
    mMapper = vtkSmartPointer<vtkPolyDataMapper>::New(); 

    mCutform = new CutForm(0);
    mLightform = new LightWidgetForm(0);
    mShadingform = new ShadingForm(0);  
    mOcclusion = new Occlusion(0);
    mFillingForm = new FilingForm(0);
    mOpacity = new Opacity(0);

    customVTKWidget = new CustomVTKWidget;  

    mLight = vtkSmartPointer<vtkLight>::New();
    mLightActor = vtkSmartPointer<vtkLightActor>::New();
    mOutlineActor = vtkSmartPointer<vtkActor>::New();
    mNormalGenerator = vtkSmartPointer<vtkPolyDataNormals>::New();

    //this->showMaximized();

    //light
    connect(mLightform, SIGNAL(ambientValue(int)), this, SLOT(setLightAmbientChange(int)));
    connect(mLightform, SIGNAL(diffuseValue(int)), this, SLOT(setLightDiffuseChange(int)));
    connect(mLightform, SIGNAL(specularValue(int)), this, SLOT(setLightSpecularChange(int)));
    connect(mLightform, SIGNAL(spotValue()), this, SLOT(setSpotChange()));
    connect(mLightform, SIGNAL(lightX()), this, SLOT(setLightXMove()));
    connect(mLightform, SIGNAL(lightY()), this, SLOT(setLightYMove()));
    connect(mLightform, SIGNAL(lightZ()), this, SLOT(setLightZMove()));
    connect(mLightform, SIGNAL(AmbientlightcolorChange()), this, SLOT(ambientColorChange()));
    connect(mLightform, SIGNAL(DiffuselightcolorChange()), this, SLOT(diffuseColorChange()));
    connect(mLightform, SIGNAL(SpecularlightcolorChange()), this, SLOT(specularColorChange()));
    connect(mLightform, SIGNAL(SpotOn()), this, SLOT(setSpotOn()));
    connect(mLightform, SIGNAL(SpotOff()), this, SLOT(setSpotOff()));
    connect(mLightform, SIGNAL(Intensity(int)), this, SLOT(setIntensityChange(int)));

    //shading
    connect(mShadingform, SIGNAL(FlatButton()), this, SLOT(setFlatChange()));
    connect(mShadingform, SIGNAL(GouraudButton()), this, SLOT(setGouraudChange()));
    connect(mShadingform, SIGNAL(PhongButton()), this, SLOT(setPhongChange()));
    connect(mShadingform, SIGNAL(TextureButton()), this, SLOT(setTexture()));

    //occlusion
    mSsaoPass = vtkSmartPointer<vtkSSAOPass>::New();
    connect(mOcclusion, SIGNAL(AmbientRadius(int)), this, SLOT(setAmbientRadiusChange(int)));
    connect(mOcclusion, SIGNAL(AmbientBias(int)), this, SLOT(setAmbientBiasChange(int)));
    connect(mOcclusion, SIGNAL(AmbientLerne(int)), this, SLOT(setAmbientKernelSizeChange(int)));
    connect(mOcclusion, SIGNAL(AmbientBlurOn()), this, SLOT(setAmbientBlurOffChange()));
    connect(mOcclusion, SIGNAL(AmbientBlurOff()), this, SLOT(setAmbientBlurOnChange()));

    //cutform
    connect(mCutform, SIGNAL(clickPathButton()), ui->openGLWidget, SLOT(PathButtonClicked()));
    connect(mCutform, SIGNAL(clickConnectButton()), ui->openGLWidget, SLOT(ConnectButtonClicked()));
    connect(mCutform, SIGNAL(clickHideButton()), ui->openGLWidget, SLOT(HideButtonClicked()));
    connect(mCutform, SIGNAL(clickColorButton()), ui->openGLWidget, SLOT(ColorButtonClicked()));

    //fillingform
    connect(mFillingForm, SIGNAL(clickMeshButton()), this, SLOT(meshButtonClicked()));
    connect(mFillingForm, SIGNAL(clickAdvanceButton()), this, SLOT(advanceButtonClicked()));

    //opacity
    connect(mOpacity, SIGNAL(opacityValue(int)), this, SLOT(on_OpacityValuChange(int)));
}

STLViewer::~STLViewer()
{
    delete ui;
} 

void STLViewer::MakeMesh(std::vector<std::vector<TriMesh::VertexHandle> > holes, OpenMesh::Vec3d centerVertex, TriMesh& triMesh)
{

    OpenMesh::VertexHandle vertexHandle2 = triMesh.add_vertex(OpenMesh::Vec3d(centerVertex[0], centerVertex[1], centerVertex[2]));
    for (int i = 0; i < holes.size(); ++i)
    {
        for (int j = 0; j < holes[i].size(); ++j)
        {
            if (j == holes[i].size() - 1)
            {
                OpenMesh::VertexHandle vertexHandle0(holes[i][j]);
                OpenMesh::VertexHandle vertexHandle1(holes[i][0]);
                triMesh.add_face({ vertexHandle0, vertexHandle1, vertexHandle2 });
                mPolyData = convertToPolyData(triMesh);
                vtkPolyDataMapper::SafeDownCast(mActor->GetMapper())->SetInputData(mPolyData);
                break;
            }
            OpenMesh::VertexHandle vertexHandle0(holes[i][j]); 
            OpenMesh::VertexHandle vertexHandle1(holes[i][j + 1]);

            triMesh.add_face({ vertexHandle0, vertexHandle1, vertexHandle2 });
        } 
    } 
    mPolyData = convertToPolyData(triMesh);
    vtkPolyDataMapper::SafeDownCast(mActor->GetMapper())->SetInputData(mPolyData);
}

TriMesh STLViewer::convertToMesh(vtkSmartPointer<vtkPolyData> polyData)
{
    int nPoints = polyData->GetNumberOfPoints();
    int nCells = polyData->GetNumberOfCells();

    TriMesh triMesh;

    for (int vertexId = 0; vertexId < nPoints; ++vertexId)
    {
        double* point = polyData->GetPoint(vertexId);
        triMesh.add_vertex(OpenMesh::Vec3d(point[0], point[1], point[2]));
    }

    for (int cellId = 0; cellId < nCells; ++cellId)
    {
        int vertexId0 = polyData->GetCell(cellId)->GetPointIds()->GetId(0);
        int vertexId1 = polyData->GetCell(cellId)->GetPointIds()->GetId(1);
        int vertexId2 = polyData->GetCell(cellId)->GetPointIds()->GetId(2);

        OpenMesh::VertexHandle vertexHandle0(vertexId0);
        OpenMesh::VertexHandle vertexHandle1(vertexId1);
        OpenMesh::VertexHandle vertexHandle2(vertexId2);
        triMesh.add_face({ vertexHandle0, vertexHandle1, vertexHandle2 });
    } 
    return triMesh;
} 

vtkSmartPointer<vtkPolyData> STLViewer::convertToPolyData(TriMesh triMesh)
{
    vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();

    vtkSmartPointer<vtkPoints>   points = vtkSmartPointer<vtkPoints>::New();
    int vid = 0;
    for (TriMesh::VertexIter vertexItr = triMesh.vertices_begin(); vertexItr != triMesh.vertices_end(); ++vertexItr, ++vid)
    {
        OpenMesh::Vec3d from = triMesh.point(*vertexItr);
        double* coords = from.data();

        while (vid < vertexItr->idx())
        {
            vid++; points->InsertNextPoint(0, 0, 0);
        }
        points->InsertNextPoint(coords[0], coords[1], coords[2]);
    }
    polyData->SetPoints(points);

    vtkSmartPointer<vtkCellArray> cellArray = vtkSmartPointer<vtkCellArray>::New();
    for (TriMesh::FaceIter faceItr = triMesh.faces_begin(); faceItr != triMesh.faces_end(); ++faceItr)
    {
        TriMesh::FaceVertexIter   faceVertexItr;
        faceVertexItr = triMesh.cfv_iter(*faceItr);
        int   v0 = (faceVertexItr++).handle().idx();
        int   v1 = (faceVertexItr++).handle().idx();
        int   v2 = faceVertexItr.handle().idx();

        vtkSmartPointer<vtkTriangle> triangle = vtkSmartPointer<vtkTriangle>::New();
        triangle->GetPointIds()->SetId(0, v0);
        triangle->GetPointIds()->SetId(1, v1);
        triangle->GetPointIds()->SetId(2, v2);
        cellArray->InsertNextCell(triangle);
    }

    polyData->SetPolys(cellArray);
    polyData->Modified();

    return polyData;
}

void STLViewer::on_CuttoolButton_clicked()
{
    int w = 0, h = 0; 
    w =  this->size().width();
    h = this->size().height(); 
    mCutform->move(w - mCutform->size().width(), 650);
    mCutform->setWindowFlag(Qt::WindowStaysOnTopHint);
    mCutform->show();
} 

void STLViewer::on_OpacitytoolButton_clicked()
{
    int w = 0, h = 0;
    w = this->size().width();
    h = this->size().height();
    mOpacity->move(w - mOpacity->size().width(), 650);
    mOpacity->setWindowFlag(Qt::WindowStaysOnTopHint);
    mOpacity->show();
}


void STLViewer::on_LighttoolButton_clicked()
{
    int w = 0, h = 0;
    w = this->size().width();
    h = this->size().height();
    mLightform->move(w - mLightform->size().width(), 650);
    mLightform->setWindowFlag(Qt::WindowStaysOnTopHint);
    mLightform->show();
}


void STLViewer::on_ShadingtoolButton_clicked()
{
    int w = 0, h = 0;
    w = this->size().width();
    h = this->size().height();
    mShadingform->move(w - mShadingform->size().width(), 650);
    mShadingform->setWindowFlag(Qt::WindowStaysOnTopHint);
    mShadingform->show();
}


void STLViewer::on_ColortoolButton_clicked(QColor color)
{
    double r = color.toRgb().redF();
    double g = color.toRgb().greenF();
    double b = color.toRgb().blueF();
    mActor->GetProperty()->SetDiffuseColor(r, g, b);
    mActor->Modified();
    ui->openGLWidget->GetRenderWindow()->Render();
}

void STLViewer::on_BoxtoolButton_clicked() // Actor에 박스를 만들기 위한 함수
{
    if (mActor == nullptr) return;
    if (boxFlag == false)
    {
        vtkSmartPointer<vtkOutlineFilter> outlineFilter =
            vtkSmartPointer<vtkOutlineFilter>::New();
        outlineFilter->SetInputConnection(mSTLReader->GetOutputPort());

        vtkSmartPointer<vtkPolyDataMapper> outLinemapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
        outLinemapper->SetInputConnection(outlineFilter->GetOutputPort());

        mOutlineActor->SetMapper(outLinemapper);
        mOutlineActor->GetProperty()->SetColor(1, 1, 1);
        // --------------------- Drawing bounding box end------------------------ 
        ui->openGLWidget->addBox(mOutlineActor);
        boxFlag = true;
    }
    else
    {
        ui->openGLWidget->removeBox();
        boxFlag = false;
    }
}

void STLViewer::on_VertextoolButton_clicked()
{
    if (mActor == nullptr) return;
    if (mActor->GetProperty()->GetRepresentation() == 0)
    {
        mActor->GetProperty()->SetRepresentationToSurface();
        ui->openGLWidget->GetRenderWindow()->Render();
        return;
    } 
    mActor->GetProperty()->SetRepresentationToPoints();
    ui->openGLWidget->GetRenderWindow()->Render();
}

void STLViewer::on_WiretoolButton_clicked()
{
    if (mActor == nullptr) return;
    if (mActor->GetProperty()->GetRepresentation() == 1)
    {
        mActor->GetProperty()->SetRepresentationToSurface();
        ui->openGLWidget->GetRenderWindow()->Render();
        return;
    }
    mActor->GetProperty()->SetRepresentationToWireframe();
    ui->openGLWidget->GetRenderWindow()->Render();
}

void STLViewer::on_AxistoolButton_clicked()
{ 
    ui->openGLWidget->showAxis();
}


void STLViewer::on_OpentoolButton_clicked()
{ 
    std::string fileName = QFileDialog::getOpenFileName(nullptr,
        "STL Files", "/home", "STL Files (*.stl)").toStdString().c_str();
    if (fileName == "") return;
    mFileName = fileName; 
    // STL Reader
    mSTLReader = vtkSmartPointer<vtkSTLReader>::New();
    mSTLReader->SetFileName(fileName.c_str());
    mSTLReader->Update();

    // Mapper 
    mPolyData->DeepCopy(mSTLReader->GetOutput());
    vtkSmartPointer<vtkPolyDataMapper> mMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mMapper->SetInputData(mPolyData);

    // Actor
    mActor = vtkSmartPointer<vtkActor>::New();
    mActor->SetMapper(mMapper);

    //ui->openGLWidget->resetFile(mActor);
    ui->openGLWidget->AddActor(mActor);
    ui->openGLWidget->GetInteractor()->GetInteractorStyle()->GetCurrentRenderer()->ResetCamera();
    ui->openGLWidget->GetRenderWindow()->Render(); 

    //vtkSmartPointer<vtkPolyData> mPolyData = vtkPolyDataMapper::SafeDownCast(mActor->GetMapper())->GetInput();
    mTriMesh = convertToMesh(mPolyData);  
}


#if 0
void STLViewer::on_SavetoolButton_clicked()
{
    mTriMesh.request_vertex_normals();
    mTriMesh.request_face_normals();
    mTriMesh.update_normals();
    mTriMesh.release_vertex_normals();
    mTriMesh.release_face_normals(); 
    vtkSmartPointer<vtkPolyData> PolyData = convertToPolyData(mTriMesh);
    vtkPolyDataMapper::SafeDownCast(mActor->GetMapper())->SetInputData(PolyData);

    //std::string fileName = QFileDialog::getOpenFileName(nullptr,
    //    "STL Files", "/home", "STL Files (*.stl)").toStdString().c_str();

    //vtkSmartPointer<vtkSTLWriter> writer =
        //vtkSmartPointer<vtkSTLWriter>::New();
    //writer->SetFileName("*.stl");
    //writer->SetInputConnection(mSTLReader->GetOutputPort());
    //writer->Write();
    QMessageBox msgBox;
    std::string fileName = QFileDialog::getSaveFileName(nullptr,
        "STL Files", "/home", "STL Files (*.stl)").toStdString().c_str();

    if (fileName.empty())
    {
        QMessageBox::warning(this, "", "File name is empty.");

        return;
    }

    vtkSmartPointer<vtkSTLWriter> writer =
        vtkSmartPointer<vtkSTLWriter>::New();
    vtkSmartPointer<vtkWindowToImageFilter> winToImg =
        vtkSmartPointer< vtkWindowToImageFilter>::New();

    winToImg->SetInputBufferTypeToRGBA();
    winToImg->ReadFrontBufferOff();

    writer->SetInputConnection(winToImg->GetOutputPort()); 
    writer->SetFileName(fileName.c_str());

    writer->SetInputData(PolyData);

    writer->SetFileTypeToBinary(); // 바이너리 파일로 저장
    writer->Write();

    QMessageBox::warning(this, "", "File save!!");

    //msgBox.setText("File save!!");
} 
#endif

void STLViewer::on_SavetoolButton_clicked()
{
    mTriMesh = convertToMesh(mPolyData);
    mTriMesh.request_vertex_normals();
    mTriMesh.request_face_normals();
    mTriMesh.update_normals();
    mTriMesh.release_vertex_normals();
    mTriMesh.release_face_normals();


    //std::string fileName = QFileDialog::getOpenFileName(nullptr,
    //    "STL Files", "/home", "STL Files (*.stl)").toStdString().c_str();

    //vtkSmartPointer<vtkSTLWriter> writer =
    //    vtkSmartPointer<vtkSTLWriter>::New();
    //writer->SetFileName("*.stl");
    //writer->SetInputConnection(mSTLReader->GetOutputPort());
    //writer->Write();

    QMessageBox msgBox;
    std::string fileName = QFileDialog::getSaveFileName(nullptr,
        "STL Files", "/home", "STL Files (*.stl)").toStdString().c_str();

    if (fileName.empty())
    {
        QMessageBox::warning(this, "", "File name is empty.");

        return;
    }

    vtkSmartPointer<vtkSTLWriter> writer =
        vtkSmartPointer<vtkSTLWriter>::New();
    vtkSmartPointer<vtkWindowToImageFilter> winToImg =
        vtkSmartPointer< vtkWindowToImageFilter>::New();

    winToImg->SetInputBufferTypeToRGBA();
    winToImg->ReadFrontBufferOff();
    winToImg->Update();

    writer->SetInputConnection(winToImg->GetOutputPort());
    writer->SetFileName(fileName.c_str());

    writer->SetInputData(convertToPolyData(mTriMesh));

    writer->SetFileTypeToBinary(); // 바이너리 파일로 저장
    writer->Write();


    QMessageBox::warning(this, "", "File save!!");

    msgBox.setText("File save!!");
}

std::vector<std::vector<TriMesh::VertexHandle>> STLViewer::FindHoleVertex(TriMesh& triMesh)
{  
    std::vector<std::vector<TriMesh::VertexHandle> > holes;
    std::vector<int> boundaryVertex;
    std::set<TriMesh::VertexHandle> visited_vertices;
    // Iterate through all halfedges and find boundary halfedges

    for (TriMesh::HalfedgeIter he_it = triMesh.halfedges_begin(); he_it != triMesh.halfedges_end(); ++he_it)
    {  
        std::vector<TriMesh::VertexHandle> hole;

        if (triMesh.is_boundary(*he_it))
        { 
            // Start from the current boundary halfedge and follow the boundary
            TriMesh::VertexHandle start = triMesh.to_vertex_handle(*he_it);
            TriMesh::HalfedgeHandle current = *he_it;

            TriMesh::VertexHandle vh = triMesh.to_vertex_handle(*he_it);
            if (triMesh.is_boundary(vh))
            { 
                boundaryVertex.push_back(vh.idx());
            }

            if (visited_vertices.count(start) == 0)
            { 
                while (visited_vertices.count(start) == 0)
                { 
                    visited_vertices.insert(start);
                    hole.push_back(start);
                    current = triMesh.next_halfedge_handle(current);
                    start = triMesh.to_vertex_handle(current);
                }
            }
            if (hole.size())
            { 
                holes.push_back(hole);
            }
        }
    } 
    return holes;
}

void STLViewer::AdvancingFrontMethod(TriMesh& triMesh, std::vector<std::vector<TriMesh::VertexHandle> > holes)
{
    vtkNew<vtkNamedColors> colors;
    triMesh.request_vertex_normals();
    triMesh.request_face_normals();
    triMesh.update_normals();
    triMesh.release_vertex_normals();
    triMesh.release_face_normals();
    
    for (int i = 0; i < holes.size(); i++)
    { 
        std::vector<std::pair<int, int>> linkedVertex(triMesh.n_vertices(), { -1, -1 });
        std::unordered_map<int, OpenMesh::Vec3d> vertexNormalMap;
        OpenMesh::Vec3d firstVertex = { 0.0,0.0,0.0, };     // v-i
        OpenMesh::Vec3d secondVertex = { 0.0,0.0,0.0, };    // v
        OpenMesh::Vec3d thirdVertex = { 0.0,0.0,0.0, };     // v+i

        double min = 360.0;
        double minAngle = 360.0;
        TriMesh::VertexHandle minIdx;

        OpenMesh::Vec3d prevVertex = { 0,0,0 };
        OpenMesh::Vec3d currentVertex = { 0,0,0 };
        OpenMesh::Vec3d nextVertex = { 0,0,0 };
        int count = 0; 
        

        // 홀의 모든 점들을 탐색
        for (int j = 0; j < holes[i].size(); j++)
        {  
            int faceCount = 0;
            if (j == 0)
            {
                prevVertex = triMesh.point(holes[i].back());
                currentVertex = triMesh.point(holes[i][j]);
                nextVertex = triMesh.point(holes[i][j + 1]);

                linkedVertex[holes[i][j].idx()].first = holes[i].back().idx();
                linkedVertex[holes[i][j].idx()].second = holes[i][j + 1].idx();
            }
            else if (j == holes[i].size() - 1)
            {
                prevVertex = triMesh.point(holes[i][j - 1]);
                currentVertex = triMesh.point(holes[i][j]);
                nextVertex = triMesh.point(holes[i].front());

                linkedVertex[holes[i][j].idx()].first = holes[i][j - 1].idx();
                linkedVertex[holes[i][j].idx()].second = holes[i].front().idx();
            }
            else
            {
                prevVertex = triMesh.point(holes[i][j - 1]);
                currentVertex = triMesh.point(holes[i][j]);
                nextVertex = triMesh.point(holes[i][j + 1]);

                linkedVertex[holes[i][j].idx()].first = holes[i][j - 1].idx();
                linkedVertex[holes[i][j].idx()].second = holes[i][j + 1].idx();
            }

            OpenMesh::Vec3d currentNextVertex;
            OpenMesh::Vec3d currentPrevVertex;
            currentNextVertex = nextVertex - currentVertex;
            currentPrevVertex = prevVertex - currentVertex;

            // dot product
            double dotProduct = 0.0;
            double currentNextMagnitude = 0.0;
            double currentPrevMagnitude = 0.0;
            double radian = 0.0;

            dotProduct = (currentNextVertex[0] * currentPrevVertex[0]) + (currentNextVertex[1] * currentPrevVertex[1]) + (currentNextVertex[2] * currentPrevVertex[2]);
            currentNextMagnitude = sqrt((currentNextVertex[0] * currentNextVertex[0]) + (currentNextVertex[1] * currentNextVertex[1]) + (currentNextVertex[2] * currentNextVertex[2]));
            currentPrevMagnitude = sqrt((currentPrevVertex[0] * currentPrevVertex[0]) + (currentPrevVertex[1] * currentPrevVertex[1]) + (currentPrevVertex[2] * currentPrevVertex[2]));

            radian = acos(dotProduct / (currentNextMagnitude * currentPrevMagnitude));
            double angle = (radian * 180) / PI;
            double frontAngle = 0.0; 
            double vectorAngle = 0.0;
            OpenMesh::Vec3d sumNormal = { 0.0,0.0,0.0 };

            for (TriMesh::VertexEdgeIter ve_it = triMesh.ve_iter(holes[i][j]); ve_it.is_valid(); ++ve_it, count++)
            { 
                if (count > 0)
                {
                    TriMesh::HalfedgeHandle heh = ve_it.current_halfedge_handle();
                    firstVertex = triMesh.point(holes[i][j]);
                    secondVertex = triMesh.point(triMesh.to_vertex_handle(heh));

                    TriMesh::HalfedgeHandle nextHeh = triMesh.next_halfedge_handle(heh);
                    thirdVertex = triMesh.point(triMesh.to_vertex_handle(nextHeh));

                    OpenMesh::Vec3d firstVec;
                    OpenMesh::Vec3d secondVec;
                    firstVec = secondVertex - firstVertex;
                    secondVec = thirdVertex - firstVertex;

                    // dot product
                    double dotProduct = 0.0;
                    double firstVecMagnitude = 0.0;
                    double secondVecMagnitude = 0.0;
                    double theta = 0.0;

                    dotProduct = (firstVec[0] * secondVec[0]) + (firstVec[1] * secondVec[1]) + (firstVec[2] * secondVec[2]);
                    firstVecMagnitude = sqrt((firstVec[0] * firstVec[0]) + (firstVec[1] * firstVec[1]) + (firstVec[2] * firstVec[2]));
                    secondVecMagnitude = sqrt((secondVec[0] * secondVec[0]) + (secondVec[1] * secondVec[1]) + (secondVec[2] * secondVec[2]));
                    theta = acos(dotProduct / (firstVecMagnitude * secondVecMagnitude));
                    double angle = (theta * 180) / PI;
                    vectorAngle += angle;
                    sumNormal += firstVec.cross(secondVec);
                }
            }      
            if (vectorAngle > 180)
            {
                frontAngle = angle;
            }
            else
            {
                continue;
            }

            min = (min > frontAngle) ? frontAngle : min;
            if (min == frontAngle)
            { 
                minAngle = min;
                minIdx = holes[i][j];
                vertexNormalMap[minIdx.idx()] = sumNormal;

                for (TriMesh::VertexFaceIter vf_it = triMesh.vf_begin(minIdx); vf_it.is_valid(); ++vf_it)
                {
                    faceCount++;
                } 
                vertexNormalMap[minIdx.idx()] = vertexNormalMap[minIdx.idx()] / faceCount; 
            }
            sumNormal = { 0.0,0.0,0.0 };
            vectorAngle = 0.0;
        }  
        if (minAngle <= thresholdA)  // 0 < angle <= 85
        {  
            OpenMesh::VertexHandle vertexHandle0(linkedVertex[minIdx.idx()].first);
            OpenMesh::VertexHandle vertexHandle1(minIdx);
            OpenMesh::VertexHandle vertexHandle2(linkedVertex[minIdx.idx()].second);
            triMesh.add_face({ vertexHandle0, vertexHandle1, vertexHandle2 }); 
            //vtkSmartPointer<vtkPolyData> mPolyData = convertToPolyData(triMesh);
            //vtkPolyDataMapper::SafeDownCast(mActor->GetMapper())->SetInputData(mPolyData);
            linkedVertex.clear();
        }
        else if (minAngle > thresholdA && minAngle <= thresholdB) // 85 < angle < 135
        { 
            OpenMesh::VertexHandle prevVertexHandle(linkedVertex[minIdx.idx()].first);
            OpenMesh::VertexHandle currentVertexHandle(minIdx);
            OpenMesh::VertexHandle nextVertexHandle(linkedVertex[minIdx.idx()].second);

            OpenMesh::Vec3d firstVector;
            OpenMesh::Vec3d secondVector;

            firstVector = triMesh.point(prevVertexHandle) - triMesh.point(currentVertexHandle);
            secondVector = triMesh.point(nextVertexHandle) - triMesh.point(currentVertexHandle);

            OpenMesh::Vec3d bisectorVector;
            bisectorVector = (firstVector.normalize() + secondVector.normalize()).normalize();

            TriMesh::Normal vNormal = { 0.0,0.0,0.0 };
            // OpenMesh를 이용한 정점 노말 구하기
            vNormal = triMesh.calc_vertex_normal(minIdx);

            //// 기준점에 연결된 Face들의 Face Normal의 평균
            //vNormal = vertexNormalMap[minIdx.idx()]; 

            //// 85 ~ 135도에서의 Normal 가중치
            //vNormal = vNormal * (thresholdA / minAngle);

            OpenMesh::Vec3d ppVector;
            OpenMesh::Vec3d pVector;
            ppVector = bisectorVector - ((bisectorVector.dot(vNormal)) * vNormal);
            pVector = ppVector / ppVector.norm();

            OpenMesh::Vec3d pNormal;
            pNormal = vNormal + (alpha * abs(vNormal.dot(bisectorVector)) * bisectorVector); 
            pNormal = pNormal / pNormal.norm();

            double nnTheta = 0.0;

            // New Vertex의 높낮이 구하기
            nnTheta = acos(vNormal.dot(pNormal));
            double k = (cos(nnTheta) - 1) / pNormal.dot(pVector);

            OpenMesh::Vec3d delta = { 0.0,0.0,0.0 };
            int weight = 0;
            for (OpenMesh::VertexHandle neighbor : triMesh.vv_range(minIdx))
            {
                weight += 1;
            }
            for (OpenMesh::VertexHandle neighbor : triMesh.vv_range(minIdx))
            {
                delta += ((1 / weight) * triMesh.point(neighbor)) - triMesh.point(minIdx);
            }
            OpenMesh::Vec3d BVector = { 0,0,0 };
            double checkCon = 0.0;
             
            checkCon = vNormal.dot(delta);
            if (checkCon <= 0)          // convex
            {
                BVector = (pVector + (k * pNormal)) / (pVector + (k * pNormal)).norm();
            }
            else                        // concave
            {
                BVector = (pVector - (k * pNormal)) / (pVector - (k * pNormal)).norm();
            }

            OpenMesh::Vec3d newVector = { 0.0,0.0,0.0 };
            firstVector; // v1
            secondVector; // v2  
            newVector = triMesh.point(currentVertexHandle) + (((firstVector + secondVector).norm() / 2) * BVector); 
            OpenMesh::VertexHandle vertexHandle0(linkedVertex[minIdx.idx()].first);
            OpenMesh::VertexHandle vertexHandle1(minIdx);
            OpenMesh::VertexHandle vertexHandle2(linkedVertex[minIdx.idx()].second);
            OpenMesh::VertexHandle newVertexHandle = triMesh.add_vertex(newVector);
            triMesh.add_face({ newVertexHandle, vertexHandle1, vertexHandle2 });
            triMesh.add_face({ newVertexHandle, vertexHandle0, vertexHandle1 });
            //vtkSmartPointer<vtkPolyData> mPolyData = convertToPolyData(triMesh);
            //vtkPolyDataMapper::SafeDownCast(mActor->GetMapper())->SetInputData(mPolyData);

            linkedVertex.clear();
        }
        else if (minAngle > thresholdB && minAngle <= 180) // 135 < angle < 180
        { 
            OpenMesh::VertexHandle prevVertexHandle(linkedVertex[minIdx.idx()].first);
            OpenMesh::VertexHandle currentVertexHandle(minIdx);
            OpenMesh::VertexHandle nextVertexHandle(linkedVertex[minIdx.idx()].second);

            OpenMesh::Vec3d firstVector;
            OpenMesh::Vec3d secondVector;

            firstVector = triMesh.point(prevVertexHandle) - triMesh.point(currentVertexHandle);
            secondVector = triMesh.point(nextVertexHandle) - triMesh.point(currentVertexHandle);

            OpenMesh::Vec3d bisectorVector;
            bisectorVector = (firstVector.normalize() + secondVector.normalize()).normalize();

            OpenMesh::Vec3d triVector1;
            OpenMesh::Vec3d triVector2;
            triVector1 = (bisectorVector.normalize() + firstVector.normalize()).normalize();
            triVector2 = (bisectorVector.normalize() + secondVector.normalize()).normalize();

            TriMesh::Normal vNormal = { 0.0,0.0,0.0 };
            vNormal = triMesh.calc_vertex_normal(minIdx);
            //vNormal = vertexNormalMap[minIdx.idx()];
            //vNormal = firstVector.cross(secondVector);

            OpenMesh::Vec3d ppVector1;
            OpenMesh::Vec3d ppVector2;
            OpenMesh::Vec3d pVector1;
            OpenMesh::Vec3d pVector2;
            ppVector1 = triVector1 - ((triVector1.dot(vNormal)) * vNormal);
            pVector1 = ppVector1 / ppVector1.norm();
            ppVector2 = triVector2 - ((triVector2.dot(vNormal)) * vNormal);
            pVector2 = ppVector2 / ppVector2.norm();

            OpenMesh::Vec3d pNormal1;
            OpenMesh::Vec3d pNormal2;
            double nvVector1;
            double nvVector2;
            nvVector1 = vNormal.dot(triVector1);
            pNormal1 = vNormal + (alpha * abs(nvVector1)) * triVector1;
            pNormal1 = pNormal1 / pNormal1.norm();
            nvVector2 = vNormal.dot(triVector2);
            pNormal2 = vNormal + (alpha * abs(nvVector2)) * triVector2;
            pNormal2 = pNormal2 / pNormal2.norm();

            double nnTheta1 = 0.0;
            nnTheta1 = acos(vNormal.dot(pNormal1));
            double k1 = (cos(nnTheta1) - 1) / pNormal1.dot(pVector1);
            double nnTheta2 = 0.0;
            nnTheta2 = acos(vNormal.dot(pNormal2));
            double k2 = (cos(nnTheta2) - 1) / pNormal2.dot(pVector2);

            OpenMesh::Vec3d delta = { 0.0,0.0,0.0 };
            int weight = 0;
            for (OpenMesh::VertexHandle neighbor : triMesh.vv_range(minIdx))
            {
                weight += 1;
            }
            for (OpenMesh::VertexHandle neighbor : triMesh.vv_range(minIdx))
            {
                delta += ((1 / weight) * triMesh.point(neighbor)) - triMesh.point(minIdx);
            }

            OpenMesh::Vec3d BVector1 = { 0,0,0 };
            OpenMesh::Vec3d BVector2 = { 0,0,0 };
            if (vNormal.dot(delta) <= 0)        // convex (볼록)
            {
                BVector1 = (pVector1 + (k1 * pNormal1)) / (pVector1 + (k1 * pNormal1)).norm();
                BVector2 = (pVector2 + (k2 * pNormal2)) / (pVector2 + (k2 * pNormal2)).norm();
            }
            else                                // concave (오목)
            {
                BVector1 = (pVector1 - (k1 * pNormal1)) / (pVector1 - (k1 * pNormal1)).norm();
                BVector2 = (pVector2 - (k2 * pNormal2)) / (pVector2 - (k2 * pNormal2)).norm();
            }

            OpenMesh::Vec3d newVector1;
            OpenMesh::Vec3d newVector2;
            newVector1 = triMesh.point(currentVertexHandle) + (((firstVector + secondVector).norm() / 2) * BVector1);
            newVector2 = triMesh.point(currentVertexHandle) + (((firstVector + secondVector).norm() / 2) * BVector2);

            OpenMesh::VertexHandle vertexHandle0(linkedVertex[minIdx.idx()].first);
            OpenMesh::VertexHandle vertexHandle1(minIdx);
            OpenMesh::VertexHandle vertexHandle2(linkedVertex[minIdx.idx()].second);
            OpenMesh::VertexHandle newVertexHandle1 = triMesh.add_vertex(newVector1);
            OpenMesh::VertexHandle newVertexHandle2 = triMesh.add_vertex(newVector2);

            triMesh.add_face({ vertexHandle1, newVertexHandle2, vertexHandle2 });
            triMesh.add_face({ vertexHandle1, newVertexHandle2, newVertexHandle1 });
            triMesh.add_face({ vertexHandle1, newVertexHandle1, vertexHandle0 }); 

            linkedVertex.clear();
        } 
    }  
} 

void STLViewer::SetLightAmbientChange(int ambient)
{ 
    if (mActor != NULL)
    {
        mActor->GetProperty()->SetAmbient(ambient / 100.0);
        mActor->Modified();
        ui->openGLWidget->GetRenderWindow()->Render();
    }
} 

void STLViewer::setBoundary()
{
    if (mPolyData == nullptr) return;
    mPolyData = vtkPolyDataMapper::SafeDownCast(mActor->GetMapper())->GetInput();
    mTriMesh = convertToMesh(mPolyData);
    vtkSmartPointer<vtkPolyDataAlgorithm> polyDataAlgorithm = vtkSmartPointer<vtkPolyDataAlgorithm>::New();
    //polyDataAlgorithm->Setinput(mPolyData);
    polyDataAlgorithm->SetOutput(mPolyData); 
    vtkNew<vtkNamedColors> colors;
    // Feature edge
    vtkNew<vtkFeatureEdges> featureEdges;  
    featureEdges->SetInputDataObject(mPolyData);
    featureEdges->BoundaryEdgesOn();
    featureEdges->FeatureEdgesOff();
    featureEdges->ManifoldEdgesOff();
    featureEdges->NonManifoldEdgesOff();
    featureEdges->ColoringOn();  
    featureEdges->Update();  
    if (edgeFlag == false)
    {
        featureEdges->SetBoundaryEdges(true);
        //Visualize
        vtkNew<vtkPolyDataMapper> edgeMapper;
        edgeMapper->SetInputData(featureEdges->GetOutput());
        edgeMapper->SetScalarModeToUseCellData();
        vtkNew<vtkActor> edgeActor;
        edgeActor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
        edgeActor->SetMapper(edgeMapper);
        ui->openGLWidget->addEdge(edgeActor); 
        edgeFlag = true;
    }
    else
    { 
        ui->openGLWidget->removeEdge();
        edgeFlag = false;
    } 
}

void STLViewer::on_OcclusiontoolButton_clicked()
{
    int w = 0, h = 0;
    w = this->size().width();
    h = this->size().height();
    mOcclusion->move(w - mOcclusion->size().width(), 650);
    mOcclusion->setWindowFlag(Qt::WindowStaysOnTopHint);
    mOcclusion->show();

    vtkNew<vtkRenderStepsPass> basicPasses;

    mSsaoPass->SetRadius(0.1);
    mSsaoPass->SetBias(0.001);
    mSsaoPass->SetKernelSize(128);
    mSsaoPass->BlurOn();
    mSsaoPass->SetDelegatePass(basicPasses);
    mRenderer->SetPass(mSsaoPass);

    ui->openGLWidget->GetRenderWindow()->Render();

}

void STLViewer::setAmbientRadiusChange(int radius) //Ambient Radius를 조절하기 위한 함수
{ 
    if (mActor != NULL)
    {
        mSsaoPass->SetRadius(radius / 100.0);
        mRenderer->SetPass(mSsaoPass); 
        ui->openGLWidget->GetRenderWindow()->Render();
    }
}

void STLViewer::setAmbientBiasChange(int bias) //Ambient Bias를 조절하기 위한 함수
{ 
    if (mActor != NULL)
    { 
        mSsaoPass->SetBias(bias / 1000.0);
        mRenderer->SetPass(mSsaoPass);
        ui->openGLWidget->GetRenderWindow()->Render();
    }
}

void STLViewer::setAmbientKernelSizeChange(int kernelSize) // Ambient KernelSize를 조절하기 위한 함수
{ 
    if (mActor != NULL)
    { 
        mSsaoPass->SetKernelSize(kernelSize); 
        ui->openGLWidget->GetRenderWindow()->Render();
    }
}
void STLViewer::setAmbientBlurOnChange()
{ 
    if (mActor != NULL)
    { 
        mSsaoPass->BlurOn();
        mSsaoPass->SetBlur(true); 
        mRenderer->SetPass(mSsaoPass);
        ui->openGLWidget->GetRenderWindow()->Render();
    }
}

void STLViewer::setAmbientBlurOffChange()
{ 
    if (mActor != NULL)
    { 
        mSsaoPass->BlurOff();
        mRenderer->SetPass(mSsaoPass); //
        ui->openGLWidget->GetRenderWindow()->Render();

    }
}

void STLViewer::setLightAmbientChange(int ambient) // ambient light 양
{ 
    if (mActor != NULL)
    {
        mActor->GetProperty()->SetAmbient(ambient / 100.0);
        mActor->Modified();
        ui->openGLWidget->GetRenderWindow()->Render();
    }
}

void STLViewer::setLightDiffuseChange(int diffuse) // Diffuse Light 양
{ 
    if (mActor != NULL)
    {
        mActor->GetProperty()->SetDiffuse(diffuse / 100.0);
        mActor->Modified();
        ui->openGLWidget->GetRenderWindow()->Render();
    }
}

void STLViewer::setLightSpecularChange(int Specular) // Specular Light 양
{ 
    if (mActor != NULL)
    {
        mActor->GetProperty()->SetSpecular(Specular / 100.0);
        mActor->Modified();
        ui->openGLWidget->GetRenderWindow()->Render();
    }
}
void STLViewer::setSpotChange() // spot Light 
{
    if (mActor != NULL)
    {
        QColor color = mColorDialog->getColor(); // 색 설정을 위한 colorDialog
        double r = color.toRgb().redF();
        double g = color.toRgb().greenF();
        double b = color.toRgb().blueF();

        mLight->SetPositional(true);
        mLight->SetLightTypeToCameraLight();
        mLight->SetColor(r, g, b);
        mLight->PositionalOn();
        mLightActor->SetLight(mLight);

        mRenderer->AddLight(mLight);
        mRenderer->AddViewProp(mLightActor);

        ui->openGLWidget->GetRenderWindow()->Render();
    }
}

void STLViewer::setSpotOn() // setSpot 버튼을 켜기 위한 함수
{
    if (!mActor) return;
    if (mLight->GetSwitch() == 0)
    { 
        mLight->SetSwitch(1);
    }
    ui->openGLWidget->GetRenderWindow()->Render();
}

void STLViewer::setSpotOff() // setSpot 버튼을 끄기 위한 함수
{
    if (!mActor) return;
    if (mLight->GetSwitch() == 1)
    { 
        mLight->SetSwitch(0);
    }
    ui->openGLWidget->GetRenderWindow()->Render();
}

void STLViewer::setLightXMove() // x방향에 빛을 비추기 위한 함수
{ 
    if (!mActor) return;
    double max = 0;
    double bound = 0;
    for (int i = 0; i < 6; i++)
    { 
        max = (max < abs(mOutlineActor->GetBounds()[i])) ? abs(mOutlineActor->GetBounds()[i]) : max;
        if (max == abs(mOutlineActor->GetBounds()[i]))
        {
            bound = abs(max);
        }
    }

    mOutlineActor->GetCenter();

    mLight->SetPosition(mOutlineActor->GetCenter()[0] + abs(mOutlineActor->GetBounds()[1] + 10.0),
        mOutlineActor->GetCenter()[1], mOutlineActor->GetCenter()[2]); // 상자 보다 멀리서 빛을 조절

    mLight->SetFocalPoint(mActor->GetPosition());
    // mLight->SetColor(mcolors->GetColor3d("Magenta").GetData());
    mLight->PositionalOn();
    mLight->SetLightTypeToCameraLight();
    mRenderer->AddLight(mLight);
    mLight->SwitchOn();

    // mLight->SetConeAngle(10);
    mLightActor->SetLight(mLight);
    mRenderer->AddViewProp(mLightActor);
    ui->openGLWidget->GetRenderWindow()->Render();
}

void STLViewer::setLightYMove() //Y 방향에서 비추기
{ 
    if (!mActor) return;
    double max = 0;
    double bound = 0;
    for (int i = 0; i < 6; i++)
    { 
        max = (max < abs(mOutlineActor->GetBounds()[i])) ? abs(mOutlineActor->GetBounds()[i]) : max;
        if (max == abs(mOutlineActor->GetBounds()[i]))
        {
            bound = abs(max);
        }
    } 

    mOutlineActor->GetCenter();

    mLight->SetPosition(mOutlineActor->GetCenter()[0], mOutlineActor->GetCenter()[1] + abs(mOutlineActor->GetBounds()[3] + 10.0),
        mOutlineActor->GetCenter()[2]);


    // mLight->SetPosition(0, 0, 1);
    mLight->SetFocalPoint(mActor->GetPosition());

    // mLight->SetColor(mcolors->GetColor3d("Magenta").GetData());
    mLight->PositionalOn();
    mLight->SetLightTypeToCameraLight();
    mRenderer->AddLight(mLight);
    mLight->SwitchOn();

    // mLight->SetConeAngle(10);
    mLightActor->SetLight(mLight);
    mRenderer->AddViewProp(mLightActor);
    ui->openGLWidget->GetRenderWindow()->Render();
}

void STLViewer::setLightZMove() //Z 방향 비추기
{ 
    if (!mActor) return;
    double max = 0;
    double bound = 0;
    for (int i = 0; i < 6; i++)
    {

        max = (max < abs(mOutlineActor->GetBounds()[i])) ? abs(mOutlineActor->GetBounds()[i]) : max;
        if (max == abs(mOutlineActor->GetBounds()[i]))
        {
            bound = abs(max);
        }
    } 
    mOutlineActor->GetCenter();
    mLight->SetPosition(mOutlineActor->GetCenter()[0], mOutlineActor->GetCenter()[1],
        mOutlineActor->GetCenter()[2] + abs(mOutlineActor->GetBounds()[5] + 10.0));

    mLight->SetFocalPoint(mActor->GetPosition());
    // mLight->SetColor(mcolors->GetColor3d("Magenta").GetData());
    mLight->PositionalOn();

    mRenderer->AddLight(mLight);
    mLight->SwitchOn();
    mLight->SetLightTypeToCameraLight();
    // mLight->SetConeAngle(10);
    mLightActor->SetLight(mLight);
    mRenderer->AddViewProp(mLightActor);
    ui->openGLWidget->GetRenderWindow()->Render();
}

void STLViewer::ambientColorChange() // ambient 색 변경을 위한 함수
{
    if (mActor != NULL)
    {
        double ambient = 0.4;
        double diffuse = 0.0;
        double specular = 0.0;
        double spbase = 0.5;
        double spscale = 0.1;

        double specularpower = spbase * spscale;

        QColor color = mColorDialog->getColor();
        double r = color.toRgb().redF();
        double g = color.toRgb().greenF();
        double b = color.toRgb().blueF();
        mLight->SetLightTypeToSceneLight();

        mLight->SetAmbientColor(r, g, b);
        mActor->GetProperty()->SetAmbientColor(r, g, b);
        mActor->GetProperty()->SetAmbient(ambient);
        mActor->GetProperty()->SetDiffuse(diffuse);
        mActor->GetProperty()->SetSpecular(specular);
        mActor->GetProperty()->SetSpecularPower(specularpower); 

        mRenderer->AddLight(mLight);
        mRenderer->AddActor(mActor);
        mRenderer->SetAmbient(0.5, 0.5, 0.5);

        mRenderer->Render();
        mInteractor->Start();
        ui->openGLWidget->GetRenderWindow()->Render();
    }
}

void STLViewer::diffuseColorChange() // diffuse 색을 바꾸기 위한 함수
{
    QColor color = mColorDialog->getColor(); // 색 설정을 위한 colorDialog
    double r = color.toRgb().redF();
    double g = color.toRgb().greenF();
    double b = color.toRgb().blueF();

    mLight->SetLightTypeToCameraLight(); // 카메라 시점에 따라 빛 이동

    //mLight->SetPosition(0, 0, 5);
    mLight->SetDiffuseColor(r, g, b); //색 정보를 받아 온다
    mRenderer->AddLight(mLight);
    mInteractor->Start();
    ui->openGLWidget->GetRenderWindow()->Render();
}

void STLViewer::specularColorChange() // specular 색을 바꾸기 위한 함수
{
    double ambient = 0.0; // 초기 설정값
    double diffuse = 0.0;
    double specular = 0.4;
    double spbase = 0.5;
    double spscale = 1.0;

    double specularpower = spbase * spscale; // 반사력 설정

    QColor color = mColorDialog->getColor();
    double r = color.toRgb().redF();
    double g = color.toRgb().greenF();
    double b = color.toRgb().blueF();

    mLight->SetPositional(true);
    mLight->SetLightTypeToSceneLight();
    mLight->SetFocalPoint(0, 0, 0); 
    mLightActor->SetLight(mLight);

    mActor->GetProperty()->SetAmbient(ambient);
    mActor->GetProperty()->SetDiffuse(diffuse);
    mActor->GetProperty()->SetSpecular(specular);
    mActor->GetProperty()->SetSpecularPower(specularpower);
    mActor->GetProperty()->SetSpecularColor(r, g, b); 

    mRenderer->AddLight(mLight); 
    mRenderer->AddViewProp(mLightActor);

    ui->openGLWidget->GetInteractor()->GetInteractorStyle()->GetCurrentRenderer()->AddViewProp(mLightActor); 
    ui->openGLWidget->GetRenderWindow()->Render(); 
    mInteractor->Start();

}

void STLViewer::setIntensityChange(int intensity) // 빛의 세기를 조절하기 위한 함수
{ 
    if (mActor != NULL)
    {
        //mActor->GetProperty()->SetAngle(intensity);
        mLight->SetConeAngle(intensity / 100.0);
        // mActor->Modified();
        ui->openGLWidget->GetRenderWindow()->Render();
    }

}  

void STLViewer::setFlatChange() // Flat shading
{ 
    mPolyData = vtkPolyDataMapper::SafeDownCast(mActor->GetMapper())->GetInput();
    mTriMesh = convertToMesh(mPolyData);

    mTriMesh = convertToMesh(mPolyData);
    mTriMesh.request_vertex_normals();
    mTriMesh.request_face_normals();
    mTriMesh.update_normals();
    mTriMesh.release_vertex_normals();
    mTriMesh.release_face_normals();

    if (mActor != NULL) {
        vtkSmartPointer<vtkPolyDataNormals> normalGenerator =
            vtkSmartPointer<vtkPolyDataNormals>::New();

        mMapper->SetInputData(convertToPolyData(mTriMesh));
        mMapper->SetScalarVisibility(0);

        mActor->SetMapper(mMapper);
        mActor->GetProperty()->SetInterpolationToFlat();

        mActor->SetTexture(mTexture);
        mRenderer->AddActor(mActor);

        ui->openGLWidget->GetRenderWindow()->Render();
    }
}

void STLViewer::setGouraudChange() // Gouraud shading
{ 
    mPolyData = vtkPolyDataMapper::SafeDownCast(mActor->GetMapper())->GetInput();
    mTriMesh = convertToMesh(mPolyData);

    mTriMesh = convertToMesh(mPolyData);
    mTriMesh.request_vertex_normals();
    mTriMesh.request_face_normals();
    mTriMesh.update_normals();
    mTriMesh.release_vertex_normals();
    mTriMesh.release_face_normals();
    if (mActor != NULL) { 
        mNormalGenerator->SetInputData(convertToPolyData(mTriMesh));
        mNormalGenerator->ComputePointNormalsOn();
        mNormalGenerator->ComputeCellNormalsOff();
        mNormalGenerator->SetFeatureAngle(60.0);
        mNormalGenerator->Update();

        mNormalGenerator->SetInputData(convertToPolyData(mTriMesh));
        mMapper->SetInputConnection(mNormalGenerator->GetOutputPort());
        mActor->SetMapper(mMapper);
        mActor->GetProperty()->SetInterpolationToGouraud();
        mActor->GetProperty()->SetSpecularPower(50);  

        mActor->SetTexture(mTexture);
        mRenderer->AddActor(mActor); 

        ui->openGLWidget->GetRenderWindow()->Render();
    } 
}

void STLViewer::setPhongChange() // Phong sading
{                               // 고러드 쉐이딩에서 하이라이트나 반사광을 표현할 수 없는것을
                                // 가능하게 해주는 쉐이딩 

    mPolyData = vtkPolyDataMapper::SafeDownCast(mActor->GetMapper())->GetInput();
    mTriMesh = convertToMesh(mPolyData);

    mTriMesh = convertToMesh(mPolyData);
    mTriMesh.request_vertex_normals();
    mTriMesh.request_face_normals();
    mTriMesh.update_normals();
    mTriMesh.release_vertex_normals();
    mTriMesh.release_face_normals();

    if (mActor != NULL) {  
        //normalGenerator->SetInputConnection(mSTLReader->GetOutputPort());
        mNormalGenerator->SetInputData(convertToPolyData(mTriMesh));
        mNormalGenerator->ComputePointNormalsOn(); //접 번선 계산을 킨다.
        mNormalGenerator->ComputeCellNormalsOff(); //셀 법선 계산을 끈다 
        mNormalGenerator->SetFeatureAngle(60.0); // 인접한 다각형 간의 각도 차이가 이 값보다 크면 공유 
        mNormalGenerator->Update();

        mMapper->SetInputConnection(mNormalGenerator->GetOutputPort());
        mActor->SetMapper(mMapper);
        mActor->GetProperty()->SetInterpolationToPhong();
        mActor->GetProperty()->SetSpecularPower(50);  
        ui->openGLWidget->GetRenderWindow()->Render();

        convertToPolyData(mTriMesh)->PrintSelf(cout, vtkIndent());
        mActor->PrintSelf(cout, vtkIndent());
    }
}

void STLViewer::setTexture() //png 파일을 입혀주기 위한 함수
{ 
    if (!mActor) return;
    mActor->GetProperty()->SetInterpolationToPhong();
    mPolyData = vtkPolyDataMapper::SafeDownCast(mActor->GetMapper())->GetInput();
    mTriMesh = convertToMesh(mPolyData);
    mTriMesh.request_vertex_normals();
    mTriMesh.request_face_normals();
    mTriMesh.update_normals();
    mTriMesh.release_vertex_normals();
    mTriMesh.release_face_normals();

    std::string fileName = QFileDialog::getOpenFileName(nullptr,
        "PNG Files", "/home", "png Files (*.png)").toStdString().c_str();

    if (fileName == "") return;

    vtkSmartPointer<vtkPNGReader> pngReader =
        vtkSmartPointer<vtkPNGReader>::New();
    pngReader->SetFileName(fileName.c_str());
    pngReader->Update(); 
    mMapper->SetInputConnection(pngReader->GetOutputPort()); 

    mTexture = vtkSmartPointer<vtkTexture>::New(); 
    mTexture->SetInputConnection(pngReader->GetOutputPort());

    mTexture->InterpolateOn(); // 선형 보간을 킨다. 

    vtkSmartPointer<vtkTextureMapToSphere> textureMapToSphere = // png 파일을 감싸기 위해 사용 
        vtkSmartPointer<vtkTextureMapToSphere>::New();

    textureMapToSphere->SetInputData(convertToPolyData(mTriMesh)); 
    textureMapToSphere->Update();
    textureMapToSphere->PreventSeamOn();
    textureMapToSphere->SetAutomaticSphereGeneration(1);

    mNormalGenerator->SetInputConnection(textureMapToSphere->GetOutputPort());
    mNormalGenerator->ComputePointNormalsOn();
    mNormalGenerator->ComputeCellNormalsOff();
    mNormalGenerator->Update();  
    mMapper->SetInputConnection(mNormalGenerator->GetOutputPort());

    mActor->SetMapper(mMapper);
    mActor->SetTexture(mTexture); 
    mRenderer->AddActor(mActor);
    mRenderer->UseDepthPeelingOn(); // texture 투명도 해결을 위한 방법
    ui->openGLWidget->GetRenderWindow()->SetAlphaBitPlanes(1);
    ui->openGLWidget->GetRenderWindow()->SetMultiSamples(1);
    ui->openGLWidget->GetRenderWindow()->SetDoubleBuffer(1);

    ui->openGLWidget->GetRenderWindow()->Render(); 
}

void STLViewer::on_FillingtoolButton_clicked()
{
    int w = 0, h = 0;
    w = this->size().width();
    h = this->size().height();
    mFillingForm->move(w - mFillingForm->size().width(), 650);
    mFillingForm->setWindowFlag(Qt::WindowStaysOnTopHint);
    mFillingForm->show();
}

void STLViewer::meshButtonClicked()
{
    if (mActor == nullptr) return;  
    mPolyData = vtkPolyDataMapper::SafeDownCast(mActor->GetMapper())->GetInput();
    TriMesh triMesh = convertToMesh(mPolyData);   
    // Find Holes
    std::vector<std::vector<TriMesh::VertexHandle> > holes;
    holes = FindHoleVertex(triMesh);
    // Print the vertex idx of the holes
    OpenMesh::Vec3d centerVertex;
    for (int i = 0; i < holes.size(); ++i)
    {
        OpenMesh::Vec3d points = { 0.0,0.0,0.0 };
        for (int j = 0; j < holes[i].size(); ++j)
        {
            points += triMesh.point(OpenMesh::VertexHandle(holes[i][j]));
        }
        centerVertex = points / holes[i].size();
        //Connect the hole vertices based on the center point of the hole mesh
        MakeMesh(holes,centerVertex, triMesh); 
    } 
    vtkSmartPointer<vtkPolyData> polyData = convertToPolyData(triMesh);
    vtkPolyDataMapper::SafeDownCast(mActor->GetMapper())->SetInputData(polyData); 
}
void STLViewer::advanceButtonClicked()
{  
    mPolyData = vtkPolyDataMapper::SafeDownCast(mActor->GetMapper())->GetInput();
    TriMesh triMesh = convertToMesh(mPolyData);

    std::vector<std::vector<TriMesh::VertexHandle> > holes;
    holes = FindHoleVertex(triMesh);
    while (holes.size())
    {
        AdvancingFrontMethod(triMesh, holes);
        holes = FindHoleVertex(triMesh);
    }
    mPolyData = convertToPolyData(triMesh);
    vtkPolyDataMapper::SafeDownCast(mActor->GetMapper())->SetInputData(mPolyData);
}

void STLViewer::closeEvent(QCloseEvent* event)
{  
    mCutform->close();
}

void STLViewer::on_OpacityValuChange(int opacity)
{ 
    if (mActor != NULL)
    {
        mActor->GetProperty()->SetOpacity(opacity / 100.0);
        mActor->Modified();
        ui->openGLWidget->GetRenderWindow()->Render();
    }
}
