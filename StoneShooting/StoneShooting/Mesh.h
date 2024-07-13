#pragma once
#include "stdafx.h"

//������ ǥ���ϱ� ���� Ŭ������ �����Ѵ�. 

class CVertex
{

protected:
	//������ ��ġ �����̴�(��� ������ �ּ��� ��ġ ���͸� ������ �Ѵ�). 
	XMFLOAT3 m_xmf3Position;

public:
	CVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); }
	CVertex(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	~CVertex() { }
};


class CDiffusedVertex : public CVertex
{

protected:
	//������ �����̴�. 
	XMFLOAT4 m_xmf4Diffuse;

public:
	CDiffusedVertex() {
		m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); 
		m_xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	CDiffusedVertex(float x, float y, float z, XMFLOAT4 xmf4Diffuse) {
		m_xmf3Position = XMFLOAT3(x, y, z); 
		m_xmf4Diffuse = xmf4Diffuse;
	}
	CDiffusedVertex(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Diffuse) {
		m_xmf3Position = xmf3Position; 
		m_xmf4Diffuse = xmf4Diffuse;
	}
	~CDiffusedVertex() { }

	void Change_Diffused_Color(XMFLOAT4 xmf4Diffuse) { m_xmf4Diffuse = xmf4Diffuse; }
};
 
class CMesh
{
protected:
	ID3D12Resource* m_pd3dIndexBuffer = NULL;
	ID3D12Resource* m_pd3dIndexUploadBuffer = NULL;

	/*�ε��� ����(�ε����� �迭)�� �ε��� ���۸� ���� ���ε� ���ۿ� ���� �������̽� �������̴�. 
	�ε��� ���۴� ���� ����(�迭)�� ���� �ε����� ������.*/
	D3D12_INDEX_BUFFER_VIEW m_d3dIndexBufferView;
	UINT m_nIndices = 0;
	//�ε��� ���ۿ� ���ԵǴ� �ε����� �����̴�. 

	UINT m_nStartIndex = 0;
	//�ε��� ���ۿ��� �޽��� �׸��� ���� ���Ǵ� ���� �ε����̴�. 
	
	int m_nBaseVertex = 0;
	//�ε��� ������ �ε����� ������ �ε����̴�. 



	ID3D12Resource* m_pd3dVertexBuffer = NULL;
	ID3D12Resource* m_pd3dVertexUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW m_d3dVertexBufferView;
	D3D12_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT m_nSlot = 0;
	UINT m_nVertices = 0;
	UINT m_nStride = 0;
	UINT m_nOffset = 0;

	//������ ��ŷ�� ���Ͽ� �����Ѵ�(���� ���۸� Map()�Ͽ� ���� �ʾƵ� �ǵ���).
	CDiffusedVertex* m_pVertices = NULL;

	//�޽��� �ε����� �����Ѵ�(�ε��� ���۸� Map()�Ͽ� ���� �ʾƵ� �ǵ���).
	UINT* m_pnIndices = NULL;

private:
	int m_nReferences = 0;

public:
	CMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CMesh();

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, bool picked);

	BoundingOrientedBox GetBoundingBox() { return(m_xmBoundingBox); }

	//������ �޽��� ������ �˻��ϰ� �����ϴ� Ƚ���� �Ÿ��� ��ȯ�ϴ� �Լ��̴�. 
	int CheckRayIntersection(XMFLOAT3& xmRayPosition, XMFLOAT3& xmRayDirection, float* pfNearHitDistance);
public:
	//�� ��ǥ���� OOBB �ٿ�� �ڽ��̴�. 
	BoundingOrientedBox m_xmBoundingBox;
	BoundingSphere m_xmBoundingSphere;
	bool picked = false;

	// ��ŷ���� ��� ����� �޽� ���ҽ� ����
	ID3D12Resource* picked_m_pd3dVertexBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW picked_m_d3dVertexBufferView;
	ID3D12Resource* picked_m_pd3dVertexUploadBuffer;
};
//////////////////////////////////////////////////////////////////////////////////////

class CCubeMeshDiffused : public CMesh
{
public:
	//������ü�� ����, ����, ������ ���̸� �����Ͽ� ������ü �޽��� �����Ѵ�. 
	CCubeMeshDiffused(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, 
		float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f, 
		XMFLOAT4 fcolor1 = { 0.5f, 0.0f, 0.0f, 1.0f }, XMFLOAT4 fcolor2 = { 0.5f, 0.0f, 0.0f, 1.0f });

	virtual ~CCubeMeshDiffused();
};
//////////////////////////////////////////////////////////////////////////////////////

class CPlaneMesh : public CMesh
{
public:
	CPlaneMesh::CPlaneMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fDepth, XMFLOAT4 xmf4Color);
	virtual ~CPlaneMesh();
}; 
//////////////////////////////////////////////////////////////////////////////////////

class CAirplaneMeshDiffused : public CMesh
{
public:
	CAirplaneMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
		float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 4.0f, XMFLOAT4 xmf4Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f));

	virtual ~CAirplaneMeshDiffused();

	void Make_Another_Color_Mesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	
	CDiffusedVertex* picked_m_pVertices = NULL;
	UINT picked_m_nVertices = 0;


};
//////////////////////////////////////////////////////////////////////////////////////

class CSphereMeshDiffused : public CMesh
{
public:
	CSphereMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		float fRadius = 2.0f, int nSlices = 20, int nStacks = 20, bool black = false);
	virtual ~CSphereMeshDiffused();
};
//////////////////////////////////////////////////////////////////////////////////////

class UIMesh : public CMesh
{
public:
	UIMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		float fWidth = 200.0f, float fHeight = 90.0f, float layer_z = 1.0f, XMFLOAT4 xmf4Color = { 1.0f, 0.0f, 0.0f, 1.0f });
	virtual ~UIMesh();
};