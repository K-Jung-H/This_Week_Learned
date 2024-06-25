#include "stdafx.h"
#include "Scene.h"
#include "GraphicsPipeline.h"

CScene::CScene(CPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
	Score = 0;
}

CScene::~CScene()
{
}

void CScene::BuildObjects()
{
	CExplosiveObject::PrepareExplosion();

	float fHalfWidth = 45.0f, fHalfHeight = 45.0f, fHalfDepth = 200.0f;
	CWallMesh* pWallCubeMesh = new CWallMesh(fHalfWidth * 2.0f, fHalfHeight * 2.0f, fHalfDepth * 2.0f, 30);

	m_pWallsObject = new CWallsObject();
	m_pWallsObject->SetPosition(0.0f, 0.0f, 0.0f);
	m_pWallsObject->SetMesh(pWallCubeMesh);
	m_pWallsObject->SetColor(RGB(100, 100, 100));
	m_pWallsObject->m_pxmf4WallPlanes[0] = XMFLOAT4(+1.0f, 0.0f, 0.0f, fHalfWidth);
	m_pWallsObject->m_pxmf4WallPlanes[1] = XMFLOAT4(-1.0f, 0.0f, 0.0f, fHalfWidth);
	m_pWallsObject->m_pxmf4WallPlanes[2] = XMFLOAT4(0.0f, +1.0f, 0.0f, fHalfHeight);
	m_pWallsObject->m_pxmf4WallPlanes[3] = XMFLOAT4(0.0f, -1.0f, 0.0f, fHalfHeight);
	m_pWallsObject->m_pxmf4WallPlanes[4] = XMFLOAT4(0.0f, 0.0f, +1.0f, fHalfDepth);
	m_pWallsObject->m_pxmf4WallPlanes[5] = XMFLOAT4(0.0f, 0.0f, -1.0f, fHalfDepth);
	m_pWallsObject->m_xmOOBBPlayerMoveCheck = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth, fHalfHeight, fHalfDepth * 0.05f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	CAirplaneMesh* pAirplaneMesh = new CAirplaneMesh();


	m_nObjects = 10;
	m_ppObjects = new CGameObject * [m_nObjects];

	CEnemyObject*pEnemyObject = new CEnemyObject();
	pEnemyObject->SetMesh(pAirplaneMesh);
	pEnemyObject->SetColor(RGB(255, 0, 0));
	pEnemyObject->SetPosition(10.0f, 10.0f, 30.0f);
	pEnemyObject->SetRespawn(XMFLOAT3(10.0f, 10.0f, 30.0f));
	pEnemyObject->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 1.0f));
	pEnemyObject->SetRotationSpeed(30.0f);
	pEnemyObject->SetMovingDirection(XMFLOAT3(1.0f, 0.0f, 0.0f));
	pEnemyObject->SetMovingSpeed(10.5f);
	pEnemyObject->Fire_Period = 2.0f;
	m_ppObjects[0] = pEnemyObject;

	pEnemyObject = new CEnemyObject();
	pEnemyObject->SetMesh(pAirplaneMesh);
	pEnemyObject->SetColor(RGB(255, 0, 0));
	pEnemyObject->SetPosition(10.0f, -10.0f, 30.0f);
	pEnemyObject->SetRespawn(XMFLOAT3(10.0f, -10.0f, 30.0f));

	pEnemyObject->SetRotationAxis(XMFLOAT3(1.0f, 1.0f, 0.0f));
	pEnemyObject->SetRotationSpeed(30.0f);
	pEnemyObject->SetMovingDirection(XMFLOAT3(-1.0f, 0.0f, 0.0f));
	pEnemyObject->SetMovingSpeed(8.8f);
	pEnemyObject->Fire_Period = 3.0f;
	m_ppObjects[1] = pEnemyObject;

	pEnemyObject = new CEnemyObject();
	pEnemyObject->SetMesh(pAirplaneMesh);
	pEnemyObject->SetColor(RGB(255, 0, 0));
	pEnemyObject->SetPosition(-10.0f, -10.0f, 30.0f);
	pEnemyObject->SetRespawn(XMFLOAT3(-10.0f, -10.0f, 30.0f));

	pEnemyObject->SetRotationAxis(XMFLOAT3(1.0f, 1.0f, 0.0f));
	pEnemyObject->SetRotationSpeed(30.0f);
	pEnemyObject->SetMovingDirection(XMFLOAT3(1.0f, -1.0f, 0.0f));
	pEnemyObject->SetMovingSpeed(5.2f);
	pEnemyObject->Fire_Period = 3.0f;
	m_ppObjects[2] = pEnemyObject;

	pEnemyObject = new CEnemyObject();
	pEnemyObject->SetMesh(pAirplaneMesh);
	pEnemyObject->SetColor(RGB(255, 0, 0));
	pEnemyObject->SetPosition(-10.0f, 10.0f, 30.0f);
	pEnemyObject->SetRespawn(XMFLOAT3(-10.0f, 10.0f, 30.0f));

	pEnemyObject->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 1.0f));
	pEnemyObject->SetRotationSpeed(30.0f);
	pEnemyObject->SetMovingDirection(XMFLOAT3(0.0f, 0.0f, 1.0f));
	pEnemyObject->SetMovingSpeed(20.4f);
	pEnemyObject->Fire_Period = 4.0f;
	m_ppObjects[3] = pEnemyObject;

	pEnemyObject = new CEnemyObject();
	pEnemyObject->SetMesh(pAirplaneMesh);
	pEnemyObject->SetColor(RGB(255, 0, 0));
	pEnemyObject->SetPosition(10.0f, 10.0f, 0.0f);
	pEnemyObject->SetRespawn(XMFLOAT3(10.0f, 10.0f, 0.0f));

	pEnemyObject->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
	pEnemyObject->SetRotationSpeed(30.0f);
	pEnemyObject->SetMovingDirection(XMFLOAT3(0.0f, 1.0f, 1.0f));
	pEnemyObject->SetMovingSpeed(6.4f);
	pEnemyObject->Fire_Period = 2.5f;
	m_ppObjects[4] = pEnemyObject;

	pEnemyObject = new CEnemyObject();
	pEnemyObject->SetMesh(pAirplaneMesh);
	pEnemyObject->SetColor(RGB(255, 0, 0));
	pEnemyObject->SetPosition(10.0f, -10.0f, 0.0f);
	pEnemyObject->SetRespawn(XMFLOAT3(10.0f, -10.0f, 0.0f));

	pEnemyObject->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
	pEnemyObject->SetRotationSpeed(30.0f);
	pEnemyObject->SetMovingDirection(XMFLOAT3(1.0f, 0.0f, 1.0f));
	pEnemyObject->SetMovingSpeed(8.9f);
	pEnemyObject->Fire_Period = 3.5f;
	m_ppObjects[5] = pEnemyObject;

	pEnemyObject = new CEnemyObject();
	pEnemyObject->SetMesh(pAirplaneMesh);
	pEnemyObject->SetColor(RGB(255, 0, 0));
	pEnemyObject->SetPosition(-10.0f, -10.0f, 0.0f);
	pEnemyObject->SetRespawn(XMFLOAT3(-10.0f, -10.0f, 0.0f));

	pEnemyObject->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
	pEnemyObject->SetRotationSpeed(30.0f);
	pEnemyObject->SetMovingDirection(XMFLOAT3(1.0f, 1.0f, 1.0f));
	pEnemyObject->SetMovingSpeed(9.7f);
	pEnemyObject->Fire_Period = 4.5f;
	m_ppObjects[6] = pEnemyObject;

	pEnemyObject = new CEnemyObject();
	pEnemyObject->SetMesh(pAirplaneMesh);
	pEnemyObject->SetColor(RGB(255, 0, 0));
	pEnemyObject->SetPosition(-10.0f, 10.0f, 0.0f);
	pEnemyObject->SetRespawn(XMFLOAT3(-10.0f, 10.0f, 0.0f));

	pEnemyObject->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
	pEnemyObject->SetRotationSpeed(30.0f);
	pEnemyObject->SetMovingDirection(XMFLOAT3(-1.0f, 1.0f, 1.0f));
	pEnemyObject->SetMovingSpeed(15.6f);
	pEnemyObject->Fire_Period = 5.0f;
	m_ppObjects[7] = pEnemyObject;

	pEnemyObject = new CEnemyObject();
	pEnemyObject->SetMesh(pAirplaneMesh);
	pEnemyObject->SetColor(RGB(255, 0, 0));
	pEnemyObject->SetPosition(-15.0f, 0.0f, -20.0f);
	pEnemyObject->SetRespawn(XMFLOAT3(-15.0f, 0.0f, -20.0f));

	pEnemyObject->SetRotationAxis(XMFLOAT3(1.0f, 1.0f, 0.0f));
	pEnemyObject->SetRotationSpeed(30.0f);
	pEnemyObject->SetMovingDirection(XMFLOAT3(0.0f, 0.0f, -1.0f));
	pEnemyObject->SetMovingSpeed(15.0f);
	pEnemyObject->Fire_Period = 5.5f;
	m_ppObjects[8] = pEnemyObject;

	pEnemyObject = new CEnemyObject();
	pEnemyObject->SetMesh(pAirplaneMesh);
	pEnemyObject->SetColor(RGB(255, 0, 0));
	pEnemyObject->SetPosition(+15.0f, 0.0f, -20.0f);
	pEnemyObject->SetRespawn(XMFLOAT3(+15.0f, 0.0f, -20.0f));

	pEnemyObject->SetRotationAxis(XMFLOAT3(1.0f, 1.0f, 0.0f));
	pEnemyObject->SetRotationSpeed(30.0f);
	pEnemyObject->SetMovingDirection(XMFLOAT3(-0.0f, 0.0f, -1.0f));
	pEnemyObject->SetMovingSpeed(15.0f);
	pEnemyObject->Fire_Period = 8.0f;
	m_ppObjects[9] = pEnemyObject;

