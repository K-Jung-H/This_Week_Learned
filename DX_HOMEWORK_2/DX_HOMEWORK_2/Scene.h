#pragma once
#include "Camera.h"
#include "Timer.h"
#include "Shader.h"

class CScene
{
public:
	CScene();
	~CScene();

	//씬에서 마우스와 키보드 메시지를 처리한다. 
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int mode);
	void ReleaseObjects();

	bool ProcessInput(UCHAR* pKeysBuffer);
	void AnimateObjects(float fTimeElapsed);
	
	void Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	//void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	void ReleaseUploadBuffers();

	//그래픽 루트 시그너쳐를 생성한다. 
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	
	void CreateGraphicsPipelineState(ID3D12Device* pd3dDevice);

	ID3D12RootSignature* GetGraphicsRootSignature();

	void CheckObjectByObjectCollisions();
	void CheckObjectByWallCollisions();
	void CheckPlayerByWallCollision(DWORD camera_mode);
	void CheckObjectByBulletCollisions();
	void CheckPlayerByBulletCollisions();
	void CheckBarrierByBulletCollisions();
	//씬의 모든 게임 객체들에 대한 마우스 픽킹을 수행한다. 
	CGameObject *PickObjectPointedByCursor(int xClient, int yClient, CCamera *pCamera);
	CGameObject* PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfNearHitDistance);
protected:
	CObjectsShader* m_pShaders = NULL;
	int m_nShaders = 1;

	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;
	//루트 시그너쳐를 나타내는 인터페이스 포인터이다.

	ID3D12PipelineState* m_pd3dPipelineState = NULL;
	//파이프라인 상태를 나타내는 인터페이스 포인터이다.
	

public:
	CPlayer* m_pPlayer = NULL;

	CGameObject** m_ppGameObjects = NULL;

	CWallsObject* m_pWallsObject = NULL;

	CGameObject* m_pSelectedObject = NULL; // 피킹된 것
	
	int m_nGameObjects = 0;

	float m_fElapsedTime = 0.0f;

	int scene_num = 1;
	bool scene_change = false;
	bool is_killed_1 = false;
	bool is_killed_2 = false;

};