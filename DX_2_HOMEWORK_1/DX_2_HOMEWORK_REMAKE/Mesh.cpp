//-----------------------------------------------------------------------------
// File: CGameObject.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Mesh.h"

CMesh::CMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
}

CMesh::~CMesh()
{
	if (m_pd3dPositionBuffer) m_pd3dPositionBuffer->Release();

	if (m_nSubMeshes > 0)
	{
		for (int i = 0; i < m_nSubMeshes; i++)
		{
			if (m_ppd3dSubSetIndexBuffers[i]) m_ppd3dSubSetIndexBuffers[i]->Release();
			if (m_ppnSubSetIndices[i]) delete[] m_ppnSubSetIndices[i];
		}
		if (m_ppd3dSubSetIndexBuffers) delete[] m_ppd3dSubSetIndexBuffers;
		if (m_pd3dSubSetIndexBufferViews) delete[] m_pd3dSubSetIndexBufferViews;

		if (m_pnSubSetIndices) delete[] m_pnSubSetIndices;
		if (m_ppnSubSetIndices) delete[] m_ppnSubSetIndices;
	}

	if (m_pxmf3Positions) delete[] m_pxmf3Positions;
}

void CMesh::ReleaseUploadBuffers()
{
	if (m_pd3dPositionUploadBuffer) m_pd3dPositionUploadBuffer->Release();
	m_pd3dPositionUploadBuffer = NULL;

	if ((m_nSubMeshes > 0) && m_ppd3dSubSetIndexUploadBuffers)
	{
		for (int i = 0; i < m_nSubMeshes; i++)
		{
			if (m_ppd3dSubSetIndexUploadBuffers[i]) m_ppd3dSubSetIndexUploadBuffers[i]->Release();
		}
		if (m_ppd3dSubSetIndexUploadBuffers) delete[] m_ppd3dSubSetIndexUploadBuffers;
		m_ppd3dSubSetIndexUploadBuffers = NULL;
	}
}

void CMesh::Render(ID3D12GraphicsCommandList *pd3dCommandList, int nSubSet)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dPositionBufferView);

	if ((m_nSubMeshes > 0) && (nSubSet < m_nSubMeshes))
	{
		pd3dCommandList->IASetIndexBuffer(&(m_pd3dSubSetIndexBufferViews[nSubSet]));
		pd3dCommandList->DrawIndexedInstanced(m_pnSubSetIndices[nSubSet], 1, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}

void CMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList, int nSubSet, UINT nInstances)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dPositionBufferView);

	if ((m_nSubMeshes > 0) && (nSubSet < m_nSubMeshes))
	{
		pd3dCommandList->IASetIndexBuffer(&(m_pd3dSubSetIndexBufferViews[nSubSet]));
		pd3dCommandList->DrawIndexedInstanced(m_pnSubSetIndices[nSubSet], nInstances, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, nInstances, m_nOffset, 0);
	}
}

int CMesh::CheckRayIntersection(XMFLOAT3& xmf3RayOrigin, XMFLOAT3& xmf3RayDirection, float* pfNearHitDistance)
{
	return 0;
}

void CMesh::CalculateTriangleListVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, int nVertices)
{
	int nPrimitives = nVertices / 3;
	UINT nIndex0, nIndex1, nIndex2;
	for (int i = 0; i < nPrimitives; i++)
	{
		nIndex0 = i * 3 + 0;
		nIndex1 = i * 3 + 1;
		nIndex2 = i * 3 + 2;
		XMFLOAT3 xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1], pxmf3Positions[nIndex0]);
		XMFLOAT3 xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2], pxmf3Positions[nIndex0]);
		pxmf3Normals[nIndex0] = pxmf3Normals[nIndex1]
			= pxmf3Normals[nIndex2]
			= Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, true);
	}
}

void CMesh::CalculateTriangleListVertexNormals(XMFLOAT3* pxmf3Normals,
	XMFLOAT3* pxmf3Positions, UINT nVertices, UINT* pnIndices, UINT nIndices)
{
	UINT nPrimitives = (pnIndices) ? (nIndices / 3) : (nVertices / 3);
	XMFLOAT3 xmf3SumOfNormal, xmf3Edge01, xmf3Edge02, xmf3Normal;
	UINT nIndex0, nIndex1, nIndex2;
	for (UINT j = 0; j < nVertices; j++)
	{
		xmf3SumOfNormal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		for (UINT i = 0; i < nPrimitives; i++)
		{
			nIndex0 = pnIndices[i * 3 + 0];
			nIndex1 = pnIndices[i * 3 + 1];
			nIndex2 = pnIndices[i * 3 + 2];
			if (pnIndices && ((nIndex0 == j) || (nIndex1 == j) || (nIndex2 == j)))
			{
				xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1],
					pxmf3Positions[nIndex0]);
				xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2],
					pxmf3Positions[nIndex0]);
				xmf3Normal = Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, false);
				xmf3SumOfNormal = Vector3::Add(xmf3SumOfNormal, xmf3Normal);
			}
		}
		pxmf3Normals[j] = Vector3::Normalize(xmf3SumOfNormal);
	}
}

void CMesh::CalculateTriangleStripVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, UINT nVertices, UINT* pnIndices, UINT nIndices)
{
	UINT nPrimitives = (pnIndices) ? (nIndices - 2) : (nVertices - 2);
	XMFLOAT3 xmf3SumOfNormal(0.0f, 0.0f, 0.0f);
	UINT nIndex0, nIndex1, nIndex2;
	for (UINT j = 0; j < nVertices; j++)
	{
		xmf3SumOfNormal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		for (UINT i = 0; i < nPrimitives; i++)
		{
			nIndex0 = ((i % 2) == 0) ? (i + 0) : (i + 1);

			if (pnIndices)
				nIndex0 = pnIndices[nIndex0];

			nIndex1 = ((i % 2) == 0) ? (i + 1) : (i + 0);

			if (pnIndices)
				nIndex1 = pnIndices[nIndex1];
			nIndex2 = (pnIndices) ? pnIndices[i + 2] : (i + 2);

			if ((nIndex0 == j) || (nIndex1 == j) || (nIndex2 == j))
			{
				XMFLOAT3 xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1], pxmf3Positions[nIndex0]);
				XMFLOAT3 xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2], pxmf3Positions[nIndex0]);
				XMFLOAT3 xmf3Normal = Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, true);
				xmf3SumOfNormal = Vector3::Add(xmf3SumOfNormal, xmf3Normal);
			}
		}
		pxmf3Normals[j] = Vector3::Normalize(xmf3SumOfNormal);
	}
}

void CMesh::CalculateVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, int nVertices, UINT* pnIndices, int nIndices)
{
	switch (m_d3dPrimitiveTopology)
	{
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		if (pnIndices)
			CalculateTriangleListVertexNormals(pxmf3Normals, pxmf3Positions, nVertices, pnIndices, nIndices);
		else
			CalculateTriangleListVertexNormals(pxmf3Normals, pxmf3Positions, nVertices);
		break;
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		CalculateTriangleStripVertexNormals(pxmf3Normals, pxmf3Positions, nVertices, pnIndices, nIndices);
		break;
	default:
		break;
	}
}

