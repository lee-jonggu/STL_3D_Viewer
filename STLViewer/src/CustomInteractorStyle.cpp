#include "CustomInteractorStyle.h"
#include <QDebug>
//#include <TriMesh.h>
#include <algorithm> 
#include <time.h>


CustomInteractorStyle::CustomInteractorStyle()
{

}

CustomInteractorStyle::~CustomInteractorStyle()
{

}

void CustomInteractorStyle::OnRightButtonDown()
{
    __super::OnLeftButtonDown();
}

void CustomInteractorStyle::OnRightButtonUp()
{
    __super::OnLeftButtonUp();
}

void CustomInteractorStyle::OnLeftButtonDown()
{
    clock_t start, finish;
    double duration;
    start = clock();
    int* pos = GetInteractor()->GetEventPosition();

    vtkSmartPointer<vtkCellPicker>cellPicker = vtkSmartPointer<vtkCellPicker>::New();
    cellPicker->SetTolerance(0.00001);

    // Pick from this location
    cellPicker->Pick(pos[0], pos[1], pos[2], this->GetCurrentRenderer());

    double* worldPosition = cellPicker->GetPickPosition();
    qDebug() << "Cell id is : " << cellPicker->GetCellId();

    if (cellPicker->GetCellId() != -1)
    {
        qDebug() << "Pick Position is : (" << worldPosition[0] << ", " << worldPosition[1] << ", " << worldPosition[2] << ")";

        vtkNew<vtkNamedColors> colors;

        // Create a sphere
        vtkNew<vtkSphereSource> sphereSource;
        sphereSource->SetCenter(worldPosition[0], worldPosition[1], worldPosition[2]);
        sphereSource->SetRadius(0.1);
        // Make the surface smooth.
        sphereSource->SetPhiResolution(100);
        sphereSource->SetThetaResolution(100);

        OpenMesh::Vec3d pickingPosition(worldPosition[0], worldPosition[1], worldPosition[2]);

        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputConnection(sphereSource->GetOutputPort());
        vtkNew<vtkActor> mActor;
        mActor->SetMapper(mapper);
        mActor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());

        mObserver->func(mActor);

        vtkSmartPointer<vtkPolyData> polyData = vtkPolyDataMapper::SafeDownCast(cellPicker->GetActor()->GetMapper())->GetInput();
        TriMesh triMesh = convertToMesh(polyData);

        //int cellId = cellPicker->GetCellId();
        //qDebug() << "cell get points ID 0 : " << polyData->GetCell(cellId)->GetPointIds()->GetPointer(0);
        //qDebug() << "cell get points ID 1 : " << polyData->GetCell(cellId)->GetPointIds()->GetPointer(1);
        //qDebug() << "cell get points ID 2 : " << polyData->GetCell(cellId)->GetPointIds()->GetPointer(2); 

        double min = 100;
        OpenMesh::Vec3d minPoint;
        OpenMesh::VertexHandle dijkstraVertexHandle;
        for (TriMesh::FaceVertexIter fv_iter = triMesh.fv_begin(TriMesh::FaceHandle(cellPicker->GetCellId()));
            fv_iter.is_valid(); fv_iter++)
        {
            OpenMesh::Vec3d point = triMesh.point(fv_iter);         // point 반환 , 월드좌표에서 가장 가까운 vertex 거리 출력
            OpenMesh::Vec3d diff = point - pickingPosition;         // 
            double distance = diff.length();                               // 거리가 가장 적은 vertex를 구해야한다. 

            min = (min > distance) ? distance : min;
            if (min == distance)
            {
                minPoint = point;
                dijkstraVertexHandle = fv_iter.handle();
            }
        }
        dijkstraVertexIdx.push_back(dijkstraVertexHandle.idx());
        //triMesh.is_boundary();

        // Create a sphere
        vtkNew<vtkSphereSource> vertexSphereSource;
        vertexSphereSource->SetCenter(minPoint[0], minPoint[1], minPoint[2]);
        vertexSphereSource->SetRadius(0.25);
        // Make the surface smooth.
        vertexSphereSource->SetPhiResolution(100);
        vertexSphereSource->SetThetaResolution(100);

        vtkNew<vtkPolyDataMapper> vertexMapper;
        vertexMapper->SetInputConnection(vertexSphereSource->GetOutputPort());
        vtkNew<vtkActor> mVertexActor;
        mVertexActor->SetMapper(vertexMapper);
        mVertexActor->GetProperty()->SetColor(colors->GetColor3d("Blue").GetData());

        mObserver->func(mVertexActor);

        // delete vertex
        //for (int i = 0; i < triMesh.n_vertices() / 2; i++)
        //{
        //    triMesh.delete_vertex(OpenMesh::VertexHandle(i));
        //}

        // delete face
        //triMesh.delete_face(OpenMesh::FaceHandle(cellPicker->GetCellId()));
        //qDebug() << "delete Cell ID : " << cellPicker->GetCellId(); 
        //triMesh.garbage_collection();

        finish = clock();
        duration = (double)(finish - start) / CLOCKS_PER_SEC;
        cout << "duration : " << duration;

        if (dijkstraVertexIdx.size() > 1)
        {
            start = clock();
            //std::vector<int> dijkstarPath = dijkstra(dijkstraVertexIdx[dijkstraVertexIdx.size() - 2], dijkstraVertexIdx.back(), triMesh);
            std::vector<int> dijkstarPath = dijkstra(dijkstraVertexIdx[dijkstraVertexIdx.size() - 2], dijkstraVertexIdx.back(), triMesh);

            for (int vertexIdx : dijkstarPath)
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

                mObserver->func(mActor2);
            }
            finish = clock();
            duration = (double)(finish - start) / CLOCKS_PER_SEC;
            cout << "duration1 : " << duration;
        }
        //vtkSmartPointer<vtkPolyData> meshToPoly = convertToPolyData(triMesh);
        //vtkPolyDataMapper::SafeDownCast(cellPicker->GetActor()->GetMapper())->SetInputData(meshToPoly);
        //vtkPolyDataMapper::SafeDownCast(cellPicker->GetActor()->GetMapper())->Modified();
    }

}


