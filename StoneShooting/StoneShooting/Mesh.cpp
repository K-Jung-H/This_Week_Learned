﻿#include "stdafx.h"
#include "Mesh.h"
#include <vector>
CMesh::CMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

CMesh::~CMesh()
{
	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();

	if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();
	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();

	if (m_pVertices) delete[] m_pVertices;
	if (m_pnIndices) delete[] m_pnIndices;

}

void CMesh::ReleaseUploadBuffers()
{
	//정점 버퍼를 위한 업로드 버퍼를 소멸시킨다. 
	if (m_pd3dVertexUploadBuffer) 
		m_pd3dVertexUploadBuffer->Release();
	m_pd3dVertexUploadBuffer = NULL;

	if (m_pd3dIndexUploadBuffer) 
		m_pd3dIndexUploadBuffer->Release();
	m_pd3dIndexUploadBuffer = NULL;
};

void CMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);

	//인덱스 버퍼가 있으면 인덱스 버퍼를 파이프라인(IA: 입력 조립기)에 연결하고 인덱스를 사용하여 렌더링한다. 
	if (m_pd3dIndexBuffer)
	{
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}

void CMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList, bool picked)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	if (picked)
	{
		pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &picked_m_d3dVertexBufferView);

	}
	else
	{
		pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);
	}

	//인덱스 버퍼가 있으면 인덱스 버퍼를 파이프라인(IA: 입력 조립기)에 연결하고 인덱스를 사용하여 렌더링한다. 
	if (m_pd3dIndexBuffer)
	{
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////

CCubeMeshDiffused::CCubeMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
	* pd3dCommandList, float fWidth, float fHeight, float fDepth, XMFLOAT4 fcolor1, XMFLOAT4 fcolor2) : CMesh(pd3dDevice,
		pd3dCommandList)
{
	//직육면체는 꼭지점(정점)이 8개이다. 
	m_nVertices = 8;
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;



	//정점 버퍼는 직육면체의 꼭지점 8개에 대한 정점 데이터를 가진다. 
	m_pVertices = new CDiffusedVertex[m_nVertices];
	m_pVertices[0] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), fcolor1);
	m_pVertices[1] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), fcolor2);
	m_pVertices[2] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), fcolor1);
	m_pVertices[3] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), fcolor2);
	m_pVertices[4] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), fcolor1);
	m_pVertices[5] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), fcolor2);
	m_pVertices[6] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), fcolor1);
	m_pVertices[7] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), fcolor2);

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices, m_nStride * m_nVertices, 
		D3D12_HEAP_TYPE_DEFAULT,D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, 
		&m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;


	/*인덱스 버퍼는 직육면체의 6개의 면(사각형)에 대한 기하 정보를 갖는다. 삼각형 리스트로 직육면체를 표현할 것이
	므로 각 면은 2개의 삼각형을 가지고 각 삼각형은 3개의 정점이 필요하다. 즉, 인덱스 버퍼는 전체 36(=6*2*3)개의 인
	덱스를 가져야 한다.*/
	m_nIndices = 36;
	m_pnIndices = new UINT[m_nIndices];
	//ⓐ 앞면(Front) 사각형의 위쪽 삼각형
	m_pnIndices[0] = 3; m_pnIndices[1] = 1; m_pnIndices[2] = 0;

	//ⓑ 앞면(Front) 사각형의 아래쪽 삼각형
	m_pnIndices[3] = 2; m_pnIndices[4] = 1; m_pnIndices[5] = 3;

	//ⓒ 윗면(Top) 사각형의 위쪽 삼각형
	m_pnIndices[6] = 0; m_pnIndices[7] = 5; m_pnIndices[8] = 4;

	//ⓓ 윗면(Top) 사각형의 아래쪽 삼각형
	m_pnIndices[9] = 1; m_pnIndices[10] = 5; m_pnIndices[11] = 0;

	//ⓔ 뒷면(Back) 사각형의 위쪽 삼각형
	m_pnIndices[12] = 3; m_pnIndices[13] = 4; m_pnIndices[14] = 7;

	//ⓕ 뒷면(Back) 사각형의 아래쪽 삼각형
	m_pnIndices[15] = 0; m_pnIndices[16] = 4; m_pnIndices[17] = 3;

	//ⓖ 아래면(Bottom) 사각형의 위쪽 삼각형
	m_pnIndices[18] = 1; m_pnIndices[19] = 6; m_pnIndices[20] = 5;

	//ⓗ 아래면(Bottom) 사각형의 아래쪽 삼각형
	m_pnIndices[21] = 2; m_pnIndices[22] = 6; m_pnIndices[23] = 1;

	//ⓘ 옆면(Left) 사각형의 위쪽 삼각형
	m_pnIndices[24] = 2; m_pnIndices[25] = 7; m_pnIndices[26] = 6;

	//ⓙ 옆면(Left) 사각형의 아래쪽 삼각형
	m_pnIndices[27] = 3; m_pnIndices[28] = 7; m_pnIndices[29] = 2;

	//ⓚ 옆면(Right) 사각형의 위쪽 삼각형
	m_pnIndices[30] = 6; m_pnIndices[31] = 4; m_pnIndices[32] = 5;

	//ⓛ 옆면(Right) 사각형의 아래쪽 삼각형
	m_pnIndices[33] = 7; m_pnIndices[34] = 4; m_pnIndices[35] = 6;


	//인덱스 버퍼를 생성한다. 
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pnIndices,
		sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER,
		&m_pd3dIndexUploadBuffer);

	//인덱스 버퍼 뷰를 생성한다. 
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;


	picked_m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		&picked_m_pd3dVertexUploadBuffer);

	picked_m_d3dVertexBufferView.BufferLocation = picked_m_pd3dVertexBuffer->GetGPUVirtualAddress();
	picked_m_d3dVertexBufferView.StrideInBytes = m_nStride;
	picked_m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;


	//메쉬의 바운딩 박스(모델 좌표계)를 생성한다.
	m_xmBoundingBox = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fx, fy, fz), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CCubeMeshDiffused::~CCubeMeshDiffused()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////