bool CMesh::Check_Polygon_Ray_Normal(XMVECTOR ray_start, XMVECTOR ray_dir, XMFLOAT3* polygon_normal_vector)
{
	XMFLOAT3 normal = { 0.0f, 0.0f, 0.0f };
	float closestHitDistance = FLT_MAX; // 가장 가까운 충돌 거리를 저장할 변수
	bool hitDetected = false;

	// 광선 방향을 정규화
	XMVECTOR rayDirection = ray_dir;
	rayDirection = XMVector3Normalize(rayDirection);

	// 삼각형을 구성하는 방식에 따라 다른 로직 적용
	if (m_d3dPrimitiveTopology == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	{
		// TRIANGLELIST의 경우, 각 3개의 인덱스가 하나의 삼각형을 구성
		for (int subMeshIndex = 0; subMeshIndex < m_nSubMeshes; ++subMeshIndex)
		{
			UINT* indices = m_ppnSubSetIndices[subMeshIndex];
			int indexCount = m_pnSubSetIndices[subMeshIndex];

			for (int i = 0; i < indexCount; i += 3)
			{
				// 삼각형 정점 인덱스
				int index0 = indices[i];
				int index1 = indices[i + 1];
				int index2 = indices[i + 2];

				// 정점 위치 로드
				XMVECTOR v0 = XMLoadFloat3(&m_pxmf3Positions[index0]);
				XMVECTOR v1 = XMLoadFloat3(&m_pxmf3Positions[index1]);
				XMVECTOR v2 = XMLoadFloat3(&m_pxmf3Positions[index2]);

				// 삼각형과 광선의 충돌 검사
				XMVECTOR faceNormal;
				if (CheckRayTriangleIntersection(ray_start, rayDirection, v0, v1, v2, faceNormal, closestHitDistance))
				{
					normal = XMFLOAT3(faceNormal.m128_f32[0], faceNormal.m128_f32[1], faceNormal.m128_f32[2]);
					hitDetected = true;
				}
			}
		}
	}
	else if (m_d3dPrimitiveTopology == D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP)
	{
		// TRIANGLESTRIP의 경우, 첫 번째 삼각형은 [0, 1, 2], 이후 삼각형은 [1, 2, 3], [2, 3, 4] 형태로 구성
		for (int subMeshIndex = 0; subMeshIndex < m_nSubMeshes; ++subMeshIndex)
		{
			UINT* indices = m_ppnSubSetIndices[subMeshIndex];
			int indexCount = m_pnSubSetIndices[subMeshIndex];

			for (int i = 0; i < indexCount - 2; ++i)
			{
				// 삼각형 정점 인덱스
				int index0 = indices[i];
				int index1 = indices[i + 1];
				int index2 = indices[i + 2];

				// 짝수 인덱스는 정방향, 홀수 인덱스는 반대로 정의해야 함
				XMVECTOR v0, v1, v2;
				if (i % 2 == 0)
				{
					v0 = XMLoadFloat3(&m_pxmf3Positions[index0]);
					v1 = XMLoadFloat3(&m_pxmf3Positions[index1]);
					v2 = XMLoadFloat3(&m_pxmf3Positions[index2]);
				}
				else
				{
					v0 = XMLoadFloat3(&m_pxmf3Positions[index0]);
					v1 = XMLoadFloat3(&m_pxmf3Positions[index2]);
					v2 = XMLoadFloat3(&m_pxmf3Positions[index1]);
				}

				// 삼각형과 광선의 충돌 검사
				XMVECTOR faceNormal;
				if (CheckRayTriangleIntersection(ray_start, rayDirection, v0, v1, v2, faceNormal, closestHitDistance))
				{
					XMStoreFloat3(&normal, faceNormal);
					hitDetected = true;
				}
			}
		}
	}

	if (hitDetected)
	{
		*polygon_normal_vector = normal;
		return true;
	}
	else
	{
		*polygon_normal_vector = XMFLOAT3(0.0f, 0.0f, 0.0f);
		return false;
	}
}

bool CMesh::CheckRayTriangleIntersection(
	XMVECTOR ray_start,             // 광선의 시작점
	XMVECTOR rayDirection,          // 광선의 방향 (정규화된 벡터)
	XMVECTOR v0,                    // 삼각형의 첫 번째 정점
	XMVECTOR v1,                    // 삼각형의 두 번째 정점
	XMVECTOR v2,                    // 삼각형의 세 번째 정점
	XMVECTOR& faceNormal,           // 교차한 삼각형의 법선 벡터
	float& closestHitDistance)      // 가장 가까운 충돌 지점까지의 거리
{
	float hitDistance = 0.0f;

	// DirectXCollision의 TriangleTests::Intersects 함수 사용
	if (TriangleTests::Intersects(ray_start, rayDirection, v0, v1, v2, hitDistance))
	{
		// 기존의 가장 가까운 거리와 비교하여 더 짧은 경우에만 갱신
		if (hitDistance < closestHitDistance)
		{
			closestHitDistance = hitDistance;

			// 삼각형의 법선 벡터 계산
			XMVECTOR edge1 = v1 - v0;
			XMVECTOR edge2 = v2 - v0;
			faceNormal = XMVector3Cross(edge1, edge2);
			faceNormal = XMVector3Normalize(faceNormal);

			return true; // 충돌이 발생한 경우
		}
	}
	return false; // 충돌이 발생하지 않음
}
//==================================================================================


OOBBMesh::OOBBMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight, float fDepth) : CMesh(pd3dDevice, pd3dCommandList)
{
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;

	// 정점 개수 설정
	m_nVertices = 8;
	m_pxmf3Positions = new XMFLOAT3[m_nVertices];

	// 큐브의 8개 정점 위치 초기화
	m_pxmf3Positions[0] = XMFLOAT3(-fx, -fy, -fz); // v0
	m_pxmf3Positions[1] = XMFLOAT3(fx, -fy, -fz);  // v1
	m_pxmf3Positions[2] = XMFLOAT3(-fx, fy, -fz);   // v2
	m_pxmf3Positions[3] = XMFLOAT3(fx, fy, -fz);    // v3
	m_pxmf3Positions[4] = XMFLOAT3(-fx, -fy, fz);   // v4
	m_pxmf3Positions[5] = XMFLOAT3(fx, -fy, fz);    // v5
	m_pxmf3Positions[6] = XMFLOAT3(-fx, fy, fz);     // v6
	m_pxmf3Positions[7] = XMFLOAT3(fx, fy, fz);      // v7

	// 버퍼 생성
	m_pd3dPositionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Positions, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

	m_d3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_d3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;

	// 서브메쉬 개수 설정
	m_nSubMeshes = 1;

	// 인덱스 개수 설정
	int nSubMeshIndices = 36;
	m_pnSubSetIndices = new int[m_nSubMeshes];
	m_ppnSubSetIndices = new UINT * [m_nSubMeshes];

	m_pnSubSetIndices[0] = nSubMeshIndices;
	m_ppnSubSetIndices[0] = new UINT[nSubMeshIndices];

	// 큐브의 인덱스 배열 초기화
	UINT* indices = m_ppnSubSetIndices[0];

	// 각 면을 구성하는 인덱스 설정
	// 면 1: v0, v1, v2, v3
	indices[0] = 0; indices[1] = 1; indices[2] = 2;
	indices[3] = 1; indices[4] = 3; indices[5] = 2;

	// 면 2: v4, v5, v6, v7
	indices[6] = 4; indices[7] = 5; indices[8] = 6;
	indices[9] = 5; indices[10] = 7; indices[11] = 6;

	// 면 3: v0, v4, v6, v2
	indices[12] = 0; indices[13] = 4; indices[14] = 6;
	indices[15] = 4; indices[16] = 2; indices[17] = 0;

	// 면 4: v1, v3, v7, v5
	indices[18] = 1; indices[19] = 3; indices[20] = 7;
	indices[21] = 3; indices[22] = 5; indices[23] = 1;

	// 면 5: v0, v1, v5, v4
	indices[24] = 0; indices[25] = 1; indices[26] = 5;
	indices[27] = 5; indices[28] = 4; indices[29] = 0;

	// 면 6: v2, v6, v7, v3
	indices[30] = 2; indices[31] = 6; indices[32] = 7;
	indices[33] = 7; indices[34] = 3; indices[35] = 2;

	// 인덱스 버퍼 생성
	m_ppd3dSubSetIndexBuffers = new ID3D12Resource * [m_nSubMeshes];
	m_ppd3dSubSetIndexUploadBuffers = new ID3D12Resource * [m_nSubMeshes];

	m_ppd3dSubSetIndexBuffers[0] = CreateBufferResource(
		pd3dDevice, pd3dCommandList, indices, sizeof(UINT) * nSubMeshIndices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_ppd3dSubSetIndexUploadBuffers[0]
	);

	// 인덱스 버퍼 뷰 설정
	m_pd3dSubSetIndexBufferViews = new D3D12_INDEX_BUFFER_VIEW[m_nSubMeshes];
	m_pd3dSubSetIndexBufferViews[0].BufferLocation = m_ppd3dSubSetIndexBuffers[0]->GetGPUVirtualAddress();
	m_pd3dSubSetIndexBufferViews[0].Format = DXGI_FORMAT_R32_UINT;
	m_pd3dSubSetIndexBufferViews[0].SizeInBytes = sizeof(UINT) * nSubMeshIndices;
}


OOBBMesh::~OOBBMesh()
{
}

void OOBBMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList, int nSubSet)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dPositionBufferView);

	if ((m_nSubMeshes > 0) && (nSubSet < m_nSubMeshes))
	{
		pd3dCommandList->IASetIndexBuffer(&(m_pd3dSubSetIndexBufferViews[nSubSet]));
		pd3dCommandList->DrawIndexedInstanced(m_pnSubSetIndices[nSubSet], 1, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CTexturedMesh::CTexturedMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth, float fHeight, float fDepth, float fxPosition, float fyPosition, float fzPosition) : CMesh(pd3dDevice, pd3dCommandList)
{
}

CTexturedMesh::~CTexturedMesh()
{
	//if (m_pd3dTextureCoord0Buffer) m_pd3dTextureCoord0Buffer->Release();
	//if (m_pxmf2TextureCoords0) delete[] m_pxmf2TextureCoords0;
}

void CTexturedMesh::ReleaseUploadBuffers()
{
	CMesh::ReleaseUploadBuffers();

	if (m_pd3dTextureCoord0UploadBuffer) m_pd3dTextureCoord0UploadBuffer->Release();
	m_pd3dTextureCoord0UploadBuffer = NULL;

	if (m_pd3dTextureCoord1UploadBuffer) m_pd3dTextureCoord1UploadBuffer->Release();
	m_pd3dTextureCoord1UploadBuffer = NULL;
}

void CTexturedMesh::Render(ID3D12GraphicsCommandList *pd3dCommandList, int nSubSet)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[2] = { m_d3dPositionBufferView, m_d3dTextureCoord0BufferView };
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 2, pVertexBufferViews);

	pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
}

Textured_Screen_Mesh::Textured_Screen_Mesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT2 left_top, XMFLOAT2 right_bottom, int camera_distance) : CTexturedMesh(pd3dDevice, pd3dCommandList)
{
	float left = left_top.x;
	float top = left_top.y;
	float right = right_bottom.x;
	float bottom = right_bottom.y;

	float distance = static_cast<float>(camera_distance); // 1~ 100 사이의 값만 입력 받기
	distance /= 100.0f;
	if (distance > 1.0f)
		distance = 0.9999999f;
	m_nVertices = 6;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_pxmf3Positions = new XMFLOAT3[m_nVertices];
	m_pxmf2TextureCoords0 = new XMFLOAT2[m_nVertices];

	m_pxmf3Positions[0] = XMFLOAT3(left, top, distance), m_pxmf2TextureCoords0[0] = XMFLOAT2(0.0f, 0.0f);
	m_pxmf3Positions[1] = XMFLOAT3(right, top, distance), m_pxmf2TextureCoords0[1] = XMFLOAT2(1.0f, 0.0f);
	m_pxmf3Positions[2] = XMFLOAT3(right, bottom, distance), m_pxmf2TextureCoords0[2] = XMFLOAT2(1.0f, 1.0f);

	m_pxmf3Positions[3] = XMFLOAT3(left, top, distance), m_pxmf2TextureCoords0[3] = XMFLOAT2(0.0f, 0.0f);
	m_pxmf3Positions[4] = XMFLOAT3(right, bottom, distance), m_pxmf2TextureCoords0[4] = XMFLOAT2(1.0f, 1.0f);
	m_pxmf3Positions[5] = XMFLOAT3(left, bottom, distance), m_pxmf2TextureCoords0[5] = XMFLOAT2(0.0f, 1.0f);


	m_pd3dPositionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Positions, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

	m_d3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_d3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;

	m_pd3dTextureCoord0Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf2TextureCoords0, sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTextureCoord0UploadBuffer);

	m_d3dTextureCoord0BufferView.BufferLocation = m_pd3dTextureCoord0Buffer->GetGPUVirtualAddress();
	m_d3dTextureCoord0BufferView.StrideInBytes = sizeof(XMFLOAT2);
	m_d3dTextureCoord0BufferView.SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;
}