#ifdef _WITH_DRAW_AXIS
	m_pWorldAxis = new CGameObject();
	CAxisMesh* pAxisMesh = new CAxisMesh(0.5f, 0.5f, 0.5f);
	m_pWorldAxis->SetMesh(pAxisMesh);
#endif
}

void CScene::ReleaseObjects()
{
	if (CExplosiveObject::m_pExplosionMesh) CExplosiveObject::m_pExplosionMesh->Release();

	for (int i = 0; i < m_nObjects; i++) 
		if (m_ppObjects[i]) delete m_ppObjects[i];
	if (m_ppObjects) delete[] m_ppObjects;

	if (m_pWallsObject) delete m_pWallsObject;

#ifdef _WITH_DRAW_AXIS
	if (m_pWorldAxis) delete m_pWorldAxis;
#endif
}

void CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
}

void CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		{
			CExplosiveObject* pExplosiveObject = (CExplosiveObject*)m_ppObjects[int(wParam - '1')];
			pExplosiveObject->m_bBlowingUp = true;
			break;
		}
		case 'A':
			for (int i = 0; i < m_nObjects; i++)
			{
				CExplosiveObject* pExplosiveObject = (CExplosiveObject*)m_ppObjects[i];
				pExplosiveObject->m_bBlowingUp = true;
			}
			break;

		case 'Z':
			((CAirplanePlayer*)m_pPlayer)->m_pBarrier->SetActive(true);
			((CAirplanePlayer*)m_pPlayer)->m_pBarrier->barrier_mode = true;
			((CAirplanePlayer*)m_pPlayer)->m_pBarrier->life = 10;
			break;

		default:
			break;
		}
		break;
	default:
		break;
	}
}