CWallMesh::CWallMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	float fWidth, float fHeight, float fDepth, XMFLOAT4 xmf4Color) : CMesh(pd3dDevice, pd3dCommandList)
{
	int nSubRects = 20;

	float fHalfWidth = fWidth * 0.5f;
	float fHalfHeight = fHeight * 0.5f;
	float fHalfDepth = fDepth * 0.5f;
	float fCellWidth = fWidth * (1.0f / nSubRects);
	float fCellHeight = fHeight * (1.0f / nSubRects);
	float fCellDepth = fDepth * (1.0f / nSubRects);

	m_nVertices = 0;
	m_nStride = sizeof(CDiffusedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


	std::vector<CDiffusedVertex> tempVertices;
	XMFLOAT4 back_and_front_color(0.0f, 0.0f, 0.3f,1.0f);

	// Left
	for (int i = 0; i < nSubRects; i++) {
		for (int j = 0; j < nSubRects; j++) {
			XMFLOAT3 v0(-fHalfWidth, -fHalfHeight + (i * fCellHeight), -fHalfDepth + (j * fCellDepth));
			XMFLOAT3 v1(-fHalfWidth, -fHalfHeight + ((i + 1) * fCellHeight), -fHalfDepth + (j * fCellDepth));
			XMFLOAT3 v2(-fHalfWidth, -fHalfHeight + ((i + 1) * fCellHeight), -fHalfDepth + ((j + 1) * fCellDepth));
			XMFLOAT3 v3(-fHalfWidth, -fHalfHeight + (i * fCellHeight), -fHalfDepth + ((j + 1) * fCellDepth));

			tempVertices.push_back(CDiffusedVertex(v0, Vector4::Add(xmf4Color, RANDOM_COLOR)));
			tempVertices.push_back(CDiffusedVertex(v1, Vector4::Add(xmf4Color, RANDOM_COLOR)));
			tempVertices.push_back(CDiffusedVertex(v2, Vector4::Add(xmf4Color, RANDOM_COLOR)));

			tempVertices.push_back(CDiffusedVertex(v0, Vector4::Add(xmf4Color, RANDOM_COLOR)));
			tempVertices.push_back(CDiffusedVertex(v2, Vector4::Add(xmf4Color, RANDOM_COLOR)));
			tempVertices.push_back(CDiffusedVertex(v3, Vector4::Add(xmf4Color, RANDOM_COLOR)));
		}
	}

	// Right
	for (int i = 0; i < nSubRects; i++) {
		for (int j = 0; j < nSubRects; j++) {
			XMFLOAT3 v0(+fHalfWidth, -fHalfHeight + (i * fCellHeight), -fHalfDepth + (j * fCellDepth));
			XMFLOAT3 v1(+fHalfWidth, -fHalfHeight + ((i + 1) * fCellHeight), -fHalfDepth + (j * fCellDepth));
			XMFLOAT3 v2(+fHalfWidth, -fHalfHeight + ((i + 1) * fCellHeight), -fHalfDepth + ((j + 1) * fCellDepth));
			XMFLOAT3 v3(+fHalfWidth, -fHalfHeight + (i * fCellHeight), -fHalfDepth + ((j + 1) * fCellDepth));

			tempVertices.push_back(CDiffusedVertex(v0, Vector4::Add(xmf4Color, RANDOM_COLOR)));
			tempVertices.push_back(CDiffusedVertex(v2, Vector4::Add(xmf4Color, RANDOM_COLOR)));
			tempVertices.push_back(CDiffusedVertex(v1, Vector4::Add(xmf4Color, RANDOM_COLOR)));

			tempVertices.push_back(CDiffusedVertex(v0, Vector4::Add(xmf4Color, RANDOM_COLOR)));
			tempVertices.push_back(CDiffusedVertex(v3, Vector4::Add(xmf4Color, RANDOM_COLOR)));
			tempVertices.push_back(CDiffusedVertex(v2, Vector4::Add(xmf4Color, RANDOM_COLOR)));
		}
	}

	// TopFace
	for (int i = 0; i < nSubRects; i++) {
		for (int j = 0; j < nSubRects; j++) {
			XMFLOAT3 v0(-fHalfWidth + (i * fCellWidth), +fHalfHeight, -fHalfDepth + (j * fCellDepth));
			XMFLOAT3 v1(-fHalfWidth + ((i + 1) * fCellWidth), +fHalfHeight, -fHalfDepth + (j * fCellDepth));
			XMFLOAT3 v2(-fHalfWidth + ((i + 1) * fCellWidth), +fHalfHeight, -fHalfDepth + ((j + 1) * fCellDepth));
			XMFLOAT3 v3(-fHalfWidth + (i * fCellWidth), +fHalfHeight, -fHalfDepth + ((j + 1) * fCellDepth));

			tempVertices.push_back(CDiffusedVertex(v0, Vector4::Add(xmf4Color, RANDOM_COLOR)));
			tempVertices.push_back(CDiffusedVertex(v1, Vector4::Add(xmf4Color, RANDOM_COLOR)));
			tempVertices.push_back(CDiffusedVertex(v2, Vector4::Add(xmf4Color, RANDOM_COLOR)));

			tempVertices.push_back(CDiffusedVertex(v0, Vector4::Add(xmf4Color, RANDOM_COLOR)));
			tempVertices.push_back(CDiffusedVertex(v2, Vector4::Add(xmf4Color, RANDOM_COLOR)));
			tempVertices.push_back(CDiffusedVertex(v3, Vector4::Add(xmf4Color, RANDOM_COLOR)));
		}
	}

	// BottomFace
	for (int i = 0; i < nSubRects; i++) {
		for (int j = 0; j < nSubRects; j++) {
			XMFLOAT3 v0(-fHalfWidth + (i * fCellWidth), -fHalfHeight, -fHalfDepth + (j * fCellDepth));
			XMFLOAT3 v1(-fHalfWidth + ((i + 1) * fCellWidth), -fHalfHeight, -fHalfDepth + (j * fCellDepth));
			XMFLOAT3 v2(-fHalfWidth + ((i + 1) * fCellWidth), -fHalfHeight, -fHalfDepth + ((j + 1) * fCellDepth));
			XMFLOAT3 v3(-fHalfWidth + (i * fCellWidth), -fHalfHeight, -fHalfDepth + ((j + 1) * fCellDepth));

			tempVertices.push_back(CDiffusedVertex(v0, Vector4::Add(xmf4Color, RANDOM_COLOR)));
			tempVertices.push_back(CDiffusedVertex(v2, Vector4::Add(xmf4Color, RANDOM_COLOR)));
			tempVertices.push_back(CDiffusedVertex(v1, Vector4::Add(xmf4Color, RANDOM_COLOR)));

			tempVertices.push_back(CDiffusedVertex(v0, Vector4::Add(xmf4Color, RANDOM_COLOR)));
			tempVertices.push_back(CDiffusedVertex(v3, Vector4::Add(xmf4Color, RANDOM_COLOR)));
			tempVertices.push_back(CDiffusedVertex(v2, Vector4::Add(xmf4Color, RANDOM_COLOR)));
		}
	}

	// FrontFace
	for (int i = 0; i < nSubRects; i++) {
		for (int j = 0; j < nSubRects; j++) {
			XMFLOAT3 v0(-fHalfWidth + (i * fCellWidth), -fHalfHeight + (j * fCellHeight), -fHalfDepth);
			XMFLOAT3 v1(-fHalfWidth + ((i + 1) * fCellWidth), -fHalfHeight + (j * fCellHeight), -fHalfDepth);
			XMFLOAT3 v2(-fHalfWidth + ((i + 1) * fCellWidth), -fHalfHeight + ((j + 1) * fCellHeight), -fHalfDepth);
			XMFLOAT3 v3(-fHalfWidth + (i * fCellWidth), -fHalfHeight + ((j + 1) * fCellHeight), -fHalfDepth);

			tempVertices.push_back(CDiffusedVertex(v0, Vector4::Add(back_and_front_color, RANDOM_COLOR)));
			tempVertices.push_back(CDiffusedVertex(v1, Vector4::Add(back_and_front_color, RANDOM_COLOR)));
			tempVertices.push_back(CDiffusedVertex(v2, Vector4::Add(back_and_front_color, RANDOM_COLOR)));

			tempVertices.push_back(CDiffusedVertex(v0, Vector4::Add(back_and_front_color, RANDOM_COLOR)));
			tempVertices.push_back(CDiffusedVertex(v2, Vector4::Add(back_and_front_color, RANDOM_COLOR)));
			tempVertices.push_back(CDiffusedVertex(v3, Vector4::Add(back_and_front_color, RANDOM_COLOR)));
		}
	}

	// BackFace
	for (int i = 0; i < nSubRects; i++) {
		for (int j = 0; j < nSubRects; j++) {
			XMFLOAT3 v0(-fHalfWidth + (i * fCellWidth), -fHalfHeight + (j * fCellHeight), +fHalfDepth);
			XMFLOAT3 v1(-fHalfWidth + ((i + 1) * fCellWidth), -fHalfHeight + (j * fCellHeight), +fHalfDepth);
			XMFLOAT3 v2(-fHalfWidth + ((i + 1) * fCellWidth), -fHalfHeight + ((j + 1) * fCellHeight), +fHalfDepth);
			XMFLOAT3 v3(-fHalfWidth + (i * fCellWidth), -fHalfHeight + ((j + 1) * fCellHeight), +fHalfDepth);

			tempVertices.push_back(CDiffusedVertex(v0, Vector4::Add(back_and_front_color, RANDOM_COLOR)));
			tempVertices.push_back(CDiffusedVertex(v2, Vector4::Add(back_and_front_color, RANDOM_COLOR)));
			tempVertices.push_back(CDiffusedVertex(v1, Vector4::Add(back_and_front_color, RANDOM_COLOR)));

			tempVertices.push_back(CDiffusedVertex(v0, Vector4::Add(back_and_front_color, RANDOM_COLOR)));
			tempVertices.push_back(CDiffusedVertex(v3, Vector4::Add(back_and_front_color, RANDOM_COLOR)));
			tempVertices.push_back(CDiffusedVertex(v2, Vector4::Add(back_and_front_color, RANDOM_COLOR)));
		}
	}

	m_nVertices = tempVertices.size();
	m_pVertices = new CDiffusedVertex[m_nVertices];  // 최종 크기로 동적 할당
	std::copy(tempVertices.begin(), tempVertices.end(), m_pVertices);

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices, m_nStride * m_nVertices, 
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;


	m_xmBoundingBox = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth , fHalfHeight , fHalfDepth ), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}