Textured_Screen_Mesh::~Textured_Screen_Mesh()
{
	if (m_pd3dTextureCoord0Buffer) m_pd3dTextureCoord0Buffer->Release();
	if (m_pxmf2TextureCoords0) delete[] m_pxmf2TextureCoords0;
}

void Textured_Screen_Mesh::ReleaseUploadBuffers()
{
	CMesh::ReleaseUploadBuffers();

	if (m_pd3dTextureCoord0UploadBuffer) m_pd3dTextureCoord0UploadBuffer->Release();
	m_pd3dTextureCoord0UploadBuffer = NULL;

	if (m_pd3dTextureCoord1UploadBuffer) m_pd3dTextureCoord1UploadBuffer->Release();
	m_pd3dTextureCoord1UploadBuffer = NULL;
}

void Textured_Screen_Mesh::Render(ID3D12GraphicsCommandList* pd3dCommandList, int nSubSet)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[2] = { m_d3dPositionBufferView, m_d3dTextureCoord0BufferView };
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 2, pVertexBufferViews);

	pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
}

int Textured_Screen_Mesh::CheckRayIntersection(XMFLOAT3& xmf3RayOrigin, XMFLOAT3& xmf3RayDirection, float* pfNearHitDistance)
{
	XMFLOAT3* m_pPos = m_pxmf3Positions;
	int nIntersections = 0;
	float fNearHitDistance = FLT_MAX;
	const int nPrimitives = 2;

	// 광선의 원점과 방향을 XMVECTOR로 변환
	XMVECTOR xmRayOrigin = XMLoadFloat3(&xmf3RayOrigin);
	XMVECTOR xmRayDirection = XMLoadFloat3(&xmf3RayDirection);

	// 삼각형의 정점을 하나씩 검사
	for (int i = 0; i < nPrimitives; i++)
	{
		// 삼각형의 각 정점 로드
		XMVECTOR v0 = XMLoadFloat3(&m_pPos[i * 3 + 0]);
		XMVECTOR v1 = XMLoadFloat3(&m_pPos[i * 3 + 1]);
		XMVECTOR v2 = XMLoadFloat3(&m_pPos[i * 3 + 2]);

		float fHitDistance; // 교차 거리를 저장할 변수
		BOOL bIntersected = TriangleTests::Intersects(xmRayOrigin, xmRayDirection, v0, v1, v2, fHitDistance);

		// 교차된 경우
		if (bIntersected)
		{
			if (fHitDistance < fNearHitDistance)
			{
				*pfNearHitDistance = fNearHitDistance = fHitDistance;
			}
			nIntersections++;
		}
	}

	return nIntersections > 0 ? 1 : 0;
}



Billboard_Mesh::Billboard_Mesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight) : CTexturedMesh(pd3dDevice, pd3dCommandList)
{
	float left = -(fWidth / 2);
	float right = fWidth / 2;
	float top = fHeight / 2;
	float bottom = -(fHeight / 2);

	m_nVertices = 6;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_pxmf3Positions = new XMFLOAT3[m_nVertices];
	m_pxmf2TextureCoords0 = new XMFLOAT2[m_nVertices];

	m_pxmf3Positions[0] = XMFLOAT3(left, top, 0.0f), m_pxmf2TextureCoords0[0] = XMFLOAT2(0.0f, 0.0f);
	m_pxmf3Positions[1] = XMFLOAT3(right, top, 0.0f), m_pxmf2TextureCoords0[1] = XMFLOAT2(1.0f, 0.0f);
	m_pxmf3Positions[2] = XMFLOAT3(right, bottom, 0.0f), m_pxmf2TextureCoords0[2] = XMFLOAT2(1.0f, 1.0f);

	m_pxmf3Positions[3] = XMFLOAT3(left, top, 0.0f), m_pxmf2TextureCoords0[3] = XMFLOAT2(0.0f, 0.0f);
	m_pxmf3Positions[4] = XMFLOAT3(right, bottom, 0.0f), m_pxmf2TextureCoords0[4] = XMFLOAT2(1.0f, 1.0f);
	m_pxmf3Positions[5] = XMFLOAT3(left, bottom, 0.0f), m_pxmf2TextureCoords0[5] = XMFLOAT2(0.0f, 1.0f);

	//====================================================
	m_pd3dPositionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Positions, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

	m_d3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_d3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;

	//====================================================
	m_pd3dTextureCoord0Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf2TextureCoords0, sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTextureCoord0UploadBuffer);

	m_d3dTextureCoord0BufferView.BufferLocation = m_pd3dTextureCoord0Buffer->GetGPUVirtualAddress();
	m_d3dTextureCoord0BufferView.StrideInBytes = sizeof(XMFLOAT2);
	m_d3dTextureCoord0BufferView.SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;

	//====================================================
	AnimationVertexinfo = new UINT[m_nVertices];

	for (int i = 0; i < m_nVertices; i++)
		AnimationVertexinfo[i] = i;

	m_pd3dAnimationVertexinfoBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, AnimationVertexinfo, sizeof(UINT) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dAnimationVertexinfoUploadBuffer);

	m_d3dAnimationVertexinfoBufferView.BufferLocation = m_pd3dAnimationVertexinfoBuffer->GetGPUVirtualAddress();
	m_d3dAnimationVertexinfoBufferView.StrideInBytes = sizeof(UINT);
	m_d3dAnimationVertexinfoBufferView.SizeInBytes = sizeof(UINT) * m_nVertices;
}
Billboard_Mesh::~Billboard_Mesh()
{

}

void Billboard_Mesh::Render(ID3D12GraphicsCommandList* pd3dCommandList, bool animation)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[3] = { m_d3dPositionBufferView, m_d3dTextureCoord0BufferView, m_d3dAnimationVertexinfoBufferView };
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 3, pVertexBufferViews);

	pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//


