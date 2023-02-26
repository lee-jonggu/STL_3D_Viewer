#include "CustomInteractorStyle.h"
#include <QDebug>
#include <TriMesh.h>
#include <windows.h>

CustomInteractorStyle::CustomInteractorStyle()
{
	pathButtonStatus = false;
	SelectDeleteAll = false;
	NotFirst = false;
	CompleteCurve = false; 
	pickCount = 0;
	firstVertex = 0;
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
	int* pos = GetInteractor()->GetEventPosition();                                            // 현재 좌표 값
	vtkSmartPointer<vtkCellPicker>cellPicker = vtkSmartPointer<vtkCellPicker>::New();          // 셀피커 선언
	cellPicker->SetTolerance(0.00001);                                                         // 피킹할 면적을 지정?
	// Pick from this location
	cellPicker->Pick(pos[0], pos[1], 0, this->GetCurrentRenderer());                        // 셀피커 로컬좌표
	double* worldPosition = cellPicker->GetPickPosition();                                  // 셀피커 월드 좌표 

	if (cellPicker->GetCellId() != -1)
	{
		vtkNew<vtkNamedColors> colors;
		static OpenMesh::Vec3d firstVec;
		double min = 0;
		int minId = 0;
		int lastId = 0;
		bool notFirst = false;
		bool DeleteComplete = false;
		OpenMesh::Vec3d minVec;
		OpenMesh::Vec3d pickingPosition(worldPosition[0], worldPosition[1], worldPosition[2]); 

		// 피커의 액터에서 매퍼정보를 꺼내 다운캐스트해서 폴리데이터 꺼냄
		vtkSmartPointer<vtkPolyData> polyData = vtkPolyDataMapper::SafeDownCast(cellPicker->GetActor()->GetMapper())->GetInput();
		mTriMesh = convertToMesh(polyData);								// 폴리데이터 메쉬화

		if (pathButtonStatus == true)
		{
			for (TriMesh::FaceVertexIter fv_iter = mTriMesh.fv_begin(TriMesh::FaceHandle(cellPicker->GetCellId()));
				fv_iter != mTriMesh.fv_end(TriMesh::FaceHandle(cellPicker->GetCellId())); fv_iter++)
			{
				OpenMesh::Vec3d point = mTriMesh.point(fv_iter);         // point 반환 , 월드좌표에서 가장 가까운 vertex 거리 출력
				OpenMesh::Vec3d diff = point - pickingPosition;         // 첫번째 포인트와 피킹한 위치사이의 벡터
				double distance = diff.length();                        // 벡터의 길이, 거리가 가장 적은 vertex를 구해야한다.

				if (notFirst == false)
				{
					min = distance;
					minId = fv_iter->idx();
					minVec = point;
				}
				else if (notFirst == true)
				{
					if (min > distance)
					{
						minId = fv_iter->idx();
						minVec = point;
					}
				}
				notFirst = true;
			}															// 피킹한 지점에서 가까운 점 구하기
			lastId = cellPicker->GetCellId();
			vertexIdxs.push_back(minId);
			if (pickCount == 0)
				firstVec = minVec;
			pickCount++;												// 피킹한 횟수

			if (pickCount > 1)
			{
				std::vector<int> dijkstarPath = dijkstra(vertexIdxs[vertexIdxs.size() - 2], vertexIdxs.back(), mTriMesh);
				std::vector<int> autoConnection = dijkstra(vertexIdxs.back(), vertexIdxs[firstVertex], mTriMesh);
				for (int vertexIdx : dijkstarPath)
				{
					if (NotFirst == false)
						curve.push_back(vertexIdx);
					else
					{
						if (curve.back() != vertexIdx)
							curve.push_back(vertexIdx);
					}
					NotFirst = true;
				}
				if ((minVec - firstVec).length() < 2)
				{
					for (int vertexIdx : autoConnection)
					{
						if (curve[0] != vertexIdx && curve.back() != vertexIdx)
							curve.push_back(vertexIdx);
					}
					CompleteCurve = true;
					pathButtonStatus = false;
					pickCount = 0;
				}
				for (int vertexIdx : curve)
				{
					mObserver->lineActor(GetSphere("Yellow", mTriMesh.point(OpenMesh::VertexHandle(vertexIdx)).data()[0],
						mTriMesh.point(OpenMesh::VertexHandle(vertexIdx)).data()[1],
						mTriMesh.point(OpenMesh::VertexHandle(vertexIdx)).data()[2]));
				}
			} 
		}

		if (CompleteCurve == true && cellPicker->GetCellId() != lastId)									// 메쉬커팅
		{
			MeshCutting(mTriMesh, cellPicker->GetCellId());
			DeleteComplete = true;
		}
		mTriMesh.garbage_collection();                                               // 메쉬 업데이트
		convertToPolyData(mTriMesh);

		if (DeleteComplete == true)
		{
			firstVertex = vertexIdxs.size();
			vtkSmartPointer<vtkPolyData> meshToPoly = convertToPolyData(mTriMesh);       // 메쉬 데이터 폴리 데이터로 바꿈
			vtkPolyDataMapper::SafeDownCast(cellPicker->GetActor()->GetMapper())->SetInputData(meshToPoly); // 매퍼에 업데이트    
		}
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
	//mActor = sphereSource;
}

void CustomInteractorStyle::addObserver(Observer* observer)
{
	mObserver = observer;
}

TriMesh CustomInteractorStyle::convertToMesh(vtkSmartPointer<vtkPolyData> polyData)         // 폴리 데이터를 메쉬로 바꿈
{
	int nPoints = polyData->GetNumberOfPoints();                                            // 점의 개수
	int nCells = polyData->GetNumberOfCells();                                              // 면의 개수

	TriMesh triMesh;                                                                        // 메쉬 

	for (int vertexId = 0; vertexId < nPoints; ++vertexId)                                  // 모든 점의 개수 만큼
	{
		double* point = polyData->GetPoint(vertexId);                                       // 버텍스 아이디에 해당하는 점을 포인터에 저장
		triMesh.add_vertex(OpenMesh::Vec3d(point[0], point[1], point[2]));                  // 배열에 점의 벡터정보 추가
	}

	for (int cellId = 0; cellId < nCells; ++cellId)                                         // 모든 면의 개수 만큼
	{
		int vertexId0 = polyData->GetCell(cellId)->GetPointIds()->GetId(0);                 // 면의 버텍스 아이디 저장
		int vertexId1 = polyData->GetCell(cellId)->GetPointIds()->GetId(1);
		int vertexId2 = polyData->GetCell(cellId)->GetPointIds()->GetId(2);

		OpenMesh::VertexHandle vertexHandle0(vertexId0);                                    // 점을 다루는 핸들러
		OpenMesh::VertexHandle vertexHandle1(vertexId1);
		OpenMesh::VertexHandle vertexHandle2(vertexId2);
		triMesh.add_face({ vertexHandle0, vertexHandle1, vertexHandle2 });                  // 면의 정보를 배열에 담음
	}

	return triMesh;                         // 점 + 면 정보, (점의 벡터정보 + 면에 해당하는 점의 아이디)
}

vtkSmartPointer<vtkPolyData> CustomInteractorStyle::convertToPolyData(TriMesh triMesh)      // 메쉬데이터를 폴리 데이터로 바꾸는 함수
{
	vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
	vtkSmartPointer<vtkPoints>   points = vtkSmartPointer<vtkPoints>::New();
	int vid = 0;

	for (TriMesh::VertexIter vertexItr = triMesh.vertices_begin(); vertexItr != triMesh.vertices_end(); ++vertexItr, ++vid)
	{                               // 점의 개수 만큼
		OpenMesh::Vec3d from = triMesh.point(*vertexItr);                           // 모든 점의 벡터 저장
		double* coords = from.data();                                               // coords에 점의 좌표 저장

		while (vid < vertexItr->idx())                                              // 모든 점의 개수 만큼 point를 0으로 초기화
		{
			vid++;
			points->InsertNextPoint(0, 0, 0);
		}
		points->InsertNextPoint(coords[0], coords[1], coords[2]);                   // 모든 점에 x,y,z값 할당              
	}
	polyData->SetPoints(points);                                                    // 폴리데이터에 점 정보

	vtkSmartPointer<vtkCellArray> cellArray = vtkSmartPointer<vtkCellArray>::New();
	for (TriMesh::FaceIter faceItr = triMesh.faces_begin(); faceItr != triMesh.faces_end(); ++faceItr)          // 3번 돌음
	{
		TriMesh::FaceVertexIter   faceVertexItr;
		faceVertexItr = triMesh.cfv_iter(*faceItr);
		int   v0 = (faceVertexItr++).handle().idx();
		int   v1 = (faceVertexItr++).handle().idx();
		int   v2 = faceVertexItr.handle().idx();                                                            // 각 점의 아이디 저장

		vtkSmartPointer<vtkTriangle> triangle = vtkSmartPointer<vtkTriangle>::New();
		triangle->GetPointIds()->SetId(0, v0);
		triangle->GetPointIds()->SetId(1, v1);
		triangle->GetPointIds()->SetId(2, v2);
		cellArray->InsertNextCell(triangle);                                                                // 면정보 배열에 넣음
	}
	polyData->SetPolys(cellArray);                                                          // 폴리데이터에 면정보 넣음
	polyData->Modified();

	return polyData;                            // 폴리데이터 = 점 좌표 + 면 정보
}

std::vector<int> CustomInteractorStyle::dijkstra(int startIdx, int endIdx, const TriMesh& triMesh)
{
	std::vector<int> dijkstraPath;
	std::vector<std::pair<double, int>> shortestPathMap(triMesh.n_vertices(), { std::numeric_limits<double>::max(), -1 });
	shortestPathMap[startIdx] = { 0.0, startIdx };							// 현재노드의 부모노드와 최단거리정보
	std::priority_queue<std::pair<double, int>> pQueue;
	pQueue.push({ 0.0, startIdx });											// 큐에 첫점의 거리와 아이디 정보 넣음

	while (pQueue.size())
	{
		double cost = -pQueue.top().first;									// 현재 노드에서 목표 지점까지 거리(거리가 가장 짧은 것 꺼냄)
		int currentIdx = pQueue.top().second;								// 노드
		pQueue.pop();														// 큐에서 꺼냄

		OpenMesh::VertexHandle currentVertexHandle(currentIdx);				// 현재 버텍스핸들을 현재 노드(아이디)로 설정
		if (currentIdx == endIdx)
			break;
		else
		{
			for (OpenMesh::VertexHandle neighbor : triMesh.vv_range(currentVertexHandle))		// 현재 버텍스 주변 아이디
			{
				double distance = (triMesh.point(currentVertexHandle) - triMesh.point(neighbor)).length();		// 거리
				double nextCost = cost + distance;								// 노드까지의 거리
				if (shortestPathMap[neighbor.idx()].first > nextCost)			// 노드까지의 거리가 기존보다 더 작으면
				{
					shortestPathMap[neighbor.idx()].first = nextCost;			// 최단거리 대체
					shortestPathMap[neighbor.idx()].second = currentVertexHandle.idx();		// 부모노드 저장
					pQueue.push({ -nextCost, neighbor.idx() });					// 큐에 최단거리+이웃점+아이디 넣음
				}
			}
		}
	}

	while (true)																// 부모노드 찾아가기
	{
		dijkstraPath.push_back(endIdx);
		endIdx = shortestPathMap[endIdx].second;
		if (startIdx == endIdx)
		{
			break;
		}
	}

	return dijkstraPath;
}

void CustomInteractorStyle::pathButton()
{
	pathButtonStatus = !pathButtonStatus;
}

void CustomInteractorStyle::MeshCutting(TriMesh& triMesh, int faceId)
{
	QHash<int, int> idHash;
	int size = 0;
	int overCount = 0;
	int lastSize = 1;
	bool findDuplicate;												// 중복점 찾는 플래그
	bool existId = false;													// 현재 점이 커브에 닿였던 점인지 확인하는 플래그
	bool findId = false;

	for (TriMesh::FaceVertexIter fv_iter = triMesh.fv_begin(TriMesh::FaceHandle(faceId));
		fv_iter != triMesh.fv_end(TriMesh::FaceHandle(faceId)); fv_iter++)
	{
		idHash[size] = fv_iter->idx();									// 셀피커로 찍은 면 주변 버텍스 아이디
		size++;
		break;
	}

	for (TriMesh::VertexVertexIter vv_it = triMesh.vv_iter(OpenMesh::VertexHandle(idHash[0])); vv_it.is_valid(); ++vv_it)
	{
		bool findId;
		for (int vertexIdx : curve)
		{
			if (vv_it->idx() == vertexIdx)								// 커브에 하나라도 닿이면 
			{
				findId = true;
				findCurveId.push_back(vv_it->idx());					// 닿인 점 findCurveId 벡터에 넣음
				break;
			}
			else
				findId = false;
		}
		if (findId == false)											// 커브에 안닿이면
		{
			idHash[size] = vv_it.handle().idx();                        // 커브에 안닿인 점 idHash에 넣음
			size++;                                                     // 전체 점의 개수
			overCount++;                                                // 근접점 개수
		}
	}																	// 첫점의 근접점
	while (1)															// 주변점의 주변점 구하기
	{
		for (int i = 0; i < overCount; i++)								// 근접점의 근접점 구하기
		{
			for (TriMesh::VertexVertexIter vv_it = triMesh.vv_iter(OpenMesh::VertexHandle(idHash[lastSize + i])); vv_it.is_valid(); ++vv_it)
			{
				for (int vertexIds : findCurveId)						// 현재 점이 커브에 닿인 점인지 확인
				{
					if (idHash[lastSize + i] == vertexIds)
					{
						existId = true;
						break;
					}
					else
						existId = false;
				}
				if (existId == false)									// 현재 점이 커브에 닿인 점이 아니라면
				{
					for (int j = 0; j < size; j++)							// 중복점 검사
					{
						if (vv_it.handle().idx() == idHash[j])
						{
							findDuplicate = true;
							break;
						}
						else
							findDuplicate = false;
					}
					if (findDuplicate == false)										// 중복 안되는 경우만
					{
						bool findId;
						for (int vertexIdx : curve)
						{
							if (vv_it->idx() == vertexIdx)							// 커브에 하나라도 닿이면 
							{
								findId = true;
								findCurveId.push_back(vv_it->idx());
								break;
							}
							else
								findId = false;
						}
						if (findId == false)
						{
							idHash[size] = vv_it.handle().idx();                   // 피킹한 점의 주변점의 아이디
							size++;                                                // 전체 점의 개수
						}
					}
				}
			}
		}
		lastSize = lastSize + overCount;
		overCount = size - lastSize;
		if (overCount <= 0)
			break;
	}

	for (int vertexIds : curve)							// 현재 idHash에는 커브점은 포함하지 않으므로 커브 점들을 넣음
	{
		idHash[size] = vertexIds;
		size++;
	}

	for (TriMesh::VertexIter v_it = triMesh.vertices_sbegin(); v_it != triMesh.vertices_end(); ++v_it)
	{
		for (int i = 0; i < size; i++)
		{
			if (v_it->idx() == idHash[i])
			{
				findId = true;
				break;
			}
			else
				findId = false;
		}
		if (findId == false)
			triMesh.delete_vertex(OpenMesh::VertexHandle(v_it->idx()));
	}
}

void CustomInteractorStyle::autoConnect()
{
	std::vector<int> autoConnection = dijkstra(vertexIdxs.back(), vertexIdxs[firstVertex], mTriMesh);

	for (int vertexIdx : autoConnection)
	{
		if (curve[0] != vertexIdx && curve.back() != vertexIdx)
			curve.push_back(vertexIdx);
	}
	CompleteCurve = true;
	pathButtonStatus = false;
	pickCount = 0;

	for (int vertexIdx : curve)
	{
		mObserver->lineActor(GetSphere("Yellow", mTriMesh.point(OpenMesh::VertexHandle(vertexIdx)).data()[0],
			mTriMesh.point(OpenMesh::VertexHandle(vertexIdx)).data()[1],
			mTriMesh.point(OpenMesh::VertexHandle(vertexIdx)).data()[2]));
	}
}

vtkNew<vtkActor> CustomInteractorStyle::GetSphere(vtkStdString color, double x, double y, double z)
{
	vtkNew<vtkNamedColors> colors;
	vtkNew<vtkSphereSource> sphereSource2;
	sphereSource2->SetCenter(x, y, z);
	sphereSource2->SetRadius(0.1);
	sphereSource2->SetPhiResolution(10);
	sphereSource2->SetThetaResolution(10);                                                      // 월드 좌표에서 구 생성

	vtkNew<vtkPolyDataMapper> mapper2;                                                           // 매퍼에 구 넣음
	mapper2->SetInputConnection(sphereSource2->GetOutputPort());
	vtkNew<vtkActor> mActor2;                                                                    // 액터에서 색 지정
	mActor2->SetMapper(mapper2);
	mActor2->GetProperty()->SetColor(colors->GetColor3d(color).GetData());

	return mActor2;
}
