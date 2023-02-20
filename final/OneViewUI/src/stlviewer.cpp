#include "stlviewer.h"
#include "ui_stlviewer.h"
#include "CustomInteractorStyle.h"
#include <QDebug>
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
    lightform = new LightWidgetForm(0);
    shadingform = new ShadingForm(0);  
    occlusion = new Occlusion(0);
    customVTKWidget = new CustomVTKWidget;  

    mLight = vtkSmartPointer<vtkLight>::New();
    mLightActor = vtkSmartPointer<vtkLightActor>::New();
    mOutlineActor = vtkSmartPointer<vtkActor>::New();
    //light
    connect(lightform, SIGNAL(ambientValue(int)), this, SLOT(setLightAmbientChange(int)));
    connect(lightform, SIGNAL(diffuseValue(int)), this, SLOT(setLightDiffuseChange(int)));
    connect(lightform, SIGNAL(specularValue(int)), this, SLOT(setLightSpecularChange(int)));
    connect(lightform, SIGNAL(spotValue()), this, SLOT(setSpotChange()));
    connect(lightform, SIGNAL(lightX()), this, SLOT(setLightXMove()));
    connect(lightform, SIGNAL(lightY()), this, SLOT(setLightYMove()));
    connect(lightform, SIGNAL(lightZ()), this, SLOT(setLightZMove()));
    connect(lightform, SIGNAL(AmbientlightcolorChange()), this, SLOT(ambientColorChange()));
    connect(lightform, SIGNAL(DiffuselightcolorChange()), this, SLOT(diffuseColorChange()));
    connect(lightform, SIGNAL(SpecularlightcolorChange()), this, SLOT(specularColorChange()));
    connect(lightform, SIGNAL(SpotOn()), this, SLOT(setSpotOn()));
    connect(lightform, SIGNAL(SpotOff()), this, SLOT(setSpotOff()));
    connect(lightform, SIGNAL(Intensity(int)), this, SLOT(setIntensityChange(int)));

    //shading
    connect(shadingform, SIGNAL(FlatButton()), this, SLOT(setFlatChange()));
    connect(shadingform, SIGNAL(GouraudButton()), this, SLOT(setGouraudChange()));
    connect(shadingform, SIGNAL(PhongButton()), this, SLOT(setPhongChange()));
    connect(shadingform, SIGNAL(TextureButton()), this, SLOT(setTexture()));

    //occlusion
    mSsaoPass = vtkSmartPointer<vtkSSAOPass>::New();
    connect(occlusion, SIGNAL(AmbientRadius(int)), this, SLOT(setAmbientRadiusChange(int)));
    connect(occlusion, SIGNAL(AmbientBias(int)), this, SLOT(setAmbientBiasChange(int)));
    connect(occlusion, SIGNAL(AmbientLerne(int)), this, SLOT(setAmbientKernelSizeChange(int)));
    connect(occlusion, SIGNAL(AmbientBlurOn()), this, SLOT(setAmbientBlurOffChange()));
    connect(occlusion, SIGNAL(AmbientBlurOff()), this, SLOT(setAmbientBlurOnChange()));

    //cutform
    connect(mCutform, SIGNAL(clickPathButton()), ui->openGLWidget, SLOT(PathButtonClicked()));
    connect(mCutform, SIGNAL(clickConnectButton()), ui->openGLWidget, SLOT(ConnectButtonClicked()));
    connect(mCutform, SIGNAL(clickHideButton()), ui->openGLWidget, SLOT(HideButtonClicked()));
    connect(mCutform, SIGNAL(clickColorButton()), ui->openGLWidget, SLOT(ColorButtonClicked()));
}

STLViewer::~STLViewer()
{
    delete ui;
}

void STLViewer::resizeEvent(QResizeEvent *event)
{
    int height = event->size().height();
    int width = event->size().width();  
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
                vtkSmartPointer<vtkPolyData> mPolyData = convertToPolyData(triMesh);
                vtkPolyDataMapper::SafeDownCast(mActor->GetMapper())->SetInputData(mPolyData);
                break;
            }
            OpenMesh::VertexHandle vertexHandle0(holes[i][j]);
            OpenMesh::VertexHandle vertexHandle1(holes[i][j + 1]);

            triMesh.add_face({ vertexHandle0, vertexHandle1, vertexHandle2 });
        }
        vtkSmartPointer<vtkPolyData> mPolyData = convertToPolyData(triMesh);
        vtkPolyDataMapper::SafeDownCast(mActor->GetMapper())->SetInputData(mPolyData);
    } 
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
    qDebug("Completed Conver To Mesh");
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
    w =  this->sizeHint().width();
    h = this->sizeHint().height();
    qDebug() << w << " / " << h;
    mCutform->setWindowFlag(Qt::WindowStaysOnTopHint);
    mCutform->show();
    mCutform->move(100, 100);
}