Textured_Cube_Mesh::Textured_Cube_Mesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight, float fDepth) : CStandardMesh(pd3dDevice, pd3dCommandList)
{
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;


	m_nVertices = 24;
	
	m_pxmf3Positions = new XMFLOAT3[m_nVertices];
	m_pxmf2TextureCoords0 = new XMFLOAT2[m_nVertices];
	m_pxmf3Normals = new  XMFLOAT3[m_nVertices];

	// Back
	m_pxmf3Positions[0] = XMFLOAT3(+fx, -fy, +fz);  m_pxmf2TextureCoords0[0] = XMFLOAT2(0.0f, 1.0f);
	m_pxmf3Positions[1] = XMFLOAT3(+fx, +fy, +fz);  m_pxmf2TextureCoords0[1] = XMFLOAT2(0.0f, 0.0f);
	m_pxmf3Positions[2] = XMFLOAT3(-fx, +fy, +fz);  m_pxmf2TextureCoords0[2] = XMFLOAT2(1.0f, 0.0f);
	m_pxmf3Positions[3] = XMFLOAT3(-fx, -fy, +fz);  m_pxmf2TextureCoords0[3] = XMFLOAT2(1.0f, 1.0f);

	// Front
	m_pxmf3Positions[4] = XMFLOAT3(-fx, +fy, -fz);  m_pxmf2TextureCoords0[4] = XMFLOAT2(0.0f, 0.0f);
	m_pxmf3Positions[5] = XMFLOAT3(+fx, +fy, -fz);  m_pxmf2TextureCoords0[5] = XMFLOAT2(1.0f, 0.0f);
	m_pxmf3Positions[6] = XMFLOAT3(+fx, -fy, -fz);  m_pxmf2TextureCoords0[6] = XMFLOAT2(1.0f, 1.0f);
	m_pxmf3Positions[7] = XMFLOAT3(-fx, -fy, -fz);  m_pxmf2TextureCoords0[7] = XMFLOAT2(0.0f, 1.0f);

	// Top
	m_pxmf3Positions[8] = XMFLOAT3(-fx, +fy, +fz);  m_pxmf2TextureCoords0[8] = XMFLOAT2(0.0f, 1.0f);
	m_pxmf3Positions[9] = XMFLOAT3(+fx, +fy, +fz);  m_pxmf2TextureCoords0[9] = XMFLOAT2(0.0f, 0.0f);
	m_pxmf3Positions[10] = XMFLOAT3(+fx, +fy, -fz); m_pxmf2TextureCoords0[10] = XMFLOAT2(1.0f, 0.0f);
	m_pxmf3Positions[11] = XMFLOAT3(-fx, +fy, -fz); m_pxmf2TextureCoords0[11] = XMFLOAT2(1.0f, 1.0f);

	// Bottom
	m_pxmf3Positions[12] = XMFLOAT3(+fx, -fy, -fz); m_pxmf2TextureCoords0[12] = XMFLOAT2(0.0f, 0.0f);
	m_pxmf3Positions[13] = XMFLOAT3(+fx, -fy, +fz); m_pxmf2TextureCoords0[13] = XMFLOAT2(0.0f, 1.0f);
	m_pxmf3Positions[14] = XMFLOAT3(-fx, -fy, +fz); m_pxmf2TextureCoords0[14] = XMFLOAT2(1.0f, 1.0f);
	m_pxmf3Positions[15] = XMFLOAT3(-fx, -fy, -fz); m_pxmf2TextureCoords0[15] = XMFLOAT2(1.0f, 0.0f);

	// Left
	m_pxmf3Positions[16] = XMFLOAT3(-fx, +fy, +fz); m_pxmf2TextureCoords0[16] = XMFLOAT2(0.0f, 0.0f);
	m_pxmf3Positions[17] = XMFLOAT3(-fx, +fy, -fz); m_pxmf2TextureCoords0[17] = XMFLOAT2(1.0f, 0.0f);
	m_pxmf3Positions[18] = XMFLOAT3(-fx, -fy, -fz); m_pxmf2TextureCoords0[18] = XMFLOAT2(1.0f, 1.0f);
	m_pxmf3Positions[19] = XMFLOAT3(-fx, -fy, +fz); m_pxmf2TextureCoords0[19] = XMFLOAT2(0.0f, 1.0f);

	// Right
	m_pxmf3Positions[20] = XMFLOAT3(+fx, +fy, -fz); m_pxmf2TextureCoords0[20] = XMFLOAT2(0.0f, 0.0f);
	m_pxmf3Positions[21] = XMFLOAT3(+fx, +fy, +fz); m_pxmf2TextureCoords0[21] = XMFLOAT2(1.0f, 0.0f);
	m_pxmf3Positions[22] = XMFLOAT3(+fx, -fy, +fz); m_pxmf2TextureCoords0[22] = XMFLOAT2(1.0f, 1.0f);
	m_pxmf3Positions[23] = XMFLOAT3(+fx, -fy, -fz); m_pxmf2TextureCoords0[23] = XMFLOAT2(0.0f, 1.0f);



	// Back (법선 벡터: (0, 0, 1))
	m_pxmf3Normals[0] = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pxmf3Normals[1] = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pxmf3Normals[2] = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pxmf3Normals[3] = XMFLOAT3(0.0f, 0.0f, 1.0f);

	// Front (법선 벡터: (0, 0, -1))
	m_pxmf3Normals[4] = XMFLOAT3(0.0f, 0.0f, -1.0f);
	m_pxmf3Normals[5] = XMFLOAT3(0.0f, 0.0f, -1.0f);
	m_pxmf3Normals[6] = XMFLOAT3(0.0f, 0.0f, -1.0f);
	m_pxmf3Normals[7] = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// Top (법선 벡터: (0, 1, 0))
	m_pxmf3Normals[8] = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_pxmf3Normals[9] = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_pxmf3Normals[10] = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_pxmf3Normals[11] = XMFLOAT3(0.0f, 1.0f, 0.0f);

	// Bottom (법선 벡터: (0, -1, 0))
	m_pxmf3Normals[12] = XMFLOAT3(0.0f, -1.0f, 0.0f);
	m_pxmf3Normals[13] = XMFLOAT3(0.0f, -1.0f, 0.0f);
	m_pxmf3Normals[14] = XMFLOAT3(0.0f, -1.0f, 0.0f);
	m_pxmf3Normals[15] = XMFLOAT3(0.0f, -1.0f, 0.0f);

	// Left (법선 벡터: (-1, 0, 0))
	m_pxmf3Normals[16] = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	m_pxmf3Normals[17] = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	m_pxmf3Normals[18] = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	m_pxmf3Normals[19] = XMFLOAT3(-1.0f, 0.0f, 0.0f);

	// Right (법선 벡터: (1, 0, 0))
	m_pxmf3Normals[20] = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_pxmf3Normals[21] = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_pxmf3Normals[22] = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_pxmf3Normals[23] = XMFLOAT3(1.0f, 0.0f, 0.0f);


	//====================================================
	m_pd3dPositionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Positions, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

	m_d3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_d3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
	//====================================================
	m_pd3dTextureCoord0Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf2TextureCoords0, sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTextureCoord0UploadBuffer);

	m_d3dTextureCoord0BufferView.BufferLocation = m_pd3dTextureCoord0Buffer->GetGPUVirtualAddress();
	m_d3dTextureCoord0BufferView.StrideInBytes = sizeof(XMFLOAT2);
	m_d3dTextureCoord0BufferView.SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;
	//====================================================
	m_pd3dNormalBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Normals, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dNormalUploadBuffer);
	
	m_d3dNormalBufferView.BufferLocation = m_pd3dNormalBuffer->GetGPUVirtualAddress();
	m_d3dNormalBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dNormalBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
	//====================================================

	m_pxmf3Tangents = new XMFLOAT3[m_nVertices];
	m_pxmf3BiTangents = new XMFLOAT3[m_nVertices];

	// 모든 정점의 탄젠트와 바이탄젠트를 초기화
	for (int i = 0; i < m_nVertices; i++) {
		m_pxmf3Tangents[i] = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_pxmf3BiTangents[i] = XMFLOAT3(0.0f, 0.0f, 0.0f);
	}

	// 각 면의 탄젠트와 바이탄젠트 계산
	for (int i = 0; i < m_nVertices; i += 4) {
		XMFLOAT3 v0 = m_pxmf3Positions[i + 0];
		XMFLOAT3 v1 = m_pxmf3Positions[i + 1];
		XMFLOAT3 v2 = m_pxmf3Positions[i + 2];

		// 삼각형 1: v0, v1, v2
		XMFLOAT2 uv0 = m_pxmf2TextureCoords0[i + 0];
		XMFLOAT2 uv1 = m_pxmf2TextureCoords0[i + 1];
		XMFLOAT2 uv2 = m_pxmf2TextureCoords0[i + 2];

		// v0 -> v1, v0 -> v2 벡터
		XMFLOAT3 edge1 = XMFLOAT3(v1.x - v0.x, v1.y - v0.y, v1.z - v0.z);
		XMFLOAT3 edge2 = XMFLOAT3(v2.x - v0.x, v2.y - v0.y, v2.z - v0.z);

		// 텍스처 좌표 차이
		XMFLOAT2 deltaUV1 = XMFLOAT2(uv1.x - uv0.x, uv1.y - uv0.y);
		XMFLOAT2 deltaUV2 = XMFLOAT2(uv2.x - uv0.x, uv2.y - uv0.y);

		// R = 1 / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x)
		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

		// 탄젠트와 바이탄젠트 계산
		XMFLOAT3 tangent, bitangent;
		tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

		bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

		// 정점에 탄젠트와 바이탄젠트 적용
		for (int j = 0; j < 4; j++) {
			m_pxmf3Tangents[i + j] = tangent;
			m_pxmf3BiTangents[i + j] = bitangent;
		}
	}

	m_pd3dTangentBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Normals, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTangentUploadBuffer);

	m_d3dTangentBufferView.BufferLocation = m_pd3dTangentBuffer->GetGPUVirtualAddress();
	m_d3dTangentBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dTangentBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;

	m_pd3dBiTangentBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Normals, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dBiTangentUploadBuffer);

	m_d3dBiTangentBufferView.BufferLocation = m_pd3dBiTangentBuffer->GetGPUVirtualAddress();
	m_d3dBiTangentBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dBiTangentBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;

	//====================================================
// 서브메쉬의 개수는 1개만 사용하도록 설정
	m_nSubMeshes = 1;

	// 서브메쉬 인덱스 개수 및 할당
	int nSubMeshIndices = 36; // 단일 서브메쉬의 인덱스 개수
	m_pnSubSetIndices = new int[m_nSubMeshes]; // 서브메쉬 인덱스 개수 저장 배열
	m_ppnSubSetIndices = new UINT * [m_nSubMeshes]; // 서브메쉬 인덱스 배열 포인터

	m_pnSubSetIndices[0] = nSubMeshIndices; // 첫 번째 서브메쉬의 인덱스 개수 설정
	m_ppnSubSetIndices[0] = new UINT[nSubMeshIndices]; // 첫 번째 서브메쉬의 인덱스 배열 할당

	// 인덱스 배열 초기화 (각 면에 대한 인덱스 설정)
	m_ppnSubSetIndices[0][0] = 0;  m_ppnSubSetIndices[0][1] = 1;  m_ppnSubSetIndices[0][2] = 2;
	m_ppnSubSetIndices[0][3] = 0;  m_ppnSubSetIndices[0][4] = 2;  m_ppnSubSetIndices[0][5] = 3;  // Back
	m_ppnSubSetIndices[0][6] = 4;  m_ppnSubSetIndices[0][7] = 5;  m_ppnSubSetIndices[0][8] = 6;
	m_ppnSubSetIndices[0][9] = 4;  m_ppnSubSetIndices[0][10] = 6; m_ppnSubSetIndices[0][11] = 7;  // Front
	m_ppnSubSetIndices[0][12] = 8; m_ppnSubSetIndices[0][13] = 9; m_ppnSubSetIndices[0][14] = 10;
	m_ppnSubSetIndices[0][15] = 8; m_ppnSubSetIndices[0][16] = 10; m_ppnSubSetIndices[0][17] = 11; // Top
	m_ppnSubSetIndices[0][18] = 12; m_ppnSubSetIndices[0][19] = 13; m_ppnSubSetIndices[0][20] = 14;
	m_ppnSubSetIndices[0][21] = 12; m_ppnSubSetIndices[0][22] = 14; m_ppnSubSetIndices[0][23] = 15; // Bottom
	m_ppnSubSetIndices[0][24] = 16; m_ppnSubSetIndices[0][25] = 17; m_ppnSubSetIndices[0][26] = 18;
	m_ppnSubSetIndices[0][27] = 16; m_ppnSubSetIndices[0][28] = 18; m_ppnSubSetIndices[0][29] = 19; // Left
	m_ppnSubSetIndices[0][30] = 20; m_ppnSubSetIndices[0][31] = 21; m_ppnSubSetIndices[0][32] = 22;
	m_ppnSubSetIndices[0][33] = 20; m_ppnSubSetIndices[0][34] = 22; m_ppnSubSetIndices[0][35] = 23; // Right

	// 서브메쉬 인덱스 버퍼 및 업로드 버퍼 생성
	m_ppd3dSubSetIndexBuffers = new ID3D12Resource * [m_nSubMeshes];
	m_ppd3dSubSetIndexUploadBuffers = new ID3D12Resource * [m_nSubMeshes];

	// 첫 번째 서브메쉬의 인덱스 버퍼 생성
	m_ppd3dSubSetIndexBuffers[0] = CreateBufferResource(
		pd3dDevice, pd3dCommandList, m_ppnSubSetIndices[0], sizeof(UINT) * nSubMeshIndices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_ppd3dSubSetIndexUploadBuffers[0]
	);

	// 서브메쉬 인덱스 버퍼 뷰 설정
	m_pd3dSubSetIndexBufferViews = new D3D12_INDEX_BUFFER_VIEW[m_nSubMeshes];
	m_pd3dSubSetIndexBufferViews[0].BufferLocation = m_ppd3dSubSetIndexBuffers[0]->GetGPUVirtualAddress();
	m_pd3dSubSetIndexBufferViews[0].Format = DXGI_FORMAT_R32_UINT;
	m_pd3dSubSetIndexBufferViews[0].SizeInBytes = sizeof(UINT) * nSubMeshIndices;


	mesh_bounding_box = new BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fx, fy, fz), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

}

