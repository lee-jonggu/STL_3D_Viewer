#include "CustomInteractorStyle.h"
#include <QDebug>
#include <TriMesh.h>
#include <algorithm>

CustomInteractorStyle::CustomInteractorStyle()
{
    mVertex = vtkSmartPointer<vtkPoints>::New(); 
    mVertexCount = 0;
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

        int cellId = cellPicker->GetCellId();
        qDebug() << "cell get points ID 0 : " << polyData->GetCell(cellId)->GetPointIds()->GetPointer(0);
        qDebug() << "cell get points ID 1 : " << polyData->GetCell(cellId)->GetPointIds()->GetPointer(1);
        qDebug() << "cell get points ID 2 : " << polyData->GetCell(cellId)->GetPointIds()->GetPointer(2);

        // for�� �̿��ؼ� �� vertex�� ���Ѵ��� start vertex, end vertex�� ����. ���ͽ�Ʈ�� �̿��ؼ� �� �׸��� 
        //for (TriMesh::FaceVertexIter fv_iter = triMesh.fv_begin(TriMesh::FaceHandle(cellPicker->GetCellId()));
        //    fv_iter > triMesh.fv_end(TriMesh::FaceHandle(cellPicker->GetCellId())); fv_iter++)        

        // nearest vertex on cell
        double min = 100;
        OpenMesh::Vec3d minPoint;
        OpenMesh::Vec3d dijkstraVec;
        for (TriMesh::FaceVertexIter fv_iter = triMesh.fv_begin(TriMesh::FaceHandle(cellPicker->GetCellId()));
            fv_iter.is_valid(); fv_iter++)
        {
            OpenMesh::Vec3d point = triMesh.point(fv_iter);         // point ��ȯ , ������ǥ���� ���� ����� vertex �Ÿ� ���
            OpenMesh::Vec3d diff = point - pickingPosition;         // 
            double distance = diff.length();                               // �Ÿ��� ���� ���� vertex�� ���ؾ��Ѵ�. 

            min = (min > distance) ? distance : min;
            if (min == distance)
            {
                minPoint = point;
            }
        }
        mVertex->InsertNextPoint(minPoint[0], minPoint[1], minPoint[2]);



        // Create a sphere
        vtkNew<vtkSphereSource> vertexSphereSource;
        vertexSphereSource->SetCenter(minPoint[0], minPoint[1], minPoint[2]);
        vertexSphereSource->SetRadius(0.1);
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

        vtkSmartPointer<vtkPolyData> meshToPoly = convertToPolyData(triMesh);
        vtkPolyDataMapper::SafeDownCast(cellPicker->GetActor()->GetMapper())->SetInputData(meshToPoly);
        vtkPolyDataMapper::SafeDownCast(cellPicker->GetActor()->GetMapper())->Modified();

        // vertex Points
        //mObserver->vertexPoints(mVertex, mVertex->GetNumberOfPoints());

        //dijkstraVec = { minPoint[0], minPoint[1], minPoint[2] };
        //double* coords = dijkstraVec.data();
        //qDebug() <<"coodrs : " << coords[0] << coords[1] << coords[2]; 

        int mVertexNumber = mVertex->GetNumberOfPoints();
        qDebug() << "Number of mVertex : " << mVertexNumber;

        startVertex = { mVertex->GetPoint(0)[0], mVertex->GetPoint(0)[1], mVertex->GetPoint(0)[2] };
        endVertex = { mVertex->GetPoint(1)[0], mVertex->GetPoint(1)[1], mVertex->GetPoint(1)[2] };
        qDebug() << "startVertex" << startVertex[0] << startVertex[1] << startVertex[2];
        qDebug() << "endVertex" << endVertex[0] << endVertex[1] << endVertex[2];

        if (mVertexNumber == 1)
        {
            // !!! ues queue !!!       


            //startVertex = endVertex;
            //endVertex = { mVertex->GetPoint(i + 1)[0], mVertex->GetPoint(1)[1], mVertex->GetPoint(i + 1)[2] };


            qDebug() << "** Vertex Information **";
            for (TriMesh::VertexIter v_it = triMesh.vertices_begin(); v_it != triMesh.vertices_end(); ++v_it)
            {
                TriMesh::VertexHandle vh(*v_it);
                //vertexHandleVector.push_back(vh);

                OpenMesh::Vec3d from = triMesh.point(*v_it);
                double* coords = from.data();

                int vid = vh.idx();

                // Print neighbor vertex indices.   
                if (coords[0] == startVertex[0] && coords[1] == startVertex[1] && coords[2] == startVertex[2])
                { 
                    for (TriMesh::VertexVertexIter vv_it = triMesh.vv_begin(vh); vv_it != triMesh.vv_end(vh); ++vv_it)
                        //for(TriMesh::VertexVertexIter vv_it = triMesh.vv_begin(vertexHandleVector))
                    {
                        TriMesh::VertexHandle n_vh(*vv_it);
                        int n_vid = n_vh.idx();
                        vertexId.push_back(n_vid);     
                        mAllVertex.push_back(n_vid);
                        mCalVertex.push_back(n_vid);
                        mVertexCount++;

                        OpenMesh::Vec3d negihborVertex = triMesh.point(*vv_it);
                        double* nCoords = negihborVertex.data(); 
                    }
                    qDebug() << vertexId; 
                }
            }
        }
        if (mVertexNumber > 1)
        {
            //flag = true;
            //while (flag)
            //{
                qDebug() << "mCalVertex : " << mCalVertex.size();
                for (int i = 0; i < mCalVertex.size(); i++)
                {  
                    TriMesh::VertexHandle neighbor_vh(mCalVertex[i]);
                    for (TriMesh::VertexVertexIter vv_it = triMesh.vv_begin(neighbor_vh); vv_it != triMesh.vv_end(neighbor_vh); ++vv_it)
                        //for(TriMesh::VertexVertexIter vv_it = triMesh.vv_begin(vertexHandleVector))
                    {
                        TriMesh::VertexHandle n_vh(*vv_it);
                        int n_vid = n_vh.idx();
                        mAllVertex.push_back(n_vid); 
                        mCalVertex.push_back(n_vid);
                        //mVertexCount++;
                        
                        negihborVertex = triMesh.point(*vv_it);
                        double* nCoords = negihborVertex.data(); 

                        // Create a sphere
                        vtkNew<vtkSphereSource> neighborSphereSource;
                        neighborSphereSource->SetCenter(nCoords[0], nCoords[1], nCoords[2]);
                        neighborSphereSource->SetRadius(0.05);
                        // Make the surface smooth.
                        neighborSphereSource->SetPhiResolution(100);
                        neighborSphereSource->SetThetaResolution(100);

                        vtkNew<vtkPolyDataMapper> neighborVertexMapper;
                        neighborVertexMapper->SetInputConnection(neighborSphereSource->GetOutputPort());
                        vtkNew<vtkActor> mNeighborVertexActor;
                        mNeighborVertexActor->SetMapper(neighborVertexMapper);
                        mNeighborVertexActor->GetProperty()->SetColor(colors->GetColor3d("HotPink").GetData());

                        mObserver->func(mNeighborVertexActor);
                        if (negihborVertex == endVertex) {  
                            i = mCalVertex.size();
                            break; 
                        }
                    }
                    //flag = false;  
                }
                //for (int i = 0; i < /*mVertexCount - vertexId.size()*/mCalVertex.size(); i++)
                //{
                //    qDebug() << "mVertexCount - vertexId.size();" << mCalVertex.size();
                //    mCalVertex.erase(mCalVertex.begin());
                //}
                //qDebug() << "!!!!";
                //mVertexCount = 0;
                //vertexId.clear();
            //}  
            
        }
        qDebug() << vertexId;
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