CPlaneMesh::CPlaneMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fDepth, XMFLOAT4 xmf4Color) : CMesh(pd3dDevice, pd3dCommandList)
{
	int nSubRects = 20;

	float fHalfWidth = fWidth * 0.5f;
	float fHalfDepth = fDepth * 0.5f;
	float fCellWidth = fWidth / nSubRects;
	float fCellDepth = fDepth / nSubRects;

	XMFLOAT4 color1 = XMFLOAT4(xmf4Color.x+0.1f, xmf4Color.y, xmf4Color.z, 1.0f);
	XMFLOAT4 color2 = XMFLOAT4(xmf4Color.x + 0.2f, xmf4Color.y + 0.1f, xmf4Color.z, 1.0f);
	XMFLOAT4 color3 = XMFLOAT4(xmf4Color.x + 0.1f, xmf4Color.y + 0.1f, xmf4Color.z, 1.0f);

	m_nVertices = 0;
	m_nStride = sizeof(CDiffusedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	std::vector<CDiffusedVertex> tempVertices;

	for (int i = 0; i < nSubRects; i++) {
		for (int j = 0; j < nSubRects; j++) {
			XMFLOAT3 v0(-fHalfWidth + (i * fCellWidth), 0.0f, -fHalfDepth + (j * fCellDepth));
			XMFLOAT3 v1(-fHalfWidth + ((i + 1) * fCellWidth), 0.0f, -fHalfDepth + (j * fCellDepth));
			XMFLOAT3 v2(-fHalfWidth + ((i + 1) * fCellWidth), 0.0f, -fHalfDepth + ((j + 1) * fCellDepth));
			XMFLOAT3 v3(-fHalfWidth + (i * fCellWidth), 0.0f, -fHalfDepth + ((j + 1) * fCellDepth));

			// Calculate average color
			XMFLOAT4 randomColor = (rand() % 3 == 0) ? color1 : (rand() % 2 == 0) ? color2 : color3;

			tempVertices.push_back(CDiffusedVertex(v0, randomColor));
			tempVertices.push_back(CDiffusedVertex(v2, randomColor));
			tempVertices.push_back(CDiffusedVertex(v1, randomColor));

			tempVertices.push_back(CDiffusedVertex(v0, randomColor));
			tempVertices.push_back(CDiffusedVertex(v3, randomColor));
			tempVertices.push_back(CDiffusedVertex(v2, randomColor));
		}
	}

	m_nVertices = tempVertices.size();
	m_pVertices = new CDiffusedVertex[m_nVertices];
	std::copy(tempVertices.begin(), tempVertices.end(), m_pVertices);

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices, m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	m_xmBoundingBox = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth, 10.0f, fHalfDepth), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CPlaneMesh::~CPlaneMesh() {
	if (m_pVertices) delete[] m_pVertices;
	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
}



//////////////////////////////////////////////////////////////////////////////////////////////


CAirplaneMeshDiffused::CAirplaneMeshDiffused(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight, float fDepth,
	XMFLOAT4 xmf4Color) : CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 24 * 3;
	m_nStride = sizeof(CDiffusedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;

	//비행기 메쉬를 표현하기 위한 정점 데이터이다.
	m_pVertices = new CDiffusedVertex[m_nVertices];
	float x1 = fx * 0.2f, y1 = fy * 0.2f, x2 = fx * 0.1f, y3 = fy * 0.3f, y2 = ((y1 - (fy - y3)) / x1) * x2 + (fy - y3);
	int i = 0;

	//비행기 메쉬의 위쪽 면
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz),Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color,RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color,RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz),Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color,RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color,RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color,RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color,RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color,RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color,RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color,RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color,RANDOM_COLOR));

	//비행기 메쉬의 아래쪽 면
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color,RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color,RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color,RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color,RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color,RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color,RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color,RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));

	//비행기 메쉬의 오른쪽 면
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	//비행기 메쉬의 뒤쪽/오른쪽 면
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	//비행기 메쉬의 왼쪽 면
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	//비행기 메쉬의 뒤쪽/왼쪽 면
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	//메쉬의 바운딩 박스(모델 좌표계)를 생성한다. 
	m_xmBoundingBox = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fx, fy, fz), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CAirplaneMeshDiffused::~CAirplaneMeshDiffused()
{
}

