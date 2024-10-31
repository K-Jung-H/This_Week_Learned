//-----------------------------------------------------------------------------
// File: Shader.h
//-----------------------------------------------------------------------------

#pragma once
#include "Scene.h"
#include "Object.h"
#include "Camera.h"

class CScene;

class CShader
{
public:
	CShader();
	virtual ~CShader();

private:
	int									m_nReferences = 0;

protected:
	ID3DBlob*							m_pd3dVertexShaderBlob = NULL;
	ID3DBlob*							m_pd3dPixelShaderBlob = NULL;

	int									m_nPipelineStates = 0;
	ID3D12PipelineState**				m_ppd3dPipelineStates = NULL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC	m_d3dPipelineStateDesc;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();

	D3D12_SHADER_BYTECODE CompileShaderFromFile(WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob **ppd3dShaderBlob);
	D3D12_SHADER_BYTECODE ReadCompiledShaderFromFile(WCHAR *pszFileName, ID3DBlob **ppd3dShaderBlob=NULL);

	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) { }
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList) { }
	virtual void ReleaseShaderVariables() { }

	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4 *pxmf4x4World) { }

	virtual void OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList, int nPipelineState=0);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState=0);

	virtual void ReleaseUploadBuffers() { }

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext = NULL) { }
	virtual void AnimateObjects(float fTimeElapsed) { }
	virtual void ReleaseObjects() { }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

class OOBBShader : public CShader
{
public:
	OOBBShader();
	virtual ~OOBBShader();

	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class ScreenShader : public CShader
{
public:
	ScreenShader();
	virtual ~ScreenShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext = NULL);
	
	bool Scroll_Update(float fTimeElapsed, float value);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void ReleaseObjects();

	void Set_Start_Sceen_UI();
	void Set_Game_Sceen_UI();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState = 0);
	virtual void Bar_Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState = 0);

	
	virtual void ReleaseUploadBuffers();

	std::string PickObjectPointedByCursor(float xClient, float yClient, CCamera* pCamera);
public:
	Screen_Rect** screen_Objects = 0;

	Screen_Rect* main_screen_ptr = NULL;
	Screen_Rect* start_button_ptr = NULL;
	Screen_Rect* option_icon_button_ptr = NULL;
	Screen_Rect* info_icon_button_ptr = NULL;

	Screen_Rect* info_screen_ptr = NULL;
	Screen_Rect* play_info_ptr = NULL;


	Screen_Rect* menu_button_ptr = NULL;
	
	Screen_Rect* pause_screen_ptr = NULL;
	Screen_Rect* pause_menu_box_ptr = NULL;

	Screen_Rect* option_screen_ptr = NULL;
	Screen_Rect* option_menu_box_ptr = NULL;

	Screen_Rect* option_player_speed_ptr = NULL;
	Screen_Rect* option_player_speed_minus_button_ptr = NULL;
	Screen_Rect* option_player_speed_plus_button_ptr = NULL;

	Screen_Rect* option_stone_speed_ptr = NULL;
	Screen_Rect* option_stone_speed_minus_button_ptr = NULL;
	Screen_Rect* option_stone_speed_plus_button_ptr = NULL;

	Screen_Rect* option_difficulty_ptr = NULL;
	Screen_Rect* option_difficulty_minus_button_ptr = NULL;
	Screen_Rect* option_difficulty_plus_button_ptr = NULL;


	Screen_Rect* Power_Bar_ptr = NULL;
	Screen_Rect* Time_Bar_ptr = NULL;
	int								m_nObjects = 0;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CSkyBoxShader : public CShader
{
public:
	CSkyBoxShader();
	virtual ~CSkyBoxShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();

	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
};

class CTerrainShader : public CShader
{
public:
	CTerrainShader();
	virtual ~CTerrainShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
};

class Outline_Shader : public CShader
{
public:
	Outline_Shader();
	virtual ~Outline_Shader();

	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);

	virtual void Outline_Shader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState);

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CStandardShader : public CShader
{
public:
	CStandardShader();
	virtual ~CStandardShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();

	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CObjectsShader : public CStandardShader
{
public:
	CObjectsShader();
	virtual ~CObjectsShader();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext = NULL);
	void AnimateObjects(float fTimeElapsed);
	void ReleaseObjects();

	void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState=0);

public:
	static CRawFormatImage* Object_map;
	std::vector<CGameObject*>m_ppObjects;

	static bool Show_Collider;
	static bool Show_Attack_Collider;
	static bool Show_ETC_Collider;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CPlayerShader : public CShader
{
public:
	CPlayerShader();
	virtual ~CPlayerShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();

	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

class Diffuse_Shader : public CObjectsShader
{
public:
	Diffuse_Shader();
	virtual ~Diffuse_Shader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext = NULL);

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState = 0);

public:

};

class BOX_Shader : public CObjectsShader
{
public:
	BOX_Shader();
	virtual ~BOX_Shader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext = NULL);

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState = 0);

public:
};

class Asteroid_Shader : public CObjectsShader
{
private:
	CHeightMapTerrain* pTerrain_info = NULL;
	CGameObject* target_obj = NULL;
	CMaterial* pAsteroidMaterial = NULL;
	CMaterial* outline_Material = NULL;
	CMesh* asteroid_mesh = NULL;

public:
	Asteroid_Shader();
	virtual ~Asteroid_Shader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	void Set_Outline_Shader(Outline_Shader* pshader) { outline_Material->SetShader(pshader); };

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext = NULL);

	void AnimateObjects(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState = 0);

	void Set_Target(CGameObject* obj_ptr) { target_obj = obj_ptr; };
	void Add_Object(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT3 pos, CGameObject* target_ptr);
};

class CBillboardObjectsShader : public CObjectsShader
{
public:
	CBillboardObjectsShader();
	virtual ~CBillboardObjectsShader();

	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext = NULL);
	virtual void ReleaseObjects();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState = 0);
	virtual D3D12_BLEND_DESC CreateBlendState();

	virtual void ReleaseUploadBuffers();

	CRawFormatImage* m_pRawFormatImage = NULL;
};

class Black_Hole_Shader : public CBillboardObjectsShader
{
private:
	Black_Hole_Object* player_black_hole = NULL;
	Black_Hole_Object* spawner_hole = NULL;
public:
	Black_Hole_Shader();
	virtual ~Black_Hole_Shader();

	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext = NULL);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState = 0);
	Black_Hole_Object* Get_Player_Black_Hole() { return player_black_hole; }

};

class Sprite_Billboard_Shader : public CBillboardObjectsShader
{
private:
	CMesh* boom_mesh = NULL;
	CMaterial* boom_material = NULL;

public:
	Sprite_Billboard_Shader();
	virtual ~Sprite_Billboard_Shader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext = NULL);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState = 0);

	void Add_Object(XMFLOAT3 pos);
};