Textured_Cube_Mesh::~Textured_Cube_Mesh()
{
}

void Textured_Cube_Mesh::Render(ID3D12GraphicsCommandList* pd3dCommandList, int nSubSet)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[5] = { m_d3dPositionBufferView, m_d3dTextureCoord0BufferView, m_d3dNormalBufferView, m_d3dTangentBufferView, m_d3dBiTangentBufferView };
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 5, pVertexBufferViews);

	if ((m_nSubMeshes > 0) && (nSubSet < m_nSubMeshes))
	{
		pd3dCommandList->IASetIndexBuffer(&(m_pd3dSubSetIndexBufferViews[nSubSet]));
		pd3dCommandList->DrawIndexedInstanced(m_pnSubSetIndices[nSubSet], 1, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}


void Asteroid_Mesh::Render(ID3D12GraphicsCommandList* pd3dCommandList, int nSubSet, UINT nInstances)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[3] = { m_d3dPositionBufferView, m_d3dNormalBufferView, m_d3dTextureCoord0BufferView};
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 3, pVertexBufferViews);

	if ((m_nSubMeshes > 0) && (nSubSet < m_nSubMeshes))
	{
		pd3dCommandList->IASetIndexBuffer(&(m_pd3dSubSetIndexBufferViews[nSubSet]));
		pd3dCommandList->DrawIndexedInstanced(m_pnSubSetIndices[nSubSet], nInstances, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, nInstances, m_nOffset, 0);
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

CSkyBoxMesh::CSkyBoxMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth, float fHeight, float fDepth) : CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 36;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_pxmf3Positions = new XMFLOAT3[m_nVertices];

	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;
	// Front Quad (quads point inward)
	m_pxmf3Positions[0] = XMFLOAT3(-fx, +fx, +fx);
	m_pxmf3Positions[1] = XMFLOAT3(+fx, +fx, +fx);
	m_pxmf3Positions[2] = XMFLOAT3(-fx, -fx, +fx);
	m_pxmf3Positions[3] = XMFLOAT3(-fx, -fx, +fx);
	m_pxmf3Positions[4] = XMFLOAT3(+fx, +fx, +fx);
	m_pxmf3Positions[5] = XMFLOAT3(+fx, -fx, +fx);
	// Back Quad										
	m_pxmf3Positions[6] = XMFLOAT3(+fx, +fx, -fx);
	m_pxmf3Positions[7] = XMFLOAT3(-fx, +fx, -fx);
	m_pxmf3Positions[8] = XMFLOAT3(+fx, -fx, -fx);
	m_pxmf3Positions[9] = XMFLOAT3(+fx, -fx, -fx);
	m_pxmf3Positions[10] = XMFLOAT3(-fx, +fx, -fx);
	m_pxmf3Positions[11] = XMFLOAT3(-fx, -fx, -fx);
	// Left Quad										
	m_pxmf3Positions[12] = XMFLOAT3(-fx, +fx, -fx);
	m_pxmf3Positions[13] = XMFLOAT3(-fx, +fx, +fx);
	m_pxmf3Positions[14] = XMFLOAT3(-fx, -fx, -fx);
	m_pxmf3Positions[15] = XMFLOAT3(-fx, -fx, -fx);
	m_pxmf3Positions[16] = XMFLOAT3(-fx, +fx, +fx);
	m_pxmf3Positions[17] = XMFLOAT3(-fx, -fx, +fx);
	// Right Quad										
	m_pxmf3Positions[18] = XMFLOAT3(+fx, +fx, +fx);
	m_pxmf3Positions[19] = XMFLOAT3(+fx, +fx, -fx);
	m_pxmf3Positions[20] = XMFLOAT3(+fx, -fx, +fx);
	m_pxmf3Positions[21] = XMFLOAT3(+fx, -fx, +fx);
	m_pxmf3Positions[22] = XMFLOAT3(+fx, +fx, -fx);
	m_pxmf3Positions[23] = XMFLOAT3(+fx, -fx, -fx);
	// Top Quad											
	m_pxmf3Positions[24] = XMFLOAT3(-fx, +fx, -fx);
	m_pxmf3Positions[25] = XMFLOAT3(+fx, +fx, -fx);
	m_pxmf3Positions[26] = XMFLOAT3(-fx, +fx, +fx);
	m_pxmf3Positions[27] = XMFLOAT3(-fx, +fx, +fx);
	m_pxmf3Positions[28] = XMFLOAT3(+fx, +fx, -fx);
	m_pxmf3Positions[29] = XMFLOAT3(+fx, +fx, +fx);
	// Bottom Quad										
	m_pxmf3Positions[30] = XMFLOAT3(-fx, -fx, +fx);
	m_pxmf3Positions[31] = XMFLOAT3(+fx, -fx, +fx);
	m_pxmf3Positions[32] = XMFLOAT3(-fx, -fx, -fx);
	m_pxmf3Positions[33] = XMFLOAT3(-fx, -fx, -fx);
	m_pxmf3Positions[34] = XMFLOAT3(+fx, -fx, +fx);
	m_pxmf3Positions[35] = XMFLOAT3(+fx, -fx, -fx);

	m_pd3dPositionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Positions, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

	m_d3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_d3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
}

CSkyBoxMesh::~CSkyBoxMesh()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
CStandardMesh::CStandardMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) : CMesh(pd3dDevice, pd3dCommandList)
{
}

CStandardMesh::~CStandardMesh()
{
	if (m_pd3dTextureCoord0Buffer) m_pd3dTextureCoord0Buffer->Release();
	if (m_pd3dTextureCoord1Buffer) m_pd3dTextureCoord1Buffer->Release();

	if (m_pd3dNormalBuffer) m_pd3dNormalBuffer->Release();
	if (m_pd3dTangentBuffer) m_pd3dTangentBuffer->Release();
	if (m_pd3dBiTangentBuffer) m_pd3dBiTangentBuffer->Release();

	if (m_pxmf4Colors) delete[] m_pxmf4Colors;
	if (m_pxmf3Normals) delete[] m_pxmf3Normals;
	if (m_pxmf3Tangents) delete[] m_pxmf3Tangents;
	if (m_pxmf3BiTangents) delete[] m_pxmf3BiTangents;
	if (m_pxmf2TextureCoords0) delete[] m_pxmf2TextureCoords0;
	if (m_pxmf2TextureCoords1) delete[] m_pxmf2TextureCoords1;
}

void CStandardMesh::ReleaseUploadBuffers()
{
	CMesh::ReleaseUploadBuffers();

	if (m_pd3dTextureCoord0UploadBuffer) m_pd3dTextureCoord0UploadBuffer->Release();
	m_pd3dTextureCoord0UploadBuffer = NULL;

	if (m_pd3dNormalUploadBuffer) m_pd3dNormalUploadBuffer->Release();
	m_pd3dNormalUploadBuffer = NULL;

	if (m_pd3dTangentUploadBuffer) m_pd3dTangentUploadBuffer->Release();
	m_pd3dTangentUploadBuffer = NULL;

	if (m_pd3dBiTangentUploadBuffer) m_pd3dBiTangentUploadBuffer->Release();
	m_pd3dBiTangentUploadBuffer = NULL;
}