CGameObject* CScene::PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera)
{
	XMFLOAT3 xmf3PickPosition;
	xmf3PickPosition.x = (((2.0f * xClient) / (float)pCamera->m_Viewport.m_nWidth) - 1) / pCamera->m_xmf4x4PerspectiveProject._11;
	xmf3PickPosition.y = -(((2.0f * yClient) / (float)pCamera->m_Viewport.m_nHeight) - 1) / pCamera->m_xmf4x4PerspectiveProject._22;
	xmf3PickPosition.z = 1.0f;

	XMVECTOR xmvPickPosition = XMLoadFloat3(&xmf3PickPosition);
	XMMATRIX xmmtxView = XMLoadFloat4x4(&pCamera->m_xmf4x4View);

	int nIntersected = 0;
	float fNearestHitDistance = FLT_MAX;
	CGameObject* pNearestObject = NULL;
	for (int i = 0; i < m_nObjects; i++)
	{
		float fHitDistance = FLT_MAX;
		nIntersected = m_ppObjects[i]->PickObjectByRayIntersection(xmvPickPosition, xmmtxView, &fHitDistance);
		if ((nIntersected > 0) && (fHitDistance < fNearestHitDistance))
		{
			fNearestHitDistance = fHitDistance;
			pNearestObject = m_ppObjects[i];
		}
	}
	return(pNearestObject);
}

