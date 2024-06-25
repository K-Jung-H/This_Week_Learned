#include "stdafx.h"
#include "Mesh.h"
#include "GraphicsPipeline.h"

using namespace std;
/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CPolygon::CPolygon(int nVertices)
{
	m_nVertices = nVertices;
	m_pVertices = new CVertex[nVertices];
}

CPolygon::~CPolygon()
{
	if (m_pVertices) delete[] m_pVertices;
}

void CPolygon::SetVertex(int nIndex, CVertex& vertex)
{
	if ((0 <= nIndex) && (nIndex < m_nVertices) && m_pVertices)
	{
		m_pVertices[nIndex] = vertex;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CMesh::CMesh(int nPolygons)
{
	m_nPolygons = nPolygons;
	m_ppPolygons = new CPolygon*[nPolygons];
}

CMesh::~CMesh()
{
	if (m_ppPolygons)
	{
		for (int i = 0; i < m_nPolygons; i++) if (m_ppPolygons[i]) delete m_ppPolygons[i];
		delete[] m_ppPolygons;
	}
}

void CMesh::SetPolygon(int nIndex, CPolygon *pPolygon)
{
	if ((0 <= nIndex) && (nIndex < m_nPolygons)) m_ppPolygons[nIndex] = pPolygon;
}

void Draw2DLine(HDC hDCFrameBuffer, XMFLOAT3& f3PreviousProject, XMFLOAT3& f3CurrentProject)
{
	XMFLOAT3 f3Previous = CGraphicsPipeline::ScreenTransform(f3PreviousProject);
	XMFLOAT3 f3Current = CGraphicsPipeline::ScreenTransform(f3CurrentProject);
	::MoveToEx(hDCFrameBuffer, (long)f3Previous.x, (long)f3Previous.y, NULL);
	::LineTo(hDCFrameBuffer, (long)f3Current.x, (long)f3Current.y);
}

void CMesh::Render(HDC hDCFrameBuffer)
{
	XMFLOAT3 f3InitialProject, f3PreviousProject;
	bool bPreviousInside = false, bInitialInside = false, bCurrentInside = false, bIntersectInside = false;

	for (int j = 0; j < m_nPolygons; j++)
	{
		int nVertices = m_ppPolygons[j]->m_nVertices;
		CVertex* pVertices = m_ppPolygons[j]->m_pVertices;

		f3PreviousProject = f3InitialProject = CGraphicsPipeline::Project(pVertices[0].m_xmf3Position);
		bPreviousInside = bInitialInside = (-1.0f <= f3InitialProject.x) && (f3InitialProject.x <= 1.0f) && (-1.0f <= f3InitialProject.y) && (f3InitialProject.y <= 1.0f);
		for (int i = 1; i < nVertices; i++)
		{
			XMFLOAT3 f3CurrentProject = CGraphicsPipeline::Project(pVertices[i].m_xmf3Position);
			bCurrentInside = (-1.0f <= f3CurrentProject.x) && (f3CurrentProject.x <= 1.0f) && (-1.0f <= f3CurrentProject.y) && (f3CurrentProject.y <= 1.0f);
			if (((0.0f <= f3CurrentProject.z) && (f3CurrentProject.z <= 1.0f)) && ((bCurrentInside || bPreviousInside))) ::Draw2DLine(hDCFrameBuffer, f3PreviousProject, f3CurrentProject);
			f3PreviousProject = f3CurrentProject;
			bPreviousInside = bCurrentInside;
		}
		if (((0.0f <= f3InitialProject.z) && (f3InitialProject.z <= 1.0f)) && ((bInitialInside || bPreviousInside))) ::Draw2DLine(hDCFrameBuffer, f3PreviousProject, f3InitialProject);
	}
}

BOOL CMesh::RayIntersectionByTriangle(XMVECTOR& xmRayOrigin, XMVECTOR& xmRayDirection, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, float* pfNearHitDistance)
{
	float fHitDistance;
	BOOL bIntersected = TriangleTests::Intersects(xmRayOrigin, xmRayDirection, v0, v1, v2, fHitDistance);
	if (bIntersected && (fHitDistance < *pfNearHitDistance)) *pfNearHitDistance = fHitDistance;

	return(bIntersected);
}

int CMesh::CheckRayIntersection(XMVECTOR& xmvPickRayOrigin, XMVECTOR& xmvPickRayDirection, float* pfNearHitDistance)
{
	int nIntersections = 0;
	bool bIntersected = m_xmOOBB.Intersects(xmvPickRayOrigin, xmvPickRayDirection, *pfNearHitDistance);
	if (bIntersected)
	{
		for (int i = 0; i < m_nPolygons; i++)
		{
			switch (m_ppPolygons[i]->m_nVertices)
			{
			case 3:
			{
				XMVECTOR v0 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[0].m_xmf3Position));
				XMVECTOR v1 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[1].m_xmf3Position));
				XMVECTOR v2 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[2].m_xmf3Position));
				BOOL bIntersected = RayIntersectionByTriangle(xmvPickRayOrigin, xmvPickRayDirection, v0, v1, v2, pfNearHitDistance);
				if (bIntersected) nIntersections++;
				break;
			}
			case 4:
			{
				XMVECTOR v0 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[0].m_xmf3Position));
				XMVECTOR v1 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[1].m_xmf3Position));
				XMVECTOR v2 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[2].m_xmf3Position));
				BOOL bIntersected = RayIntersectionByTriangle(xmvPickRayOrigin, xmvPickRayDirection, v0, v1, v2, pfNearHitDistance);
				if (bIntersected) nIntersections++;
				v0 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[0].m_xmf3Position));
				v1 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[2].m_xmf3Position));
				v2 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[3].m_xmf3Position));
				bIntersected = RayIntersectionByTriangle(xmvPickRayOrigin, xmvPickRayDirection, v0, v1, v2, pfNearHitDistance);
				if (bIntersected) nIntersections++;
				break;
			}
			case 9:
			case 18:
			case 19:
				nIntersections++;
				break;
			}
		}
	}
	return(nIntersections);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CCubeMesh::CCubeMesh(float fWidth, float fHeight, float fDepth) : CMesh(6)
{
	float fHalfWidth = fWidth * 0.5f;
	float fHalfHeight = fHeight * 0.5f;
	float fHalfDepth = fDepth * 0.5f;

	CPolygon *pFrontFace = new CPolygon(4);
	pFrontFace->SetVertex(0, CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(1, CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	SetPolygon(0, pFrontFace);

	CPolygon *pTopFace = new CPolygon(4);
	pTopFace->SetVertex(0, CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	pTopFace->SetVertex(1, CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pTopFace->SetVertex(2, CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pTopFace->SetVertex(3, CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));
	SetPolygon(1, pTopFace);

	CPolygon *pBackFace = new CPolygon(4);
	pBackFace->SetVertex(0, CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(1, CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(2, CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(3, CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	SetPolygon(2, pBackFace);

	CPolygon *pBottomFace = new CPolygon(4);
	pBottomFace->SetVertex(0, CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	pBottomFace->SetVertex(1, CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	pBottomFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBottomFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	SetPolygon(3, pBottomFace);

	CPolygon *pLeftFace = new CPolygon(4);
	pLeftFace->SetVertex(0, CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	pLeftFace->SetVertex(1, CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));
	pLeftFace->SetVertex(2, CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	pLeftFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	SetPolygon(4, pLeftFace);

	CPolygon *pRightFace = new CPolygon(4);
	pRightFace->SetVertex(0, CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pRightFace->SetVertex(1, CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pRightFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pRightFace->SetVertex(3, CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	SetPolygon(5, pRightFace);

	m_xmOOBB = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth, fHalfHeight, fHalfDepth), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CWallMesh::CWallMesh(float fWidth, float fHeight, float fDepth, int nSubRects) : CMesh((4 * nSubRects * nSubRects) + 2)
{
	float fHalfWidth = fWidth * 0.5f;
	float fHalfHeight = fHeight * 0.5f;
	float fHalfDepth = fDepth * 0.5f;
	float fCellWidth = fWidth * (1.0f / nSubRects);
	float fCellHeight = fHeight * (1.0f / nSubRects);
	float fCellDepth = fDepth * (1.0f / nSubRects);

	int k = 0;
	CPolygon* pLeftFace;
	for (int i = 0; i < nSubRects; i++)
	{
		for (int j = 0; j < nSubRects; j++)
		{
			pLeftFace = new CPolygon(4);
			pLeftFace->SetVertex(0, CVertex(-fHalfWidth, -fHalfHeight + (i * fCellHeight), -fHalfDepth + (j * fCellDepth)));
			pLeftFace->SetVertex(1, CVertex(-fHalfWidth, -fHalfHeight + ((i + 1) * fCellHeight), -fHalfDepth + (j * fCellDepth)));
			pLeftFace->SetVertex(2, CVertex(-fHalfWidth, -fHalfHeight + ((i + 1) * fCellHeight), -fHalfDepth + ((j + 1) * fCellDepth)));
			pLeftFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight + (i * fCellHeight), -fHalfDepth + ((j + 1) * fCellDepth)));
			SetPolygon(k++, pLeftFace);
		}
	}

	CPolygon* pRightFace;
	for (int i = 0; i < nSubRects; i++)
	{
		for (int j = 0; j < nSubRects; j++)
		{
			pRightFace = new CPolygon(4);
			pRightFace->SetVertex(0, CVertex(+fHalfWidth, -fHalfHeight + (i * fCellHeight), -fHalfDepth + (j * fCellDepth)));
			pRightFace->SetVertex(1, CVertex(+fHalfWidth, -fHalfHeight + ((i + 1) * fCellHeight), -fHalfDepth + (j * fCellDepth)));
			pRightFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight + ((i + 1) * fCellHeight), -fHalfDepth + ((j + 1) * fCellDepth)));
			pRightFace->SetVertex(3, CVertex(+fHalfWidth, -fHalfHeight + (i * fCellHeight), -fHalfDepth + ((j + 1) * fCellDepth)));
			SetPolygon(k++, pRightFace);
		}
	}

	CPolygon* pTopFace;
	for (int i = 0; i < nSubRects; i++)
	{
		for (int j = 0; j < nSubRects; j++)
		{
			pTopFace = new CPolygon(4);
			pTopFace->SetVertex(0, CVertex(-fHalfWidth + (i * fCellWidth), +fHalfHeight, -fHalfDepth + (j * fCellDepth)));
			pTopFace->SetVertex(1, CVertex(-fHalfWidth + ((i + 1) * fCellWidth), +fHalfHeight, -fHalfDepth + (j * fCellDepth)));
			pTopFace->SetVertex(2, CVertex(-fHalfWidth + ((i + 1) * fCellWidth), +fHalfHeight, -fHalfDepth + ((j + 1) * fCellDepth)));
			pTopFace->SetVertex(3, CVertex(-fHalfWidth + (i * fCellWidth), +fHalfHeight, -fHalfDepth + ((j + 1) * fCellDepth)));
			SetPolygon(k++, pTopFace);
		}
	}

	CPolygon* pBottomFace;
	for (int i = 0; i < nSubRects; i++)
	{
		for (int j = 0; j < nSubRects; j++)
		{
			pBottomFace = new CPolygon(4);
			pBottomFace->SetVertex(0, CVertex(-fHalfWidth + (i * fCellWidth), -fHalfHeight, -fHalfDepth + (j * fCellDepth)));
			pBottomFace->SetVertex(1, CVertex(-fHalfWidth + ((i + 1) * fCellWidth), -fHalfHeight, -fHalfDepth + (j * fCellDepth)));
			pBottomFace->SetVertex(2, CVertex(-fHalfWidth + ((i + 1) * fCellWidth), -fHalfHeight, -fHalfDepth + ((j + 1) * fCellDepth)));
			pBottomFace->SetVertex(3, CVertex(-fHalfWidth + (i * fCellWidth), -fHalfHeight, -fHalfDepth + ((j + 1) * fCellDepth)));
			SetPolygon(k++, pBottomFace);
		}
	}

	CPolygon* pFrontFace = new CPolygon(4);
	pFrontFace->SetVertex(0, CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(1, CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	SetPolygon(k++, pFrontFace);

	CPolygon* pBackFace = new CPolygon(4);
	pBackFace->SetVertex(0, CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(1, CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(2, CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(3, CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	SetPolygon(k++, pBackFace);

	m_xmOOBB = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth, fHalfHeight, fHalfDepth), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAirplaneMesh::CAirplaneMesh(float fWidth, float fHeight, float fDepth) : CMesh(24)
{
	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	float x1 = fx * 0.2f, y1 = fy * 0.2f, x2 = fx * 0.1f, y3 = fy * 0.3f, y2 = ((y1 - (fy - y3)) / x1)*x2 + (fy - y3);
	int i = 0;

	//Upper Plane
	CPolygon *pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, +(fy + y3), -fz));
	pFace->SetVertex(1, CVertex(+x1, -y1, -fz));
	pFace->SetVertex(2, CVertex(0.0f, 0.0f, -fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, +(fy + y3), -fz));
	pFace->SetVertex(1, CVertex(0.0f, 0.0f, -fz));
	pFace->SetVertex(2, CVertex(-x1, -y1, -fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(+x2, +y2, -fz));
	pFace->SetVertex(1, CVertex(+fx, -y3, -fz));
	pFace->SetVertex(2, CVertex(+x1, -y1, -fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(-x2, +y2, -fz));
	pFace->SetVertex(1, CVertex(-x1, -y1, -fz));
	pFace->SetVertex(2, CVertex(-fx, -y3, -fz));
	SetPolygon(i++, pFace);

	//Lower Plane
	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, +(fy + y3), +fz));
	pFace->SetVertex(1, CVertex(0.0f, 0.0f, +fz));
	pFace->SetVertex(2, CVertex(+x1, -y1, +fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, +(fy + y3), +fz));
	pFace->SetVertex(1, CVertex(-x1, -y1, +fz));
	pFace->SetVertex(2, CVertex(0.0f, 0.0f, +fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(+x2, +y2, +fz));
	pFace->SetVertex(1, CVertex(+x1, -y1, +fz));
	pFace->SetVertex(2, CVertex(+fx, -y3, +fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(-x2, +y2, +fz));
	pFace->SetVertex(1, CVertex(-fx, -y3, +fz));
	pFace->SetVertex(2, CVertex(-x1, -y1, +fz));
	SetPolygon(i++, pFace);

	//Right Plane
	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, +(fy + y3), -fz));
	pFace->SetVertex(1, CVertex(0.0f, +(fy + y3), +fz));
	pFace->SetVertex(2, CVertex(+x2, +y2, -fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(+x2, +y2, -fz));
	pFace->SetVertex(1, CVertex(0.0f, +(fy + y3), +fz));
	pFace->SetVertex(2, CVertex(+x2, +y2, +fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(+x2, +y2, -fz));
	pFace->SetVertex(1, CVertex(+x2, +y2, +fz));
	pFace->SetVertex(2, CVertex(+fx, -y3, -fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(+fx, -y3, -fz));
	pFace->SetVertex(1, CVertex(+x2, +y2, +fz));
	pFace->SetVertex(2, CVertex(+fx, -y3, +fz));
	SetPolygon(i++, pFace);

	//Back/Right Plane
	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(+x1, -y1, -fz));
	pFace->SetVertex(1, CVertex(+fx, -y3, -fz));
	pFace->SetVertex(2, CVertex(+fx, -y3, +fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(+x1, -y1, -fz));
	pFace->SetVertex(1, CVertex(+fx, -y3, +fz));
	pFace->SetVertex(2, CVertex(+x1, -y1, +fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, 0.0f, -fz));
	pFace->SetVertex(1, CVertex(+x1, -y1, -fz));
	pFace->SetVertex(2, CVertex(+x1, -y1, +fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, 0.0f, -fz));
	pFace->SetVertex(1, CVertex(+x1, -y1, +fz));
	pFace->SetVertex(2, CVertex(0.0f, 0.0f, +fz));
	SetPolygon(i++, pFace);

	//Left Plane
	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, +(fy + y3), +fz));
	pFace->SetVertex(1, CVertex(0.0f, +(fy + y3), -fz));
	pFace->SetVertex(2, CVertex(-x2, +y2, -fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, +(fy + y3), +fz));
	pFace->SetVertex(1, CVertex(-x2, +y2, -fz));
	pFace->SetVertex(2, CVertex(-x2, +y2, +fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(-x2, +y2, +fz));
	pFace->SetVertex(1, CVertex(-x2, +y2, -fz));
	pFace->SetVertex(2, CVertex(-fx, -y3, -fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(-x2, +y2, +fz));
	pFace->SetVertex(1, CVertex(-fx, -y3, -fz));
	pFace->SetVertex(2, CVertex(-fx, -y3, +fz));
	SetPolygon(i++, pFace);

	//Back/Left Plane
	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, 0.0f, -fz));
	pFace->SetVertex(1, CVertex(0.0f, 0.0f, +fz));
	pFace->SetVertex(2, CVertex(-x1, -y1, +fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, 0.0f, -fz));
	pFace->SetVertex(1, CVertex(-x1, -y1, +fz));
	pFace->SetVertex(2, CVertex(-x1, -y1, -fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(-x1, -y1, -fz));
	pFace->SetVertex(1, CVertex(-x1, -y1, +fz));
	pFace->SetVertex(2, CVertex(-fx, -y3, +fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(-x1, -y1, -fz));
	pFace->SetVertex(1, CVertex(-fx, -y3, +fz));
	pFace->SetVertex(2, CVertex(-fx, -y3, -fz));
	SetPolygon(i++, pFace);

	m_xmOOBB = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fx, fy, fz), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CAxisMesh::CAxisMesh(float fWidth, float fHeight, float fDepth) : CMesh(3)
{
	float fHalfWidth = fWidth * 0.5f;
	float fHalfHeight = fHeight * 0.5f;
	float fHalfDepth = fDepth * 0.5f;

	CPolygon* pxAxis = new CPolygon(2);
	pxAxis->SetVertex(0, CVertex(-fHalfWidth, 0.0f, 0.0f));
	pxAxis->SetVertex(1, CVertex(+fHalfWidth, 0.0f, 0.0f));
	SetPolygon(0, pxAxis);

	CPolygon* pyAxis = new CPolygon(2);
	pyAxis->SetVertex(0, CVertex(0.0f, -fHalfWidth, 0.0f));
	pyAxis->SetVertex(1, CVertex(0.0f, +fHalfWidth, 0.0f));
	SetPolygon(1, pyAxis);

	CPolygon* pzAxis = new CPolygon(2);
	pzAxis->SetVertex(0, CVertex(0.0f, 0.0f, -fHalfWidth));
	pzAxis->SetVertex(1, CVertex(0.0f, 0.0f, +fHalfWidth));
	SetPolygon(2, pzAxis);
}

void CAxisMesh::Render(HDC hDCFrameBuffer)
{
	XMFLOAT3 f3PreviousProject = CGraphicsPipeline::Project(m_ppPolygons[0]->m_pVertices[0].m_xmf3Position);
	XMFLOAT3 f3CurrentProject = CGraphicsPipeline::Project(m_ppPolygons[0]->m_pVertices[1].m_xmf3Position);
	HPEN hPen = ::CreatePen(PS_SOLID, 0, RGB(255, 0, 0));
	HPEN hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);
	::Draw2DLine(hDCFrameBuffer, f3PreviousProject, f3CurrentProject);
	::SelectObject(hDCFrameBuffer, hOldPen);
	::DeleteObject(hPen);

	f3PreviousProject = CGraphicsPipeline::Project(m_ppPolygons[1]->m_pVertices[0].m_xmf3Position);
	f3CurrentProject = CGraphicsPipeline::Project(m_ppPolygons[1]->m_pVertices[1].m_xmf3Position);
	hPen = ::CreatePen(PS_SOLID, 0, RGB(0, 0, 255));
	hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);
	::Draw2DLine(hDCFrameBuffer, f3PreviousProject, f3CurrentProject);
	::SelectObject(hDCFrameBuffer, hOldPen);
	::DeleteObject(hPen);

	f3PreviousProject = CGraphicsPipeline::Project(m_ppPolygons[2]->m_pVertices[0].m_xmf3Position);
	f3CurrentProject = CGraphicsPipeline::Project(m_ppPolygons[2]->m_pVertices[1].m_xmf3Position);
	hPen = ::CreatePen(PS_SOLID, 0, RGB(0, 255, 0));
	hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);
	::Draw2DLine(hDCFrameBuffer, f3PreviousProject, f3CurrentProject);
	::SelectObject(hDCFrameBuffer, hOldPen);
	::DeleteObject(hPen);
}



CStartMesh::CStartMesh(float fWidth, float fHeight, float fDepth) : CMesh(21)
{
	float fHalfWidth = fWidth * 0.5f;
	float fHalfHeight = fHeight * 0.5f;
	float fHalfDepth = fDepth * 0.5f;

	//lattering
	CPolygon* pFace = new CPolygon(18); // S의 앞면
	pFace->SetVertex(0, CVertex(+25.0f, +7.5f, +fHalfDepth));
	pFace->SetVertex(1, CVertex(+22.5f, +10.0f, +fHalfDepth));
	pFace->SetVertex(2, CVertex(+17.5f, +10.0f, +fHalfDepth));
	pFace->SetVertex(3, CVertex(+15.0f, +7.5f, +fHalfDepth));
	pFace->SetVertex(4, CVertex(+15.0f, +5.0f, +fHalfDepth));
	pFace->SetVertex(5, CVertex(+17.5f, +7.5f, +fHalfDepth));
	pFace->SetVertex(6, CVertex(+22.5f, +7.5f, +fHalfDepth));
	pFace->SetVertex(7, CVertex(+23.25f,+6.25f, +fHalfDepth));
	pFace->SetVertex(8, CVertex(+15.0f, -5.0f, +fHalfDepth));
	pFace->SetVertex(9, CVertex(+15.0f, -7.5f, +fHalfDepth));
	pFace->SetVertex(10, CVertex(+17.5f, -10.0f, +fHalfDepth));
	pFace->SetVertex(11, CVertex(+22.5f, -10.0f, +fHalfDepth));
	pFace->SetVertex(12, CVertex(+25.0f, -7.5f, +fHalfDepth));
	pFace->SetVertex(13, CVertex(+25.0f, -5.0f, +fHalfDepth));
	pFace->SetVertex(14, CVertex(+22.5f, -7.5f, +fHalfDepth));
	pFace->SetVertex(15, CVertex(+17.5f, -7.0f, +fHalfDepth));
	pFace->SetVertex(16, CVertex(+18.25f,-6.25f, +fHalfDepth));
	pFace->SetVertex(17, CVertex(+25.0f, +5.0f, +fHalfDepth));
	SetPolygon(0, pFace);

	pFace = new CPolygon(18); // S의 뒷면
	pFace->SetVertex(0, CVertex(+25.0f, +7.5f, -fHalfDepth));
	pFace->SetVertex(1, CVertex(+22.5f, +10.0f, -fHalfDepth));
	pFace->SetVertex(2, CVertex(+17.5f, +10.0f, -fHalfDepth));
	pFace->SetVertex(3, CVertex(+15.0f, +7.5f, -fHalfDepth));
	pFace->SetVertex(4, CVertex(+15.0f, +5.0f, -fHalfDepth));
	pFace->SetVertex(5, CVertex(+17.5f, +7.5f, -fHalfDepth));
	pFace->SetVertex(6, CVertex(+22.5f, +7.5f, -fHalfDepth));
	pFace->SetVertex(7, CVertex(+23.25f, +6.25f, -fHalfDepth));
	pFace->SetVertex(8, CVertex(+15.0f, -5.0f, -fHalfDepth));
	pFace->SetVertex(9, CVertex(+15.0f, -7.5f, -fHalfDepth));
	pFace->SetVertex(10, CVertex(+17.5f, -10.0f, -fHalfDepth));
	pFace->SetVertex(11, CVertex(+22.5f, -10.0f, -fHalfDepth));
	pFace->SetVertex(12, CVertex(+25.0f, -7.5f, -fHalfDepth));
	pFace->SetVertex(13, CVertex(+25.0f, -5.0f, -fHalfDepth));
	pFace->SetVertex(14, CVertex(+22.5f, -7.5f, -fHalfDepth));
	pFace->SetVertex(15, CVertex(+17.5f, -7.0f, -fHalfDepth));
	pFace->SetVertex(16, CVertex(+18.25f, -6.25f, -fHalfDepth));
	pFace->SetVertex(17, CVertex(+25.0f, +5.0f, -fHalfDepth));
	SetPolygon(1, pFace);

	pFace = new CPolygon(36); // S의 옆면
	pFace->SetVertex(0, CVertex(+25.0f, +7.5f, +fHalfDepth));
	pFace->SetVertex(1, CVertex(+25.0f, +7.5f, -fHalfDepth));
	pFace->SetVertex(2, CVertex(+22.5f, +10.0f, -fHalfDepth));
	pFace->SetVertex(3, CVertex(+22.5f, +10.0f, +fHalfDepth));
	pFace->SetVertex(4, CVertex(+17.5f, +10.0f, +fHalfDepth));
	pFace->SetVertex(5, CVertex(+17.5f, +10.0f, -fHalfDepth));
	pFace->SetVertex(6, CVertex(+15.0f, +7.5f, -fHalfDepth));
	pFace->SetVertex(7, CVertex(+15.0f, +7.5f, +fHalfDepth));
	pFace->SetVertex(8, CVertex(+15.0f, +5.0f, +fHalfDepth));
	pFace->SetVertex(9, CVertex(+15.0f, +5.0f, -fHalfDepth));
	pFace->SetVertex(10, CVertex(+17.5f, +7.5f, -fHalfDepth));
	pFace->SetVertex(11, CVertex(+17.5f, +7.5f, +fHalfDepth));
	pFace->SetVertex(12, CVertex(+22.5f, +7.5f, +fHalfDepth));
	pFace->SetVertex(13, CVertex(+22.5f, +7.5f, -fHalfDepth));
	pFace->SetVertex(14, CVertex(+23.25f, +6.25f, -fHalfDepth));
	pFace->SetVertex(15, CVertex(+23.25f, +6.25f, +fHalfDepth));
	pFace->SetVertex(16, CVertex(+15.0f, -5.0f, +fHalfDepth));
	pFace->SetVertex(17, CVertex(+15.0f, -5.0f, -fHalfDepth));
	pFace->SetVertex(18, CVertex(+15.0f, -7.5f, -fHalfDepth));
	pFace->SetVertex(19, CVertex(+15.0f, -7.5f, +fHalfDepth));
	pFace->SetVertex(20, CVertex(+17.5f, -10.0f, +fHalfDepth));
	pFace->SetVertex(21, CVertex(+17.5f, -10.0f, -fHalfDepth));
	pFace->SetVertex(22, CVertex(+22.5f, -10.0f, -fHalfDepth));
	pFace->SetVertex(23, CVertex(+22.5f, -10.0f, +fHalfDepth));
	pFace->SetVertex(24, CVertex(+25.0f, -7.5f, +fHalfDepth));
	pFace->SetVertex(25, CVertex(+25.0f, -7.5f, -fHalfDepth));
	pFace->SetVertex(26, CVertex(+25.0f, -5.0f, -fHalfDepth));
	pFace->SetVertex(27, CVertex(+25.0f, -5.0f, +fHalfDepth));
	pFace->SetVertex(28, CVertex(+22.5f, -7.5f, +fHalfDepth));
	pFace->SetVertex(29, CVertex(+22.5f, -7.5f, -fHalfDepth));
	pFace->SetVertex(30, CVertex(+17.5f, -7.0f, -fHalfDepth));
	pFace->SetVertex(31, CVertex(+17.5f, -7.0f, +fHalfDepth));
	pFace->SetVertex(32, CVertex(+18.25f, -6.25f, +fHalfDepth));
	pFace->SetVertex(33, CVertex(+18.25f, -6.25f, -fHalfDepth));
	pFace->SetVertex(34, CVertex(+25.0f, +5.0f, -fHalfDepth));
	pFace->SetVertex(35, CVertex(+25.0f, +5.0f, +fHalfDepth));
	SetPolygon(2, pFace);




//==============================================================
	pFace = new CPolygon(9); // T의 앞면
	pFace->SetVertex(0, CVertex(+15.0f, +10.0f, +fHalfDepth));
	pFace->SetVertex(1, CVertex(+5.0f, +10.0f, +fHalfDepth));
	pFace->SetVertex(2, CVertex(+7.0f, +7.5f, +fHalfDepth));
	pFace->SetVertex(3, CVertex(+9.0f, +7.5f, +fHalfDepth));
	pFace->SetVertex(4, CVertex(+9.0f, -7.5f, +fHalfDepth));
	pFace->SetVertex(5, CVertex(+10.0f,-10.0f, +fHalfDepth));
	pFace->SetVertex(6, CVertex(+11.0f, -7.5f, +fHalfDepth));
	pFace->SetVertex(7, CVertex(+11.0f, +7.5f, +fHalfDepth));
	pFace->SetVertex(8, CVertex(+13.0f, +7.5f, +fHalfDepth));
	SetPolygon(3, pFace);

	pFace = new CPolygon(9); // T의 뒷면
	pFace->SetVertex(0, CVertex(+15.0f, +10.0f, -fHalfDepth));
	pFace->SetVertex(1, CVertex(+5.0f, +10.0f, -fHalfDepth));
	pFace->SetVertex(2, CVertex(+7.0f, +7.5f, -fHalfDepth));
	pFace->SetVertex(3, CVertex(+9.0f, +7.5f, -fHalfDepth));
	pFace->SetVertex(4, CVertex(+9.0f, -7.5f, -fHalfDepth));
	pFace->SetVertex(5, CVertex(+10.0f, -10.0f, -fHalfDepth));
	pFace->SetVertex(6, CVertex(+11.0f, -7.5f, -fHalfDepth));
	pFace->SetVertex(7, CVertex(+11.0f, +7.5f, -fHalfDepth));
	pFace->SetVertex(8, CVertex(+13.0f, +7.5f, -fHalfDepth));
	SetPolygon(4, pFace);

	pFace = new CPolygon(19); // T의 옆면
	pFace->SetVertex(0, CVertex(+15.0f, +10.0f, +fHalfDepth));
	pFace->SetVertex(1, CVertex(+15.0f, +10.0f, -fHalfDepth));
	pFace->SetVertex(2, CVertex(+5.0f, +10.0f, -fHalfDepth));
	pFace->SetVertex(3, CVertex(+5.0f, +10.0f, +fHalfDepth));
	pFace->SetVertex(4, CVertex(+7.0f, +7.5f, +fHalfDepth));
	pFace->SetVertex(5, CVertex(+7.0f, +7.5f, -fHalfDepth));
	pFace->SetVertex(6, CVertex(+9.0f, +7.5f, -fHalfDepth));
	pFace->SetVertex(7, CVertex(+9.0f, +7.5f, +fHalfDepth));
	pFace->SetVertex(8, CVertex(+9.0f, -7.5f, +fHalfDepth));
	pFace->SetVertex(9, CVertex(+9.0f, -7.5f, -fHalfDepth));
	pFace->SetVertex(10, CVertex(+10.0f, -10.0f, -fHalfDepth));
	pFace->SetVertex(11, CVertex(+10.0f, -10.0f, +fHalfDepth));
	pFace->SetVertex(12, CVertex(+11.0f, -7.5f, +fHalfDepth));
	pFace->SetVertex(13, CVertex(+11.0f, -7.5f, -fHalfDepth));
	pFace->SetVertex(14, CVertex(+11.0f, +7.5f, -fHalfDepth));
	pFace->SetVertex(15, CVertex(+11.0f, +7.5f, +fHalfDepth));
	pFace->SetVertex(16, CVertex(+13.0f, +7.5f, +fHalfDepth));
	pFace->SetVertex(17, CVertex(+13.0f, +7.5f, -fHalfDepth));
	pFace->SetVertex(18, CVertex(+15.0f, +10.0f, -fHalfDepth));
	SetPolygon(5, pFace);

//==============================================================


	pFace = new CPolygon(10); // A의 앞면
	pFace->SetVertex(0, CVertex(+1.0f, +10.0f, +fHalfDepth));
	pFace->SetVertex(1, CVertex(-1.0f, +10.0f, +fHalfDepth));
	pFace->SetVertex(2, CVertex(-5.0f, -8.0f, +fHalfDepth));
	pFace->SetVertex(3, CVertex(-5.0f, -10.0f, +fHalfDepth));
	pFace->SetVertex(4, CVertex(-3.75f,-10.0f, +fHalfDepth));
	pFace->SetVertex(5, CVertex(-1.5f, -3.0f, +fHalfDepth));
	pFace->SetVertex(6, CVertex(+1.5f, -3.0f, +fHalfDepth));
	pFace->SetVertex(7, CVertex(+3.75f,-10.0f, +fHalfDepth));
	pFace->SetVertex(8, CVertex(+5.0f, -10.0f, +fHalfDepth));
	pFace->SetVertex(9, CVertex(+5.0f, -8.0f, +fHalfDepth));
	SetPolygon(6, pFace);

	pFace = new CPolygon(10); // A의 뒷면
	pFace->SetVertex(0, CVertex(+1.0f, +10.0f, -fHalfDepth));
	pFace->SetVertex(1, CVertex(-1.0f, +10.0f, -fHalfDepth));
	pFace->SetVertex(2, CVertex(-5.0f, -8.0f, -fHalfDepth));
	pFace->SetVertex(3, CVertex(-5.0f, -10.0f, -fHalfDepth));
	pFace->SetVertex(4, CVertex(-3.75f, -10.0f, -fHalfDepth));
	pFace->SetVertex(5, CVertex(-1.5f, -3.0f, -fHalfDepth));
	pFace->SetVertex(6, CVertex(+1.5f, -3.0f, -fHalfDepth));
	pFace->SetVertex(7, CVertex(+3.75f, -10.0f, -fHalfDepth));
	pFace->SetVertex(8, CVertex(+5.0f, -10.0f, -fHalfDepth));
	pFace->SetVertex(9, CVertex(+5.0f, -8.0f, -fHalfDepth));
	SetPolygon(7, pFace);

	pFace = new CPolygon(20); // A의 옆면
	pFace->SetVertex(0, CVertex(+1.0f, +10.0f, +fHalfDepth));
	pFace->SetVertex(1, CVertex(+1.0f, +10.0f, -fHalfDepth));
	pFace->SetVertex(2, CVertex(-1.0f, +10.0f, -fHalfDepth));
	pFace->SetVertex(3, CVertex(-1.0f, +10.0f, +fHalfDepth));
	pFace->SetVertex(4, CVertex(-5.0f, -8.0f, +fHalfDepth));
	pFace->SetVertex(5, CVertex(-5.0f, -8.0f, -fHalfDepth));
	pFace->SetVertex(6, CVertex(-5.0f, -10.0f, -fHalfDepth));
	pFace->SetVertex(7, CVertex(-5.0f, -10.0f, +fHalfDepth));
	pFace->SetVertex(8, CVertex(-3.75f, -10.0f, +fHalfDepth));
	pFace->SetVertex(9, CVertex(-3.75f, -10.0f, -fHalfDepth));
	pFace->SetVertex(10, CVertex(-1.5f, -3.0f, -fHalfDepth));
	pFace->SetVertex(11, CVertex(-1.5f, -3.0f, +fHalfDepth));
	pFace->SetVertex(12, CVertex(+1.5f, -3.0f, +fHalfDepth));
	pFace->SetVertex(13, CVertex(+1.5f, -3.0f, -fHalfDepth));
	pFace->SetVertex(14, CVertex(+3.75f, -10.0f, -fHalfDepth));
	pFace->SetVertex(15, CVertex(+3.75f, -10.0f, +fHalfDepth));
	pFace->SetVertex(16, CVertex(+5.0f, -10.0f, +fHalfDepth));
	pFace->SetVertex(17, CVertex(+5.0f, -10.0f, -fHalfDepth));
	pFace->SetVertex(18, CVertex(+5.0f, -8.0f, -fHalfDepth));
	pFace->SetVertex(19, CVertex(+5.0f, -8.0f, +fHalfDepth));
	SetPolygon(8, pFace);


	pFace = new CPolygon(4); // A앞면 내부
	pFace->SetVertex(0, CVertex(+0.5f, +7.0f, +fHalfDepth));
	pFace->SetVertex(1, CVertex(-0.5f, +7.0f, +fHalfDepth));
	pFace->SetVertex(2, CVertex(-2.0f, -1.0f, +fHalfDepth));
	pFace->SetVertex(3, CVertex(+2.0f, -1.0f, +fHalfDepth));
	SetPolygon(9, pFace);

	pFace = new CPolygon(4); // A뒷면 내부
	pFace->SetVertex(0, CVertex(+0.5f, +7.0f, -fHalfDepth));
	pFace->SetVertex(1, CVertex(-0.5f, +7.0f, -fHalfDepth));
	pFace->SetVertex(2, CVertex(-2.0f, -1.0f, -fHalfDepth));
	pFace->SetVertex(3, CVertex(+2.0f, -1.0f, -fHalfDepth));
	SetPolygon(10, pFace);


	pFace = new CPolygon(8); // A면 내부
	pFace->SetVertex(0, CVertex(+0.5f, +7.0f, +fHalfDepth));
	pFace->SetVertex(1, CVertex(+0.5f, +7.0f, -fHalfDepth));
	pFace->SetVertex(2, CVertex(-0.5f, +7.0f, -fHalfDepth));
	pFace->SetVertex(3, CVertex(-0.5f, +7.0f, +fHalfDepth));
	pFace->SetVertex(4, CVertex(-2.0f, -1.0f, +fHalfDepth));
	pFace->SetVertex(5, CVertex(-2.0f, -1.0f, -fHalfDepth));
	pFace->SetVertex(6, CVertex(+2.0f, -1.0f, -fHalfDepth));
	pFace->SetVertex(7, CVertex(+2.0f, -1.0f, +fHalfDepth));

	SetPolygon(11, pFace);


//==============================================================


	pFace = new CPolygon(10); // R앞면
	pFace->SetVertex(0, CVertex(-5.5f, +10.0f, +fHalfDepth));
	pFace->SetVertex(1, CVertex(-12.0f,+10.0f, +fHalfDepth));
	pFace->SetVertex(2, CVertex(-14.0f, +6.0f, +fHalfDepth));
	pFace->SetVertex(3, CVertex(-11.5f, +2.0f, +fHalfDepth));
	pFace->SetVertex(4, CVertex(-15.0f, -6.0f, +fHalfDepth));
	pFace->SetVertex(5, CVertex(-15.0f, -10.0f, +fHalfDepth));
	pFace->SetVertex(6, CVertex(-10.0f, +1.0f, +fHalfDepth));
	pFace->SetVertex(7, CVertex(-7.0f, -1.0f, +fHalfDepth));
	pFace->SetVertex(8, CVertex(-7.0f, -6.0f, +fHalfDepth));
	pFace->SetVertex(9, CVertex(-5.5f, -10.0f, +fHalfDepth));
	SetPolygon(12, pFace);

	pFace = new CPolygon(10); // R뒷면
	pFace->SetVertex(0, CVertex(-5.5f, +10.0f, -fHalfDepth));
	pFace->SetVertex(1, CVertex(-12.0f, +10.0f, -fHalfDepth));
	pFace->SetVertex(2, CVertex(-14.0f, +6.0f, -fHalfDepth));
	pFace->SetVertex(3, CVertex(-11.5f, +2.0f, -fHalfDepth));
	pFace->SetVertex(4, CVertex(-15.0f, -6.0f, -fHalfDepth));
	pFace->SetVertex(5, CVertex(-15.0f, -10.0f, -fHalfDepth));
	pFace->SetVertex(6, CVertex(-10.0f, +1.0f, -fHalfDepth));
	pFace->SetVertex(7, CVertex(-7.0f, -1.0f, -fHalfDepth));
	pFace->SetVertex(8, CVertex(-7.0f, -6.0f, -fHalfDepth));
	pFace->SetVertex(9, CVertex(-5.5f, -10.0f, -fHalfDepth));
	SetPolygon(13, pFace);

	pFace = new CPolygon(20); // R옆면
	pFace->SetVertex(0, CVertex(-5.5f, +10.0f, +fHalfDepth));
	pFace->SetVertex(1, CVertex(-5.5f, +10.0f, -fHalfDepth));
	pFace->SetVertex(2, CVertex(-12.0f, +10.0f, -fHalfDepth));
	pFace->SetVertex(3, CVertex(-12.0f, +10.0f, +fHalfDepth));
	pFace->SetVertex(4, CVertex(-14.0f, +6.0f, +fHalfDepth));
	pFace->SetVertex(5, CVertex(-14.0f, +6.0f, -fHalfDepth));
	pFace->SetVertex(6, CVertex(-11.5f, +2.0f, -fHalfDepth));
	pFace->SetVertex(7, CVertex(-11.5f, +2.0f, +fHalfDepth));
	pFace->SetVertex(8, CVertex(-15.0f, -6.0f, +fHalfDepth));
	pFace->SetVertex(9, CVertex(-15.0f, -6.0f, -fHalfDepth));
	pFace->SetVertex(10, CVertex(-15.0f, -10.0f, -fHalfDepth));
	pFace->SetVertex(11, CVertex(-15.0f, -10.0f, +fHalfDepth));
	pFace->SetVertex(12, CVertex(-10.0f, +1.0f, +fHalfDepth));
	pFace->SetVertex(13, CVertex(-10.0f, +1.0f, -fHalfDepth));
	pFace->SetVertex(14, CVertex(-7.0f, -1.0f, -fHalfDepth));
	pFace->SetVertex(15, CVertex(-7.0f, -1.0f, +fHalfDepth));
	pFace->SetVertex(16, CVertex(-7.0f, -6.0f, +fHalfDepth));
	pFace->SetVertex(17, CVertex(-7.0f, -6.0f, -fHalfDepth));
	pFace->SetVertex(18, CVertex(-5.5f, -10.0f, -fHalfDepth));
	pFace->SetVertex(19, CVertex(-5.5f, -10.0f, +fHalfDepth));
	SetPolygon(14, pFace);

	pFace = new CPolygon(5); // R앞면 내부
	pFace->SetVertex(0, CVertex(-6.5f, +8.0f, +fHalfDepth));
	pFace->SetVertex(1, CVertex(-11.0f, +8.0f, +fHalfDepth));
	pFace->SetVertex(2, CVertex(-12.0f, +6.0f, +fHalfDepth));
	pFace->SetVertex(3, CVertex(-11.0f, +3.0f, +fHalfDepth));
	pFace->SetVertex(4, CVertex(-6.5f, +3.0f, +fHalfDepth));
	SetPolygon(15, pFace);

	pFace = new CPolygon(5); // R뒷면 내부
	pFace->SetVertex(0, CVertex(-6.5f, +8.0f, -fHalfDepth));
	pFace->SetVertex(1, CVertex(-11.0f, +8.0f, -fHalfDepth));
	pFace->SetVertex(2, CVertex(-12.0f, +6.0f, -fHalfDepth));
	pFace->SetVertex(3, CVertex(-11.0f, +3.0f, -fHalfDepth));
	pFace->SetVertex(4, CVertex(-6.5f, +3.0f, -fHalfDepth));
	SetPolygon(16, pFace);

	pFace = new CPolygon(11); // R옆면 내부
	pFace->SetVertex(0, CVertex(-6.5f, +8.0f, +fHalfDepth));
	pFace->SetVertex(1, CVertex(-6.5f, +8.0f, -fHalfDepth));
	pFace->SetVertex(2, CVertex(-11.0f, +8.0f, -fHalfDepth));
	pFace->SetVertex(3, CVertex(-11.0f, +8.0f, +fHalfDepth));
	pFace->SetVertex(4, CVertex(-12.0f, +6.0f, +fHalfDepth));
	pFace->SetVertex(5, CVertex(-12.0f, +6.0f, -fHalfDepth));
	pFace->SetVertex(6, CVertex(-11.0f, +3.0f, -fHalfDepth));
	pFace->SetVertex(7, CVertex(-11.0f, +3.0f, +fHalfDepth));
	pFace->SetVertex(8, CVertex(-6.5f, +3.0f, +fHalfDepth));
	pFace->SetVertex(9, CVertex(-6.5f, +3.0f, -fHalfDepth));
	pFace->SetVertex(10, CVertex(-6.5f, +8.0f, -fHalfDepth));
	SetPolygon(17, pFace);

//==============================================================

	pFace = new CPolygon(9); // T의 앞면
	pFace->SetVertex(0, CVertex(-15.0f, +10.0f, +fHalfDepth));
	pFace->SetVertex(1, CVertex(-25.0f, +10.0f, +fHalfDepth));
	pFace->SetVertex(2, CVertex(-23.0f, +7.5f, +fHalfDepth));
	pFace->SetVertex(3, CVertex(-21.0f, +7.5f, +fHalfDepth));
	pFace->SetVertex(4, CVertex(-21.0f, -7.5f, +fHalfDepth));
	pFace->SetVertex(5, CVertex(-20.0f, -10.0f, +fHalfDepth));
	pFace->SetVertex(6, CVertex(-19.0f, -7.5f, +fHalfDepth));
	pFace->SetVertex(7, CVertex(-19.0f, +7.5f, +fHalfDepth));
	pFace->SetVertex(8, CVertex(-17.0f, +7.5f, +fHalfDepth));
	SetPolygon(18, pFace);

	pFace = new CPolygon(9); // T의 back면
	pFace->SetVertex(0, CVertex(-15.0f, +10.0f, -fHalfDepth));
	pFace->SetVertex(1, CVertex(-25.0f, +10.0f, -fHalfDepth));
	pFace->SetVertex(2, CVertex(-23.0f, +7.5f, -fHalfDepth));
	pFace->SetVertex(3, CVertex(-21.0f, +7.5f, -fHalfDepth));
	pFace->SetVertex(4, CVertex(-21.0f, -7.5f, -fHalfDepth));
	pFace->SetVertex(5, CVertex(-20.0f, -10.0f,-+fHalfDepth));
	pFace->SetVertex(6, CVertex(-19.0f, -7.5f, -fHalfDepth));
	pFace->SetVertex(7, CVertex(-19.0f, +7.5f, -fHalfDepth));
	pFace->SetVertex(8, CVertex(-17.0f, +7.5f, -fHalfDepth));
	SetPolygon(19, pFace);

	pFace = new CPolygon(19); // T의 side면
	pFace->SetVertex(0, CVertex(-15.0f, +10.0f, +fHalfDepth));
	pFace->SetVertex(1, CVertex(-15.0f, +10.0f, -fHalfDepth));
	pFace->SetVertex(2, CVertex(-25.0f, +10.0f, -fHalfDepth));
	pFace->SetVertex(3, CVertex(-25.0f, +10.0f, +fHalfDepth));
	pFace->SetVertex(4, CVertex(-23.0f, +7.5f, +fHalfDepth));
	pFace->SetVertex(5, CVertex(-23.0f, +7.5f, -fHalfDepth));
	pFace->SetVertex(6, CVertex(-21.0f, +7.5f, -fHalfDepth));
	pFace->SetVertex(7, CVertex(-21.0f, +7.5f, +fHalfDepth));
	pFace->SetVertex(8, CVertex(-21.0f, -7.5f, +fHalfDepth));
	pFace->SetVertex(9, CVertex(-21.0f, -7.5f, -fHalfDepth));
	pFace->SetVertex(10, CVertex(-20.0f, -10.0f, -fHalfDepth));
	pFace->SetVertex(11, CVertex(-20.0f, -10.0f, +fHalfDepth));
	pFace->SetVertex(12, CVertex(-19.0f, -7.5f, +fHalfDepth));
	pFace->SetVertex(13, CVertex(-19.0f, -7.5f, -fHalfDepth));
	pFace->SetVertex(14, CVertex(-19.0f, +7.5f, -fHalfDepth));
	pFace->SetVertex(15, CVertex(-19.0f, +7.5f, +fHalfDepth));
	pFace->SetVertex(16, CVertex(-17.0f, +7.5f, +fHalfDepth));
	pFace->SetVertex(17, CVertex(-17.0f, +7.5f, -fHalfDepth));
	pFace->SetVertex(18, CVertex(-15.0f, +10.0f, -fHalfDepth));
	SetPolygon(20, pFace);

	m_xmOOBB = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(25.0f, 10.0f, 5.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}


CSphereMesh::CSphereMesh(float fWidth, float fHeight, float fDepth)
{
	float fHalfWidth = fWidth * 0.5f;
	float fHalfHeight = fHeight * 0.5f;
	float fHalfDepth = fDepth * 0.5f;


	vector<pair<int, CPolygon*>> obj_data;
	int f_num = Load_Object("objfile\\sphere.obj", obj_data);

	m_nPolygons = f_num;
	m_ppPolygons = new CPolygon * [f_num];

	for (pair<int, CPolygon*>& f_data : obj_data)
	{
		int face_num = f_data.first;
		CPolygon* face = f_data.second;
		SetPolygon(face_num, face);
	}

	m_xmOOBB = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth, fHalfHeight, fHalfDepth), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	m_xmBSphere = BoundingSphere(XMFLOAT3(0.0f, 0.0f, 0.0f), 1.0f);
}

// obj 파일을 읽어들이는 함수
int Load_Object(const char* path, vector<pair<int, CPolygon*>>& pPolygon)
{
	vector<XMFLOAT3> vertices;
	vertices.clear();

	ifstream in(path);
	if (!in) {
		cerr << path << "파일 못찾음";
		exit(1);
	}

	int n_faces = 0;
	float Middle_x{};
	float Middle_y{};
	float Middle_z{};
	while (in)
	{
		string lineHeader;
		in >> lineHeader;
		if (lineHeader == "v")
		{
			XMFLOAT3 vertex{};
			in >> vertex.x >> vertex.y >> vertex.z;
			Middle_x += vertex.x;
			Middle_y += vertex.y;
			Middle_z += vertex.z;
			vertices.push_back(vertex);
		}
		else if (lineHeader == "f")
		{
			char a{};
			unsigned int vertexIndex[3];

			for (int i = 0; i < 3; ++i)
			{
				char separator{};
				int temp{};
				in >> vertexIndex[i] >> separator >> temp >> separator >> temp;
			}
			CPolygon* poly = new CPolygon(3);
			for (int i = 0; i < 3; ++i)
			{
				poly->SetVertex(i, CVertex(vertices[vertexIndex[i] - 1]));
			}

			pPolygon.push_back(make_pair(n_faces++, poly));
		}
	}

	float mx = Middle_x / pPolygon.size();
	float my = Middle_y / pPolygon.size();
	float mz = Middle_z / pPolygon.size();

	for (pair<int, CPolygon*>& face: pPolygon)
	{
		for (int i = 0; i < 3; ++i)
		{
			face.second->m_pVertices[i].m_xmf3Position.x -= mx * 2;
			face.second->m_pVertices[i].m_xmf3Position.y -= my * 2;
			face.second->m_pVertices[i].m_xmf3Position.z -= mz * 2;
		}
	}
	

	cout << mx << my << mz;
	return pPolygon.size();
}