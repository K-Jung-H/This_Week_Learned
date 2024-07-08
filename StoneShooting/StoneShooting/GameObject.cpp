#include "stdafx.h"
#include "Shader.h"
#include "GameObject.h"

inline float RandF(float fMin, float fMax)
{
	return(fMin + ((float)rand() / (float)RAND_MAX) * (fMax - fMin));
}

XMVECTOR RandomUnitVectorOnSphere()
{
	XMVECTOR xmvOne = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR xmvZero = XMVectorZero();

	while (true)
	{
		XMVECTOR v = XMVectorSet(RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), 0.0f);
		if (!XMVector3Greater(XMVector3LengthSq(v), xmvOne)) return(XMVector3Normalize(v));
	}
}


CGameObject::CGameObject()
{
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
}

CGameObject::~CGameObject()
{
	if (m_pMesh) m_pMesh->Release();
	if (m_pShader)
	{
		m_pShader->ReleaseShaderVariables();
		m_pShader->Release();
	}
}

void CGameObject::SetShader(CShader* pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}
void CGameObject::SetMesh(CMesh* pMesh)
{
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
}

void CGameObject::ReleaseUploadBuffers()
{
	////���� ���۸� ���� ���ε� ���۸� �Ҹ��Ų��. 
	//if (m_pMesh) 
	//	m_pMesh->ReleaseUploadBuffers();
}

void CGameObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}
void CGameObject::ReleaseShaderVariables()
{
}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));

	//��ü�� ���� ��ȯ ����� ��Ʈ ���(32-��Ʈ ��)�� ���Ͽ� ���̴� ����(��� ����)�� �����Ѵ�.
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
}



void CGameObject::Animate(float fTimeElapsed)
{
	if (m_fMovingSpeed != 0.0f)
		Move(m_xmf3MovingDirection, m_fMovingSpeed * fTimeElapsed);

	UpdateBoundingBox();
}

void CGameObject::UpdateBoundingBox()
{
	if (m_pMesh)
	{
		m_pMesh->m_xmBoundingBox.Transform(m_xmOOBB, XMLoadFloat4x4(&m_xmf4x4World));
		XMStoreFloat4(&m_xmOOBB.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBB.Orientation)));
	}
}

void CGameObject::UpdateFriction(float fTimeElapsed)
{ 
	if (m_fMovingSpeed > 0.0f)
	{
		m_fMovingSpeed -= m_fFriction * m_fMovingSpeed * fTimeElapsed;

		if (m_fMovingSpeed < 0.1f)
			m_fMovingSpeed = 0.0f;
	}
}

XMFLOAT3 CGameObject::ApplyTransform(XMFLOAT3 xmfloat3, XMFLOAT4X4 xmfloat4x4)
{
	XMVECTOR temp_pos = XMVectorSet(xmfloat3.x, xmfloat3.y, xmfloat3.z, 1.0f);

	XMMATRIX worldMatrix = XMLoadFloat4x4(&xmfloat4x4);
	temp_pos = XMVector3Transform(temp_pos, worldMatrix);

	XMFLOAT3 now_pos;
	XMStoreFloat3(&now_pos, temp_pos);

	return now_pos;
}

void CGameObject::OnPrepareRender()
{
}

void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	//���� ��ü�� ī�޶� ���̸� �������Ѵ�. // IsVisible(pCamera)
	if (true)
	{
		UpdateShaderVariables(pd3dCommandList);
		if (m_pShader) 
			m_pShader->Render(pd3dCommandList, pCamera);
		if (m_pMesh) 
			m_pMesh->Render(pd3dCommandList, picked);
	}

}


void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}

void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

XMFLOAT3 CGameObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}
//���� ��ü�� ���� z-�� ���͸� ��ȯ�Ѵ�.
XMFLOAT3 CGameObject::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32,m_xmf4x4World._33)));
}
//���� ��ü�� ���� y-�� ���͸� ��ȯ�Ѵ�.
XMFLOAT3 CGameObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22,
		m_xmf4x4World._23)));
}
//���� ��ü�� ���� x-�� ���͸� ��ȯ�Ѵ�.
XMFLOAT3 CGameObject::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12,
		m_xmf4x4World._13)));
}
//���� ��ü�� ���� x-�� �������� �̵��Ѵ�.
void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

//���� ��ü�� ���� y-�� �������� �̵��Ѵ�.
void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

//���� ��ü�� ���� z-�� �������� �̵��Ѵ�.
void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::Move(XMFLOAT3& vDirection, float fSpeed)
{
	SetPosition(m_xmf4x4World._41 + vDirection.x * fSpeed, m_xmf4x4World._42 + vDirection.y * fSpeed, m_xmf4x4World._43 + vDirection.z * fSpeed);
}


void CGameObject::SetMovingDirection_Reverse()
{
	m_xmf3MovingDirection.x *= -1.0f;
	m_xmf3MovingDirection.y *= -1.0f;
	m_xmf3MovingDirection.z *= -1.0f;
}