void CScene::CheckObjectByObjectCollisions()
{
	for (int i = 0; i < m_nObjects; i++) m_ppObjects[i]->m_pObjectCollided = NULL;
	for (int i = 0; i < m_nObjects; i++)
	{
		for (int j = (i + 1); j < m_nObjects; j++)
		{
			if (m_ppObjects[i]->m_xmOOBB.Intersects(m_ppObjects[j]->m_xmOOBB))
			{
				m_ppObjects[i]->m_pObjectCollided = m_ppObjects[j];
				m_ppObjects[j]->m_pObjectCollided = m_ppObjects[i];
			}
		}
	}
	for (int i = 0; i < m_nObjects; i++)
	{
		if (m_ppObjects[i]->m_pObjectCollided)
		{
			XMFLOAT3 xmf3MovingDirection = m_ppObjects[i]->m_xmf3MovingDirection;
			float fMovingSpeed = m_ppObjects[i]->m_fMovingSpeed;
			m_ppObjects[i]->m_xmf3MovingDirection = m_ppObjects[i]->m_pObjectCollided->m_xmf3MovingDirection;
			m_ppObjects[i]->m_fMovingSpeed = m_ppObjects[i]->m_pObjectCollided->m_fMovingSpeed;
			m_ppObjects[i]->m_pObjectCollided->m_xmf3MovingDirection = xmf3MovingDirection;
			m_ppObjects[i]->m_pObjectCollided->m_fMovingSpeed = fMovingSpeed;
			m_ppObjects[i]->m_pObjectCollided->m_pObjectCollided = NULL;
			m_ppObjects[i]->m_pObjectCollided = NULL;
		}
	}
}

