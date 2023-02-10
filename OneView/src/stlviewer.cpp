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

    // --------------------- start to draw bounding box ------------------------ 

    vtkSmartPointer<vtkOutlineFilter> outlineFilter =
        vtkSmartPointer<vtkOutlineFilter>::New();
    outlineFilter->SetInputConnection(mSTLReader->GetOutputPort());

    vtkSmartPointer<vtkPolyDataMapper> outLinemapper =
        vtkSmartPointer<vtkPolyDataMapper>::New();
    outLinemapper->SetInputConnection(outlineFilter->GetOutputPort());

    vtkSmartPointer<vtkActor> outlineActor = vtkSmartPointer<vtkActor>::New();
    outlineActor->SetMapper(outLinemapper);
    outlineActor->GetProperty()->SetColor(1, 1, 1);
    // --------------------- Drawing bounding box end------------------------ 
    ui->openGLWidget->AddActor(outlineActor);   
    
    double max = 0;
    double bound = 0;
    for (int i = 0; i < 6; i++)
    {
        cout << "i : " << abs(outlineActor->GetBounds()[i]) << endl;
        max = (max < abs(outlineActor->GetBounds()[i])) ? abs(outlineActor->GetBounds()[i]) : max;
        if (max == abs(outlineActor->GetBounds()[i]))
        {
            bound = max; 
        }
    }
    cout << "max :" << abs(max) << endl;

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

    vtkSmartPointer<vtkPolyData> mPolyData = vtkPolyDataMapper::SafeDownCast(mActor->GetMapper())->GetInput();
    TriMesh triMesh = convertToMesh(mPolyData); 


    // Find Holes
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

    //Print the vertex idx of the holes
    OpenMesh::Vec3d centerVertex;
    for (int i = 0; i < holes.size(); ++i)
    { 
        OpenMesh::Vec3d points = { 0.0,0.0,0.0 }; 
        for (int j = 0; j < holes[i].size(); ++j)
        {  
            points += triMesh.point(OpenMesh::VertexHandle(holes[i][j]));  
        }  
        centerVertex = points / holes[i].size(); 

        //MakeMesh(holes, centerVertex, triMesh);

        vtkNew<vtkNamedColors> colors;
        //vtkNew<vtkSphereSource> sphereSource2;
        //sphereSource2->SetCenter(centerVertex[0], centerVertex[1], centerVertex[2]);
        //sphereSource2->SetRadius(0.1);
        //// Make the surface smooth.
        //sphereSource2->SetPhiResolution(100);
        //sphereSource2->SetThetaResolution(100);                                                     

        //vtkNew<vtkPolyDataMapper> mapper2;                                                          
        //mapper2->SetInputConnection(sphereSource2->GetOutputPort());
        //vtkNew<vtkActor> mActor2;                                                                  
        //mActor2->SetMapper(mapper2);
        //mActor2->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
        //ui->openGLWidget->AddActor(mActor2);
    }

    // hole boundary vertex
    vtkNew<vtkNamedColors> colors;
    //for (int vertexIdx : boundaryVertex)
    //{
    //    vtkNew<vtkSphereSource> sphereSource2;
    //    sphereSource2->SetCenter(triMesh.point(OpenMesh::VertexHandle(vertexIdx)).data());
    //    sphereSource2->SetRadius(0.1);
    //    // Make the surface smooth.
    //    sphereSource2->SetPhiResolution(100);
    //    sphereSource2->SetThetaResolution(100);                                                      

    //    vtkNew<vtkPolyDataMapper> mapper2;                                                          
    //    mapper2->SetInputConnection(sphereSource2->GetOutputPort());
    //    vtkNew<vtkActor> mActor2;                                                                   
    //    mActor2->SetMapper(mapper2);
    //    mActor2->GetProperty()->SetColor(colors->GetColor3d("Yellow").GetData());
    //    ui->openGLWidget->AddActor(mActor2);
    //}  

    for (int i = 0; i < holes.size(); i++)
    { 
        cout << "holes " << i << ": ";
        for (int j = 0; j < holes[i].size(); j++)
        { 
            cout <<  holes[i][j] << ", ";  
        }
        cout << endl;
    }


    //////////////////////////////////// Minimum Angle ///////////////////
    //// Calculate 3 vertex angle 
    //// v-i, v, v+i, min Angle
    //OpenMesh::Vec3d prevVertex = { 0,0,0 };
    //OpenMesh::Vec3d currentVertex = { 0,0,0 };
    //OpenMesh::Vec3d nextVertex = { 0,0,0 };

    //TriMesh::VertexHandle minAngleVH;  
    //TriMesh::VertexHandle vertexHandle;

    ////find minimum angle
    //for (int i = 0; i < holes.size(); i++)
    //{   
    //    double min = 360.0;
    //    double minAngle = 360.0; 
    //    double minRadian = 0.0;
    //    for (int j = 0; j < holes[i].size() ; j++)   // 0 ~ 107
    //    {  
    //        if (j == 0)
    //        {
    //            prevVertex = triMesh.point(holes[i].back()); 
    //            currentVertex = triMesh.point(holes[i][j]);
    //            nextVertex = triMesh.point(holes[i][j + 1]);
    //        } 
    //        else if (j == holes[i].size() - 1)
    //        {
    //            prevVertex = triMesh.point(holes[i][j-1]);
    //            currentVertex = triMesh.point(holes[i][j]);
    //            nextVertex = triMesh.point(holes[i].front()); 
    //        }
    //        else
    //        { 
    //            prevVertex = triMesh.point(holes[i][j - 1]);
    //            currentVertex = triMesh.point(holes[i][j]);
    //            nextVertex = triMesh.point(holes[i][j + 1]); 
    //        } 

    //        OpenMesh::Vec3d currentNextVertex;
    //        OpenMesh::Vec3d currentPrevVertex;
    //        currentNextVertex = nextVertex - currentVertex;
    //        currentPrevVertex = currentVertex - prevVertex;

    //        // dot product
    //        double dotProduct = 0.0;
    //        double currentNextMagnitude = 0.0;
    //        double currentPrevMagnitude = 0.0;
    //        double radian = 0.0;
    //        double calAngle = 0.0;

    //        dotProduct = (currentNextVertex[0] * currentPrevVertex[0]) + (currentNextVertex[1] * currentPrevVertex[1]) + (currentNextVertex[2] * currentPrevVertex[2]);
    //        currentNextMagnitude = sqrt((currentNextVertex[0] * currentNextVertex[0]) + (currentNextVertex[1] * currentNextVertex[1]) + (currentNextVertex[2] * currentNextVertex[2]));
    //        currentPrevMagnitude = sqrt((currentPrevVertex[0] * currentPrevVertex[0]) + (currentPrevVertex[1] * currentPrevVertex[1]) + (currentPrevVertex[2] * currentPrevVertex[2]));

    //        //cout << "dotProduct : " << dotProduct << endl;
    //        //cout << "(currentNextMagnitude * currentNextMagnitude) : " << (currentNextMagnitude * currentNextMagnitude) << endl;

    //        //radian = acos(dotProduct / (currentNextMagnitude * currentPrevMagnitude));
    //        radian = dotProduct / (currentNextMagnitude * currentPrevMagnitude);
    //        double angle = (radian * 180) / PI;


    //        //cout << "angle : " << angle << endl;
    //        //cout << "cal Angle : " << calAngle << endl;

    //        cout << "Hole Vertex : " << holes[i][j].idx() << " / " << "radian : " << radian << endl;
    //        //cout << "!!!!!!!!!!!!!!!!!!!!!!! : " << sin(angle) << endl; 
    //        //cout << " angle : " << angle << endl;

    //        //if (sin(angle) < 0)
    //        //{
    //        //    cout << "angle : " << 360 - angle << endl;
    //        //}

    //        //if (cos(angle) > cos(360 - angle))
    //        //{  
    //        //    calAngle = angle;
    //        //    //cout << "angle : " << calAngle << endl;
    //        //}
    //        //else
    //        //{
    //        //    cout << "over 180 -> " << calAngle << " ";
    //        //    calAngle = 360 - angle;
    //        //    calAngle = angle;
    //        //    //cout << "angle : " << 360 - calAngle << endl;
    //        //}
    //        //cout << "angle : " << calAngle << endl;

    //        //min = (min > calAngle) ? calAngle : min;
    //        //if (min == calAngle)
    //        //{
    //        //    minAngle = min;
    //        //    minAngleVH = holes[i][j];
    //        //} 
    //        minAngle = (minAngle > radian) ? radian : minAngle;
    //        if (minAngle == radian)
    //        {
    //            minRadian = minAngle;
    //            minAngleVH = holes[i][j];
    //        }

    //        //minAngle = std::min(minAngle, radian); 
    //        //vertexHandle = holes[i][j];
    //    }
    //     //cout << "holes " << i << ": ";
    //    //cout << "vertex " << i << ": ";
    //    //cout << "Smallest angle: " << minRadian << " radians" << endl;
    //    ////cout << " -> minAngle : " << minAngle << ", minAngleVH : " << minAngleVH << endl; 

    //    //if (minRadian < PI) {
    //    //    std::cout << "Vertex : " << minAngleVH << " Mesh is not filled.\n";
    //    //}
    //    //else {
    //    //    std::cout << "Vertex : " << minAngleVH << " Mesh is filled.\n";
    //    //}

    //    vtkNew<vtkSphereSource> sphereSource2;
    //    sphereSource2->SetCenter(triMesh.point(minAngleVH).data());
    //    sphereSource2->SetRadius(0.1);
    //    // Make the surface smooth.
    //    sphereSource2->SetPhiResolution(100);
    //    sphereSource2->SetThetaResolution(100); 
    //    vtkNew<vtkPolyDataMapper> mapper2;
    //    mapper2->SetInputConnection(sphereSource2->GetOutputPort());
    //    vtkNew<vtkActor> mActor2;
    //    mActor2->SetMapper(mapper2);
    //    mActor2->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());
    //    ui->openGLWidget->AddActor(mActor2);
    //}
    ////////////////////////


    //for (int i = 0; i < holes.size(); i++)
    //{
    //    int max_angle_vertex_index = -1;
    //    double max_angle = 360.0;
    //    double min = 360.0;
    //    double minAngle = 0.0;
    //    TriMesh::VertexHandle minVertex;
    //    for (int j = 0; j < holes[i].size(); j++) 
    //    {
    //        OpenMesh::Vec3d vertexNormal = triMesh.normal(holes[i][j]);
    //        double angle = acos(vertexNormal.dot(centerVertex) / (vertexNormal.norm() * centerVertex.norm()));
    //        if (angle < max_angle)
    //        {
    //            max_angle = angle;
    //            max_angle_vertex_index = holes[i][j].idx();
    //        }
    //    }
    //    OpenMesh::Vec3d hole_direction = centerVertex.cross(triMesh.normal(TriMesh::VertexHandle(max_angle_vertex_index)));
    //    std::cout << "Mesh hole direction: " << hole_direction << std::endl;
    //}
      

    //vtkNew<vtkSphereSource> sphereSource2;
    //sphereSource2->SetCenter(triMesh.point(minAngleVH).data());
    //sphereSource2->SetRadius(0.1);
    //// Make the surface smooth.
    //sphereSource2->SetPhiResolution(100);
    //sphereSource2->SetThetaResolution(100);

    //vtkNew<vtkPolyDataMapper> mapper2;
    //mapper2->SetInputConnection(sphereSource2->GetOutputPort());
    //vtkNew<vtkActor> mActor2;
    //mActor2->SetMapper(mapper2);
    //mActor2->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());
    //ui->openGLWidget->AddActor(mActor2);


