//-----------------------------------------------------------------------------
// File: Scene.h
//-----------------------------------------------------------------------------

#pragma once

#include "Shader.h"
#include "Player.h"

class CDescriptorHeap
{
private:
	int								m_nReferences = 0;
public:
	CDescriptorHeap();
	~CDescriptorHeap();

	void AddRef() { m_nReferences++; } // 새로운 씬이 생성될때마다 호출 == 씬의 BuildObject 에 위치 시키기
	void Release() { if (--m_nReferences <= 0) delete this; } // 씬 제거 될때 호출 == 씬의 ReleaseObject에 위치 시키기

	ID3D12DescriptorHeap* m_pd3dCbvSrvDescriptorHeap = NULL;

	D3D12_CPU_DESCRIPTOR_HANDLE			m_d3dCbvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE			m_d3dCbvGPUDescriptorStartHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE			m_d3dSrvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE			m_d3dSrvGPUDescriptorStartHandle;

	D3D12_CPU_DESCRIPTOR_HANDLE			m_d3dCbvCPUDescriptorNextHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE			m_d3dCbvGPUDescriptorNextHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE			m_d3dSrvCPUDescriptorNextHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE			m_d3dSrvGPUDescriptorNextHandle;

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandleForHeapStart() { return(m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart()); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandleForHeapStart() { return(m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart()); }

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUCbvDescriptorStartHandle() { return(m_d3dCbvCPUDescriptorStartHandle); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUCbvDescriptorStartHandle() { return(m_d3dCbvGPUDescriptorStartHandle); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUSrvDescriptorStartHandle() { return(m_d3dSrvCPUDescriptorStartHandle); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUSrvDescriptorStartHandle() { return(m_d3dSrvGPUDescriptorStartHandle); }
};

class CScene
{
public:
    CScene();
    ~CScene();

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseObjects();
	virtual void ReleaseUploadBuffers();


	virtual ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	virtual ID3D12RootSignature *GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }
	void SetGraphicsRootSignature(ID3D12GraphicsCommandList *pd3dCommandList) { pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature); }

	virtual bool ProcessInput(UCHAR *pKeysBuffer);
	virtual void AnimateObjects(float fDeltaTime);

	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera=NULL);

	CGameObject* PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera);



	CHeightMapTerrain *GetTerrain() { return(m_pTerrain); }

	void SetPlayer(CPlayer* pPlayer) { m_pPlayer = pPlayer; }

protected:

	std::vector<CShader*>			shader_list;
	CTextureToScreenShader*		screen_mesh_shader = NULL;

	CPlayer*								m_pPlayer = NULL;


	CHeightMapTerrain			*m_pTerrain = NULL;
	CSkyBox						*m_pSkyBox = NULL;

	ID3D12RootSignature			*m_pd3dGraphicsRootSignature = NULL;

public:
	static CDescriptorHeap*		m_pDescriptorHeap;

	static void CreateCbvSrvDescriptorHeaps(ID3D12Device* pd3dDevice, int nConstantBufferViews, int nShaderResourceViews);
	static void CreateConstantBufferViews(ID3D12Device* pd3dDevice, int nConstantBufferViews, ID3D12Resource* pd3dConstantBuffers, UINT nStride);
	static D3D12_GPU_DESCRIPTOR_HANDLE CreateConstantBufferView(ID3D12Device* pd3dDevice, ID3D12Resource* pd3dConstantBuffer, UINT nStride);
	static D3D12_GPU_DESCRIPTOR_HANDLE CreateConstantBufferView(ID3D12Device* pd3dDevice, D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress, UINT nStride);
	static void CreateShaderResourceViews(ID3D12Device* pd3dDevice, CTexture* pTexture, UINT nDescriptorHeapIndex, UINT nRootParameterStartIndex);
	static void CreateShaderResourceView(ID3D12Device* pd3dDevice, CTexture* pTexture, int nIndex, UINT nRootParameterStartIndex);
	static void CreateShaderResourceView(ID3D12Device* pd3dDevice, CTexture* pTexture, int nIndex);

	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandleForHeapStart() { return(m_pDescriptorHeap->m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart()); }
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandleForHeapStart() { return(m_pDescriptorHeap->m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart()); }

	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUCbvDescriptorStartHandle() { return(m_pDescriptorHeap->m_d3dCbvCPUDescriptorStartHandle); }
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUCbvDescriptorStartHandle() { return(m_pDescriptorHeap->m_d3dCbvGPUDescriptorStartHandle); }
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUCbvDescriptorNextHandle() { return(m_pDescriptorHeap->m_d3dCbvGPUDescriptorNextHandle); }
	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUSrvDescriptorStartHandle() { return(m_pDescriptorHeap->m_d3dSrvCPUDescriptorStartHandle); }
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUSrvDescriptorStartHandle() { return(m_pDescriptorHeap->m_d3dSrvGPUDescriptorStartHandle); }

};

class Start_Scene : public CScene
{
private:
	std::string selected_screen_info;
public:
	Start_Scene();
	~Start_Scene();

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseObjects();

	virtual ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	virtual ID3D12RootSignature* GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }

	virtual bool ProcessInput(UCHAR* pKeysBuffer);
	virtual void AnimateObjects(float fDeltaTime);

	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);

	virtual void ReleaseUploadBuffers();
};

//=====================================================================

class Game_Scene : public CScene
{
public:
	Game_Scene();
	~Game_Scene();

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseObjects();

	virtual ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	virtual ID3D12RootSignature* GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }

	virtual bool ProcessInput(UCHAR* pKeysBuffer);
	virtual void AnimateObjects(float fDeltaTime);

	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);

	virtual void ReleaseUploadBuffers();
};