//���� ��ü�� �־��� ������ ȸ���Ѵ�.
void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch),
		XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis),
		XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::LookTo(XMFLOAT3& xmf3LookTo, XMFLOAT3& xmf3Up)
{
	XMFLOAT4X4 xmf4x4View = Matrix4x4::LookToLH(GetPosition(), xmf3LookTo, xmf3Up);
	m_xmf4x4World._11 = xmf4x4View._11; m_xmf4x4World._12 = xmf4x4View._21; m_xmf4x4World._13 = xmf4x4View._31;
	m_xmf4x4World._21 = xmf4x4View._12; m_xmf4x4World._22 = xmf4x4View._22; m_xmf4x4World._23 = xmf4x4View._32;
	m_xmf4x4World._31 = xmf4x4View._13; m_xmf4x4World._32 = xmf4x4View._23; m_xmf4x4World._33 = xmf4x4View._33;
}

void CGameObject::LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up)
{
	XMFLOAT4X4 xmf4x4View = Matrix4x4::LookAtLH(GetPosition(), xmf3LookAt, xmf3Up);
	m_xmf4x4World._11 = xmf4x4View._11; m_xmf4x4World._12 = xmf4x4View._21; m_xmf4x4World._13 = xmf4x4View._31;
	m_xmf4x4World._21 = xmf4x4View._12; m_xmf4x4World._22 = xmf4x4View._22; m_xmf4x4World._23 = xmf4x4View._32;
	m_xmf4x4World._31 = xmf4x4View._13; m_xmf4x4World._32 = xmf4x4View._23; m_xmf4x4World._33 = xmf4x4View._33;
}



bool CGameObject::IsVisible(CCamera* pCamera)
{
	OnPrepareRender();
	bool bIsVisible = false;
	BoundingOrientedBox xmBoundingBox = m_pMesh->GetBoundingBox();

	//�� ��ǥ���� �ٿ�� �ڽ��� ���� ��ǥ��� ��ȯ�Ѵ�. 
	xmBoundingBox.Transform(xmBoundingBox, XMLoadFloat4x4(&m_xmf4x4World));
	if (pCamera) 
		bIsVisible = pCamera->IsInFrustum(xmBoundingBox);
	return(bIsVisible);
}

void CGameObject::GenerateRayForPicking(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4&
	xmf4x4View, XMFLOAT3* pxmf3PickRayOrigin, XMFLOAT3* pxmf3PickRayDirection)
{
	XMFLOAT4X4 xmf4x4WorldView = Matrix4x4::Multiply(m_xmf4x4World, xmf4x4View);
	XMFLOAT4X4 xmf4x4Inverse = Matrix4x4::Inverse(xmf4x4WorldView);
	XMFLOAT3 xmf3CameraOrigin(0.0f, 0.0f, 0.0f);
	//ī�޶� ��ǥ���� ������ �� ��ǥ��� ��ȯ�Ѵ�. 
	*pxmf3PickRayOrigin = Vector3::TransformCoord(xmf3CameraOrigin, xmf4x4Inverse);
	//ī�޶� ��ǥ���� ��(���콺 ��ǥ�� ����ȯ�Ͽ� ���� ��)�� �� ��ǥ��� ��ȯ�Ѵ�. 
	*pxmf3PickRayDirection= Vector3::TransformCoord(xmf3PickPosition, xmf4x4Inverse);
	//������ ���� ���͸� ���Ѵ�. 
	*pxmf3PickRayDirection = Vector3::Normalize(Vector3::Subtract(*pxmf3PickRayDirection, *pxmf3PickRayOrigin));
}

int CGameObject::PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfHitDistance)
{
	int nIntersected = 0;
	if (m_pMesh)
	{
		XMFLOAT3 xmf3PickRayOrigin, xmf3PickRayDirection;
		//�� ��ǥ���� ������ �����Ѵ�. 
		GenerateRayForPicking(xmf3PickPosition, xmf4x4View, &xmf3PickRayOrigin, 
		&xmf3PickRayDirection);
		//�� ��ǥ���� ������ �޽��� ������ �˻��Ѵ�. 
		nIntersected = m_pMesh->CheckRayIntersection(xmf3PickRayOrigin, 
		xmf3PickRayDirection, pfHitDistance);
	}
	return(nIntersected);
}

/// //////////////////////////////////////////////////////////////////////////////////////////////////

CRotatingObject::CRotatingObject()
{
	m_xmf3RotationAxis = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_fRotationSpeed = 360.0f;
}
CRotatingObject::~CRotatingObject()
{
}

void CRotatingObject::Rotate_to_Player(float fElapsedTime, XMFLOAT3& xmf3LookTo)
{
	LookAt(xmf3LookTo, GetLook());
	//m_xmf4x4World = Matrix4x4::Multiply(XMMatrixRotationRollPitchYaw(XMConvertToRadians(90.0f), 0.0f, 0.0f), m_xmf4x4World);
	//m_xmf4x4World = Matrix4x4::Multiply(XMMatrixRotationRollPitchYaw(0.0f, XMConvertToRadians(180.0f), 0.0f), m_xmf4x4World);
}