//////////////////////
// 한 버텍스에서 연결된 모든 엣지를 이용하여 내적을 구한 다음 다 더해서 360에서 빼기
// ////////////////// 
    for (int i = 0; i < holes.size(); i++)
    {
        OpenMesh::Vec3d firstVertex = { 0.0,0.0,0.0, };
        OpenMesh::Vec3d secondVertex = { 0.0,0.0,0.0, };
        OpenMesh::Vec3d thirdVertex = { 0.0,0.0,0.0, };
         

        double min = 360.0;
        double minAngle = 360.0;
        TriMesh::VertexHandle minIdx; 

        OpenMesh::Vec3d prevVertex = { 0,0,0 };
        OpenMesh::Vec3d currentVertex = { 0,0,0 };
        OpenMesh::Vec3d nextVertex = { 0,0,0 };
        cout << "holes : " << i << endl;
        int count = 0;
        for (int j = 0; j < holes[i].size(); j++)
        {
            if (j == 0)
            {
                prevVertex = triMesh.point(holes[i].back());
                currentVertex = triMesh.point(holes[i][j]);
                nextVertex = triMesh.point(holes[i][j + 1]);
            }
            else if (j == holes[i].size() - 1)
            {
                prevVertex = triMesh.point(holes[i][j - 1]);
                currentVertex = triMesh.point(holes[i][j]);
                nextVertex = triMesh.point(holes[i].front()); 
            }
            else
            {
                prevVertex = triMesh.point(holes[i][j - 1]);
                currentVertex = triMesh.point(holes[i][j]);
                nextVertex = triMesh.point(holes[i][j + 1]);
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
            double frontAngle = (radian * 180) / PI;
            cout << "frontAngle : " << frontAngle << endl;
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

            cout << "sum vectorAngle : " << vectorAngle << endl;
            if (vectorAngle < 180)
            { 
                frontAngle = 360 - frontAngle; 
            }
            else
            { 
                frontAngle = frontAngle; 
            }
            cout << "Vertex Idx : " << holes[i][j] << " / front Angle : " << frontAngle << endl; 

            min = (min > frontAngle) ? frontAngle : min;
            if (min == frontAngle)
            {
                minAngle = min;
                minIdx = holes[i][j];
            }  

            vectorAngle = 0.0; 
        } 
        cout << "Min Angle Vertex Idx : " << minIdx << " / Min Angle : " << minAngle << endl;
        vtkNew<vtkSphereSource> sphereSource;
        sphereSource->SetCenter(triMesh.point(minIdx).data());
        sphereSource->SetRadius(0.1);
        // Make the surface smooth.
        sphereSource->SetPhiResolution(100);
        sphereSource->SetThetaResolution(100);
        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputConnection(sphereSource->GetOutputPort());
        vtkNew<vtkActor> mActor;
        mActor->SetMapper(mapper);
        mActor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
        ui->openGLWidget->AddActor(mActor);
    } 
    //////////////////////
// 한 버텍스에서 연결된 모든 엣지를 이용하여 내적을 구한 다음 다 더해서 360에서 빼기    끝
// //////////////////

    


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