void CStandardMesh::LoadMeshFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, FILE *pInFile)
{
	char pstrToken[64] = { '\0' };
	BYTE nStrLength = 0;

	int nPositions = 0, nColors = 0, nNormals = 0, nTangents = 0, nBiTangents = 0, nTextureCoords = 0, nIndices = 0, nSubMeshes = 0, nSubIndices = 0;

	UINT nReads = (UINT)::fread(&m_nVertices, sizeof(int), 1, pInFile);
	nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
	nReads = (UINT)::fread(m_pstrMeshName, sizeof(char), nStrLength, pInFile);
	m_pstrMeshName[nStrLength] = '\0';

	for ( ; ; )
	{
		nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
		nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
		pstrToken[nStrLength] = '\0';

		if (!strcmp(pstrToken, "<Bounds>:"))
		{
			nReads = (UINT)::fread(&m_xmf3AABBCenter, sizeof(XMFLOAT3), 1, pInFile);
			nReads = (UINT)::fread(&m_xmf3AABBExtents, sizeof(XMFLOAT3), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Positions>:"))
		{
			nReads = (UINT)::fread(&nPositions, sizeof(int), 1, pInFile);
			if (nPositions > 0)
			{
				m_nType |= VERTEXT_POSITION;
				m_pxmf3Positions = new XMFLOAT3[nPositions];
				nReads = (UINT)::fread(m_pxmf3Positions, sizeof(XMFLOAT3), nPositions, pInFile);

				m_pd3dPositionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Positions, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

				m_d3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
				m_d3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_d3dPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
			}
		}
		else if (!strcmp(pstrToken, "<Colors>:"))
		{
			nReads = (UINT)::fread(&nColors, sizeof(int), 1, pInFile);
			if (nColors > 0)
			{
				m_nType |= VERTEXT_COLOR;
				m_pxmf4Colors = new XMFLOAT4[nColors];
				nReads = (UINT)::fread(m_pxmf4Colors, sizeof(XMFLOAT4), nColors, pInFile);
			}
		}
		else if (!strcmp(pstrToken, "<TextureCoords0>:"))
		{
			nReads = (UINT)::fread(&nTextureCoords, sizeof(int), 1, pInFile);
			if (nTextureCoords > 0)
			{
				m_nType |= VERTEXT_TEXTURE_COORD0;
				m_pxmf2TextureCoords0 = new XMFLOAT2[nTextureCoords];
				nReads = (UINT)::fread(m_pxmf2TextureCoords0, sizeof(XMFLOAT2), nTextureCoords, pInFile);

				m_pd3dTextureCoord0Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf2TextureCoords0, sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTextureCoord0UploadBuffer);

				m_d3dTextureCoord0BufferView.BufferLocation = m_pd3dTextureCoord0Buffer->GetGPUVirtualAddress();
				m_d3dTextureCoord0BufferView.StrideInBytes = sizeof(XMFLOAT2);
				m_d3dTextureCoord0BufferView.SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;
			}
		}
		else if (!strcmp(pstrToken, "<TextureCoords1>:"))
		{
			nReads = (UINT)::fread(&nTextureCoords, sizeof(int), 1, pInFile);
			if (nTextureCoords > 0)
			{
				m_nType |= VERTEXT_TEXTURE_COORD1;
				m_pxmf2TextureCoords1 = new XMFLOAT2[nTextureCoords];
				nReads = (UINT)::fread(m_pxmf2TextureCoords1, sizeof(XMFLOAT2), nTextureCoords, pInFile);

				m_pd3dTextureCoord1Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf2TextureCoords1, sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTextureCoord1UploadBuffer);

				m_d3dTextureCoord1BufferView.BufferLocation = m_pd3dTextureCoord1Buffer->GetGPUVirtualAddress();
				m_d3dTextureCoord1BufferView.StrideInBytes = sizeof(XMFLOAT2);
				m_d3dTextureCoord1BufferView.SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;
			}
		}
		else if (!strcmp(pstrToken, "<Normals>:"))
		{
			nReads = (UINT)::fread(&nNormals, sizeof(int), 1, pInFile);
			if (nNormals > 0)
			{
				m_nType |= VERTEXT_NORMAL;
				m_pxmf3Normals = new XMFLOAT3[nNormals];
				nReads = (UINT)::fread(m_pxmf3Normals, sizeof(XMFLOAT3), nNormals, pInFile);

				m_pd3dNormalBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Normals, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dNormalUploadBuffer);

				m_d3dNormalBufferView.BufferLocation = m_pd3dNormalBuffer->GetGPUVirtualAddress();
				m_d3dNormalBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_d3dNormalBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
			}
		}
		else if (!strcmp(pstrToken, "<Tangents>:"))
		{
			nReads = (UINT)::fread(&nTangents, sizeof(int), 1, pInFile);
			if (nTangents > 0)
			{
				m_nType |= VERTEXT_TANGENT;
				m_pxmf3Tangents = new XMFLOAT3[nTangents];
				nReads = (UINT)::fread(m_pxmf3Tangents, sizeof(XMFLOAT3), nTangents, pInFile);

				m_pd3dTangentBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Tangents, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTangentUploadBuffer);

				m_d3dTangentBufferView.BufferLocation = m_pd3dTangentBuffer->GetGPUVirtualAddress();
				m_d3dTangentBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_d3dTangentBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
			}
		}
		else if (!strcmp(pstrToken, "<BiTangents>:"))
		{
			nReads = (UINT)::fread(&nBiTangents, sizeof(int), 1, pInFile);
			if (nBiTangents > 0)
			{
				m_pxmf3BiTangents = new XMFLOAT3[nBiTangents];
				nReads = (UINT)::fread(m_pxmf3BiTangents, sizeof(XMFLOAT3), nBiTangents, pInFile);

				m_pd3dBiTangentBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3BiTangents, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dBiTangentUploadBuffer);

				m_d3dBiTangentBufferView.BufferLocation = m_pd3dBiTangentBuffer->GetGPUVirtualAddress();
				m_d3dBiTangentBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_d3dBiTangentBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
			}
		}
		else if (!strcmp(pstrToken, "<SubMeshes>:"))
		{
			nReads = (UINT)::fread(&(m_nSubMeshes), sizeof(int), 1, pInFile);
			if (m_nSubMeshes > 0)
			{
				m_pnSubSetIndices = new int[m_nSubMeshes];
				m_ppnSubSetIndices = new UINT*[m_nSubMeshes];

				m_ppd3dSubSetIndexBuffers = new ID3D12Resource*[m_nSubMeshes];
				m_ppd3dSubSetIndexUploadBuffers = new ID3D12Resource*[m_nSubMeshes];
				m_pd3dSubSetIndexBufferViews = new D3D12_INDEX_BUFFER_VIEW[m_nSubMeshes];

				for (int i = 0; i < m_nSubMeshes; i++)
				{
					nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
					nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
					pstrToken[nStrLength] = '\0';
					if (!strcmp(pstrToken, "<SubMesh>:"))
					{
						int nIndex = 0;
						nReads = (UINT)::fread(&nIndex, sizeof(int), 1, pInFile);
						nReads = (UINT)::fread(&(m_pnSubSetIndices[i]), sizeof(int), 1, pInFile);
						if (m_pnSubSetIndices[i] > 0)
						{
							m_ppnSubSetIndices[i] = new UINT[m_pnSubSetIndices[i]];
							nReads = (UINT)::fread(m_ppnSubSetIndices[i], sizeof(UINT)*m_pnSubSetIndices[i], 1, pInFile);

							m_ppd3dSubSetIndexBuffers[i] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_ppnSubSetIndices[i], sizeof(UINT) * m_pnSubSetIndices[i], D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_ppd3dSubSetIndexUploadBuffers[i]);

							m_pd3dSubSetIndexBufferViews[i].BufferLocation = m_ppd3dSubSetIndexBuffers[i]->GetGPUVirtualAddress();
							m_pd3dSubSetIndexBufferViews[i].Format = DXGI_FORMAT_R32_UINT;
							m_pd3dSubSetIndexBufferViews[i].SizeInBytes = sizeof(UINT) * m_pnSubSetIndices[i];
						}
					}
				}
			}
		}
		else if (!strcmp(pstrToken, "</Mesh>"))
		{
			break;
		}
	}
}

void CStandardMesh::Render(ID3D12GraphicsCommandList *pd3dCommandList, int nSubSet)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[5] = { m_d3dPositionBufferView, m_d3dTextureCoord0BufferView, m_d3dNormalBufferView, m_d3dTangentBufferView, m_d3dBiTangentBufferView };
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 5, pVertexBufferViews);

	if ((m_nSubMeshes > 0) && (nSubSet < m_nSubMeshes))
	{
		pd3dCommandList->IASetIndexBuffer(&(m_pd3dSubSetIndexBufferViews[nSubSet]));
		pd3dCommandList->DrawIndexedInstanced(m_pnSubSetIndices[nSubSet], 1, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}

//==================================================================

CRawFormatImage::CRawFormatImage(LPCTSTR pFileName, int nWidth, int nLength, bool bFlipY)
{
	m_nWidth = nWidth;
	m_nLength = nLength;

	BYTE* pRawImagePixels = new BYTE[m_nWidth * m_nLength];

	HANDLE hFile = ::CreateFile(pFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, NULL);
	DWORD dwBytesRead;
	::ReadFile(hFile, pRawImagePixels, (m_nWidth * m_nLength), &dwBytesRead, NULL);
	::CloseHandle(hFile);

	if (bFlipY)
	{
		m_pRawImagePixels = new BYTE[m_nWidth * m_nLength];
		for (int z = 0; z < m_nLength; z++)
		{
			for (int x = 0; x < m_nWidth; x++)
			{
				m_pRawImagePixels[x + ((m_nLength - 1 - z) * m_nWidth)] = pRawImagePixels[x + (z * m_nWidth)];
			}
		}

		if (pRawImagePixels) delete[] pRawImagePixels;
	}
	else
	{
		m_pRawImagePixels = pRawImagePixels;
	}
}

CRawFormatImage::~CRawFormatImage()
{
	if (m_pRawImagePixels) delete[] m_pRawImagePixels;
	m_pRawImagePixels = NULL;
}

CHeightMapImage::CHeightMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale) : CRawFormatImage(pFileName, nWidth, nLength, true)
{
	m_xmf3Scale = xmf3Scale;
}