void STLViewer::on_FillingtoolButton_clicked()
{
    if (mActor == nullptr) return;
    vtkNew<vtkNamedColors> colors;

    vtkSmartPointer<vtkPolyData> mPolyData = vtkPolyDataMapper::SafeDownCast(mActor->GetMapper())->GetInput();
    TriMesh triMesh = convertToMesh(mPolyData);

    /* 
    * Hole Vertex의 중심좌표를 이용한 Mesh Hole Filling
    * 
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
    *
    */
    
    AdvancingFrontMethod(triMesh);
}


void STLViewer::on_OpacitytoolButton_clicked(int opacity)
{
    if (mActor != NULL)
    {
        mActor->GetProperty()->SetOpacity(opacity / 100.0);
        mActor->Modified();
        ui->openGLWidget->GetRenderWindow()->Render();
    }
}


void STLViewer::on_LighttoolButton_clicked()
{
    int width = ui->openGLWidget->sizeHint().width();
    int height =ui->openGLWidget->sizeHint().height();

    lightform->setWindowFlag(Qt::WindowStaysOnTopHint);
    lightform->show();
//    lightform->setGeometry(width,height,10,10); 
}


void STLViewer::on_ShadingtoolButton_clicked()
{
    shadingform->setWindowFlag(Qt::WindowStaysOnTopHint);
    shadingform->show();
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
    vtkNew<vtkNamedColors> colors;
    qDebug("Triggered Open Button");
    std::string fileName = QFileDialog::getOpenFileName(nullptr,
        "STL Files", "/home", "STL Files (*.stl)").toStdString().c_str();

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

    ui->openGLWidget->AddActor(mActor);
    ui->openGLWidget->GetInteractor()->GetInteractorStyle()->GetCurrentRenderer()->ResetCamera();
    ui->openGLWidget->GetRenderWindow()->Render();

    //ui->openGLWidget->GetPolyData(polyData);
    qDebug() << "Actor" << mActor;
    qDebug() << "PolyData" << mPolyData;

    //vtkSmartPointer<vtkPolyData> mPolyData = vtkPolyDataMapper::SafeDownCast(mActor->GetMapper())->GetInput();
    mTriMesh = convertToMesh(mPolyData);
}


