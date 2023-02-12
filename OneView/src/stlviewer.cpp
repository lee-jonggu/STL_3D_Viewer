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
    vtkNew<vtkNamedColors> colors;

    vtkSmartPointer<vtkPolyData> mPolyData = vtkPolyDataMapper::SafeDownCast(mActor->GetMapper())->GetInput();
    TriMesh triMesh = convertToMesh(mPolyData); 

    // Find Holes
    std::vector<std::vector<TriMesh::VertexHandle> > holes;
    holes = FindHoleVertex(triMesh);

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

        // Connect the hole vertices based on the center point of the hole mesh
        //MakeMesh(holes, centerVertex, triMesh); 
    }
     

    // Print Hole Vertex
    for (int i = 0; i < holes.size(); i++)
    { 
        cout << "holes " << i << ": ";
        for (int j = 0; j < holes[i].size(); j++)
        { 
            cout <<  holes[i][j] << ", ";  
        }
        cout << endl;
    }
    AdvancingFrontMethod(holes, triMesh);

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

void STLViewer::AdvancingFrontMethod(std::vector<std::vector<TriMesh::VertexHandle>> holes, TriMesh& triMesh)
{
    vtkNew<vtkNamedColors> colors;
    for (int i = 0; i < holes.size(); i++)
    {
        std::vector<std::pair<int, int>> linkedVertex(triMesh.n_vertices(), { -1, -1 });
        std::unordered_map<int, OpenMesh::Vec3d> vertexNormalMap;
        cout << "linkedVertex.size()" << linkedVertex.size() << endl;
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
                cout << "holes[i][j].idx() : " << holes[i][j].idx() << " / " << linkedVertex[holes[i][j].idx()].first << " / " << linkedVertex[holes[i][j].idx()].second << endl;
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
                    normal += firstVec.cross(secondVec);
                } 
            }

            //cout << "sum vectorAngle : " << vectorAngle << endl;
            if (vectorAngle < 180)
            {
                frontAngle = 360 - frontAngle;
            }
            else
            {
                frontAngle = frontAngle;
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

        int cnt = 0;
        for (OpenMesh::VertexHandle neighbor : triMesh.vv_range(minIdx))
        {
            cnt++;
        } 
        normal /= cnt - 1;
        vertexNormalMap.insert({ minIdx.idx(), normal }); 
        
        cout << "Min Angle Vertex Idx : " << minIdx << " / Min Angle : " << minAngle << endl;

        cout << "minAngle : " << minAngle << " / thresholdA : " << thresholdA << endl;
        /* 
        각도에 따른 메쉬 홀 필링
        */
        if (minAngle <= thresholdA)  // 0 < angle <= 85
        { 
            OpenMesh::VertexHandle vertexHandle0(linkedVertex[minIdx.idx()].first); 
            OpenMesh::VertexHandle vertexHandle1(minIdx); 
            OpenMesh::VertexHandle vertexHandle2(linkedVertex[minIdx.idx()].second);
            cout << "vertexHandle : " << vertexHandle0 << ", " << vertexHandle1  << ", " << vertexHandle2 << endl;
            triMesh.add_face({ vertexHandle0, vertexHandle1, vertexHandle2 });
            vtkSmartPointer<vtkPolyData> meshToPoly = convertToPolyData(triMesh);
            vtkPolyDataMapper::SafeDownCast(mActor->GetMapper())->SetInputData(meshToPoly);
            triMesh.add_face({ vertexHandle0, vertexHandle1, vertexHandle2 });
            linkedVertex.clear();
        }
        else if (minAngle > thresholdA && minAngle <= thresholdB) // 85 < angle < 135
        {
            cout << "thresholdB minIdx : " << minIdx << "minAngle : " << minAngle << endl;
            
            OpenMesh::VertexHandle prevVertexHandle(linkedVertex[minIdx.idx()].first);
            OpenMesh::VertexHandle currentVertexHandle(minIdx);
            OpenMesh::VertexHandle nextVertexHandle(linkedVertex[minIdx.idx()].second);

            OpenMesh::Vec3d firstVector;
            OpenMesh::Vec3d secondVector;

            firstVector = triMesh.point(prevVertexHandle) - triMesh.point(currentVertexHandle);
            secondVector = triMesh.point(nextVertexHandle) - triMesh.point(currentVertexHandle);

            OpenMesh::Vec3d firstUnitVector;
            OpenMesh::Vec3d secondUnitVector;

            //firstUnitVector = firstVector / sqrt((firstVector[0] * firstVector[0]) + (firstVector[1] * firstVector[1]) + (firstVector[2] * firstVector[2]));
            //secondUnitVector = secondVector / sqrt((secondVector[0] * secondVector[0]) + (secondVector[1] * secondVector[1]) + (secondVector[2] * secondVector[2]));
            cout << "3333333333333333333      " << sqrt((secondVector[0] * secondVector[0]) + (secondVector[1] * secondVector[1]) + (secondVector[2] * secondVector[2])) << endl;
            cout << "4444444444444444444      " << secondVector.dot(secondVector) << endl;
            firstUnitVector = firstVector / firstVector.dot(firstVector);
            secondUnitVector = secondVector / secondVector.dot(secondVector);

            OpenMesh::Vec3d sumVector;
            sumVector = firstUnitVector + secondUnitVector;

            OpenMesh::Vec3d bisectorVector; 
            bisectorVector = sumVector / sumVector.dot(sumVector);

            TriMesh::Normal vNormal = { 0.0,0.0,0.0 };
            vNormal = vertexNormalMap[minIdx.idx()]; 

            OpenMesh::Vec3d ppVector;
            OpenMesh::Vec3d pVector;
            ppVector = bisectorVector - ((bisectorVector * vNormal) * vNormal);
            pVector = ppVector / ppVector.dot(ppVector);

            OpenMesh::Vec3d pNormal;  
            OpenMesh::Vec3d nvVector; 
            nvVector = vNormal * bisectorVector;
            nvVector[0] = abs(nvVector[0]);
            nvVector[1] = abs(nvVector[1]);
            nvVector[2] = abs(nvVector[2]);
            pNormal = vNormal + alpha * nvVector * bisectorVector;
            pNormal = pNormal / pNormal.dot(pNormal);

            double nnTheta = 0.0;
            nnTheta = acos(vNormal.dot(pNormal));

            cout << "11111111111111      " << pNormal * pVector << endl << "2222222222222222222222   " << pNormal.dot(pVector) << endl;
            double k = (cos(nnTheta) - 1) / pNormal.dot(pVector);

            OpenMesh::Vec3d delta = { 0.0,0.0,0.0 };
            for (OpenMesh::VertexHandle neighbor : triMesh.vv_range(minIdx))
            {
                delta += triMesh.point(neighbor) - triMesh.point(minIdx);
            }
            //for (int j = 0; j < holes[i].size(); j++)
            //{ 
            //    delta += triMesh.point(holes[i][j]) - triMesh.point(minIdx); 
            //} 

            cout << "vNormal * delta : " << vNormal * delta << endl;
            bool checkCon; 
            if (vNormal.dot(delta) <= 0)
            {
                checkCon = true;            // convex (볼록)
            }
            else
            {
                checkCon = false;           // concave (오목)
            }

            if (checkCon == true)           // convex (볼록)
            {

            }
        }
        
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