void CAirplaneMeshDiffused::Make_Another_Color_Mesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (int i = 0; i < m_nVertices; i++)
	{
		m_pVertices[i].Change_Diffused_Color(Vector4::Add(XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f), RANDOM_COLOR));
	}

	picked_m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, 
		&picked_m_pd3dVertexUploadBuffer);

	picked_m_d3dVertexBufferView.BufferLocation = picked_m_pd3dVertexBuffer->GetGPUVirtualAddress();
	picked_m_d3dVertexBufferView.StrideInBytes = m_nStride;
	picked_m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CSphereMeshDiffused::CSphereMeshDiffused(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList, float fRadius, int nSlices, int nStacks, bool black) :
	CMesh(pd3dDevice, pd3dCommandList)
{
	XMFLOAT4 stone_color1;
	XMFLOAT4 stone_color2;

	float y_scale = 0.5f;

	if (black)
	{
		stone_color1 = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
		stone_color2 = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	}
	else
	{
		stone_color1 = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
		stone_color2 = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	}
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	/*원기둥의 표면에 있는 사각형(줄)의 개수는 {nSlices * (nStacks-2)}이다. 사각형들이 원기둥의 표면을 따라 연속되
	고 처음과 마지막 사각형이 연결되어 있으므로 첫 번째 원기둥을 제외하고 사각형 하나를 표현하기 위하여 하나의 정
	점이 필요하다. 첫 번째 원기둥은 위아래로 두 개의 정점이 필요하므로 원기둥의 표면의 사각형들을 표현하기 위하여
	필요한 정점의 개수는 {(nSlices * (nStacks–1))}이다. 그런데 구의 위와 아래(구가 지구라고 가정할 때 남극과 북극)
	를 자르면 원기둥이 아니라 원뿔이 되므로 이 원뿔을 표현하기 위하여 2개의 정점이 더 필요하다. 그러므로 정점의 전
	체 개수는 {(nSlices * (nStacks–1)) + 2}이다.*/
	m_nVertices = 2 + (nSlices * (nStacks - 1));
	m_pVertices = new CDiffusedVertex[m_nVertices];

	//180도를 nStacks 만큼 분할한다. 
	float fDeltaPhi = float(XM_PI / nStacks);
	//360도를 nSlices 만큼 분할한다. 

	float fDeltaTheta = float((2.0f * XM_PI) / nSlices);
	int k = 0;



	//구의 위(북극)를 나타내는 정점이다. 
	m_pVertices[k++] = CDiffusedVertex(0.0f, +fRadius * y_scale, 0.0f, (rand() % 2 == 0 ? stone_color1 : stone_color2)
	);

	float theta_i, phi_j;
	//원기둥 표면의 정점이다. 
	for (int j = 1; j < nStacks; j++)
	{
		phi_j = fDeltaPhi * j;
		for (int i = 0; i < nSlices; i++)
		{
			theta_i = fDeltaTheta * i;
			m_pVertices[k++] = CDiffusedVertex(
				fRadius * sinf(phi_j) * cosf(theta_i), 
				fRadius * cosf(phi_j) * y_scale,
				fRadius * sinf(phi_j) * sinf(theta_i), 
				(rand() % 2 == 0 ? stone_color1 : stone_color2));
		}
	}
	//구의 아래(남극)를 나타내는 정점이다. 
	m_pVertices[k] = CDiffusedVertex(0.0f, -fRadius * y_scale, 0.0f, (rand() % 2 == 0 ? stone_color1 : stone_color2));

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
	/*원기둥의 표면에 존재하는 사각형의 개수는 {nSlices * (nStacks-2)}이고 사각형은 2개의 삼각형으로 구성되므로
	삼각형 리스트일 때 필요한 인덱스의 개수는 {nSlices * (nStacks-2) * 2 * 3}이다. 그리고 구의 위아래 원뿔의 표면
	에 존재하는 삼각형의 개수는 nSlices개이므로 구의 위아래 원뿔을 표현하기 위한 인덱스의 개수는 {(nSlices * 3) *
	2}이다. 그러므로 구의 표면을 삼각형 리스트로 표현하기 위하여 필요한 인덱스의 개수는 {(nSlices * 3) * 2 +
	(nSlices * (nStacks - 2) * 3 * 2)}이다*/
	m_nIndices = (nSlices * 3) * 2 + (nSlices * (nStacks - 2) * 3 * 2);
	m_pnIndices = new UINT[m_nIndices];
	k = 0;
	//구의 위쪽 원뿔의 표면을 표현하는 삼각형들의 인덱스이다. 
	for (int i = 0; i < nSlices; i++)
	{
		m_pnIndices[k++] = 0;
		m_pnIndices[k++] = 1 + ((i + 1) % nSlices);
		m_pnIndices[k++] = 1 + i;
	}
	//구의 원기둥의 표면을 표현하는 삼각형들의 인덱스이다. 
	for (int j = 0; j < nStacks-2; j++)
	{
		for (int i = 0; i < nSlices; i++)
		{
			//사각형의 첫 번째 삼각형의 인덱스이다. 
			m_pnIndices[k++] = 1 + (i + (j * nSlices));
			m_pnIndices[k++] = 1 + (((i + 1) % nSlices) + (j * nSlices));
			m_pnIndices[k++] = 1 + (i + ((j + 1) * nSlices));
			//사각형의 두 번째 삼각형의 인덱스이다. 
			m_pnIndices[k++] = 1 + (i + ((j + 1) * nSlices));
			m_pnIndices[k++] = 1 + (((i + 1) % nSlices) + (j * nSlices));
			m_pnIndices[k++] = 1 + (((i + 1) % nSlices) + ((j + 1) * nSlices));
		}
	}
	//구의 아래쪽 원뿔의 표면을 표현하는 삼각형들의 인덱스이다. 
	for (int i = 0; i < nSlices; i++)
	{
		m_pnIndices[k++] = (m_nVertices - 1);
		m_pnIndices[k++] = ((m_nVertices - 1) - nSlices) + i;
		m_pnIndices[k++] = ((m_nVertices - 1) - nSlices) + ((i + 1) % nSlices);
	}
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pnIndices,
		sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;



	m_xmBoundingSphere = BoundingSphere(XMFLOAT3(0.0f, 0.0f, 0.0f), fRadius);
	m_xmBoundingBox = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fRadius,fRadius, fRadius), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CSphereMeshDiffused::~CSphereMeshDiffused()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int CMesh::CheckRayIntersection(XMFLOAT3& xmf3RayOrigin, XMFLOAT3& xmf3RayDirection,
	float* pfNearHitDistance)
{
	//하나의 메쉬에서 광선은 여러 개의 삼각형과 교차할 수 있다. 교차하는 삼각형들 중 가장 가까운 삼각형을 찾는다. 
	int nIntersections = 0;
	BYTE* pbPositions = (BYTE*)m_pVertices;
	int nOffset = (m_d3dPrimitiveTopology == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? 3 : 1;

	/*메쉬의 프리미티브(삼각형)들의 개수이다. 삼각형 리스트인 경우 (정점의 개수 / 3) 또는 (인덱스의 개수 / 3), 삼각
	형 스트립의 경우 (정점의 개수 - 2) 또는 (인덱스의 개수 – 2)이다.*/
	int nPrimitives = (m_d3dPrimitiveTopology == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ?
		(m_nVertices / 3) : (m_nVertices - 2);
	if (m_nIndices > 0) nPrimitives = (m_d3dPrimitiveTopology ==
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nIndices / 3) : (m_nIndices - 2);

	//광선은 모델 좌표계로 표현된다. 
	XMVECTOR xmRayOrigin = XMLoadFloat3(&xmf3RayOrigin);
	XMVECTOR xmRayDirection = XMLoadFloat3(&xmf3RayDirection);

	//모델 좌표계의 광선과 메쉬의 바운딩 박스(모델 좌표계)와의 교차를 검사한다. 
	bool bIntersected = m_xmBoundingBox.Intersects(xmRayOrigin, xmRayDirection,
		*pfNearHitDistance);

	//모델 좌표계의 광선이 메쉬의 바운딩 박스와 교차하면 메쉬와의 교차를 검사한다. 
	if (bIntersected)
	{
		float fNearHitDistance = FLT_MAX;
		/*메쉬의 모든 프리미티브(삼각형)들에 대하여 픽킹 광선과의 충돌을 검사한다.
		충돌하는 모든 삼각형을 찾아 광선의 시작점(실제로는 카메라 좌표계의 원점)에 가장 가까운 삼각형을 찾는다.*/

		for (int i = 0; i < nPrimitives; i++)
		{
			XMVECTOR v0 = XMLoadFloat3((XMFLOAT3*)(pbPositions + ((m_pnIndices) ?
				(m_pnIndices[(i * nOffset) + 0]) : ((i * nOffset) + 0)) * m_nStride));
			XMVECTOR v1 = XMLoadFloat3((XMFLOAT3*)(pbPositions + ((m_pnIndices) ?
				(m_pnIndices[(i * nOffset) + 1]) : ((i * nOffset) + 1)) * m_nStride));
			XMVECTOR v2 = XMLoadFloat3((XMFLOAT3*)(pbPositions + ((m_pnIndices) ?
				(m_pnIndices[(i * nOffset) + 2]) : ((i * nOffset) + 2)) * m_nStride));
			float fHitDistance;
			BOOL bIntersected = TriangleTests::Intersects(xmRayOrigin, xmRayDirection, v0,
				v1, v2, fHitDistance);
			if (bIntersected)
			{
				if (fHitDistance < fNearHitDistance)
				{
					*pfNearHitDistance = fNearHitDistance = fHitDistance;
				}
				nIntersections++;
			}
		}
	}
	return(nIntersections);
}