void CRotatingObject::Animate(float fTimeElapsed)
{
	if (m_fRotationSpeed != 0.0f)
		Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);

	CGameObject::Animate(fTimeElapsed);

}

/////////////////////////////////////////////////////////////////////////////////////////
XMFLOAT3 CExplosiveObject::m_pxmf3SphereVectors[EXPLOSION_DEBRISES];
CMesh* CExplosiveObject::m_WExplosionMesh = NULL;
CMesh* CExplosiveObject::m_BExplosionMesh = NULL;

CExplosiveObject::CExplosiveObject()
{
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());

}

CExplosiveObject::~CExplosiveObject()
{
}

void CExplosiveObject::PrepareExplosion(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (int i = 0; i < EXPLOSION_DEBRISES; i++) 
		XMStoreFloat3(&m_pxmf3SphereVectors[i], ::RandomUnitVectorOnSphere());

	m_BExplosionMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 0.5f, 0.5f, 0.5f,
		{ 0.1f, 0.1f, 0.1f, 1.0f }, { 0.2f, 0.2f, 0.2f, 1.0f });

	m_WExplosionMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 0.5f, 0.5f, 0.5f,
		{ 0.9f, 0.9f, 0.9f, 1.0f }, { 0.7f, 0.7f, 0.7f, 1.0f });
}

void CExplosiveObject::Animate(float fElapsedTime)
{
	if (m_bBlowingUp)
	{
		m_fElapsedTimes += fElapsedTime;
		if (m_fElapsedTimes <= m_fDuration)
		{
			XMFLOAT3 xmf3Position = GetPosition();
			for (int i = 0; i < EXPLOSION_DEBRISES; i++)
			{
				m_pxmf4x4Transforms[i] = Matrix4x4::Identity();
				m_pxmf4x4Transforms[i]._41 = xmf3Position.x + m_pxmf3SphereVectors[i].x * m_fExplosionSpeed * m_fElapsedTimes;
				m_pxmf4x4Transforms[i]._42 = xmf3Position.y + m_pxmf3SphereVectors[i].y * m_fExplosionSpeed * m_fElapsedTimes;
				m_pxmf4x4Transforms[i]._43 = xmf3Position.z + m_pxmf3SphereVectors[i].z * m_fExplosionSpeed * m_fElapsedTimes;
				m_pxmf4x4Transforms[i] = Matrix4x4::Multiply(Matrix4x4::RotationAxis(m_pxmf3SphereVectors[i], m_fExplosionRotation * m_fElapsedTimes), m_pxmf4x4Transforms[i]);
			}
		}
		else
		{
			m_bBlowingUp = false;
			m_fElapsedTimes = 0.0f;
			m_bActive = false;
		}
	}
	else
	{
		CRotatingObject::Animate(fElapsedTime);
	}
}

void CExplosiveObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (m_bBlowingUp)
	{
		Render_Particle(pd3dCommandList, pCamera);
	}
	else
	{
		CGameObject::Render(pd3dCommandList, pCamera);
	}
}

void CExplosiveObject::Render_Particle(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	//IsVisible(pCamera)
	if (true)
	{
		if (m_pShader)
			m_pShader->Render(pd3dCommandList, pCamera);
		for (int i = 0; i < EXPLOSION_DEBRISES; i++)
		{
			XMFLOAT4X4 xmf4x4World;
			XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_pxmf4x4Transforms[i])));

			//��ü�� ���� ��ȯ ����� ��Ʈ ���(32-��Ʈ ��)�� ���Ͽ� ���̴� ����(��� ����)�� �����Ѵ�.
			pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);

			if (player_team)
			{
				if (m_WExplosionMesh)
					m_WExplosionMesh->Render(pd3dCommandList);
			}
			else
			{
				if (m_BExplosionMesh)
					m_BExplosionMesh->Render(pd3dCommandList);
			}
		}
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////

CBoardObject::CBoardObject()
{
	
}

CBoardObject::~CBoardObject()
{
}

void CBoardObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	UpdateShaderVariables(pd3dCommandList);
	if (m_pShader)
		m_pShader->Render(pd3dCommandList, pCamera);

	if (m_pMesh)
		m_pMesh->Render(pd3dCommandList);
}

///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////





/////////////////////////////////////////////////////////////////////////////////////////////////////

StoneObject::StoneObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_fDuration = 0.5f;
	m_fExplosionSpeed = 50.0f;
}

StoneObject::~StoneObject()
{
}

void StoneObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	CExplosiveObject::Render(pd3dCommandList, pCamera);
}

void StoneObject::Animate(float fElapsedTime)
{
	CExplosiveObject::Animate(fElapsedTime);

	XMVECTOR center = XMLoadFloat3(&m_pMesh->m_xmBoundingSphere.Center);
	XMVECTOR transformedCenter = XMVector3Transform(center, XMLoadFloat4x4(&m_xmf4x4World));
	XMStoreFloat3(&m_xmOOSP.Center, transformedCenter);

	m_xmOOSP.Radius = m_pMesh->m_xmBoundingSphere.Radius;
}



////////////////////////////////////////////////////////////////////////////////////////////////
