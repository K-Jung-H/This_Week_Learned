//-----------------------------------------------------------------------------
// File: Scene.h
//-----------------------------------------------------------------------------

#pragma once

#include "Shader.h"
#include "Player.h"

#define MAX_LIGHTS			16 

#define POINT_LIGHT			1
#define SPOT_LIGHT			2
#define DIRECTIONAL_LIGHT	3

struct LIGHT
{
	XMFLOAT4				m_xmf4Ambient;
	XMFLOAT4				m_xmf4Diffuse;
	XMFLOAT4				m_xmf4Specular;
	XMFLOAT3				m_xmf3Position;
	float 					m_fFalloff;
	XMFLOAT3				m_xmf3Direction;
	float 					m_fTheta; //cos(m_fTheta)
	XMFLOAT3				m_xmf3Attenuation;
	float					m_fPhi; //cos(m_fPhi)
	bool					m_bEnable;
	int						m_nType;
	float					m_fRange;
	float					padding;
};

struct LIGHTS
{
	LIGHT					m_pLights[MAX_LIGHTS];
	XMFLOAT4				m_xmf4GlobalAmbient;
	int						m_nLights;
};

class CDescriptorHeap
{
public:
	CDescriptorHeap();
	~CDescriptorHeap();

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

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseObjects();

	ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	ID3D12RootSignature *GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }

	virtual bool ProcessInput(UCHAR *pKeysBuffer);

	virtual void AnimateObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fDeltaTime);
	virtual void Scene_Update();

	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera=NULL);
	virtual void Message_Render(ID2D1DeviceContext2* pd2dDevicecontext);

	virtual void ReleaseUploadBuffers();
	virtual void Reset();

	virtual void Check_Click_Button(string button_info);

	CHeightMapTerrain* GetTerrain() { return(m_pTerrain); }
	CPlayer								*m_pPlayer = NULL;
	float m_fElapsedTime = 0.0f;


public:
	ID3D12RootSignature					*m_pd3dGraphicsRootSignature = NULL;

	int									m_nGameObjects = 0;
	CGameObject							**m_ppGameObjects = NULL;

	int									m_nShaders = 0;
	CShader								**m_ppShaders = NULL;

	ScreenShader						*screen_shader = NULL;

	CSkyBox								*m_pSkyBox = NULL;
	CHeightMapTerrain* m_pTerrain = NULL;

	std::vector<IDWriteTextFormat*> write_font_list;
	std::vector< ID2D1SolidColorBrush*> brush_list;

	void Add_Font(IDWriteTextFormat* pfont) { write_font_list.push_back(pfont); }
	void Add_Brush(ID2D1SolidColorBrush* pbrush) { brush_list.push_back(pbrush); }

public:
	static CDescriptorHeap*				m_pDescriptorHeap;

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
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUCbvDescriptorNextHandle() { return(m_pDescriptorHeap->m_d3dSrvGPUDescriptorNextHandle); }
	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUSrvDescriptorStartHandle() { return(m_pDescriptorHeap->m_d3dSrvCPUDescriptorStartHandle); }
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUSrvDescriptorStartHandle() { return(m_pDescriptorHeap->m_d3dSrvGPUDescriptorStartHandle); }
};


class Start_Scene : public CScene
{
private:
	std::string selected_screen_info;
	bool start_button_down = false;
	float scroll_value = 0;
public:
	Start_Scene();
	~Start_Scene();

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseObjects();

	//virtual ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	virtual ID3D12RootSignature* GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }

	virtual bool ProcessInput(UCHAR* pKeysBuffer);
	virtual void AnimateObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fDeltaTime);

	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
	virtual void Message_Render(ID2D1DeviceContext2* pd2dDevicecontext);

	virtual void ReleaseUploadBuffers();
	
	void Set_Start_Signal(bool n) { start_button_down = n; }
	bool Get_Start_Signal() { return start_button_down; }

	virtual void Check_Click_Button(string button_info);
	virtual void Reset();

};

//=====================================================================
class CObjectsShader;
class BOX_Shader;
class Asteroid_Shader;
class Black_Hole_Shader;
class Sprite_Billboard_Shader;
class Diffuse_Shader;

struct CollisionInfo {
	CGameObject* collided_obj_A;
	CGameObject* collided_obj_B;
};

class Game_Scene : public CScene
{
protected:
	LIGHT* m_pLights = NULL;
	int									m_nLights = 0;

	XMFLOAT4							m_xmf4GlobalAmbient;

	ID3D12Resource* m_pd3dcbLights = NULL;
	LIGHTS* m_pcbMappedLights = NULL;

	std::string selected_screen_info;

	bool Show_Collider = false;
	bool Pause_Mode = false;
	bool Reset_Signal = false;

	unsigned int game_score = 0;
	float to_be_point = 0.0f;
	float now_point = 0.0f;

	float gravity_hole_runtime = 0.0f;
	float to_be_time = 0.0f;
	float now_time = 0.0f;


	float time_to_score = 0.0f;

public:
	BOX_Shader* box_shader = NULL;
	Asteroid_Shader* enemy_shader = NULL;
	Black_Hole_Shader* black_hole_shader = NULL;
	Sprite_Billboard_Shader* effect_shader = NULL;
	Diffuse_Shader* bullet_shader = NULL;

	Game_Scene();
	~Game_Scene();

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	void BuildDefaultLightsAndMaterials();
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseObjects();

	virtual ID3D12RootSignature* GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }

	virtual bool ProcessInput(UCHAR* pKeysBuffer);
	virtual void AnimateObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fDeltaTime);
	virtual void Scene_Update(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
	virtual void Message_Render(ID2D1DeviceContext2* pd2dDevicecontext);
	bool Get_Pause_State() { return Pause_Mode; }
	bool Get_Reset_State() { return Reset_Signal; }

	virtual void ReleaseUploadBuffers();
	virtual void Check_Click_Button(string button_info);
	virtual void Reset();


	void Add_Boom_Effect(XMFLOAT3 exlposion_pos, bool color_change = false);

	void Update_Power_Bar(float add_point = 0.0f);
	void Update_Time_Bar(float add_point = 0.0f);

	void Collision_Defender(CPlayer* player_ptr, CObjectsShader* pshader);
	void Check_Enemy_Collision();
	void Check_Enemy_Black_Hole();
	void Check_Enemy_Overlap();
	void Check_Bullet_Collision();
};