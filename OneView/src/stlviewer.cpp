#include "stlviewer.h"
#include "ui_stlviewer.h"
#include "CustomInteractorStyle.h"
#include <QDebug>
#include <QDebug>
#include <TriMesh.h>
#include <algorithm> 

#include <vtkPolyDataNormals.h>
#include <vtkConeSource.h>
#include <vtkOutlineFilter.h>

const int PI = 3.141592; 
const double thresholdA = 85.0;
const double thresholdB = 135.0;
const double alpha = 0.45;

STLViewer::STLViewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::STLViewer)
{ 
   
    ui->setupUi(this);

    w = new QWidget(this);
    m_Slider = new QSlider(Qt::Horizontal, this);
    m_Slider->setRange(0,100); 
    m_Slider->setMaximumSize(QSize(200, 50));
    m_Cutbutton = new QPushButton("Cut", this);
    m_Openbutton = new QPushButton("Open", this);
    m_Colorbutton = new QPushButton("Color", this);
    m_HoleFilling = new QPushButton("Hole Filling", this);
    m_Light = new QPushButton("Light", this);
    mColorDialog = new QColorDialog(); 


    m_Sliderbutton = new QPushButton("Opacity", this);
    connect(m_Sliderbutton, SIGNAL(clicked()), this, SLOT(on_pushButton_clicked()));
    connect(m_Openbutton, SIGNAL(clicked()), this, SLOT(ClickedOpen()));
    connect(m_Colorbutton, &QPushButton::clicked, this, [this](bool) { mColorDialog->show(); });
    connect(mColorDialog, SIGNAL(currentColorChanged(QColor)), this, SLOT(SetColor(QColor)));
    connect(m_Slider, SIGNAL(valueChanged(int)), this, SLOT(SetOpacity(int)));
    connect(m_HoleFilling, SIGNAL(clicked()), this, SLOT(HoleFilling()));
    connect(m_Light, SIGNAL(clicked()), this, SLOT(SetLight()));
    mLight = vtkSmartPointer<vtkLight>::New();

    m_Hlayout = new QHBoxLayout();
    m_Vlayout = new QVBoxLayout();
    m_Slider->hide();
    //m_layout->addWidget(m_Slider);
    m_Hlayout->addWidget(m_Openbutton); 
    m_Hlayout->addWidget(m_Colorbutton); 
    m_Hlayout->addWidget(m_Cutbutton); 
    m_Hlayout->addWidget(m_Light);
    m_Hlayout->addWidget(m_HoleFilling);
    m_Hlayout->addWidget(m_Sliderbutton); 

    m_Vlayout->addLayout(m_Hlayout);
    m_Vlayout->addWidget(m_Slider);


    w->setLayout(m_Vlayout);
    w->setMinimumSize(500, 100);
    
    
    w->setStyleSheet("QWidget::horver{"
                     "background-color:#fff;"
                     "border: 10px;}"); 

    mRenderer = ui->openGLWidget->GetInteractor()->GetInteractorStyle()->GetCurrentRenderer();


}

STLViewer::~STLViewer()
{
    delete ui;
}

void STLViewer::resizeEvent(QResizeEvent *event)
{
    int height = event->size().height();
    int width = event->size().width(); 
    w->move(width/2 - 200, 50);  
}

void STLViewer::SetColor(QColor color)
{
    double r = color.toRgb().redF();
    double g = color.toRgb().greenF();
    double b = color.toRgb().blueF();
    mActor->GetProperty()->SetDiffuseColor(r, g, b);
    mActor->Modified();
    ui->openGLWidget->GetRenderWindow()->Render();
}

void STLViewer::SetOpacity(int opacity)
{
    if (mActor != NULL)
    {
        mActor->GetProperty()->SetOpacity(opacity / 100.0);
        mActor->Modified();
        ui->openGLWidget->GetRenderWindow()->Render();
    }
}

void STLViewer:: on_pushButton_clicked()
{ 
    if (m_Slider->isHidden())
            m_Slider->show();
        else
            m_Slider->hide();
}