void CScene::CheckObjectByWallCollisions()
{
	for (int i = 0; i < m_nObjects; i++)
	{
		ContainmentType containType = m_pWallsObject->m_xmOOBB.Contains(m_ppObjects[i]->m_xmOOBB);
		switch (containType)
		{
		case DISJOINT:
		{
			int nPlaneIndex = -1;
			for (int j = 0; j < 6; j++)
			{
				PlaneIntersectionType intersectType = m_ppObjects[i]->m_xmOOBB.Intersects(XMLoadFloat4(&m_pWallsObject->m_pxmf4WallPlanes[j]));
				if (intersectType == BACK)
				{
					nPlaneIndex = j;
					break;
				}
			}
			if (nPlaneIndex != -1)
			{
				XMVECTOR xmvNormal = XMVectorSet(m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].x, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].y, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
				XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(&m_ppObjects[i]->m_xmf3MovingDirection), xmvNormal);
				XMStoreFloat3(&m_ppObjects[i]->m_xmf3MovingDirection, xmvReflect);
			}
			break;
		}
		case INTERSECTS:
		{
			int nPlaneIndex = -1;
			for (int j = 0; j < 6; j++)
			{
				PlaneIntersectionType intersectType = m_ppObjects[i]->m_xmOOBB.Intersects(XMLoadFloat4(&m_pWallsObject->m_pxmf4WallPlanes[j]));
				if (intersectType == INTERSECTING)
				{
					nPlaneIndex = j;
					break;
				}
			}
			if (nPlaneIndex != -1)
			{
				XMVECTOR xmvNormal = XMVectorSet(m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].x, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].y, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
				XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(&m_ppObjects[i]->m_xmf3MovingDirection), xmvNormal);
				XMStoreFloat3(&m_ppObjects[i]->m_xmf3MovingDirection, xmvReflect);
			}
			break;
		}
		case CONTAINS:
			break;
		}
	}
}

void CScene::CheckPlayerByWallCollision()
{
	BoundingOrientedBox xmOOBBPlayerMoveCheck;
	m_pWallsObject->m_xmOOBBPlayerMoveCheck.Transform(xmOOBBPlayerMoveCheck, XMLoadFloat4x4(&m_pWallsObject->m_xmf4x4World));
	XMStoreFloat4(&xmOOBBPlayerMoveCheck.Orientation, XMQuaternionNormalize(XMLoadFloat4(&xmOOBBPlayerMoveCheck.Orientation)));

	if (!xmOOBBPlayerMoveCheck.Intersects(m_pPlayer->m_xmOOBB)) 
		m_pWallsObject->SetPosition(m_pPlayer->m_xmf3Position);
}

void CScene::CheckObjectByBulletCollisions()
{
	CBulletObject** ppBullets = ((CAirplanePlayer*)m_pPlayer)->m_ppBullets;
	for (int i = 0; i < m_nObjects; i++)
	{
		for (int j = 0; j < BULLETS; j++)
		{
			if (ppBullets[j]->m_bActive && m_ppObjects[i]->m_xmOOBB.Intersects(ppBullets[j]->m_xmOOBB))
			{
				CExplosiveObject* pExplosiveObject = (CExplosiveObject*)m_ppObjects[i];
				if (!pExplosiveObject->m_bBlowingUp)
					Score += 100;

				pExplosiveObject->m_bBlowingUp = true;
				ppBullets[j]->Reset();
				
			}
		}
	}
}

void CScene::CheckPlayerByBulletCollisions()
{

	for (int i = 0; i < m_nObjects; i++)
	{
		for (int j = 0; j < E_BULLETS; j++)
		{
			CEnemyBulletObject** ppBullets = static_cast<CEnemyObject*>(m_ppObjects[i])->m_ppBullets;
			if (ppBullets[j]->m_bActive && !ppBullets[j]->crashed && m_pPlayer->m_xmOOBB.Intersects(ppBullets[j]->m_xmOOBB))
			{
				ppBullets[j]->SetColor(RGB(255, 0, 0));
				ppBullets[j]->crashed = true;
				ppBullets[j]->m_fExplosionSpeed = 50.0f;
				CAirplanePlayer* aPlayer = static_cast<CAirplanePlayer*>(m_pPlayer);
				if (aPlayer->Life > 0)
				{
					if (!aPlayer->m_Armored)
					{
						aPlayer->Life -= 1;
						aPlayer->m_Armored = true;

					}
				}
				else
					aPlayer->Life = 0;
			}
		}
	}
}

