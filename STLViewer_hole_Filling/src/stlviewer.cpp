#include "stlviewer.h"
#include "./ui_stlviewer.h"
#include <QDebug>
#include <QFileDialog>
#include <QColorDialog>
#include <vtkPointData.h> 

STLViewer::STLViewer(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::STLViewer)
{
    ui->setupUi(this);

    mColorDialog = new QColorDialog(this);

    mSlider = new QSlider(Qt::Horizontal, this);
    mSlider->setRange(0, 100);
    
    connect(ui->actionOpen, SIGNAL(triggered(bool)), this, SLOT(ClickedOpen(bool)));
    connect(ui->actionColor, &QAction::triggered, this, [this](bool) { mColorDialog->show(); });
    connect(mColorDialog, SIGNAL(currentColorChanged(QColor)), this, SLOT(SetColor(QColor)));
    connect(mSlider, SIGNAL(valueChanged(int)), this, SLOT(SetOpacity(int))); 

    mHoleFilling = new QPushButton("Hole Filling", this);
    mHLayout = new QHBoxLayout();
    mWidget = new QWidget(this);

    mHLayout->addWidget(mHoleFilling);
    mWidget->setLayout(mHLayout);

    connect(mHoleFilling, SIGNAL(clicked()), this, SLOT(HoleFilling()));

}

STLViewer::~STLViewer()
{
    delete ui;
}

void STLViewer::ClickedOpen(bool)
{
    qDebug("Triggered Open Button");
    std::string fileName = QFileDialog::getOpenFileName(nullptr,
        "STL Files", "/home", "STL Files (*.stl)").toStdString().c_str();
    
    // STL Reader
    vtkSmartPointer<vtkSTLReader> mSTLReader = vtkSmartPointer<vtkSTLReader>::New();
    mSTLReader->SetFileName(fileName.c_str());
    mSTLReader->Update();

    // Mapper
    mPolyData = vtkSmartPointer<vtkPolyData>::New();
    mPolyData->DeepCopy(mSTLReader->GetOutput());
    vtkSmartPointer<vtkPolyDataMapper> mMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mMapper->SetInputData(mPolyData);

    // Actor
    mActor = vtkSmartPointer<vtkActor>::New(); 
    mActor->SetMapper(mMapper); 

    ui->openGLWidget->AddActor(mActor); 
    qDebug() << "Actor" << mActor;
    qDebug() << "PolyData" << mPolyData;
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
        mActor->GetProperty()->SetOpacity(opacity/100.0);
        mActor->Modified();
        ui->openGLWidget->GetRenderWindow()->Render();
    }
} 

void STLViewer::HoleFilling()
{
    if (mActor == nullptr) return;
    
    vtkSmartPointer<vtkPolyData> mPolyData = vtkPolyDataMapper::SafeDownCast(mActor->GetMapper())->GetInput();
    TriMesh triMesh = convertToMesh(mPolyData); 
    
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

     //Print the vertex indices of the holes
    for (int i = 0; i < holes.size(); ++i)
    {
        std::cout << "Hole " << i << ": ";
        for (int j = 0; j < holes[i].size(); ++j)
        {
            std::cout << holes[i][j].idx() << " ";
        }
        std::cout << std::endl;
    }

    vtkNew<vtkNamedColors> colors;
    for (int vertexIdx : boundaryVertex)
    {
        vtkNew<vtkSphereSource> sphereSource2;
        sphereSource2->SetCenter(triMesh.point(OpenMesh::VertexHandle(vertexIdx)).data());
        sphereSource2->SetRadius(0.1);
        // Make the surface smooth.
        sphereSource2->SetPhiResolution(100);
        sphereSource2->SetThetaResolution(100);                                                      // ���� ��ǥ���� �� ����

        vtkNew<vtkPolyDataMapper> mapper2;                                                           // ���ۿ� �� ����
        mapper2->SetInputConnection(sphereSource2->GetOutputPort());
        vtkNew<vtkActor> mActor2;                                                                    // ���Ϳ��� �� ����
        mActor2->SetMapper(mapper2);
        mActor2->GetProperty()->SetColor(colors->GetColor3d("Yellow").GetData());
        ui->openGLWidget->AddActor(mActor2);
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

void STLViewer::resizeEvent(QResizeEvent* event)
{

    int h = event->size().height();
    int w = event->size().width(); 
    //m_slider->maximumSize();
    mSlider->setGeometry(11,h - 200, 200, 50);  
    mWidget->setGeometry(w/2, 50, 200, 100);
}