void STLViewer::ClickedOpen()
{
    qDebug("Triggered Open Button");
    std::string fileName = QFileDialog::getOpenFileName(nullptr,
        "STL Files", "/home", "STL Files (*.stl)").toStdString().c_str();

    // STL Reader
    vtkSmartPointer<vtkSTLReader> mSTLReader = vtkSmartPointer<vtkSTLReader>::New();
    mSTLReader->SetFileName(fileName.c_str());
    mSTLReader->Update();

    // Mapper
    vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
    polyData->DeepCopy(mSTLReader->GetOutput());
    vtkSmartPointer<vtkPolyDataMapper> mMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mMapper->SetInputData(polyData);

    // Actor
    mActor = vtkSmartPointer<vtkActor>::New();
    mActor->SetMapper(mMapper);

    ui->openGLWidget->AddActor(mActor);  
    ui->openGLWidget->GetInteractor()->GetInteractorStyle()->GetCurrentRenderer()->ResetCamera();
    ui->openGLWidget->GetRenderWindow()->Render();

    //ui->openGLWidget->GetPolyData(polyData);
    qDebug() << "Actor" << mActor;
    qDebug() << "PolyData" << polyData;
}

void STLViewer::HoleFilling()
{
    if (mActor == nullptr) return;
    vtkNew<vtkNamedColors> colors;

    vtkSmartPointer<vtkPolyData> mPolyData = vtkPolyDataMapper::SafeDownCast(mActor->GetMapper())->GetInput();
    TriMesh triMesh = convertToMesh(mPolyData); 

    // Find Holes
    //std::vector<std::vector<TriMesh::VertexHandle> > holes;
    //holes = FindHoleVertex(triMesh);

    //Print the vertex idx of the holes
    //OpenMesh::Vec3d centerVertex;
    //for (int i = 0; i < holes.size(); ++i)
    //{ 
    //    OpenMesh::Vec3d points = { 0.0,0.0,0.0 }; 
    //    for (int j = 0; j < holes[i].size(); ++j)
    //    {  
    //        points += triMesh.point(OpenMesh::VertexHandle(holes[i][j]));  
    //    }  
    //    centerVertex = points / holes[i].size();  

    //    // Connect the hole vertices based on the center point of the hole mesh
    //    //MakeMesh(holes, centerVertex, triMesh); 
    //}
    // 

    //// Print Hole Vertex
    //for (int i = 0; i < holes.size(); i++)
    //{ 
    //    cout << "holes " << i << ": ";
    //    for (int j = 0; j < holes[i].size(); j++)
    //    { 
    //        cout <<  holes[i][j] << ", ";  
    //    }
    //    cout << endl;
    //}
    AdvancingFrontMethod(triMesh);

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
                vtkSmartPointer<vtkPolyData> meshToPoly = convertToPolyData(triMesh);
                vtkPolyDataMapper::SafeDownCast(mActor->GetMapper())->SetInputData(meshToPoly);
                break;
            }
            OpenMesh::VertexHandle vertexHandle0(holes[i][j]);
            OpenMesh::VertexHandle vertexHandle1(holes[i][j + 1]);

            triMesh.add_face({ vertexHandle0, vertexHandle1, vertexHandle2 });  
        }
        //vtkSmartPointer<vtkPolyData> meshToPoly = convertToPolyData(triMesh); 
        //vtkPolyDataMapper::SafeDownCast(mActor->GetMapper())->SetInputData(meshToPoly);
        
    } 
}

