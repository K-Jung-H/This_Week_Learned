#pragma once
#include "GameObject.h"
#include "Camera.h"

// 객체를 렌더링할 때 적용하는 상수 버퍼 데이터
struct CB_GAMEOBJECT_INFO
{
	XMFLOAT4X4 m_xmf4x4World;
	
	// 객체에 적용될 재질 번호
	UINT m_nMaterial;
};

// 플레이어 객체를 렌더링할 때 적용하는 상수 버퍼 데이터
struct CB_PLAYER_INFO
{
	XMFLOAT4X4 m_xmf4x4World;
};

class CShader
{
private:
	int m_nReferences = 0;

protected:
	CGameObject** m_ppObjects = NULL; //셰이더가 포함하는 게임 객체들의 리스트 
	int m_nObjects = 0;

	ID3D12PipelineState** m_ppd3dPipelineStates = NULL; 	//파이프라인 상태 객체들의 리스트
	int m_nPipelineStates = 0;



public:
	CShader();
	virtual ~CShader();

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	D3D12_SHADER_BYTECODE CompileShaderFromFile(WCHAR* pszFileName,
		LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob);

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	//virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World);

	void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World) { }
	void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, MATERIAL* pMaterial) { }

	virtual void ReleaseShaderVariables();
	virtual void ReleaseUploadBuffers();

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext = NULL);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void ReleaseObjects();

	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);


};

class CPlayerShader : public CShader
{
public:
	CPlayerShader();
	virtual ~CPlayerShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void ReleaseShaderVariables();

	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList) { }
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World);

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

protected:
	// 플레이어 객체에 대한 리소스와 리소스 포인터
	ID3D12Resource* m_pd3dcbPlayer = NULL; // 플레이어 객체에 대한 리소스
	CB_PLAYER_INFO* m_pcbMappedPlayer = NULL; // 플레이어 객체에 대한 리소스 포인터
};

// "CObjectsShader" 클래스는 게임 객체들을 포함하는 셰이더 객체
class CObjectsShader : public CShader
{
public:
	CObjectsShader();
	virtual ~CObjectsShader();
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void ReleaseObjects();
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void ReleaseUploadBuffers();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

protected:
	CGameObject** m_ppObjects = NULL;
	int m_nObjects = 0;

	// 쉐이더 객체에 포함되어 있는 모든 게임 객체들에 대한 리소스
	ID3D12Resource* m_pd3dcbGameObjects = NULL; 

	// 쉐이더 객체에 포함되어 있는 모든 게임 객체들에 대한 리소스 포인터
	UINT8* m_pcbMappedGameObjects = NULL; 
};

class UIShader : public CShader
{
public:
	UIShader();
	virtual ~UIShader();
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void ReleaseObjects();
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void ReleaseUploadBuffers();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

protected:
	CGameObject** m_ppObjects = NULL;
	int m_nObjects = 0;

	// 쉐이더 객체에 포함되어 있는 모든 게임 객체들에 대한 리소스
	ID3D12Resource* m_pd3dcbGameObjects = NULL;

	// 쉐이더 객체에 포함되어 있는 모든 게임 객체들에 대한 리소스 포인터
	UINT8* m_pcbMappedGameObjects = NULL;
};