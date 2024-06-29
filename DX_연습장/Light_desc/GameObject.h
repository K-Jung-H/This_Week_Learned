#pragma once
#include "Mesh.h"
//#include "Camera.h"

class CCamera;

class CShader;

struct MATERIAL
{
	XMFLOAT4 m_xmf4Ambient;
	XMFLOAT4 m_xmf4Diffuse;
	XMFLOAT4 m_xmf4Specular; //(r,g,b,a=power)
	XMFLOAT4 m_xmf4Emissive;
};

class CMaterial
{
public:
	CMaterial();
	virtual ~CMaterial();
private:
	int m_nReferences = 0;
public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	//재질의 기본 색상
	XMFLOAT4 m_xmf4Albedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	//재질의 번호
	UINT m_nReflection = 0;

	//재질을 적용하여 렌더링을 하기 위한 쉐이더
	CShader* m_pShader = NULL;
	void SetAlbedo(XMFLOAT4& xmf4Albedo) { m_xmf4Albedo = xmf4Albedo; }
	void SetReflection(UINT nReflection) { m_nReflection = nReflection; }
	void SetShader(CShader* pShader);
};



class CGameObject
{
private:
	int m_nReferences = 0;

public:
	XMFLOAT4X4 m_xmf4x4World;
	CMesh* m_pMesh = NULL;

	CMaterial* m_pMaterial = NULL; // 게임 객체가 쉐이더를 가지지 않고 재질을 가짐

public:
	CGameObject();
	virtual ~CGameObject();

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	void ReleaseUploadBuffers();

	virtual void SetMesh(CMesh* pMesh);
	virtual void SetShader(CShader* pShader);

	void SetMaterial(CMaterial *pMaterial);
	void SetMaterial(UINT nReflection);


	// 상수 버퍼 생성 / 갱신
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList); 
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList); 
	virtual void ReleaseShaderVariables();

	//객체 월드 변환 행렬의 위치 벡터와 방향(x축, y축, z축) 벡터를 반환
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	//게임 객체의 위치를 설정
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);

	//게임 객체를 로컬 x-축, y-축, z-축 방향으로 이동
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	//게임 객체를 (x축, y축, z축)회전
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);


	virtual void Animate(float fTimeElapsed);
	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
};

class CRotatingObject : public CGameObject
{
public:
	CRotatingObject();
	virtual ~CRotatingObject();

private:
	XMFLOAT3 m_xmf3RotationAxis;
	float m_fRotationSpeed;

public:
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) { m_xmf3RotationAxis = xmf3RotationAxis; }
	virtual void Animate(float fTimeElapsed);
};