void STLViewer::on_SavetoolButton_clicked()
{
    mTriMesh.request_vertex_normals();
    mTriMesh.request_face_normals();
    mTriMesh.update_normals();
    mTriMesh.release_vertex_normals();
    mTriMesh.release_face_normals();
    qDebug() << "SaveFile";

    //std::string fileName = QFileDialog::getOpenFileName(nullptr,
    //    "STL Files", "/home", "STL Files (*.stl)").toStdString().c_str();

    //vtkSmartPointer<vtkSTLWriter> writer =
    //    vtkSmartPointer<vtkSTLWriter>::New();
    //writer->SetFileName("*.stl");
    //writer->SetInputConnection(m_STLReader->GetOutputPort());
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

    writer->SetInputData(mMeshToPoly);

    writer->SetFileTypeToBinary(); // 바이너리 파일로 저장
    writer->Write();

    QMessageBox::warning(this, "", "File save!!");

    //msgBox.setText("File save!!");
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

void STLViewer::AdvancingFrontMethod(TriMesh& triMesh)
{
    //for (int i = 0 ; i < 100 ; i++)
    //{
    vtkNew<vtkNamedColors> colors;
    triMesh.request_vertex_normals();
    triMesh.request_face_normals();
    triMesh.update_normals();
    triMesh.release_vertex_normals();
    triMesh.release_face_normals();
    std::vector<std::vector<TriMesh::VertexHandle> > holes;
    holes = FindHoleVertex(triMesh);

    for (int i = 0; i < holes.size(); i++)
    {
        std::vector<std::pair<int, int>> linkedVertex(triMesh.n_vertices(), { -1, -1 });
        std::unordered_map<int, OpenMesh::Vec3d> vertexNormalMap;
        OpenMesh::Vec3d firstVertex = { 0.0,0.0,0.0, };
        OpenMesh::Vec3d secondVertex = { 0.0,0.0,0.0, };
        OpenMesh::Vec3d thirdVertex = { 0.0,0.0,0.0, };

        double min = 360.0;
        double minAngle = 360.0;
        TriMesh::VertexHandle minIdx;

        OpenMesh::Vec3d prevVertex = { 0,0,0 };
        OpenMesh::Vec3d currentVertex = { 0,0,0 };
        OpenMesh::Vec3d nextVertex = { 0,0,0 };
        int count = 0;

        for (int j = 0; j < holes[i].size(); j++)
        {
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

            //cout << "dotProduct : " << dotProduct << endl;
            //cout << "(currentNextMagnitude * currentNextMagnitude) : " << (currentNextMagnitude * currentNextMagnitude) << endl;

            radian = acos(dotProduct / (currentNextMagnitude * currentPrevMagnitude));
            double angle = (radian * 180) / PI;
            double frontAngle = 0.0;
            //cout << "frontAngle : " << frontAngle << endl;
            double vectorAngle = 0.0;

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
            }
            vectorAngle = 0.0;
        }

        /*
        각도에 따른 메쉬 홀 필링
        */
        if (minAngle <= thresholdA)  // 0 < angle <= 85
        {
            OpenMesh::VertexHandle vertexHandle0(linkedVertex[minIdx.idx()].first);
            OpenMesh::VertexHandle vertexHandle1(minIdx);
            OpenMesh::VertexHandle vertexHandle2(linkedVertex[minIdx.idx()].second);
            triMesh.add_face({ vertexHandle0, vertexHandle1, vertexHandle2 });
            vtkSmartPointer<vtkPolyData> mPolyData = convertToPolyData(triMesh);
            vtkPolyDataMapper::SafeDownCast(mActor->GetMapper())->SetInputData(mPolyData);
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
            vNormal = triMesh.calc_vertex_normal(minIdx);

            //double total_area = 0.0;
            //TriMesh::Normal vertex_Normal = { 0.0,0.0,0.0 };
            //cout << "triMesh.calc_vertex_normal(minIdx) : " << triMesh.calc_vertex_normal(minIdx).normalize() << endl;
            //for (TriMesh::VertexFaceIter vf_it = triMesh.vf_begin(minIdx); vf_it.is_valid(); ++vf_it)
            //{
            //    OpenMesh::FaceHandle fh;
            //    fh = vf_it.handle(); 

            //    auto faceNormal = triMesh.calc_face_normal(fh);
            //    double area = triMesh.calc_face_area(fh);
            //    triMesh.calc_face_area(fh);
            //    
            //    vertex_Normal += area * faceNormal;
            //    total_area += area;
            //}
            //if (total_area > 0.0)
            //{
            //    vertex_Normal /= total_area;
            //    vNormal = vertex_Normal.normalize();
            //}
            //cout << "vNormal : " << vNormal << endl;

            OpenMesh::Vec3d ppVector;
            OpenMesh::Vec3d pVector;
            ppVector = bisectorVector - ((bisectorVector.dot(vNormal)) * vNormal);
            pVector = ppVector / ppVector.norm();

            OpenMesh::Vec3d pNormal;
            pNormal = vNormal + (alpha * abs(vNormal.dot(bisectorVector)) * bisectorVector);
            pNormal = pNormal / pNormal.norm();

            double nnTheta = 0.0;
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

            //delta = delta / weight; 
            checkCon = vNormal.dot(delta);
            if (checkCon <= 0)          // convex (볼록)
            {
                BVector = (pVector + (k * pNormal)) / (pVector + (k * pNormal)).norm();
            }
            else                        // concave (오목)
            {
                BVector = (pVector - (k * pNormal)) / (pVector - (k * pNormal)).norm();
            }

            OpenMesh::Vec3d newVector = { 0.0,0.0,0.0 };
            firstVector; // v1
            secondVector; // v2  
            newVector = triMesh.point(currentVertexHandle) + ((firstVector + secondVector).norm() / 2) * BVector;
            OpenMesh::VertexHandle vertexHandle0(linkedVertex[minIdx.idx()].first);
            OpenMesh::VertexHandle vertexHandle1(minIdx);
            OpenMesh::VertexHandle vertexHandle2(linkedVertex[minIdx.idx()].second);
            OpenMesh::VertexHandle newVertexHandle = triMesh.add_vertex(newVector);
            triMesh.add_face({ newVertexHandle, vertexHandle1, vertexHandle2 });
            triMesh.add_face({ newVertexHandle, vertexHandle0, vertexHandle1 });
            vtkSmartPointer<vtkPolyData> mPolyData = convertToPolyData(triMesh);
            vtkPolyDataMapper::SafeDownCast(mActor->GetMapper())->SetInputData(mPolyData);

            linkedVertex.clear();

            //vtkNew<vtkSphereSource> sphereSource;
            //sphereSource->SetCenter(triMesh.point(minIdx).data());
            //sphereSource->SetRadius(0.1);
            //// Make the surface smooth.
            //sphereSource->SetPhiResolution(10);
            //sphereSource->SetThetaResolution(10);
            //vtkNew<vtkPolyDataMapper> mapper;
            //mapper->SetInputConnection(sphereSource->GetOutputPort());
            //vtkNew<vtkActor> mActor;
            //mActor->SetMapper(mapper);
            //mActor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
            //ui->openGLWidget->AddActor(mActor);

            //vtkNew<vtkSphereSource> sphereSource2;
            //sphereSource2->SetCenter(triMesh.point(vertexHandle1).data());
            //sphereSource2->SetRadius(0.1);
            //// Make the surface smooth.
            //sphereSource2->SetPhiResolution(100);
            //sphereSource2->SetThetaResolution(100);
            //vtkNew<vtkPolyDataMapper> mapper2;
            //mapper2->SetInputConnection(sphereSource2->GetOutputPort());
            //vtkNew<vtkActor> mActor2;
            //mActor2->SetMapper(mapper2);
            //mActor2->GetProperty()->SetColor(colors->GetColor3d("Blue").GetData());
            //ui->openGLWidget->AddActor(mActor2);

            //vtkNew<vtkSphereSource> sphereSource3;
            //sphereSource3->SetCenter(triMesh.point(vertexHandle0).data());
            //sphereSource3->SetRadius(0.1);
            //// Make the surface smooth.
            //sphereSource3->SetPhiResolution(100);
            //sphereSource3->SetThetaResolution(100);
            //vtkNew<vtkPolyDataMapper> mapper3;
            //mapper3->SetInputConnection(sphereSource3->GetOutputPort());
            //vtkNew<vtkActor> mActor3;
            //mActor3->SetMapper(mapper3);
            //mActor3->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());
            //ui->openGLWidget->AddActor(mActor3);

            //vtkNew<vtkSphereSource> sphereSource4;
            //sphereSource4->SetCenter(triMesh.point(vertexHandle2).data());
            //sphereSource4->SetRadius(0.1);
            //// Make the surface smooth.
            //sphereSource4->SetPhiResolution(100);
            //sphereSource4->SetThetaResolution(100);
            //vtkNew<vtkPolyDataMapper> mapper4;
            //mapper4->SetInputConnection(sphereSource4->GetOutputPort());
            //vtkNew<vtkActor> mActor4;
            //mActor4->SetMapper(mapper4);
            //mActor4->GetProperty()->SetColor(colors->GetColor3d("Pink").GetData());
            //ui->openGLWidget->AddActor(mActor4);
        }
        else if (minAngle > thresholdB && minAngle <= 180) // 135 < angle < 180
        {
            //OpenMesh::VertexHandle prevVertexHandle(linkedVertex[minIdx.idx()].first);
            //OpenMesh::VertexHandle currentVertexHandle(minIdx);
            //OpenMesh::VertexHandle nextVertexHandle(linkedVertex[minIdx.idx()].second);

            //OpenMesh::Vec3d firstVector;
            //OpenMesh::Vec3d secondVector;

            //firstVector = triMesh.point(prevVertexHandle) - triMesh.point(currentVertexHandle);
            //secondVector = triMesh.point(nextVertexHandle) - triMesh.point(currentVertexHandle);

            //OpenMesh::Vec3d firstUnitVector;
            //OpenMesh::Vec3d secondUnitVector;

            //firstUnitVector = firstVector / firstVector.norm();
            //secondUnitVector = secondVector / secondVector.norm();

            //OpenMesh::Vec3d sumVector;
            //sumVector = firstUnitVector + secondUnitVector;

            //OpenMesh::Vec3d triVector1;
            //OpenMesh::Vec3d triVector2;
            //triVector1 = (sumVector * cos(minAngle / 3)) / (sumVector * cos(minAngle / 3)).norm();
            //triVector2 = (sumVector * cos(minAngle * 2 / 3)) / (sumVector * cos(minAngle * 2 / 3)).norm();

            //TriMesh::Normal vNormal = { 0.0,0.0,0.0 };
            //vNormal = vertexNormalMap[minIdx.idx()];

            //OpenMesh::Vec3d ppVector1;
            //OpenMesh::Vec3d ppVector2;
            //OpenMesh::Vec3d pVector1;
            //OpenMesh::Vec3d pVector2;
            //ppVector1 = triVector1 - ((triVector1.dot(vNormal)) * vNormal);
            //pVector1 = ppVector1 / ppVector1.norm();
            //ppVector2 = triVector2 - ((triVector2.dot(vNormal)) * vNormal);
            //pVector2 = ppVector2 / ppVector2.norm();

            //OpenMesh::Vec3d pNormal1;
            //OpenMesh::Vec3d pNormal2;
            //double nvVector1;
            //double nvVector2;
            //nvVector1 = vNormal.dot(triVector1);
            //pNormal1 = vNormal + alpha * abs(nvVector1) * triVector1;
            //pNormal1 = pNormal1 / pNormal1.norm();
            //nvVector2 = vNormal.dot(triVector2);
            //pNormal2 = vNormal + alpha * abs(nvVector2) * triVector2;
            //pNormal2 = pNormal2 / pNormal2.norm();

            //double nnTheta1 = 0.0;
            //nnTheta1 = acos(vNormal.dot(pNormal1) / (vNormal.norm() * pNormal1.norm()));
            //double k1 = (cos(nnTheta1) - 1) / pNormal1.dot(pVector1);
            //double nnTheta2 = 0.0;
            //nnTheta2 = acos(vNormal.dot(pNormal2) / (vNormal.norm() * pNormal2.norm()));
            //double k2 = (cos(nnTheta2) - 1) / pNormal2.dot(pVector2);

            //OpenMesh::Vec3d delta = { 0.0,0.0,0.0 };
            //for (OpenMesh::VertexHandle neighbor : triMesh.vv_range(minIdx))
            //{
            //    delta += triMesh.point(neighbor) - triMesh.point(minIdx);
            //}

            //OpenMesh::Vec3d BVector1 = { 0,0,0 };
            //OpenMesh::Vec3d BVector2 = { 0,0,0 };
            //if (vNormal.dot(delta) <= 0)        // convex (볼록)
            //{
            //    BVector1 = (pVector1 + (k1 * pNormal1)) / (pVector1 + (k1 * pNormal1)).norm();
            //    BVector2 = (pVector2 + (k2 * pNormal2)) / (pVector2 + (k2 * pNormal2)).norm();
            //}
            //else                                // concave (오목)
            //{
            //    BVector1 = (pVector1 - (k1 * pNormal1)) / (pVector1 - (k1 * pNormal1)).norm();
            //    BVector2 = (pVector2 - (k2 * pNormal2)) / (pVector2 - (k2 * pNormal2)).norm();
            //}

            //OpenMesh::Vec3d newVector1;
            //OpenMesh::Vec3d newVector2;
            //newVector1 = triMesh.point(currentVertexHandle) + (((firstVector + secondVector).norm() / 2) * BVector1);
            //newVector2 = triMesh.point(currentVertexHandle) + (((firstVector + secondVector).norm() / 2) * BVector2);

            //OpenMesh::VertexHandle vertexHandle0(linkedVertex[minIdx.idx()].first);
            //OpenMesh::VertexHandle vertexHandle1(minIdx);
            //OpenMesh::VertexHandle vertexHandle2(linkedVertex[minIdx.idx()].second);
            //OpenMesh::VertexHandle newVertexHandle1 = triMesh.add_vertex(newVector1);
            //OpenMesh::VertexHandle newVertexHandle2 = triMesh.add_vertex(newVector2);

            //triMesh.add_face({ vertexHandle1, newVertexHandle2, vertexHandle2 });
            //triMesh.add_face({ vertexHandle1, newVertexHandle2, newVertexHandle1 });
            //triMesh.add_face({ vertexHandle1, newVertexHandle1, vertexHandle0 });

            //linkedVertex.clear();

            //vtkNew<vtkSphereSource> sphereSource;
            //sphereSource->SetCenter(triMesh.point(vertexHandle0).data());
            //sphereSource->SetRadius(0.01);
            //// Make the surface smooth.
            //sphereSource->SetPhiResolution(100);
            //sphereSource->SetThetaResolution(100);
            //vtkNew<vtkPolyDataMapper> mapper;
            //mapper->SetInputConnection(sphereSource->GetOutputPort());
            //vtkNew<vtkActor> mActor;
            //mActor->SetMapper(mapper);
            //mActor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
            //ui->openGLWidget->AddActor(mActor);

            //vtkNew<vtkSphereSource> sphereSource2;
            //sphereSource2->SetCenter(triMesh.point(vertexHandle1).data());
            //sphereSource2->SetRadius(0.01);
            //// Make the surface smooth.
            //sphereSource2->SetPhiResolution(100);
            //sphereSource2->SetThetaResolution(100);
            //vtkNew<vtkPolyDataMapper> mapper2;
            //mapper2->SetInputConnection(sphereSource2->GetOutputPort());
            //vtkNew<vtkActor> mActor2;
            //mActor2->SetMapper(mapper2);
            //mActor2->GetProperty()->SetColor(colors->GetColor3d("Blue").GetData());
            //ui->openGLWidget->AddActor(mActor2);

            //vtkNew<vtkSphereSource> sphereSource3;
            //sphereSource3->SetCenter(triMesh.point(vertexHandle2).data());
            //sphereSource3->SetRadius(0.01);
            //// Make the surface smooth.
            //sphereSource3->SetPhiResolution(100);
            //sphereSource3->SetThetaResolution(100);
            //vtkNew<vtkPolyDataMapper> mapper3;
            //mapper3->SetInputConnection(sphereSource3->GetOutputPort());
            //vtkNew<vtkActor> mActor3;
            //mActor3->SetMapper(mapper3);
            //mActor3->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());
            //ui->openGLWidget->AddActor(mActor3);

            //vtkNew<vtkSphereSource> sphereSource4;
            //sphereSource4->SetCenter(triMesh.point(newVertexHandle1).data());
            //sphereSource4->SetRadius(0.01);
            //// Make the surface smooth.
            //sphereSource4->SetPhiResolution(100);
            //sphereSource4->SetThetaResolution(100);
            //vtkNew<vtkPolyDataMapper> mapper4;
            //mapper4->SetInputConnection(sphereSource4->GetOutputPort());
            //vtkNew<vtkActor> mActor4;
            //mActor4->SetMapper(mapper4);
            //mActor4->GetProperty()->SetColor(colors->GetColor3d("Pink").GetData());
            //ui->openGLWidget->AddActor(mActor4);

            //vtkNew<vtkSphereSource> sphereSource5;
            //sphereSource5->SetCenter(triMesh.point(newVertexHandle2).data());
            //sphereSource5->SetRadius(0.01);
            //// Make the surface smooth.
            //sphereSource5->SetPhiResolution(100);
            //sphereSource5->SetThetaResolution(100);
            //vtkNew<vtkPolyDataMapper> mapper5;
            //mapper5->SetInputConnection(sphereSource5->GetOutputPort());
            //vtkNew<vtkActor> mActor5;
            //mActor5->SetMapper(mapper5);
            //mActor5->GetProperty()->SetColor(colors->GetColor3d("Yellow").GetData());
            //ui->openGLWidget->AddActor(mActor5);
        }
    }
    qDebug() << "advance : " << mPolyData;
    //}
} 

