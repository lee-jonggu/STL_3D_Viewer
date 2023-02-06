#include "stlviewer.h"
#include "ui_stlviewer.h"
#include "CustomInteractorStyle.h"
#include <QDebug>
#include <QDebug>
#include <TriMesh.h>
#include <algorithm> 


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

    vtkSmartPointer<vtkPolyData> mPolyData = vtkPolyDataMapper::SafeDownCast(mActor->GetMapper())->GetInput();
    TriMesh triMesh = convertToMesh(mPolyData);

    CleanBoundary(triMesh);

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

        MakeMesh(holes, centerVertex, triMesh);

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

    //// hole boundary vertex 
    //vtkNew<vtkSphereSource> sphereSource2;
    //sphereSource2->SetCenter(triMesh.point(OpenMesh::VertexHandle(holes[0][1])).data());
    //sphereSource2->SetRadius(0.2);
    //// Make the surface smooth.
    //sphereSource2->SetPhiResolution(100);
    //sphereSource2->SetThetaResolution(100);                                                      

    //vtkNew<vtkPolyDataMapper> mapper2;                                                           
    //mapper2->SetInputConnection(sphereSource2->GetOutputPort());
    //vtkNew<vtkActor> mActor2;                                                                   
    //mActor2->SetMapper(mapper2);
    //mActor2->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());
    //ui->openGLWidget->AddActor(mActor2);

    //// hole boundary vertex 
    //vtkNew<vtkSphereSource> sphereSource3;
    //sphereSource3->SetCenter(triMesh.point(OpenMesh::VertexHandle(holes[0][2])).data());
    //sphereSource3->SetRadius(0.2);
    //// Make the surface smooth.
    //sphereSource3->SetPhiResolution(100);
    //sphereSource3->SetThetaResolution(100);                                                     

    //vtkNew<vtkPolyDataMapper> mapper3;                                                        
    //mapper3->SetInputConnection(sphereSource3->GetOutputPort());
    //vtkNew<vtkActor> mActor3;                                                                   
    //mActor3->SetMapper(mapper3);
    //mActor3->GetProperty()->SetColor(colors->GetColor3d("Pink").GetData());
    //ui->openGLWidget->AddActor(mActor3);
}

void STLViewer::CleanBoundary(TriMesh& triMesh)
{
    
    int num = 0;
    std::set<TriMesh::FaceHandle> edgeFace;
     
    for (TriMesh::FaceIter f_it = triMesh.faces_begin(); f_it != triMesh.faces_end(); ++f_it)
    {      
        if (triMesh.is_boundary(*f_it))
        {
            for (TriMesh::FaceHalfedgeIter fhe_it = triMesh.fh_iter(*f_it); fhe_it.is_valid(); ++fhe_it)
            {
                TriMesh::FaceHandle fh = f_it.handle();
                TriMesh::HalfedgeHandle heh = *fhe_it;
                cout << "triMesh.is_boundary(*fh_it) : " << triMesh.is_boundary(*fhe_it) << endl;
                if (triMesh.is_boundary(*fhe_it))
                {
                    cout << "triMesh.is_boundary(fh_it) : " << triMesh.is_boundary(*fhe_it) << endl;
                }
                //if (triMesh.is_boundary(heh))
                //{
                //    num++;
                //}
                //if (num > 1)
                //{
                //    cout << "fh : " << fh << endl;
                //    num = 0;
                //    edgeFace.insert(fh);
                //}
            }
        }
    } 

    // delete face
    for (TriMesh::FaceHandle i : edgeFace)
    {
        triMesh.delete_face(i);
    }
    triMesh.garbage_collection();
    edgeFace.clear();

    vtkSmartPointer<vtkPolyData> meshToPoly = convertToPolyData(triMesh);
    vtkPolyDataMapper::SafeDownCast(mActor->GetMapper())->SetInputData(meshToPoly);
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