void CScene::CheckBarrierByBulletCollisions()
{
	for (int i = 0; i < m_nObjects; i++)
	{
		for (int j = 0; j < E_BULLETS; j++)
		{
			CEnemyBulletObject** ppBullets = static_cast<CEnemyObject*>(m_ppObjects[i])->m_ppBullets;
			CBarrierObject* pBarrier = static_cast<CAirplanePlayer*>(m_pPlayer)->m_pBarrier;
			if (ppBullets[j]->m_bActive && !ppBullets[j]->crashed && pBarrier->m_pMesh->m_xmBSphere.Intersects(ppBullets[j]->m_xmOOBB))
			{
				if (pBarrier->life > 1)
				{
					pBarrier->life -= 1;
					float new_speed = 200 - 20 * (10 - pBarrier->life);
					pBarrier->SetRotationSpeed(new_speed);
					ppBullets[j]->SetMovingDirection_Reverse();
					ppBullets[j]->SetColor(RGB(128, 128, 255));
					ppBullets[j]->crashed = true;
					ppBullets[j]->m_fExplosionSpeed = 30.0f;
				}
				else
				{
					pBarrier->m_bBlowingUp = true;
				}
			}
		}
	}
}

void CScene::Animate(float fElapsedTime)
{
	m_pWallsObject->Animate(fElapsedTime);

	for (int i = 0; i < m_nObjects; i++)
	{
		if (GameMode != 0)
		{
			XMFLOAT3 Ppos = m_pPlayer->GetPosition();
			m_ppObjects[i]->Animate(fElapsedTime);
			if (!static_cast<CEnemyObject*>(m_ppObjects[i])->m_bBlowingUp)
			{
				static_cast<CRotatingObject*>(m_ppObjects[i])->Rotate_to_Player(fElapsedTime, Ppos);
			}
		}
		else
		{
			m_ppObjects[i]->Animate(fElapsedTime);
		}

	}


	CheckPlayerByWallCollision();

	CheckObjectByWallCollisions();

	CheckObjectByBulletCollisions();

	if (GameMode != 0)
	{
		CheckObjectByObjectCollisions();

		if (static_cast<CAirplanePlayer*>(m_pPlayer)->m_pBarrier->barrier_mode)
			CheckBarrierByBulletCollisions();
		else 
			CheckPlayerByBulletCollisions();
		Score += 0.2;
	}



}

void CScene::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{
	CGraphicsPipeline::SetViewport(&pCamera->m_Viewport);

	CGraphicsPipeline::SetViewPerspectiveProjectTransform(&pCamera->m_xmf4x4ViewPerspectiveProject);
	m_pWallsObject->Render(hDCFrameBuffer, pCamera);
	for (int i = 0; i < m_nObjects; i++) 
		m_ppObjects[i]->Render(hDCFrameBuffer, pCamera);

	if (m_pPlayer) 
		m_pPlayer->Render(hDCFrameBuffer, pCamera);


	// 텍스트 출력 위치
	if (GameMode != 0)
	{
		SetTextColor(hDCFrameBuffer, RGB(255, 255, 255));
		CAirplanePlayer* pPlayer = static_cast<CAirplanePlayer*>(m_pPlayer);
		CBarrierObject* pBarrier = pPlayer->m_pBarrier;
		if (!pBarrier->m_bBlowingUp && pBarrier->barrier_mode)
		{
			int barrier_life = pBarrier->life;
			std::wstring text = L"Barrier Life: " + std::to_wstring(barrier_life);
			TextOut(hDCFrameBuffer, 0, 15, text.c_str(), text.length());

		}

		if (pPlayer->Life > 0) // 플레이어의 체력
		{
			int player_life = pPlayer->Life;
			std::wstring text = L"Player Life: " + std::to_wstring(player_life);
			TextOut(hDCFrameBuffer, 0, 0, text.c_str(), text.length());
		}

		if (GameMode == 1) // 게임 점수
		{
			int game_score = Score;
			std::wstring text = L"Game Score: " + std::to_wstring(game_score);
			TextOut(hDCFrameBuffer, 0, 30, text.c_str(), text.length());
		}
	}
	else
	{
		SetTextColor(hDCFrameBuffer, RGB(255, 255, 255));
		int last_score = Last_Score;
		std::wstring text = L"Last Score: " + std::to_wstring(last_score);
		TextOut(hDCFrameBuffer, 0, 0, text.c_str(), text.length());
	}

//UI
#ifdef _WITH_DRAW_AXIS
	CGraphicsPipeline::SetViewOrthographicProjectTransform(&pCamera->m_xmf4x4ViewOrthographicProject);
	m_pWorldAxis->SetRotationTransform(&m_pPlayer->m_xmf4x4World);
	m_pWorldAxis->Render(hDCFrameBuffer, pCamera);
#endif
}