void STLViewer::SetLightAmbientChange(int ambient)
{
    qDebug() << "SetLightChange";
    if (mActor != NULL)
    {
        mActor->GetProperty()->SetAmbient(ambient / 100.0);
        mActor->Modified();
        ui->openGLWidget->GetRenderWindow()->Render();
    }
} 

void STLViewer::setBoundary()
{
    if (mSTLReader == nullptr) return; 
    vtkSmartPointer<vtkPolyDataAlgorithm> polyDataAlgorithm = vtkSmartPointer<vtkPolyDataAlgorithm>::New();
    //polyDataAlgorithm->Setinput(mPolyData);
    polyDataAlgorithm->SetOutput(mPolyData); 
    vtkNew<vtkNamedColors> colors;
    // Feature edge
    vtkNew<vtkFeatureEdges> featureEdges; 
    //featureEdges->SetInputConnection(mSTLReader->GetOutputPort());  
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
    occlusion->setWindowFlag(Qt::WindowStaysOnTopHint);
    occlusion->show();

    qDebug() << "occlusion" << occlusion->geometry();

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
    qDebug() << "Radius";
    if (mActor != NULL)
    {
        mSsaoPass->SetRadius(radius / 100.0);
        mRenderer->SetPass(mSsaoPass);
        // mActor->Modified();
        ui->openGLWidget->GetRenderWindow()->Render();
    }
}