void STLViewer::AdvancingFrontMethod(TriMesh& triMesh)
{
    for (int i = 0 ; i < 100 ; i++)
    {
    vtkNew<vtkNamedColors> colors; 
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
            //cout << "holes : " << i << endl;
            int count = 0;
            OpenMesh::Vec3d normalVertex = { 0,0,0 };
            OpenMesh::Vec3d normal = { 0,0,0 };

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
                    //cout << "holes[i][j].idx() : " << holes[i][j].idx() << " / " << linkedVertex[holes[i][j].idx()].first << " / " << linkedVertex[holes[i][j].idx()].second << endl;
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

                        // Find Vertex Normal  
                        //normal += firstVec.cross(secondVec) / firstVec.cross(secondVec).norm();
                        normal += (firstVec.cross(secondVec));
                    }
                }

                //cout << "sum vectorAngle : " << vectorAngle << endl;
                if (vectorAngle > 180)
                {
                    frontAngle = angle;
                }
                else
                {
                    continue;
                }
                //cout << "Vertex Idx : " << holes[i][j] << " / front Angle : " << frontAngle << endl; 

                min = (min > frontAngle) ? frontAngle : min;
                if (min == frontAngle)
                {
                    minAngle = min;
                    minIdx = holes[i][j];
                }
                vectorAngle = 0.0;
            }

            normal = normal.normalize();
            vertexNormalMap.insert({ minIdx.idx(), normal });

            //cout << "Min Angle Vertex Idx : " << minIdx << " / Min Angle : " << minAngle << endl;

            /*
            각도에 따른 메쉬 홀 필링
            */
            if (minAngle <= thresholdA)  // 0 < angle <= 85
            {
                OpenMesh::VertexHandle vertexHandle0(linkedVertex[minIdx.idx()].first);
                OpenMesh::VertexHandle vertexHandle1(minIdx);
                OpenMesh::VertexHandle vertexHandle2(linkedVertex[minIdx.idx()].second);
                triMesh.add_face({ vertexHandle0, vertexHandle1, vertexHandle2 });
                vtkSmartPointer<vtkPolyData> meshToPoly = convertToPolyData(triMesh);
                vtkPolyDataMapper::SafeDownCast(mActor->GetMapper())->SetInputData(meshToPoly);
                linkedVertex.clear();

                //vtkNew<vtkSphereSource> sphereSource;
                //sphereSource->SetCenter(triMesh.point(vertexHandle1).data());
                //sphereSource->SetRadius(0.1);
                //// Make the surface smooth.
                //sphereSource->SetPhiResolution(100);
                //sphereSource->SetThetaResolution(100);
                //vtkNew<vtkPolyDataMapper> mapper;
                //mapper->SetInputConnection(sphereSource->GetOutputPort());
                //vtkNew<vtkActor> mActor;
                //mActor->SetMapper(mapper);
                //mActor->GetProperty()->SetColor(colors->GetColor3d("Blue").GetData());
                //ui->openGLWidget->AddActor(mActor);
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
                vNormal = vertexNormalMap[minIdx.idx()];

                OpenMesh::Vec3d ppVector;
                OpenMesh::Vec3d pVector;
                ppVector = bisectorVector - ((bisectorVector.dot(vNormal)) * vNormal);
                pVector = ppVector / ppVector.norm();

                OpenMesh::Vec3d pNormal;
                pNormal = vNormal + alpha * abs(vNormal.dot(bisectorVector)) * bisectorVector;
                pNormal = pNormal / pNormal.norm();

                double nnTheta = 0.0;
                nnTheta = acos(vNormal.dot(pNormal));
                double k = (cos(nnTheta) - 1) / pNormal.dot(pVector);

                OpenMesh::Vec3d delta = { 0.0,0.0,0.0 };
                int weight = 0;
                for (OpenMesh::VertexHandle neighbor : triMesh.vv_range(minIdx))
                {
                    //weight += 1;

                    //delta += (1 / weight) * (triMesh.point(neighbor) - triMesh.point(minIdx));
                    delta += triMesh.point(neighbor) - triMesh.point(minIdx);
                }

                OpenMesh::Vec3d BVector = { 0,0,0 };
                if (vNormal.dot(delta) <= 0)        // convex (볼록)
                {
                    BVector = (pVector + (k * pNormal)) / (pVector + (k * pNormal)).norm();
                }
                else                                // concave (오목)
                {
                    BVector = (pVector - (k * pNormal)) / (pVector - (k * pNormal)).norm();
                }

                OpenMesh::Vec3d newVector;
                firstVector; // v1
                secondVector; // v2  
                newVector = triMesh.point(currentVertexHandle) + (((firstVector + secondVector).norm() / 2) * BVector);

                OpenMesh::VertexHandle vertexHandle0(linkedVertex[minIdx.idx()].first);
                OpenMesh::VertexHandle vertexHandle1(minIdx);
                OpenMesh::VertexHandle vertexHandle2(linkedVertex[minIdx.idx()].second);
                OpenMesh::VertexHandle newVertexHandle = triMesh.add_vertex(newVector);
                triMesh.add_face({ newVertexHandle, vertexHandle1, vertexHandle2 });
                triMesh.add_face({ newVertexHandle, vertexHandle0, vertexHandle1 });
                vtkSmartPointer<vtkPolyData> meshToPoly = convertToPolyData(triMesh);
                vtkPolyDataMapper::SafeDownCast(mActor->GetMapper())->SetInputData(meshToPoly);

                linkedVertex.clear();

                //vtkNew<vtkSphereSource> sphereSource;
                //sphereSource->SetCenter(triMesh.point(newVertexHandle).data());
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

void STLViewer::SetLight()
{
    QColor color = mColorDialog->getColor();
    double r = color.toRgb().redF();
    double g = color.toRgb().greenF();
    double b = color.toRgb().blueF();

    mLight->SetLightTypeToSceneLight();
    //mLight->SetLightTypeToCameraLight();

    mLight->SetAmbientColor(r, g, b);
    qDebug() << "mLight" << mLight;
    mRenderer->AddLight(mLight);
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