void CustomInteractorStyle::OnLeftButtonUp()
{

}

void CustomInteractorStyle::OnMouseWheelForward()
{
    __super::OnMouseWheelForward();
}

void CustomInteractorStyle::OnMouseWheelBackward()
{
    __super::OnMouseWheelBackward();
}

void CustomInteractorStyle::GetPolyData(vtkSmartPointer<vtkPolyData> polyData)
{
    mPolyData = polyData;
}

void CustomInteractorStyle::GetSphere(vtkSmartPointer<vtkSphereSource> sphereSource)
{
    mSphere = sphereSource;
}

void CustomInteractorStyle::GetActor(vtkSmartPointer<vtkActor> sphereSource)
{
    mActor = sphereSource;
}

void CustomInteractorStyle::addObserver(Observer* observer)
{
    mObserver = observer;
}

TriMesh CustomInteractorStyle::convertToMesh(vtkSmartPointer<vtkPolyData> polyData)
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

vtkSmartPointer<vtkPolyData> CustomInteractorStyle::convertToPolyData(TriMesh triMesh)
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
    qDebug("Completed Conver To polyData");
    return polyData;
}

std::vector<int> CustomInteractorStyle::dijkstra(int startIdx, int endIdx, const TriMesh& triMesh)
{
    std::vector<int> dijkstraPath;

    std::vector<std::pair<double, int>> shortestPathMap(triMesh.n_vertices(), { std::numeric_limits<double>::max(), -1 });
    shortestPathMap[startIdx] = { 0.0, startIdx };
    std::priority_queue<std::pair<double, int>> pQueue;
    pQueue.push({ 0.0, startIdx });
    while (pQueue.size())
    {
        double cost = -pQueue.top().first;
        int currentIdx = pQueue.top().second;
        pQueue.pop();

        OpenMesh::VertexHandle currentVertexHandle(currentIdx);
        for (OpenMesh::VertexHandle neighbor : triMesh.vv_range(currentVertexHandle))
        {
            double distance = (triMesh.point(currentVertexHandle) - triMesh.point(neighbor)).length();
            double nextCost = cost + distance;
            if (shortestPathMap[neighbor.idx()].first > nextCost)
            {
                shortestPathMap[neighbor.idx()].first = nextCost;
                shortestPathMap[neighbor.idx()].second = currentVertexHandle.idx();
                pQueue.push({ -nextCost, neighbor.idx() });
            }
        }
    }

    while (true)
    {
        dijkstraPath.push_back(endIdx);
        endIdx = shortestPathMap[endIdx].second;
        if (startIdx == endIdx)
        {
            dijkstraPath.push_back(endIdx);
            break;
        }
    }
    return dijkstraPath;
}