void STLViewer::setAmbientBiasChange(int bias) //Ambient Bias를 조절하기 위한 함수
{
    qDebug() << "Bias";
    if (mActor != NULL)
    {
        // mActor->GetProperty()->SetAngle(Angle);
        mSsaoPass->SetBias(bias / 1000.0);
        mRenderer->SetPass(mSsaoPass); //
        //ssao->SetDelegatePass(basicPasses);
      // mActor->Modified();
        ui->openGLWidget->GetRenderWindow()->Render();
    }
}

void STLViewer::setAmbientKernelSizeChange(int kernelSize) // Ambient KernelSize를 조절하기 위한 함수
{
    qDebug() << "Kernel";
    if (mActor != NULL)
    {
        // mActor->GetProperty()->SetAngle(Angle);
        mSsaoPass->SetKernelSize(kernelSize);
        //mRenderer->SetPass(mSsaoPass); //
        ui->openGLWidget->GetRenderWindow()->Render();
    }
}
void STLViewer::setAmbientBlurOnChange()
{
    qDebug() << "Bluronon";
    if (mActor != NULL)
    {
        // mActor->GetProperty()->SetAngle(Angle);

        qDebug() << "Blur On";
        mSsaoPass->BlurOn();
        mSsaoPass->SetBlur(true);

        // mActor->Modified();
        mRenderer->SetPass(mSsaoPass);
        ui->openGLWidget->GetRenderWindow()->Render();
    }
}