CHeightMapImage::~CHeightMapImage()
{
}

XMFLOAT3 CHeightMapImage::GetHeightMapNormal(int x, int z)
{
	if ((x < 0.0f) || (z < 0.0f) || (x >= m_nWidth) || (z >= m_nLength)) return(XMFLOAT3(0.0f, 1.0f, 0.0f));

	int nHeightMapIndex = x + (z * m_nWidth);
	int xHeightMapAdd = (x < (m_nWidth - 1)) ? 1 : -1;
	int zHeightMapAdd = (z < (m_nLength - 1)) ? m_nWidth : -m_nWidth;
	float y1 = (float)m_pRawImagePixels[nHeightMapIndex] * m_xmf3Scale.y;
	float y2 = (float)m_pRawImagePixels[nHeightMapIndex + xHeightMapAdd] * m_xmf3Scale.y;
	float y3 = (float)m_pRawImagePixels[nHeightMapIndex + zHeightMapAdd] * m_xmf3Scale.y;
	XMFLOAT3 xmf3Edge1 = XMFLOAT3(0.0f, y3 - y1, m_xmf3Scale.z);
	XMFLOAT3 xmf3Edge2 = XMFLOAT3(m_xmf3Scale.x, y2 - y1, 0.0f);
	XMFLOAT3 xmf3Normal = Vector3::CrossProduct(xmf3Edge1, xmf3Edge2, true);

	return(xmf3Normal);
}

#define _WITH_APPROXIMATE_OPPOSITE_CORNER

float CHeightMapImage::GetHeight(float fx, float fz, bool bReverseQuad)
{
	fx = fx / m_xmf3Scale.x;
	fz = fz / m_xmf3Scale.z;
	if ((fx < 0.0f) || (fz < 0.0f) || (fx >= m_nWidth) || (fz >= m_nLength)) return(0.0f);

	int x = (int)fx;
	int z = (int)fz;
	float fxPercent = fx - x;
	float fzPercent = fz - z;

	float fBottomLeft = (float)m_pRawImagePixels[x + (z * m_nWidth)];
	float fBottomRight = (float)m_pRawImagePixels[(x + 1) + (z * m_nWidth)];
	float fTopLeft = (float)m_pRawImagePixels[x + ((z + 1) * m_nWidth)];
	float fTopRight = (float)m_pRawImagePixels[(x + 1) + ((z + 1) * m_nWidth)];
#ifdef _WITH_APPROXIMATE_OPPOSITE_CORNER
	if (bReverseQuad)
	{
		if (fzPercent >= fxPercent)
			fBottomRight = fBottomLeft + (fTopRight - fTopLeft);
		else
			fTopLeft = fTopRight + (fBottomLeft - fBottomRight);
	}
	else
	{
		if (fzPercent < (1.0f - fxPercent))
			fTopRight = fTopLeft + (fBottomRight - fBottomLeft);
		else
			fBottomLeft = fTopLeft + (fBottomRight - fTopRight);
	}
#endif
	float fTopHeight = fTopLeft * (1 - fxPercent) + fTopRight * fxPercent;
	float fBottomHeight = fBottomLeft * (1 - fxPercent) + fBottomRight * fxPercent;
	float fHeight = fBottomHeight * (1 - fzPercent) + fTopHeight * fzPercent;

	return(fHeight);
}

CHeightMapGridMesh::CHeightMapGridMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int xStart, int zStart, int nWidth, int nLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color, void* pContext)
	: CStandardMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = nWidth * nLength;
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	m_nWidth = nWidth;
	m_nLength = nLength;
	m_xmf3Scale = xmf3Scale;

	m_pxmf3Positions = new XMFLOAT3[m_nVertices];
	m_pxmf3Normals = new XMFLOAT3[m_nVertices];
	m_pxmf2TextureCoords0 = new XMFLOAT2[m_nVertices];
	m_pxmf2TextureCoords1 = new XMFLOAT2[m_nVertices];

	CHeightMapImage* pHeightMapImage = (CHeightMapImage*)pContext;
	int cxHeightMap = pHeightMapImage->GetRawImageWidth();
	int czHeightMap = pHeightMapImage->GetRawImageLength();

	float fHeight = 0.0f, fMinHeight = +FLT_MAX, fMaxHeight = -FLT_MAX;

	for (int i = 0, z = zStart; z < (zStart + nLength); z++)
	{
		for (int x = xStart; x < (xStart + nWidth); x++, i++)
		{
			fHeight = OnGetHeight(x, z, pContext);
			m_pxmf3Positions[i] = XMFLOAT3((x * m_xmf3Scale.x), fHeight, (z * m_xmf3Scale.z));
			m_pxmf3Normals[i] = XMFLOAT3(0.0f, 1.0f, 0.0f);
			m_pxmf2TextureCoords0[i] = XMFLOAT2(float(x) / float(cxHeightMap - 1), float(czHeightMap - 1 - z) / float(czHeightMap - 1));

			float textureCoordScaleFactor = 5.0f; // 간격을 넓히기 스케일 팩터
			m_pxmf2TextureCoords1[i] = XMFLOAT2(float(x) / (m_xmf3Scale.x * textureCoordScaleFactor), float(z) / (m_xmf3Scale.z * textureCoordScaleFactor));
			if (fHeight < fMinHeight) fMinHeight = fHeight;
			if (fHeight > fMaxHeight) fMaxHeight = fHeight;

		}
	}

	// 서브메쉬의 개수는 1개만 사용하도록 설정
	m_nSubMeshes = 1;

	// 서브메쉬 인덱스 개수 및 할당
	int nSubMeshIndices = ((nWidth * 2) * (nLength - 1)) + ((nLength - 1) - 1); // 단일 서브메쉬의 인덱스 개수
	m_pnSubSetIndices = new int[m_nSubMeshes]; // 서브메쉬 인덱스 개수 저장 배열
	m_ppnSubSetIndices = new UINT * [m_nSubMeshes]; // 서브메쉬 인덱스 배열 포인터

	m_pnSubSetIndices[0] = nSubMeshIndices; // 첫 번째 서브메쉬의 인덱스 개수 설정
	m_ppnSubSetIndices[0] = new UINT[nSubMeshIndices]; // 첫 번째 서브메쉬의 인덱스 배열 할당

	for (int j = 0, z = 0; z < nLength - 1; z++)
	{
		if ((z % 2) == 0)
		{
			for (int x = 0; x < nWidth; x++)
			{
				if ((x == 0) && (z > 0)) 
					m_ppnSubSetIndices[0][j++] = (UINT)(x + (z * nWidth));
				m_ppnSubSetIndices[0][j++] = (UINT)(x + (z * nWidth));
				m_ppnSubSetIndices[0][j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
		else
		{
			for (int x = nWidth - 1; x >= 0; x--)
			{
				if (x == (nWidth - 1)) 
					m_ppnSubSetIndices[0][j++] = (UINT)(x + (z * nWidth));
				m_ppnSubSetIndices[0][j++] = (UINT)(x + (z * nWidth));
				m_ppnSubSetIndices[0][j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
	}




	// 서브메쉬 인덱스 버퍼 및 업로드 버퍼 생성
	m_ppd3dSubSetIndexBuffers = new ID3D12Resource * [m_nSubMeshes];
	m_ppd3dSubSetIndexUploadBuffers = new ID3D12Resource * [m_nSubMeshes];

	// 첫 번째 서브메쉬의 인덱스 버퍼 생성
	m_ppd3dSubSetIndexBuffers[0] = CreateBufferResource(
		pd3dDevice, pd3dCommandList, m_ppnSubSetIndices[0], sizeof(UINT) * nSubMeshIndices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, 
		&m_ppd3dSubSetIndexUploadBuffers[0]);

	// 서브메쉬 인덱스 버퍼 뷰 설정
	m_pd3dSubSetIndexBufferViews = new D3D12_INDEX_BUFFER_VIEW[m_nSubMeshes];
	m_pd3dSubSetIndexBufferViews[0].BufferLocation = m_ppd3dSubSetIndexBuffers[0]->GetGPUVirtualAddress();
	m_pd3dSubSetIndexBufferViews[0].Format = DXGI_FORMAT_R32_UINT;
	m_pd3dSubSetIndexBufferViews[0].SizeInBytes = sizeof(UINT) * nSubMeshIndices;

	//===========================================================
	m_pd3dPositionBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Positions, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

	m_d3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_d3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;

	//===========================================================
	m_pd3dNormalBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Normals, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dNormalUploadBuffer);

	m_d3dNormalBufferView.BufferLocation = m_pd3dNormalBuffer->GetGPUVirtualAddress();
	m_d3dNormalBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dNormalBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;

	//===========================================================
	m_pd3dTextureCoord0Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf2TextureCoords0, sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTextureCoord0UploadBuffer);

	m_d3dTextureCoord0BufferView.BufferLocation = m_pd3dTextureCoord0Buffer->GetGPUVirtualAddress();
	m_d3dTextureCoord0BufferView.StrideInBytes = sizeof(XMFLOAT2);
	m_d3dTextureCoord0BufferView.SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;
	//===========================================================
	m_pd3dTextureCoord1Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf2TextureCoords1, sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTextureCoord1UploadBuffer);

	m_d3dTextureCoord1BufferView.BufferLocation = m_pd3dTextureCoord1Buffer->GetGPUVirtualAddress();
	m_d3dTextureCoord1BufferView.StrideInBytes = sizeof(XMFLOAT2);
	m_d3dTextureCoord1BufferView.SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;
}

CHeightMapGridMesh::~CHeightMapGridMesh()
{
}

void CHeightMapGridMesh::ReleaseUploadBuffers()
{
	if (m_pd3dTextureCoord0UploadBuffer) m_pd3dTextureCoord0UploadBuffer->Release();
	m_pd3dTextureCoord0UploadBuffer = NULL;

	if (m_pd3dTextureCoord1UploadBuffer) m_pd3dTextureCoord1UploadBuffer->Release();
	m_pd3dTextureCoord1UploadBuffer = NULL;

}


float CHeightMapGridMesh::OnGetHeight(int x, int z, void* pContext)
{
	CHeightMapImage* pHeightMapImage = (CHeightMapImage*)pContext;
	BYTE* pHeightMapPixels = pHeightMapImage->GetRawImagePixels();
	XMFLOAT3 xmf3Scale = pHeightMapImage->GetScale();
	int nWidth = pHeightMapImage->GetRawImageWidth();
	float fHeight = pHeightMapPixels[x + (z * nWidth)] * xmf3Scale.y;
	return(fHeight);
}

XMFLOAT4 CHeightMapGridMesh::OnGetColor(int x, int z, void* pContext)
{
	XMFLOAT3 xmf3LightDirection = XMFLOAT3(-1.0f, 1.0f, 1.0f);
	xmf3LightDirection = Vector3::Normalize(xmf3LightDirection);
	CHeightMapImage* pHeightMapImage = (CHeightMapImage*)pContext;
	XMFLOAT3 xmf3Scale = pHeightMapImage->GetScale();
	XMFLOAT4 xmf4IncidentLightColor(0.9f, 0.8f, 0.4f, 1.0f);
	float fScale = Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x, z), xmf3LightDirection);
	fScale += Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x + 1, z), xmf3LightDirection);
	fScale += Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x + 1, z + 1), xmf3LightDirection);
	fScale += Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x, z + 1), xmf3LightDirection);
	fScale = (fScale / 4.0f) + 0.05f;
	if (fScale > 1.0f) fScale = 1.0f;
	if (fScale < 0.25f) fScale = 0.25f;
	XMFLOAT4 xmf4Color = Vector4::Multiply(fScale, xmf4IncidentLightColor);
	return(xmf4Color);
}

void CHeightMapGridMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList, int nSubSet)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[5] = { m_d3dPositionBufferView, m_d3dNormalBufferView, m_d3dTextureCoord0BufferView, m_d3dTextureCoord1BufferView };
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 5, pVertexBufferViews);

	if ((m_nSubMeshes > 0) && (nSubSet < m_nSubMeshes))
	{
		pd3dCommandList->IASetIndexBuffer(&(m_pd3dSubSetIndexBufferViews[nSubSet]));
		pd3dCommandList->DrawIndexedInstanced(m_pnSubSetIndices[nSubSet], 1, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}

CSphereMeshDiffused::CSphereMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fRadius, int nSlices, int nStacks)
	: CStandardMesh(pd3dDevice, pd3dCommandList)
{
	XMFLOAT3 color1 = { 0.5f,0.5f,0.8f };
	XMFLOAT3 color2 = { 0.0f,0.0f,0.5f };


	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_nVertices = 2 + (nSlices * (nStacks - 1));

	m_pxmf3Positions = new XMFLOAT3[m_nVertices];
	m_pxmf3Normals = new XMFLOAT3[m_nVertices];


	float fDeltaPhi = float(XM_PI / nStacks);
	float fDeltaTheta = float((2.0f * XM_PI) / nSlices);
	int k = 0;

	//구의 위(북극)를 나타내는 정점이다. 
	m_pxmf3Positions[k++] = XMFLOAT3(0.0f, +fRadius, 0.0f);

	float theta_i, phi_j;
	//원기둥 표면의 정점이다. 
	for (int j = 1; j < nStacks; j++)
	{
		phi_j = fDeltaPhi * j;
		for (int i = 0; i < nSlices; i++)
		{
			theta_i = fDeltaTheta * i;
			m_pxmf3Positions[k++] = XMFLOAT3(
				fRadius * sinf(phi_j) * cosf(theta_i),
				fRadius * cosf(phi_j), 
				fRadius * sinf(phi_j) * sinf(theta_i));
		}
	}
	//구의 아래(남극)를 나타내는 정점이다. 
	m_pxmf3Positions[k] = XMFLOAT3(0.0f, -fRadius, 0.0f);

	for (int i = 0; i < m_nVertices; ++i)
	{
		m_pxmf3Normals[i] = XMFLOAT3(rand() % 2 == 0 ? color1 : color2);
	}

	// 서브메쉬의 개수는 1개만 사용하도록 설정
	m_nSubMeshes = 1;

	// 서브메쉬 인덱스 개수 및 할당
	int nSubMeshIndices = (nSlices * 3) * 2 + (nSlices * (nStacks - 2) * 3 * 2);	 // 단일 서브메쉬의 인덱스 개수
	m_pnSubSetIndices = new int[m_nSubMeshes];									 // 서브메쉬 인덱스 개수 저장 배열
	m_ppnSubSetIndices = new UINT * [m_nSubMeshes];							 // 서브메쉬 인덱스 배열 포인터

	m_pnSubSetIndices[0] = nSubMeshIndices; // 첫 번째 서브메쉬의 인덱스 개수 설정
	m_ppnSubSetIndices[0] = new UINT[nSubMeshIndices]; // 첫 번째 서브메쉬의 인덱스 배열 할당


	k = 0;
	//구의 위쪽 원뿔의 표면을 표현하는 삼각형들의 인덱스이다. 
	for (int i = 0; i < nSlices; i++)
	{
		m_ppnSubSetIndices[0][k++] = 0;
		m_ppnSubSetIndices[0][k++] = 1 + ((i + 1) % nSlices);
		m_ppnSubSetIndices[0][k++] = 1 + i;
	}
	//구의 원기둥의 표면을 표현하는 삼각형들의 인덱스이다. 
	for (int j = 0; j < nStacks - 2; j++)
	{
		for (int i = 0; i < nSlices; i++)
		{
			//사각형의 첫 번째 삼각형의 인덱스이다. 
			m_ppnSubSetIndices[0][k++] = 1 + (i + (j * nSlices));
			m_ppnSubSetIndices[0][k++] = 1 + (((i + 1) % nSlices) + (j * nSlices));
			m_ppnSubSetIndices[0][k++] = 1 + (i + ((j + 1) * nSlices));
			//사각형의 두 번째 삼각형의 인덱스이다. 
			m_ppnSubSetIndices[0][k++] = 1 + (i + ((j + 1) * nSlices));
			m_ppnSubSetIndices[0][k++] = 1 + (((i + 1) % nSlices) + (j * nSlices));
			m_ppnSubSetIndices[0][k++] = 1 + (((i + 1) % nSlices) + ((j + 1) * nSlices));
		}
	}
	//구의 아래쪽 원뿔의 표면을 표현하는 삼각형들의 인덱스이다. 
	for (int i = 0; i < nSlices; i++)
	{
		m_ppnSubSetIndices[0][k++] = (m_nVertices - 1);
		m_ppnSubSetIndices[0][k++] = ((m_nVertices - 1) - nSlices) + i;
		m_ppnSubSetIndices[0][k++] = ((m_nVertices - 1) - nSlices) + ((i + 1) % nSlices);
	}
	
	// 서브메쉬 인덱스 버퍼 및 업로드 버퍼 생성
	m_ppd3dSubSetIndexBuffers = new ID3D12Resource * [m_nSubMeshes];
	m_ppd3dSubSetIndexUploadBuffers = new ID3D12Resource * [m_nSubMeshes];

	// 첫 번째 서브메쉬의 인덱스 버퍼 생성
	m_ppd3dSubSetIndexBuffers[0] = CreateBufferResource(
		pd3dDevice, pd3dCommandList, m_ppnSubSetIndices[0], sizeof(UINT) * nSubMeshIndices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER,
		&m_ppd3dSubSetIndexUploadBuffers[0]);

	// 서브메쉬 인덱스 버퍼 뷰 설정
	m_pd3dSubSetIndexBufferViews = new D3D12_INDEX_BUFFER_VIEW[m_nSubMeshes];
	m_pd3dSubSetIndexBufferViews[0].BufferLocation = m_ppd3dSubSetIndexBuffers[0]->GetGPUVirtualAddress();
	m_pd3dSubSetIndexBufferViews[0].Format = DXGI_FORMAT_R32_UINT;
	m_pd3dSubSetIndexBufferViews[0].SizeInBytes = sizeof(UINT) * nSubMeshIndices;

	//===========================================================
	m_pd3dPositionBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Positions, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

	m_d3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_d3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;

	//===========================================================
	m_pd3dNormalBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Normals, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dNormalUploadBuffer);

	m_d3dNormalBufferView.BufferLocation = m_pd3dNormalBuffer->GetGPUVirtualAddress();
	m_d3dNormalBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dNormalBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;


	mesh_bounding_box = new BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fRadius, fRadius, fRadius), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CSphereMeshDiffused::~CSphereMeshDiffused()
{
}

void CSphereMeshDiffused::Render(ID3D12GraphicsCommandList* pd3dCommandList, int nSubSet)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[2] = { m_d3dPositionBufferView, m_d3dNormalBufferView};
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 2, pVertexBufferViews);

	if ((m_nSubMeshes > 0) && (nSubSet < m_nSubMeshes))
	{
		pd3dCommandList->IASetIndexBuffer(&(m_pd3dSubSetIndexBufferViews[nSubSet]));
		pd3dCommandList->DrawIndexedInstanced(m_pnSubSetIndices[nSubSet], 1, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}
