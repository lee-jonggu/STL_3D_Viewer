#include "stlviewer.h"
#include "./ui_stlviewer.h"
#include <QDebug>
#include <QFileDialog>
#include <QColorDialog>
#include <vtkPointData.h>



STLViewer::STLViewer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::STLViewer)
{
    ui->setupUi(this);

    mColorDialog = new QColorDialog(this);


    connect(ui->actionOpen, SIGNAL(triggered(bool)), this, SLOT(ClickedOpen(bool)));  
    connect(ui->actionColor,&QAction::triggered, this, [this](bool) { mColorDialog->show(); });
    connect(mColorDialog, SIGNAL(currentColorChanged(QColor)), this, SLOT(SetColor(QColor)));  
    connect(ui->opacitySlider, SIGNAL(valueChanged(int)), this, SLOT(SetOpacity(int)));
    connect(ui->actionHole_Filling, SIGNAL(triggered(bool)), this, SLOT(HoleFilling(bool)));
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
    vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
    polyData->DeepCopy(mSTLReader->GetOutput()); 
    vtkSmartPointer<vtkPolyDataMapper> mMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mMapper->SetInputData(polyData);

    // Actor
    mActor = vtkSmartPointer<vtkActor>::New(); 
    mActor->SetMapper(mMapper); 

    ui->openGLWidget->AddActor(mActor);
    ui->openGLWidget->GetPolyData(polyData);
    qDebug() << "Actor" << mActor;
    qDebug() << "PolyData" << polyData; 

    ui->openGLWidget->GetInteractor()->GetInteractorStyle()->GetCurrentRenderer()->ResetCamera();
    ui->openGLWidget->GetRenderWindow()->Render();
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

void STLViewer::HoleFilling(bool)
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
                //cout << "start vertex : " << hole[hole.size() - 2];
                //cout << " / end vertex : " << hole.back() << endl;
            }
        }
    }

    //Print the vertex indices of the holes
    OpenMesh::Vec3d points;
    OpenMesh::Vec3d centerVertex;
    for (int i = 0; i < holes.size(); ++i)
    {
        std::cout << "Hole " << i << ": " << endl;
        for (int j = 0; j < holes[i].size(); ++j)
        {
            std::cout << holes[i][j].idx() << " ";
            //cout << triMesh.point(OpenMesh::VertexHandle(holes[i][j])) << endl;
            points += triMesh.point(OpenMesh::VertexHandle(holes[i][j]));

            cout << "start vertex : " << holes[i][j].idx();
            cout << " / end vertex : " << holes[i][j+1].idx() << endl;

            if (j == holes[i].size() - 1)
            {
                cout << "start vertex : " << holes[i][j].idx();
                cout << " / end vertex1 : " << holes[i][0].idx() << endl; 
            }
        } 
        std::cout << std::endl; 

        centerVertex = points / holes[i].size();
        cout << "centerVertex : " << centerVertex;
        vtkNew<vtkNamedColors> colors;
        vtkNew<vtkSphereSource> sphereSource2;
        sphereSource2->SetCenter(centerVertex[0], centerVertex[1], centerVertex[2]);
        sphereSource2->SetRadius(0.1);
        // Make the surface smooth.
        sphereSource2->SetPhiResolution(100);
        sphereSource2->SetThetaResolution(100);                                                      // ���� ��ǥ���� �� ����

        vtkNew<vtkPolyDataMapper> mapper2;                                                           // ���ۿ� �� ����
        mapper2->SetInputConnection(sphereSource2->GetOutputPort());
        vtkNew<vtkActor> mActor2;                                                                    // ���Ϳ��� �� ����
        mActor2->SetMapper(mapper2);
        mActor2->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
        ui->openGLWidget->AddActor(mActor2);
    }

    // hole boundary vertex
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

    cout << endl << "boundaryVertex" << boundaryVertex[0] << " /// " << boundaryVertex[1] << endl;

    // hole boundary vertex 
    vtkNew<vtkSphereSource> sphereSource2;
    sphereSource2->SetCenter(triMesh.point(OpenMesh::VertexHandle(holes[0][1])).data()); 
    sphereSource2->SetRadius(0.2);
    // Make the surface smooth.
    sphereSource2->SetPhiResolution(100);
    sphereSource2->SetThetaResolution(100);                                                      // ���� ��ǥ���� �� ����

    vtkNew<vtkPolyDataMapper> mapper2;                                                           // ���ۿ� �� ����
    mapper2->SetInputConnection(sphereSource2->GetOutputPort());
    vtkNew<vtkActor> mActor2;                                                                    // ���Ϳ��� �� ����
    mActor2->SetMapper(mapper2);
    mActor2->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());
    ui->openGLWidget->AddActor(mActor2);

    // hole boundary vertex 
    vtkNew<vtkSphereSource> sphereSource3;
    sphereSource3->SetCenter(triMesh.point(OpenMesh::VertexHandle(holes[0][2])).data());
    sphereSource3->SetRadius(0.2);
    // Make the surface smooth.
    sphereSource3->SetPhiResolution(100);
    sphereSource3->SetThetaResolution(100);                                                      // ���� ��ǥ���� �� ����

    vtkNew<vtkPolyDataMapper> mapper3;                                                           // ���ۿ� �� ����
    mapper3->SetInputConnection(sphereSource3->GetOutputPort());
    vtkNew<vtkActor> mActor3;                                                                    // ���Ϳ��� �� ����
    mActor3->SetMapper(mapper3);
    mActor3->GetProperty()->SetColor(colors->GetColor3d("Pink").GetData());
    ui->openGLWidget->AddActor(mActor3);


    /*
    Mesh Hole Filling 
    */

    //triMesh = MakeMesh(holes, centerVertex, triMesh);
    OpenMesh::VertexHandle vertexHandle2 = triMesh.add_vertex(OpenMesh::Vec3d(centerVertex[0], centerVertex[1], centerVertex[2]));
    for (int i = 0; i < holes.size(); ++i)
    {
        for (int j = 0; j < holes[i].size(); ++j)
        {
            OpenMesh::VertexHandle vertexHandle0(holes[i][j]);
            OpenMesh::VertexHandle vertexHandle1(holes[i][j + 1]); 
            if (j == holes[i].size() - 1)
            {
                OpenMesh::VertexHandle vertexHandle1(holes[i][0]);
                triMesh.add_face({ vertexHandle0, vertexHandle1, vertexHandle2 });
                vtkSmartPointer<vtkPolyData> meshToPoly = convertToPolyData(triMesh);
                vtkPolyDataMapper::SafeDownCast(mActor->GetMapper())->SetInputData(meshToPoly);
            }

            triMesh.add_face({ vertexHandle0, vertexHandle1, vertexHandle2 });
        }
        vtkSmartPointer<vtkPolyData> meshToPoly = convertToPolyData(triMesh);
        vtkPolyDataMapper::SafeDownCast(mActor->GetMapper())->SetInputData(meshToPoly);
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

TriMesh STLViewer::MakeMesh(std::vector<std::vector<TriMesh::VertexHandle> > holes, OpenMesh::Vec3d centerVertex, TriMesh& triMesh)
{
    OpenMesh::VertexHandle vertexHandle2 = triMesh.add_vertex(OpenMesh::Vec3d(centerVertex[0], centerVertex[1], centerVertex[2]));
    for (int i = 0; i < holes.size(); ++i)
    {
        for (int j = 0; j < holes[i].size(); ++j)
        {
            OpenMesh::VertexHandle vertexHandle0(holes[i][j]);
            OpenMesh::VertexHandle vertexHandle1(holes[i][j+1]);
            vertexHandle2;
            if (j == holes[i].size() - 1)
            {
                OpenMesh::VertexHandle vertexHandle1(holes[i][0]);
            }

            triMesh.add_face({ vertexHandle0, vertexHandle1, vertexHandle2 });
        }
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