void STLViewer::setAmbientBlurOffChange()
{
    qDebug() << "Bluroff1";
    if (mActor != NULL)
    {
        qDebug() << "Blur Off";
        mSsaoPass->BlurOff();
        mRenderer->SetPass(mSsaoPass); //
        ui->openGLWidget->GetRenderWindow()->Render();

    }
}

void STLViewer::setLightAmbientChange(int ambient) // ambient light 양
{
    qDebug() << "SetLightChange";
    if (mActor != NULL)
    {
        mActor->GetProperty()->SetAmbient(ambient / 100.0);
        mActor->Modified();
        ui->openGLWidget->GetRenderWindow()->Render();
    }
}

void STLViewer::setLightDiffuseChange(int diffuse) // Diffuse Light 양
{
    qDebug() << "SetLightDiffuseChange";
    if (mActor != NULL)
    {
        mActor->GetProperty()->SetDiffuse(diffuse / 100.0);
        mActor->Modified();
        ui->openGLWidget->GetRenderWindow()->Render();
    }
}

void STLViewer::setLightSpecularChange(int Specular) // Specular Light 양
{
    qDebug() << "SetLightSpeclarChange";
    if (mActor != NULL)
    {
        mActor->GetProperty()->SetSpecular(Specular / 100.0);
        mActor->Modified();
        ui->openGLWidget->GetRenderWindow()->Render();
    }
}
void STLViewer::setSpotChange() // spot Light 
{
    QColor color = mColorDialog->getColor(); // 색 설정을 위한 colorDialog
    double r = color.toRgb().redF();
    double g = color.toRgb().greenF();
    double b = color.toRgb().blueF();

    //mLight->SetConeAngle(0.3); // 콘의 각도
   // LightActor->GetConeProperty()->SetColor(r, g, b);

 //   mLight->SetLightTypeToCameraLight();
    mLight->SetPositional(true);
    mLight->SetLightTypeToCameraLight();
    //mLight->SetColor(mcolors->GetColor3d("Magenta").GetData());
    mLight->SetColor(r, g, b);
    mLight->PositionalOn();

    mLightActor->SetLight(mLight);

    //mActor->GetProperty()->SetAmbient(ambient);
    //mActor->GetProperty()->SetDiffuse(diffuse);
    //mActor->GetProperty()->SetSpecular(specular);
    //mActor->GetProperty()->SetSpecularPower(specularpower);
    //mActor->GetProperty()->SetSpecularColor(r, g, b);

    //mActor->AddPosition(position.data());

    qDebug() << "mLight" << mLight;
    mRenderer->AddLight(mLight);
    mRenderer->AddViewProp(mLightActor);

    ui->openGLWidget->GetRenderWindow()->Render();

}