/////////////////////////////////////////////////////////////////////////////////////////////////
///
//

void CScene::ReadyObjects()
{
	CStartObject::PrepareExplosion();

	float fHalfWidth = 45.0f, fHalfHeight = 45.0f, fHalfDepth = 200.0f;
	CWallMesh* pWallCubeMesh = new CWallMesh(fHalfWidth * 2.0f, fHalfHeight * 2.0f, fHalfDepth * 2.0f, 30);

	m_pWallsObject = new CWallsObject();
	m_pWallsObject->SetPosition(0.0f, 0.0f, 0.0f);
	m_pWallsObject->SetMesh(pWallCubeMesh);
	m_pWallsObject->SetColor(RGB(100, 100, 100));
	m_pWallsObject->m_pxmf4WallPlanes[0] = XMFLOAT4(+1.0f, 0.0f, 0.0f, fHalfWidth);
	m_pWallsObject->m_pxmf4WallPlanes[1] = XMFLOAT4(-1.0f, 0.0f, 0.0f, fHalfWidth);
	m_pWallsObject->m_pxmf4WallPlanes[2] = XMFLOAT4(0.0f, +1.0f, 0.0f, fHalfHeight);
	m_pWallsObject->m_pxmf4WallPlanes[3] = XMFLOAT4(0.0f, -1.0f, 0.0f, fHalfHeight);
	m_pWallsObject->m_pxmf4WallPlanes[4] = XMFLOAT4(0.0f, 0.0f, +1.0f, fHalfDepth);
	m_pWallsObject->m_pxmf4WallPlanes[5] = XMFLOAT4(0.0f, 0.0f, -1.0f, fHalfDepth);
	m_pWallsObject->m_xmOOBBPlayerMoveCheck = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth, fHalfHeight, fHalfDepth * 0.05f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));


	CStartMesh* pStartMesh = new CStartMesh(50.0f, 20.0f, 4.0f);
	m_nObjects = 1;

	m_ppObjects = new CGameObject * [m_nObjects];
	 
	CStartObject* pStartObject = new CStartObject();

	pStartObject->SetMesh(pStartMesh);

	pStartObject->SetColor(RGB(255, 0, 0));
	pStartObject->SetPosition(0.0f, 0.0f, 50.0f);
	pStartObject->SetRotationAxis(XMFLOAT3(0.0f, 0.0f, 0.0f));
	pStartObject->SetRotationSpeed(0.0f);
	pStartObject->SetMovingDirection(XMFLOAT3(0.0f, 1.0f, 0.0f));
	pStartObject->SetMovingSpeed(10.5f);

	m_ppObjects[0] = pStartObject;


}


bool CScene::Get_Start()
{
	CStartObject* startObject = static_cast<CStartObject*>(m_ppObjects[0]);
	return startObject->Get_Start_Value();
}
bool CScene::Get_End()
{
	CAirplanePlayer* Player = static_cast<CAirplanePlayer*>(m_pPlayer);
	if (Player->Life > 0)
	{
		return false;
	}
	else
	{
		return true;
	}

}