void STLViewer::setSpotOn() // setSpot 버튼을 켜기 위한 함수
{
    if (mLight->GetSwitch() == 0)
    {
        qDebug() << "On";
        mLight->SetSwitch(1);
    }
    ui->openGLWidget->GetRenderWindow()->Render();
}

void STLViewer::setSpotOff() // setSpot 버튼을 끄기 위한 함수
{
    if (mLight->GetSwitch() == 1)
    {
        qDebug() << "Off";
        mLight->SetSwitch(0);
    }
    ui->openGLWidget->GetRenderWindow()->Render();
}

void STLViewer::setLightXMove() // x방향에 빛을 비추기 위한 함수
{
    qDebug() << "LightMove";

    double max = 0;
    double bound = 0;
    for (int i = 0; i < 6; i++)
    {
        // cout << "i : " << abs(outlineActor->GetBounds()[i]) << endl;
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
    qDebug() << "LightMove";

    double max = 0;
    double bound = 0;
    for (int i = 0; i < 6; i++)
    {
        // cout << "i : " << abs(outlineActor->GetBounds()[i]) << endl;
        max = (max < abs(mOutlineActor->GetBounds()[i])) ? abs(mOutlineActor->GetBounds()[i]) : max;
        if (max == abs(mOutlineActor->GetBounds()[i]))
        {
            bound = abs(max);
        }
    }
    //cout << "max :" << abs(max) << endl;

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
    qDebug() << "LightMove";
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
    double ambient = 0.4;
    double diffuse = 0.4;
    double specular = 0.4;
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

    qDebug() << "mLight" << mLight;
    mRenderer->AddLight(mLight);
    mRenderer->AddActor(mActor);
    mRenderer->SetAmbient(0.5, 0.5, 0.5);

    mRenderer->Render();
    mInteractor->Start();
    ui->openGLWidget->GetRenderWindow()->Render();
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
    double ambient = 0.4; // 초기 설정값
    double diffuse = 0.4;
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

    //mActor->AddPosition(position.data());

    qDebug() << "mLight" << mLight;
    mRenderer->AddLight(mLight);

    mRenderer->AddViewProp(mLightActor);

    ui->openGLWidget->GetInteractor()->GetInteractorStyle()->GetCurrentRenderer()->AddViewProp(mLightActor);

    ui->openGLWidget->GetRenderWindow()->Render();

    mInteractor->Start();

}




void STLViewer::setIntensityChange(int intensity) // 빛의 세기를 조절하기 위한 함수
{
    qDebug() << "ConAngleChange";
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
    qDebug() << "Flat";

    if (mActor != NULL) {
        mMapper->SetInputConnection(mSTLReader->GetOutputPort());
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
    qDebug() << "gour";
    if (mActor != NULL) {
        vtkSmartPointer<vtkPolyDataNormals> normalGenerator =
            vtkSmartPointer<vtkPolyDataNormals>::New();
        normalGenerator->SetInputConnection(mSTLReader->GetOutputPort());
        normalGenerator->ComputePointNormalsOn();
        normalGenerator->ComputeCellNormalsOff();
        normalGenerator->SetFeatureAngle(60.0);
        normalGenerator->Update();

        mMapper->SetInputConnection(normalGenerator->GetOutputPort());
        mActor->SetMapper(mMapper);
        mActor->GetProperty()->SetInterpolationToGouraud();
        mActor->GetProperty()->SetSpecularPower(50);


        //vtkSmartPointer<vtkLight> light = vtkSmartPointer<vtkLight>::New();
       // light->SetPosition(0, 0, 1);
       // light->SetFocalPoint(mRenderer->GetActiveCamera()->GetFocalPoint());
       //// light->SetColor(1, 1, 1);
       // mRenderer->AddLight(light);

        //vtkSmartPointer<vtkCamera> camera =
        //    vtkSmartPointer<vtkCamera>::New();

        //camera->SetPosition(0, 0, 5);
        //camera->SetFocalPoint(0, 0, 0);
        //camera->SetViewUp(0, 1, 0);

        mActor->SetTexture(mTexture);
        mRenderer->AddActor(mActor);
        //mRenderer->AddLight(light);

        ui->openGLWidget->GetRenderWindow()->Render();
    }

}

void STLViewer::setPhongChange() // Phong sading
{                               // 고러드 쉐이딩에서 하이라이트나 반사광을 표현할 수 없는것을
                                // 가능하게 해주는 쉐이딩
    qDebug() << "Phong";
    if (mActor != NULL) {
        /*mActor->GetProperty()->SetSpecularPower(0.3);*/

        vtkSmartPointer<vtkPolyDataNormals> normalGenerator = // 다각형 메시에 대한 법선 계산을 위해서 사용
            vtkSmartPointer<vtkPolyDataNormals>::New();
        normalGenerator->SetInputConnection(mSTLReader->GetOutputPort());
        normalGenerator->ComputePointNormalsOn(); //접 번선 계산을 킨다.
        normalGenerator->ComputeCellNormalsOff(); //셀 법선 계산을 끈다

        normalGenerator->SetFeatureAngle(60.0); // 인접한 다각형 간의 각도 차이가 이 값보다 크면 공유 
        normalGenerator->Update();

        mMapper->SetInputConnection(normalGenerator->GetOutputPort());
        mActor->SetMapper(mMapper);
        mActor->GetProperty()->SetInterpolationToPhong();
        mActor->GetProperty()->SetSpecularPower(50);
        //vtkSmartPointer<vtkLight> light = vtkSmartPointer<vtkLight>::New();
        //light->SetPosition(0, 0, 1);
        //light->SetFocalPoint(mRenderer->GetActiveCamera()->GetFocalPoint());
        ///*light->SetColor(1, 1, 1);*/
        //mRenderer->AddLight(light);

        //vtkSmartPointer<vtkCamera> camera =
        //    vtkSmartPointer<vtkCamera>::New();
        //camera->SetPosition(0, 0, 3);
        //camera->SetFocalPoint(0, 0, 0);
        //camera->SetViewUp(0, 1, 0);
        //camera->Azimuth(30);
        //camera->Elevation(30);

        mActor->SetTexture(mTexture);
        mRenderer->AddActor(mActor);
        ui->openGLWidget->GetRenderWindow()->Render();
    }
}

void STLViewer::setTexture() //png 파일을 입혀주기 위한 함수
{
    qDebug() << "SetTexture";

    std::string fileName = QFileDialog::getOpenFileName(nullptr,
        "PNG Files", "/home", "png Files (*.png)").toStdString().c_str();

    vtkSmartPointer<vtkPNGReader> pngReader =
        vtkSmartPointer<vtkPNGReader>::New();
    pngReader->SetFileName(fileName.c_str());
    pngReader->Update();
    qDebug() << "pngRender" << pngReader->GetFileName();
    mMapper->SetInputConnection(pngReader->GetOutputPort());
    //qDebug() << "m_polyData" << mPolyData;

    mTexture = vtkSmartPointer<vtkTexture>::New();

    qDebug() << "m_Texture" << mTexture->GetTextureUnit();
    mTexture->SetInputConnection(pngReader->GetOutputPort());

    mTexture->InterpolateOn(); // 선형 보간을 킨다.

    qDebug() << "MaxFiltering" << mTexture->GetMaximumAnisotropicFiltering();

    vtkSmartPointer<vtkTextureMapToSphere> textureMapToSphere = // png 파일을 감싸기 위해 사용 
        vtkSmartPointer<vtkTextureMapToSphere>::New();

    textureMapToSphere->SetInputData(mPolyData);
    textureMapToSphere->SetInputConnection(mSTLReader->GetOutputPort());
    textureMapToSphere->Update();
    textureMapToSphere->PreventSeamOn();
    textureMapToSphere->SetAutomaticSphereGeneration(1);

    mMapper->SetInputConnection(textureMapToSphere->GetOutputPort());
    mActor->SetMapper(mMapper);
    mActor->SetTexture(mTexture);

    qDebug() << "texture" << mTexture;
    mRenderer->AddActor(mActor);
    mRenderer->UseDepthPeelingOn(); // texture 투명도 해결을 위한 방법
    ui->openGLWidget->GetRenderWindow()->SetAlphaBitPlanes(1);
    ui->openGLWidget->GetRenderWindow()->SetMultiSamples(1);

    ui->openGLWidget->GetRenderWindow()->Render();
    /* m_Interactor->Start();*/
    //m_OriginProp = mActor->GetProperty();
    //qDebug() << "m_OrigninProp" << m_OriginProp;
}