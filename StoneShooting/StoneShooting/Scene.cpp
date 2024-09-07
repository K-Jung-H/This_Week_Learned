#include "stdafx.h"
#include "Scene.h"
#include "Player.h"

/*
�߰� ��  ����:

��ǻ���� ���� ����� ������, �������� �ϴÿ��� ������ Ÿ���� ������ ��ġ�� �ϳ� �������� �ϱ�

//=============================================
--�߰� ��� ����--
�������� ������ ��, �ƾ� ó��, ���� ���鿡�� �������� �������°� �� �� �Ͽ� �ٶ󺸰� �ϸ� ���� ��

*/
//=========================================================================================
CMaterial* Playing_Scene::material_color_white_stone = NULL;
CMaterial* Playing_Scene::material_color_black_stone = NULL;

CMaterial* Playing_Scene::material_color_player_selected = NULL;
CMaterial* Playing_Scene::material_color_com_selected = NULL;

CMaterial* Playing_Scene::material_color_white_particle = NULL;
CMaterial* Playing_Scene::material_color_black_particle = NULL;

CMaterial* Playing_Scene::material_color_board = NULL;
CMaterial* Playing_Scene::material_color_mountain = NULL;
CMaterial* Playing_Scene::material_color_none = NULL; 



CMaterial* Start_Scene::material_color_white_stone = NULL;
CMaterial* Start_Scene::material_color_black_stone = NULL;
CMaterial* Start_Scene::material_color_board = NULL;
CMaterial* Start_Scene::material_color_mountain = NULL;
CMaterial* Start_Scene::material_color_none = NULL;

CScene::CScene()
{
}

CScene::~CScene()
{
}

ID3D12RootSignature* CScene::GetGraphicsRootSignature()
{
	return(m_pd3dGraphicsRootSignature);
}


bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}
bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

void CScene::BuildScene(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}
void CScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}
void CScene::BuildUIs(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CScene::ReleaseObjects()
{
}

void CScene::ProcessInput(UCHAR* pKeysBuffer, XMFLOAT3 rotate, float fTimeElapsed)
{
	return;
}
void CScene::AnimateObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed)
{
}
void CScene::Scene_Update(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed)
{
}

void CScene::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
}
void CScene::Particle_Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
}
void CScene::UI_Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CScene::Message_Render(ID2D1DeviceContext2* pd2dDevicecontext)
{

}

void CScene::ReleaseUploadBuffers()
{
}
ID3D12RootSignature* CScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	return NULL;
}
ID3D12RootSignature* CScene::Create_UI_GraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	return NULL;
}
 void CScene::CreateGraphicsPipelineState(ID3D12Device* pd3dDevice)
 {
 }
void CScene::Create_Shader_Resource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}
void CScene::Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList)
{
}
void CScene::Release_Shader_Resource()
{
}
void CScene::Build_Lights_and_Materials()
{
}
void CScene::Update_Lights(ID3D12GraphicsCommandList* pd3dCommandList)
{
}
void CScene::Set_BackGround_Color(XMFLOAT4 color)
{
	background_color[0] = color.x;
	background_color[1] = color.y;
	background_color[2] = color.z;
	background_color[3] = color.w;
}
float* CScene::Get_BackGround_Color()
{
	return background_color;
}

void CScene::Update_Camera_Zoom(float fTimeElapsed, float m_fTimeLag)
{
	if (zooming)
	{
		XMFLOAT3 start_pos = m_pPlayer->GetPosition();  // ���� �÷��̾��� ��ġ
		XMFLOAT3 end_pos = { 0.0f, 1.0f, 0.0f };        // ��ǥ ��ġ (�� �� ��ġ)

		XMVECTOR start_Vec = XMLoadFloat3(&start_pos); // ���� ��ġ ����
		XMVECTOR end_Vec = XMLoadFloat3(&end_pos);     // ��ǥ ��ġ ����

		// �� ���� ��� (0.0f ~ 1.0f)
		float T = (float)zoom_value / 100.0f;

		// XMVectorLerp�� ����Ͽ� ���۰� �� ���̸� ����
		XMVECTOR target_zoom_vec = XMVectorLerp(start_Vec, end_Vec, T);

		// ���� ī�޶� ��ġ ��������
		XMFLOAT3 current_cam_pos = m_pPlayer->GetCamera()->GetPosition();
		XMVECTOR current_cam_vec = XMLoadFloat3(&current_cam_pos);

		// Ÿ�� ���� ������ ���
		float fTimeLagScale = (m_fTimeLag > 0) ? fTimeElapsed * (1.0f / m_fTimeLag) : 1.0f;

		// XMVectorLerp�� ����Ͽ� ���� ��ġ���� ��ǥ ��ġ�� �ε巴�� �̵�
		XMVECTOR new_cam_vec = XMVectorLerp(current_cam_vec, target_zoom_vec, fTimeLagScale);

		// ����� XMFLOAT3�� ��ȯ�Ͽ� ����
		XMFLOAT3 new_cam_pos;
		XMStoreFloat3(&new_cam_pos, new_cam_vec);

		// ī�޶� ��ġ ����
		m_pPlayer->GetCamera()->SetPosition(new_cam_pos);
		m_pPlayer->GetCamera()->camera_focus = { 0.0f, 0.0f, 0.0f };

		// ���� �Ϸ�Ǿ����� Ȯ��
		if (zoom_value == 0) {
			zooming = false;
			m_pPlayer->GetCamera()->SetPosition(start_pos);
			m_pPlayer->GetCamera()->camera_focus = { 0.0f, 0.0f, 0.0f };
		}
	}
}

void CScene::Update_Player_pos_Oribit(float fTimeElapsed, float m_fTimeLag)
{
	// ���� ������ �������� ��ȯ
	float angleInRadians = XMConvertToRadians(orbit_value);

	// ��ǥ ��ġ ��� (XZ ��鿡���� ���� �˵�)
	float x = 200.0f * cosf(angleInRadians); // X ��ǥ
	float z = 200.0f * sinf(angleInRadians); // Z ��ǥ
	XMFLOAT3 targetPosition = XMFLOAT3(x, 200.0f, z);

	// ���� �÷��̾� ��ġ
	XMFLOAT3 currentPosition = m_pPlayer->GetPosition();

	// Ÿ�� ���� ������ ��� (0�� 1 ������ ����, 1�� ���� (1.0f / m_fTimeLag) ��ŭ �̵�)
	float fTimeLagScale = (m_fTimeLag > 0) ? fTimeElapsed * (1.0f / m_fTimeLag) : 1.0f;

	// XMVectorLerp�� �̿��� ���� ��ġ���� ��ǥ ��ġ�� �ε巴�� �̵�
	XMVECTOR currentPosVec = XMLoadFloat3(&currentPosition);
	XMVECTOR targetPosVec = XMLoadFloat3(&targetPosition);
	XMVECTOR newPosVec = XMVectorLerp(currentPosVec, targetPosVec, fTimeLagScale);

	// ����� XMFLOAT3�� ��ȯ�Ͽ� �÷��̾� ��ġ ������Ʈ
	XMFLOAT3 newPosition;
	XMStoreFloat3(&newPosition, newPosVec);
	m_pPlayer->SetPosition(newPosition);

	// ī�޶��� ��Ŀ�� �� Up ���� ����
	m_pPlayer->GetCamera()->camera_focus = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_pPlayer->GetCamera()->GetUpVector() = XMFLOAT3(0.0f, 1.0f, 0.0f);
}

//=========================================================================================

Start_Scene::Start_Scene()
{
}
Start_Scene::~Start_Scene()
{
}

void Start_Scene::Build_Lights_and_Materials()
{
	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));
	m_pLights->m_xmf4GlobalAmbient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	m_pLights->m_pLights[0].m_bEnable = true;
	m_pLights->m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[0].m_fRange = 300.0f;
	m_pLights->m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Position = XMFLOAT3(0.0f, 50.0f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);

	m_pLights->m_pLights[1].m_bEnable = true;
	m_pLights->m_pLights[1].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);


	//=====================================================
	CMaterialColors white_stone_color = {
		XMFLOAT4(0.5f, 0.5, 0.5f, 1.0f),
		XMFLOAT4(1.0f, 1.0, 1.0f, 1.0f),
		XMFLOAT4(1.0f, 1.0, 1.0f, 20.0f),
		XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f)
	};

	CMaterialColors black_stone_color = {
		XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f),
		XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f),
		XMFLOAT4(0.1f, 0.1f, 0.1f, 20.0f),
		XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)
	};

	CMaterialColors white_particle_color = {
		XMFLOAT4(0.0f, 0.0, 0.0f, 1.0f),
		XMFLOAT4(0.0f, 0.0, 0.0f, 1.0f),
		XMFLOAT4(0.0f, 0.0, 0.0f, 1.0f),
		XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f)
	};

	CMaterialColors black_particle_color = {
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 30.0f),
		XMFLOAT4(0.05f, 0.05f, 0.05f, 1.0f)
	};

	CMaterialColors player_selected_color = {
		 XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f),
		 XMFLOAT4(0.1f, 0.3f, 0.1f, 1.0f),
		 XMFLOAT4(0.1f, 0.1f, 0.1f, 100.0f),
		 XMFLOAT4(0.0f, 0.2f, 0.0f, 1.0f)
	};

	CMaterialColors com_selected_color = {
		XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f),
		XMFLOAT4(0.3f, 0.1f, 0.1f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 100.0f),
		XMFLOAT4(0.2f, 0.0f, 0.0f, 1.0f)
	};

	CMaterialColors board_color = {
		XMFLOAT4(2.0f, 1.5f, 0.2f, 1.0f),
		XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f),
		XMFLOAT4(0.4f, 0.4f, 0.4f, 20.0f),
		XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f)
	};

	CMaterialColors mountain_color = {
		XMFLOAT4(0.0f, 0.3725f, 0.4196f, 1.0f),
		XMFLOAT4(0.0f, 0.3725f, 0.4196f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT4(0.0f, 0.1f, 0.1f, 1.0f)
	};

	CMaterialColors none_color = {
	XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
	XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
	XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
	XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)
	};


	material_color_white_stone = new CMaterial(&white_stone_color);
	material_color_black_stone = new CMaterial(&black_stone_color);
	material_color_board = new CMaterial(&board_color);
	material_color_mountain = new CMaterial(&mountain_color);
	material_color_none = new CMaterial(&none_color);
}

ID3D12RootSignature* Start_Scene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;
	D3D12_ROOT_PARAMETER pd3dRootParameters[6];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 0; //Player
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[1].Descriptor.ShaderRegister = 1; //Camera
	pd3dRootParameters[1].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 2; //GameObject
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[3].Descriptor.ShaderRegister = 3; //Materials
	pd3dRootParameters[3].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[4].Descriptor.ShaderRegister = 4; //Lights
	pd3dRootParameters[4].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[5].Descriptor.ShaderRegister = 5; // Outline
	pd3dRootParameters[5].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[5].Constants.Num32BitValues = 8;
	pd3dRootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = NULL;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;

	::D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);

	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(),
		__uuidof(ID3D12RootSignature), (void**)&pd3dGraphicsRootSignature);

	if (pd3dSignatureBlob)
		pd3dSignatureBlob->Release();

	if (pd3dErrorBlob)
		pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}
ID3D12RootSignature* Start_Scene::Create_UI_GraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	{
		ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;
		D3D12_ROOT_PARAMETER pd3dRootParameters[3];

		pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		pd3dRootParameters[0].Descriptor.ShaderRegister = 0; // GameObject_pos
		pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
		pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

		pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		pd3dRootParameters[1].Descriptor.ShaderRegister = 1; //Camera
		pd3dRootParameters[1].Descriptor.RegisterSpace = 0;
		pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		pd3dRootParameters[2].Descriptor.ShaderRegister = 2; // UI_INFO
		pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
		pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

		D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
		::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
		d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
		d3dRootSignatureDesc.pParameters = pd3dRootParameters;
		d3dRootSignatureDesc.NumStaticSamplers = 0;
		d3dRootSignatureDesc.pStaticSamplers = NULL;
		d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

		ID3DBlob* pd3dSignatureBlob = NULL;
		ID3DBlob* pd3dErrorBlob = NULL;

		::D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);

		pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(),
			__uuidof(ID3D12RootSignature), (void**)&pd3dGraphicsRootSignature);

		if (pd3dSignatureBlob)
			pd3dSignatureBlob->Release();

		if (pd3dErrorBlob)
			pd3dErrorBlob->Release();

		return(pd3dGraphicsRootSignature);
	}
}
void Start_Scene::CreateGraphicsPipelineState(ID3D12Device* pd3dDevice)
{
	//���� ���̴��� �ȼ� ���̴��� �����Ѵ�.
	ID3DBlob* pd3dVertexShaderBlob = NULL;
	ID3DBlob* pd3dPixelShaderBlob = NULL;
	UINT nCompileFlags = 0;
#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	D3DCompileFromFile(L"Shaders.hlsl", NULL, NULL, "VSMain", "vs_5_1", nCompileFlags, 0, &pd3dVertexShaderBlob, NULL);
	D3DCompileFromFile(L"Shaders.hlsl", NULL, NULL, "PSMain", "ps_5_1", nCompileFlags, 0, &pd3dPixelShaderBlob, NULL);
	//�����Ͷ����� ���¸� �����Ѵ�.
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	//���� ���¸� �����Ѵ�.
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//�׷��� ���������� ���¸� �����Ѵ�.
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineStateDesc.pRootSignature = m_pd3dGraphicsRootSignature;
	d3dPipelineStateDesc.VS.pShaderBytecode = pd3dVertexShaderBlob->GetBufferPointer();
	d3dPipelineStateDesc.VS.BytecodeLength = pd3dVertexShaderBlob->GetBufferSize();
	d3dPipelineStateDesc.PS.pShaderBytecode = pd3dPixelShaderBlob->GetBufferPointer();
	d3dPipelineStateDesc.PS.BytecodeLength = pd3dPixelShaderBlob->GetBufferSize();
	d3dPipelineStateDesc.RasterizerState = d3dRasterizerDesc;
	d3dPipelineStateDesc.BlendState = d3dBlendDesc;
	d3dPipelineStateDesc.DepthStencilState.DepthEnable = FALSE;
	d3dPipelineStateDesc.DepthStencilState.StencilEnable = FALSE;
	d3dPipelineStateDesc.InputLayout.pInputElementDescs = NULL;
	d3dPipelineStateDesc.InputLayout.NumElements = 0;
	d3dPipelineStateDesc.SampleMask = UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineStateDesc.NumRenderTargets = 1;
	d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineStateDesc.SampleDesc.Count = 1;
	d3dPipelineStateDesc.SampleDesc.Quality = 0;
	pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc,
		__uuidof(ID3D12PipelineState), (void**)&m_pd3dPipelineState);
	if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
	if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();
}

void Start_Scene::Create_Shader_Resource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256�� ���
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbLights->Map(0, NULL, (void**)&m_pcbMappedLights);
}

void Start_Scene::Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList)
{
	Update_Lights(pd3dCommandList);
}

void Start_Scene::Update_Lights(ID3D12GraphicsCommandList* pd3dCommandList)
{
	// ������ �ּҿ� ���� ���� ����(����)
	::memcpy(m_pcbMappedLights, m_pLights, sizeof(LIGHTS));

	// ���� ���ҽ��� ���� ��� ���� �並 ���̴� ������ ����(���ε�) 
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(4, d3dcbLightsGpuVirtualAddress);
}

void Start_Scene::Release_Shader_Resource()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}
}

void Start_Scene::BuildScene(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	Build_Lights_and_Materials();
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);
	UI_GraphicsRootSignature = Create_UI_GraphicsRootSignature(pd3dDevice);

	Object_GraphicsRootSignature_ptr = m_pd3dGraphicsRootSignature;
	UI_GraphicsRootSignature_ptr = UI_GraphicsRootSignature;

	// ���� �� ���� ���ҽ� ����
	Create_Shader_Resource(pd3dDevice, pd3dCommandList);

	BuildObjects(pd3dDevice, pd3dCommandList);

	Set_BackGround_Color(XMFLOAT4(0.8f, 0.8f, 0.8f, 0.0f));
}

void Start_Scene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	//===========================================================
// ���� ��ü
	Object_Shader = new CObjectsShader[N_Object_Shader];
	Object_Shader[0].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);

	//===========================================================

	Create_Board(pd3dDevice, pd3dCommandList, 300, 300);


	XMFLOAT3 xmf3Scale(16.0f, 4.0f, 16.0f);
	XMFLOAT4 xmf4Color(0.0f, 0.2f, 0.0f, 0.0f);

	 CGameObject* m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("HeightMap.raw"), 257, 257, 17, 17, xmf3Scale, xmf4Color);
	m_pTerrain->SetMaterial(material_color_none);
	m_pTerrain->AddMaterial(material_color_mountain, true);
	m_pTerrain->SetPosition(XMFLOAT3(-257.0f * xmf3Scale.x / 2, -500.0f, -257.0f * xmf3Scale.z / 2));
	game_objects.push_back(m_pTerrain);
	//===========================================================

	//CSphereMeshIlluminated* StoneMesh = new CSphereMeshIlluminated(pd3dDevice, pd3dCommandList, 8.0f, 20, 20, 0.5f);
	//std::vector<XMFLOAT3> w_stone_pos_list;

	//w_stone_pos_list.push_back({ -50, 5, 100 });
	//w_stone_pos_list.push_back({ 0, 5, 100 });
	//w_stone_pos_list.push_back({ 50, 5, 100 });
	//w_stone_pos_list.push_back({ -25, 5, 150 });
	//w_stone_pos_list.push_back({ 0, 5, 150 });
	//w_stone_pos_list.push_back({ 25, 5, 150 });

	//for (XMFLOAT3& w_stone_pos : w_stone_pos_list)
	//	Setting_Stone(pd3dDevice, pd3dCommandList, StoneMesh, w_stone_pos, true);

	////-------------------------------------------------------------------

	//std::vector<XMFLOAT3> b_stone_pos_list;

	//b_stone_pos_list.push_back({ -50, 5, -100 });
	//b_stone_pos_list.push_back({ 0, 5, -100 });
	//b_stone_pos_list.push_back({ 50, 5, -100 });
	//b_stone_pos_list.push_back({ -25, 5, -150 });
	//b_stone_pos_list.push_back({ 0, 5, -150 });
	//b_stone_pos_list.push_back({ 25, 5, -150 });

	//for (XMFLOAT3& b_stone_pos : b_stone_pos_list)
	//	Setting_Stone(pd3dDevice, pd3dCommandList, StoneMesh, b_stone_pos, false);

	//===========================================================


}

void Start_Scene::Create_Board(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float Board_Width, float Board_Depth)
{
	float Board_Half_Width = Board_Width / 2;
	float Board_Half_Depth = Board_Depth / 2;

	CPlaneMeshIlluminated* pboard_mesh = new CPlaneMeshIlluminated(pd3dDevice, pd3dCommandList, Board_Half_Width * 2.0f, Board_Half_Depth * 2.0f, 500);
	CGameObject* board_obj = new CBoardObject(pd3dDevice, pd3dCommandList);

	board_obj->SetPosition(0.0f, 0.0f, 0.0f);
	board_obj->AddMesh(pboard_mesh);
	board_obj->SetMaterial(material_color_board);

	//-------------------------------------------------------

	CGameObject* board_line = NULL;
	CPlaneMeshIlluminated* pboard_line_mesh = new CPlaneMeshIlluminated(pd3dDevice, pd3dCommandList, Board_Half_Width / 80.0f, Board_Half_Depth * 2, 1);

	std::vector<XMFLOAT3> horizontal_line_pos;
	
	float horizontal_pos = -Board_Half_Width;
	while (horizontal_pos <= Board_Half_Width)
	{
		horizontal_line_pos.push_back(XMFLOAT3(horizontal_pos, 0.3f, 0.0f));
		horizontal_pos += 15.0f;
	}

	for (const XMFLOAT3& pos : horizontal_line_pos)
	{
		board_line = new CGameObject(pd3dDevice, pd3dCommandList);
		board_line->SetPosition(XMFLOAT3(pos));
		board_line->SetScale(1.0f, 1.0f, 1.0f);
		board_line->AddMesh(pboard_line_mesh);
		board_line->o_type = Object_Type::Board;
		board_line->SetMaterial(material_color_black_stone);

		board_obj->Add_Child(board_line);
	}

	std::vector<XMFLOAT3> vertical_line_pos;
	XMFLOAT3 vertical_angle = XMFLOAT3{ 0.0f, 1.0f, 0.0f };

	float vertical_pos = -Board_Half_Depth;
	while (vertical_pos <= Board_Half_Depth)
	{
		vertical_line_pos.push_back(XMFLOAT3(0.0f, 0.3f, vertical_pos));
		vertical_pos += 15.0f;
	}

	for (const XMFLOAT3& pos : vertical_line_pos)
	{
		board_line = new CGameObject(pd3dDevice, pd3dCommandList);
		board_line->SetPosition(XMFLOAT3(pos));
		board_line->SetScale(1.0f, 1.0f, Board_Half_Width / Board_Half_Depth);
		board_line->Rotate(&vertical_angle, 90.0f);
		board_line->AddMesh(pboard_line_mesh);
		board_line->o_type = Object_Type::Board;
		board_line->SetMaterial(material_color_black_stone);

		board_obj->Add_Child(board_line);
	}

	game_objects.push_back(board_obj);
}

void Start_Scene::BuildUIs(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void Start_Scene::ReleaseObjects()
{
}
void Start_Scene::ReleaseUploadBuffers()
{
}

void Start_Scene::AnimateObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed)
{
	Scene_Update(pd3dDevice, pd3dCommandList, fTimeElapsed);
}

void Start_Scene::Scene_Update(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed)
{
	if (m_pPlayer->GetCamera()->GetMode() == TOP_VIEW_CAMERA)
	{
		Update_Camera_Zoom(fTimeElapsed, 0.5f);
		Update_Player_pos_Oribit(fTimeElapsed, 0.5f);
	}
}

void Start_Scene::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	Object_Shader[0].Setting_PSO(pd3dCommandList);

	// ī�޶� ���� �� ���� ������Ʈ
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->Update_Shader_Resource(pd3dCommandList);


	// ���� ������Ʈ
	Update_Shader_Resource(pd3dCommandList);
	for (CGameObject* gameobject : game_objects)
	{
		gameobject->UpdateTransform(NULL);
		gameobject->Render(pd3dCommandList, pCamera, &Object_Shader[0]);
	}

	UI_Render(pd3dDevice, pd3dCommandList);
}
void Start_Scene::UI_Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}
void Start_Scene::Message_Render(ID2D1DeviceContext2* pd2dDevicecontext)
{
	static float T = 1.0f;    
	static bool increasing = true; 

	if (increasing)
	{
		T += 0.01f;
		if (T >= 1.5f)
		{
			T = 1.5f;     
			increasing = false; 
		}
	}
	else
	{
		T -= 0.01f;
		if (T <= 1.0f)
		{
			T = 1.0f;  
			increasing = true; 
		}
	}


	D2D1_MATRIX_3X2_F scaleMatrix = D2D1::Matrix3x2F::Scale(T, T);
	D2D1_MATRIX_3X2_F oldTransform;
	pd2dDevicecontext->GetTransform(&oldTransform);
	pd2dDevicecontext->SetTransform(scaleMatrix * oldTransform);

	D2D1_RECT_F message_area = D2D1::RectF(50, 30, 700, 200);
	std::wstring message = L"Press Any Key!!!";
	pd2dDevicecontext->DrawTextW(message.c_str(), (UINT32)wcslen(message.c_str()), write_font_list[0], &message_area, brush_list[0]);

	// ��� �� ���� ��ȯ���� ����
	pd2dDevicecontext->SetTransform(oldTransform);
}

bool Start_Scene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		break;

	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		break;

	case WM_MOUSEWHEEL:
	{
		if (m_pPlayer->GetCamera()->GetMode() == TOP_VIEW_CAMERA)
		{
			zooming = true;

			int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

			if (zDelta > 0) // ��ũ�� ����
				zoom_value -= 2;
			else // ��ũ�� �Ʒ���
				zoom_value += 2;

			zoom_value = std::clamp(zoom_value, 0, 99);
		}
	}
	break;

	default:
		break;
	}
	return(false);
}
bool Start_Scene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_LEFT:		
			orbit_value += 1.0f;
			break;

		case VK_RIGHT:
			orbit_value -= 1.0f;			
			break;

		default:
			break;
		}
		break;
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_TAB:
			break;

		default:
			break;
		}
		break;
	default:
		break;
	}


	return true;
}

void Start_Scene::ProcessInput(UCHAR* pKeysBuffer, XMFLOAT3 rotate, float fTimeElapsed)
{
	float cxDelta = rotate.y;
	float cyDelta = rotate.x;

	//���콺 �Ǵ� Ű �Է��� ������ �÷��̾ �̵��ϰų�(dwDirection) ȸ���Ѵ�(cxDelta �Ǵ� cyDelta).
	if ((cxDelta != 0.0f) || (cyDelta != 0.0f))
	{
		m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
	}
	m_pPlayer->Update(fTimeElapsed);
}

//=========================================================================================

Playing_Scene::Playing_Scene()
{
}
Playing_Scene::~Playing_Scene()
{
}

void Playing_Scene::Build_Lights_and_Materials()
{
	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));
	m_pLights->m_xmf4GlobalAmbient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	m_pLights->m_pLights[0].m_bEnable = true;
	m_pLights->m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[0].m_fRange = 300.0f;
	m_pLights->m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Position = XMFLOAT3(0.0f, 50.0f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);

	m_pLights->m_pLights[1].m_bEnable = true;
	m_pLights->m_pLights[1].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, -0.5f, 0.5f);

	m_pLights->m_pLights[2].m_bEnable = true;
	m_pLights->m_pLights[2].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[2].m_fRange = 30.0f;
	m_pLights->m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Specular = XMFLOAT4(0.2, 0.2, 0.2, 0.0f);
	m_pLights->m_pLights[2].m_xmf3Position = XMFLOAT3(0.0f, 30.0f, 0.0f);
	m_pLights->m_pLights[2].m_xmf3Direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
	m_pLights->m_pLights[2].m_xmf3Attenuation = XMFLOAT3(0.5f, 0.001f, 0.0001f);
	m_pLights->m_pLights[2].m_fFalloff = 0.5f;
	m_pLights->m_pLights[2].m_fPhi = (float)cos(XMConvertToRadians(90.0f));
	m_pLights->m_pLights[2].m_fTheta = (float)cos(XMConvertToRadians(30.0f));

	m_pLights->m_pLights[3].m_bEnable = true;
	m_pLights->m_pLights[3].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[3].m_fRange = 0.0f;
	m_pLights->m_pLights[3].m_xmf4Ambient = XMFLOAT4(-0.3f, 0.1f, 1.0f, 1.0f);
	m_pLights->m_pLights[3].m_xmf4Diffuse = XMFLOAT4(-0.0f, 0.0f, 1.0f, 1.0f);
	m_pLights->m_pLights[3].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 1.0f, 0.0f);
	m_pLights->m_pLights[3].m_xmf3Position = XMFLOAT3(0.0f, 5.0f, 0.0f);
	m_pLights->m_pLights[3].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[3].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.005f);

	

	//=====================================================
	CMaterialColors white_stone_color = {
		XMFLOAT4(0.5f, 0.5, 0.5f, 1.0f),
		XMFLOAT4(1.0f, 1.0, 1.0f, 1.0f),
		XMFLOAT4(1.0f, 1.0, 1.0f, 20.0f),
		XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f) 
	};

	CMaterialColors black_stone_color = {
		XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f),
		XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f),
		XMFLOAT4(0.1f, 0.1f, 0.1f, 20.0f),
		XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)
	};

	CMaterialColors white_particle_color = {
		XMFLOAT4(0.0f, 0.0, 0.0f, 1.0f),
		XMFLOAT4(0.0f, 0.0, 0.0f, 1.0f),
		XMFLOAT4(0.0f, 0.0, 0.0f, 1.0f),
		XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f)
	};

	CMaterialColors black_particle_color = {
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 30.0f),
		XMFLOAT4(0.05f, 0.05f, 0.05f, 1.0f)
	};

	CMaterialColors player_selected_color = {
		 XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f),
		 XMFLOAT4(0.1f, 0.3f, 0.1f, 1.0f),
		 XMFLOAT4(0.1f, 0.1f, 0.1f, 100.0f),
		 XMFLOAT4(0.0f, 0.2f, 0.0f, 1.0f)
	};

	CMaterialColors com_selected_color = {
		XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f),
		XMFLOAT4(0.3f, 0.1f, 0.1f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 100.0f),
		XMFLOAT4(0.2f, 0.0f, 0.0f, 1.0f)
	};

	CMaterialColors board_color = {
		XMFLOAT4(2.0f, 1.5f, 0.2f, 1.0f),
		XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f),
		XMFLOAT4(0.4f, 0.4f, 0.4f, 20.0f),
		XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f)
	};

	CMaterialColors mountain_color = {
		XMFLOAT4(0.0f, 0.3725f, 0.4196f, 1.0f),
		XMFLOAT4(0.0f, 0.3725f, 0.4196f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT4(0.0f, 0.1f, 0.1f, 1.0f)
	};

	CMaterialColors none_color = {
	XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
	XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
	XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
	XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)
	};

	material_color_white_stone = new CMaterial(&white_stone_color);
	material_color_black_stone = new CMaterial(&black_stone_color);

	material_color_white_particle = new CMaterial(&white_particle_color);
	material_color_black_particle = new CMaterial(&black_particle_color);

	material_color_player_selected = new CMaterial(&player_selected_color);
	material_color_com_selected = new CMaterial(&com_selected_color);
	material_color_board = new CMaterial(&board_color);
	material_color_mountain = new CMaterial(&mountain_color);
	
	material_color_none = new CMaterial(&none_color);
}


void Playing_Scene::Create_Shader_Resource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256�� ���
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbLights->Map(0, NULL, (void**)&m_pcbMappedLights);
}

void Playing_Scene::Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList)
{
	Update_Lights(pd3dCommandList);
}

void Playing_Scene::Update_Lights(ID3D12GraphicsCommandList* pd3dCommandList)
{
	// ������ �ּҿ� ���� ���� ����(����)
	::memcpy(m_pcbMappedLights, m_pLights, sizeof(LIGHTS));

	// ���� ���ҽ��� ���� ��� ���� �並 ���̴� ������ ����(���ε�) 
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(4, d3dcbLightsGpuVirtualAddress);
}

void Playing_Scene::Release_Shader_Resource()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}
}

void Playing_Scene::BuildScene(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	Explosion_Particle::Prepare_Particle(pd3dDevice, pd3dCommandList);
	Charge_Particle::Prepare_Particle(pd3dDevice, pd3dCommandList);
	Firework_Particle::Prepare_Particle(pd3dDevice, pd3dCommandList);
	Snow_Particle::Prepare_Particle(pd3dDevice, pd3dCommandList);
	Item::Prepare_Item(pd3dDevice, pd3dCommandList);
	Build_Lights_and_Materials();

	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);
	UI_GraphicsRootSignature = Create_UI_GraphicsRootSignature(pd3dDevice);

	Object_GraphicsRootSignature_ptr = m_pd3dGraphicsRootSignature;
	UI_GraphicsRootSignature_ptr = UI_GraphicsRootSignature;

	CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, TEXTURES);

	// ���� �� ���� ���ҽ� ����
	Create_Shader_Resource(pd3dDevice, pd3dCommandList);
	
	BuildObjects(pd3dDevice, pd3dCommandList);
	BuildUIs(pd3dDevice, pd3dCommandList);


	// �̸� �����Ͽ� �غ�
	item_manager = new Item_Manager();
	item_manager->Frozen_Light = &m_pLights->m_pLights[3];

	Charge_Effect = new Charge_Particle(pd3dDevice, pd3dCommandList, 50.0f, 5.0f, material_color_white_particle, Particle_Type::Charge);
	Charge_Effect->SetMaterial(material_color_none,true);
	Charge_Effect->AddMaterial(material_color_black_particle);
	Charge_Effect->active = true;
	Charge_Effect->m_ppMaterials[0].second = true;
	Charge_Effect->m_ppMaterials[1].second = true;

	Setting_Item(pd3dDevice, pd3dCommandList, XMFLOAT3(30.0f, 10.0f, 0.0f), Item_Type::Frozen_Time);
	Setting_Item(pd3dDevice, pd3dCommandList, XMFLOAT3(-30.0f, 10.0f, 0.0f), Item_Type::Ghost);
	Set_BackGround_Color(XMFLOAT4(0.8f, 0.8f, 0.8f, 0.0f));

}

void Playing_Scene::Create_Board(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float Board_Width, float Board_Depth)
{
	float Board_Half_Width = Board_Width / 2;
	float Board_Half_Depth = Board_Depth / 2;

	CPlaneMeshIlluminated* pboard_mesh = new CPlaneMeshIlluminated(pd3dDevice, pd3dCommandList, Board_Half_Width * 2.0f, Board_Half_Depth * 2.0f, 500);

	m_pBoards = new CBoardObject(pd3dDevice, pd3dCommandList);
	m_pBoards->SetPosition(0.0f, 0.0f, 0.0f);
	m_pBoards->AddMesh(pboard_mesh);
	m_pBoards->SetMaterial(material_color_none);
	m_pBoards->AddMaterial(material_color_board, true);
	m_pBoards->default_collider = m_pBoards->mesh_list[0]->m_xmBoundingBox; // ������ �� �ѹ��� �ϸ� ��


	//-------------------------------------------------------

	CGameObject* board_line = NULL;
	CPlaneMeshIlluminated* pboard_line_mesh = new CPlaneMeshIlluminated(pd3dDevice, pd3dCommandList, Board_Half_Width / 100.0f, Board_Half_Depth, 1);
	
	std::vector<XMFLOAT3> horizontal_line_pos;
	horizontal_line_pos.push_back(XMFLOAT3(-50.0f, 0.3f, 0.0f));
	horizontal_line_pos.push_back(XMFLOAT3(0.0f, 0.3f, 0.0f));
	horizontal_line_pos.push_back(XMFLOAT3(50.0f, 0.3f, 0.0f));

	for (const XMFLOAT3& pos : horizontal_line_pos)
	{
		board_line = new CGameObject(pd3dDevice, pd3dCommandList);
		board_line->SetPosition(XMFLOAT3(pos));
		board_line->SetScale(1.0f, 1.0f, 2.0f);
		board_line->AddMesh(pboard_line_mesh);
		board_line->o_type = Object_Type::Board;
		board_line->SetMaterial(material_color_black_particle);

		m_pBoards->Add_Child(board_line);
	}

	std::vector<XMFLOAT3> vertical_line_pos;
	XMFLOAT3 vertical_angle = XMFLOAT3{ 0.0f, 1.0f, 0.0f };
	vertical_line_pos.push_back(XMFLOAT3(0.0f, 0.3f, -100.0f));
	vertical_line_pos.push_back(XMFLOAT3(0.0f, 0.3f, 0.0f));
	vertical_line_pos.push_back(XMFLOAT3(0.0f, 0.3f, 100.0f));

	for (const XMFLOAT3& pos : vertical_line_pos)
	{
		board_line = new CGameObject(pd3dDevice, pd3dCommandList);
		board_line->SetPosition(XMFLOAT3(pos));
		board_line->SetScale(2.0f/3.0f,1.0f,1.0f);
		board_line->Rotate(&vertical_angle, 90.0f);
		board_line->AddMesh(pboard_line_mesh);
		board_line->o_type = Object_Type::Board;
		board_line->SetMaterial(material_color_black_particle);

		m_pBoards->Add_Child(board_line);
	}

}

void Playing_Scene::Setting_Stone(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CMesh* mesh, XMFLOAT3 pos, bool player_team)
{
	StoneObject* pStoneObject = NULL;

	pStoneObject = new StoneObject(pd3dDevice, pd3dCommandList);
	pStoneObject->AddMesh(mesh);
	pStoneObject->default_collider = mesh->m_xmBoundingBox;
	pStoneObject->SetPosition(pos.x, pos.y, pos.z);
	pStoneObject->SetFriction(2);										// Default

	pStoneObject->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));			// Default
	pStoneObject->SetRotationSpeed(0.0f);								// Default

	pStoneObject->SetMaterial(material_color_none);
	if (player_team)
	{
		pStoneObject->AddMaterial(material_color_white_stone, true);
		pStoneObject->AddMaterial(material_color_player_selected, false);
		pStoneObject->SetMovingDirection(XMFLOAT3(0.0f, 0.0f, -1.0f));	// Default
	}
	else
	{
		pStoneObject->AddMaterial(material_color_black_stone, true);
		pStoneObject->AddMaterial(material_color_com_selected, false);
		pStoneObject->SetMovingDirection(XMFLOAT3(0.0f, 0.0f, 1.0f));	// Default
	}

	pStoneObject->SetMovingSpeed(0.0f);									// Default

	pStoneObject->player_team = player_team;

	if (player_team)
		player1.stone_list.push_back(pStoneObject);
	else
		computer.stone_list.push_back(pStoneObject);

	GameObject_Stone.push_back(pStoneObject);
}

void Playing_Scene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	//===========================================================
	// ���� ��ü
	Object_Shader = new CObjectsShader[N_Object_Shader];
	Object_Shader[0].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	
	Texture_Shader = new CTexturedShader[N_Texture_Shader];
	Texture_Shader[0].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);


	CTexture* ppTextures[TEXTURES];
	ppTextures[0] = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	ppTextures[0]->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Stone01.dds", RESOURCE_TEXTURE2D, 0);
	ppTextures[1] = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	ppTextures[1]->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Stone01.dds", RESOURCE_TEXTURE2D, 0);
	ppTextures[2] = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	ppTextures[2]->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Metal01.dds", RESOURCE_TEXTURE2D, 0);
	ppTextures[3] = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	ppTextures[3]->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Metal02.dds", RESOURCE_TEXTURE2D, 0);
	ppTextures[4] = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	ppTextures[4]->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Rock01.dds", RESOURCE_TEXTURE2D, 0);
	ppTextures[5] = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	ppTextures[5]->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Lava(Emissive).dds", RESOURCE_TEXTURE2D, 0);
	
	for (int i = 0; i < TEXTURES; i++)
		CreateShaderResourceViews(pd3dDevice, ppTextures[i], 0, 6);

	CMaterial* ppMaterials[TEXTURES];
	for (int i = 0; i < TEXTURES; i++)
	{
		ppMaterials[i] = new CMaterial();
		ppMaterials[i]->SetTexture(ppTextures[i]);

		ppMaterials[i]->SetShader(&Texture_Shader[0]);
	}



	// ������
	Outline_Shader = new OutlineShader[N_Outline_Shader];
	Outline_Shader[0].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	((OutlineShader*)&Outline_Shader[0])->Create_Outline_Buffer(pd3dDevice, pd3dCommandList);
	//===========================================================
	material_color_none->SetShader(&Outline_Shader[0]);

	Create_Board(pd3dDevice, pd3dCommandList, 200, 600);
	m_pBoards->AddMaterial(ppMaterials[0], true);


	XMFLOAT3 xmf3Scale(16.0f, 4.0f, 16.0f);
	XMFLOAT4 xmf4Color(0.0f, 0.2f, 0.0f, 0.0f);
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("HeightMap.raw"), 257, 257, 17, 17, xmf3Scale, xmf4Color);
	m_pTerrain->SetMaterial(material_color_none);
	m_pTerrain->AddMaterial(material_color_mountain, true);
	m_pTerrain->SetPosition(XMFLOAT3(-257.0f * xmf3Scale.x / 2, -500.0f, -257.0f * xmf3Scale.z / 2));
	//===========================================================

	CSphereMeshIlluminated* StoneMesh = new CSphereMeshIlluminated(pd3dDevice, pd3dCommandList, 8.0f, 20, 20, 0.5f);
	std::vector<XMFLOAT3> w_stone_pos_list;
	
	w_stone_pos_list.push_back({ -50, 5, 100 });
	w_stone_pos_list.push_back({ 0, 5, 100 });
	w_stone_pos_list.push_back({ 50, 5, 100 });
	w_stone_pos_list.push_back({ -25, 5, 150 });
	w_stone_pos_list.push_back({ 0, 5, 150 });
	w_stone_pos_list.push_back({ 25, 5, 150 });

	for(XMFLOAT3& w_stone_pos : w_stone_pos_list)
		Setting_Stone(pd3dDevice, pd3dCommandList, StoneMesh, w_stone_pos, true);

	//-------------------------------------------------------------------

	std::vector<XMFLOAT3> b_stone_pos_list;

	b_stone_pos_list.push_back({ -50, 5, -100 });
	b_stone_pos_list.push_back({ 0, 5, -100 });
	b_stone_pos_list.push_back({50, 5, -100});
	b_stone_pos_list.push_back({ -25, 5, -150 });
	b_stone_pos_list.push_back({ 0, 5, -150 });
	b_stone_pos_list.push_back({ 25, 5, -150 });

	for (XMFLOAT3& b_stone_pos : b_stone_pos_list)
		Setting_Stone(pd3dDevice, pd3dCommandList, StoneMesh, b_stone_pos, false);

	//===========================================================


}

void Playing_Scene::BuildUIs(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// UI ��ü // UIShader 
	UI_Shader = new UIShader[N_UI_Shader];
	UI_Shader[0].CreateShader(pd3dDevice, UI_GraphicsRootSignature);

	//=======================================================================
	// UI Area
	D3D12_RECT power_ui_area_1 = { 600, 0, 800, 90 };

	D3D12_RECT power_ui_area_2 = { 0, 0, 200, 90 };
	
	//=======================================================================

	// UI Mesh
	CMesh* ui_power_mesh = new UIMesh(pd3dDevice, pd3dCommandList, 200.0f, 90.0f, 1.0f, XMFLOAT4(1.0f, 0.5f, 0.0f, 1.0f));
	
	CMesh* ui_endline_mesh = new UIMesh(pd3dDevice, pd3dCommandList, 5.0f, 90.0f, 1.0f, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), false);

	//=======================================================================

	// UI's Object
	UI_Object* ui_power_bar = new BAR_UI_Object(pd3dDevice, pd3dCommandList, 4);
	ui_power_bar->AddMesh(ui_power_mesh);
	ui_power_bar->active = true;

	UI_Object* ui_endline =  new UI_Object(pd3dDevice, pd3dCommandList);
	ui_endline->AddMesh(ui_endline_mesh);
	ui_endline->SetPosition(-100.0f, 0.0f, 0.0f);
	ui_endline->active = true;

	//-----------------------------------

	UI_Object* ui_power_bar_com = new BAR_UI_Object(pd3dDevice, pd3dCommandList, 3);
	ui_power_bar_com->AddMesh(ui_power_mesh);
	ui_power_bar_com->active = true;

	UI_Object* ui_endline_com = new UI_Object(pd3dDevice, pd3dCommandList);
	ui_endline_com->AddMesh(ui_endline_mesh);
	ui_endline_com->SetPosition(100.0f, 0.0f, 0.0f);
	ui_endline_com->active = true;

	//=======================================================================

	// UI Manager
	UI* Power_UI_player = new BAR_UI(pd3dDevice, pd3dCommandList, power_ui_area_1);
	ui_player_power = Power_UI_player;
	
	((BAR_UI*)Power_UI_player)->Set_Bar_obj((BAR_UI_Object*)ui_power_bar);
	Power_UI_player->ui_object.push_back(ui_power_bar);
	Power_UI_player->ui_object.push_back(ui_endline);
	Power_UI_player->Active = true;
	UI_list.push_back(Power_UI_player);

	//-----------------------------------
	
	UI* Power_UI_com = new BAR_UI(pd3dDevice, pd3dCommandList, power_ui_area_2);
	ui_com_power = Power_UI_com;

	((BAR_UI*)Power_UI_com)->Set_Bar_obj((BAR_UI_Object*)ui_power_bar_com);
	Power_UI_com->ui_object.push_back(ui_power_bar_com);
	Power_UI_com->ui_object.push_back(ui_endline_com);
	Power_UI_com->Active = true;
	UI_list.push_back(Power_UI_com);

	//=======================================================================

	D3D12_RECT inventory_ui_area = { 0, 500, 800, 600 };

	Inventory_UI* Inventory_ui = Create_Inventory_UI(pd3dDevice, pd3dCommandList, inventory_ui_area); 
	player_inventory = Inventory_ui;

	UI_list.push_back(Inventory_ui);

	ui_num = UI_list.size();

}

Inventory_UI* Playing_Scene::Create_Inventory_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, D3D12_RECT area)
{
	float inventory_width = area.right - area.left;
	float inventory_height = area.bottom - area.top;
	float start_pos_x = -(inventory_width / 2);

	if (inventory_width < 100.0f)
		inventory_width = 100;


	float icon_Box_size = inventory_width / (8 * 2); // ������ 8��, ������ �� ����
	float icon_distance = icon_Box_size; // ������ �����ܺ� ����


	// �κ��丮�� ������ ������ ũ�� ����
	float min_width = 100;
	float max_width = 800;
	float min_scale = 0.5f; // 100�� ���� scale ��
	float max_scale = 3.0f; // 800�� ���� scale ��

	// ���� ���� ���
	float icon_scale = (inventory_width - min_width) / (max_width - min_width) * (max_scale - min_scale) + min_scale;

	//------------------------------------------------------------------------------

	CMesh* ui_inventory_mesh = new UIMesh(pd3dDevice, pd3dCommandList, inventory_width, inventory_height, 1.0f, XMFLOAT4(1.0f, 0.3f, 0.8f, 1.0f), false); // 1.0f, 1.0f, 1.0f

	//------------------------------------------------------------------------------
	Inventory_UI* Inventory_ui = new Inventory_UI(pd3dDevice, pd3dCommandList, area);
	
	UI_Object* ui_inventory_obj = new UI_Object(pd3dDevice, pd3dCommandList);
	ui_inventory_obj->AddMesh(ui_inventory_mesh);
	ui_inventory_obj->SetPosition(0.0f, 0.0f, 30.0f);
	ui_inventory_obj->active = true;
	
	((Inventory_UI*)Inventory_ui)->Set_Inventory_board_obj(ui_inventory_obj);
	Inventory_ui->ui_object.push_back(ui_inventory_obj);

	//------------------------------------------------------------------------------

	std::vector <Item_Type> item_type_list
	{ 
		Item_Type::Taunt,
		Item_Type::Fire_Shot,
		Item_Type::Double_Power, 
		Item_Type::ETC,
		Item_Type::ETC,
		Item_Type::Frozen_Time,
		Item_Type::Ghost, 
		Item_Type::Max_Power
	};

	std::vector<std::pair<Item_Type, XMFLOAT3>> icon_info;

	for (int i = 0; i < item_type_list.size(); ++i)
		icon_info.push_back(std::make_pair(item_type_list[i], XMFLOAT3(start_pos_x + icon_Box_size * (i + 1) + (icon_distance * i), 10.0f, 0.0f)));




	for (std::pair < Item_Type, XMFLOAT3> info : icon_info)
	{
		Item* item = new Item(pd3dDevice, pd3dCommandList, info.first);

		item->SetActive(true);
		item->outer_frame->Set_MaterialShader(&Object_Shader[0], 0);
		item->outer_frame->SetMaterial(material_color_none,true);
		item->outer_frame->m_ppMaterials[1].second = true;

		item->inner_frame->Set_MaterialShader(&Object_Shader[0], 0);
		item->inner_frame->SetMaterial(material_color_none, true);
		item->inner_frame->m_ppMaterials[1].second = true;

		item->SetScale(icon_scale, icon_scale, icon_scale);
		item->SetPosition(info.second);
		ui_inventory_obj->Add_Child(item);

		Inventory_ui->item_list.push_back(std::make_pair(info.first, 0));
	}

	for (std::pair<Item_Type, XMFLOAT3> info : icon_info)
	{
		D2D1_RECT_F temp{ inventory_width / 2 + info.second.x - icon_Box_size, 550, inventory_width / 2 + info.second.x + icon_Box_size, 600 };
		Inventory_ui->text_area.push_back(temp);
	}

	Inventory_ui->Active = true;

	return Inventory_ui;
}
void Playing_Scene::ReleaseObjects()
{
	Release_Shader_Resource();

	if (m_pLights)
		delete m_pLights;

	//if (m_pMaterials)
	//	delete m_pMaterials;


	if (m_pd3dGraphicsRootSignature) 
		m_pd3dGraphicsRootSignature->Release();

	if (GameObject_Stone.size())
		GameObject_Stone.clear();

	if (Object_Shader)
		delete[] Object_Shader;

	/*for (int j = 0; j < m_n_uiShaders; ++j)
	{
		m_uiShaders[j].ReleaseShaderVariables();
		m_uiShaders[j].ReleaseObjects();
	}*/



	//if (m_uiShaders)
	//	delete[] m_uiShaders;
}

void Playing_Scene::ReleaseUploadBuffers()
{
	for (int j = 0; j < N_Object_Shader; ++j)
		Object_Shader[j].ReleaseUploadBuffers();
}


ID3D12RootSignature* Playing_Scene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;

	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[1];

	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[0].NumDescriptors = 1;
	pd3dDescriptorRanges[0].BaseShaderRegister = 0; //t0: gtxtTexture
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER pd3dRootParameters[7];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 0; //Player
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[1].Descriptor.ShaderRegister = 1; //Camera
	pd3dRootParameters[1].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	
	//------------------------------------

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 2;
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0; //GameObject
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[3].Descriptor.ShaderRegister = 3;
	pd3dRootParameters[3].Descriptor.RegisterSpace = 0; //Material
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	//------------------------------------

	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[4].Descriptor.ShaderRegister = 4; //Lights
	pd3dRootParameters[4].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[5].Descriptor.ShaderRegister = 5; // Outline
	pd3dRootParameters[5].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[5].Constants.Num32BitValues = 8;
	pd3dRootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	//------------------------------------

	pd3dRootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[6].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[6].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[0];
	pd3dRootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	//================================================
	// ���÷�
	D3D12_STATIC_SAMPLER_DESC d3dSamplerDesc;
	::ZeroMemory(&d3dSamplerDesc, sizeof(D3D12_STATIC_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc.MipLODBias = 0;
	d3dSamplerDesc.MaxAnisotropy = 1;
	d3dSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	d3dSamplerDesc.ShaderRegister = 0;
	d3dSamplerDesc.RegisterSpace = 0;
	d3dSamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;


	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
	
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 1;
	d3dRootSignatureDesc.pStaticSamplers = &d3dSamplerDesc;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;
	
	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;
	
	::D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(),
		__uuidof(ID3D12RootSignature), (void**)&pd3dGraphicsRootSignature);
	
	if (pd3dSignatureBlob) 
		pd3dSignatureBlob->Release();
	
	if (pd3dErrorBlob) 
		pd3dErrorBlob->Release();
	
	return(pd3dGraphicsRootSignature);
}

ID3D12RootSignature* Playing_Scene::Create_UI_GraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;
	D3D12_ROOT_PARAMETER pd3dRootParameters[3];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 0; // GameObject_pos
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[1].Descriptor.ShaderRegister = 1; //Camera
	pd3dRootParameters[1].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 2; // UI_INFO
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = NULL;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;

	::D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);

	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(),
		__uuidof(ID3D12RootSignature), (void**)&pd3dGraphicsRootSignature);

	if (pd3dSignatureBlob)
		pd3dSignatureBlob->Release();

	if (pd3dErrorBlob)
		pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}


void Playing_Scene::CreateGraphicsPipelineState(ID3D12Device* pd3dDevice)
{
	//���� ���̴��� �ȼ� ���̴��� �����Ѵ�.
	ID3DBlob* pd3dVertexShaderBlob = NULL;
	ID3DBlob* pd3dPixelShaderBlob = NULL;
	UINT nCompileFlags = 0;
#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	D3DCompileFromFile(L"Shaders.hlsl", NULL, NULL, "VSMain", "vs_5_1", nCompileFlags, 0, &pd3dVertexShaderBlob, NULL);
	D3DCompileFromFile(L"Shaders.hlsl", NULL, NULL, "PSMain", "ps_5_1", nCompileFlags, 0, &pd3dPixelShaderBlob, NULL);
	//�����Ͷ����� ���¸� �����Ѵ�.
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	//���� ���¸� �����Ѵ�.
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//�׷��� ���������� ���¸� �����Ѵ�.
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineStateDesc.pRootSignature = m_pd3dGraphicsRootSignature;
	d3dPipelineStateDesc.VS.pShaderBytecode = pd3dVertexShaderBlob->GetBufferPointer();
	d3dPipelineStateDesc.VS.BytecodeLength = pd3dVertexShaderBlob->GetBufferSize();
	d3dPipelineStateDesc.PS.pShaderBytecode = pd3dPixelShaderBlob->GetBufferPointer();
	d3dPipelineStateDesc.PS.BytecodeLength = pd3dPixelShaderBlob->GetBufferSize();
	d3dPipelineStateDesc.RasterizerState = d3dRasterizerDesc;
	d3dPipelineStateDesc.BlendState = d3dBlendDesc;
	d3dPipelineStateDesc.DepthStencilState.DepthEnable = FALSE;
	d3dPipelineStateDesc.DepthStencilState.StencilEnable = FALSE;
	d3dPipelineStateDesc.InputLayout.pInputElementDescs = NULL;
	d3dPipelineStateDesc.InputLayout.NumElements = 0;
	d3dPipelineStateDesc.SampleMask = UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineStateDesc.NumRenderTargets = 1;
	d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineStateDesc.SampleDesc.Count = 1;
	d3dPipelineStateDesc.SampleDesc.Quality = 0;
	pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc,
		__uuidof(ID3D12PipelineState), (void**)&m_pd3dPipelineState);
	if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
	if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();
}



void Playing_Scene::CreateCbvSrvDescriptorHeaps(ID3D12Device* pd3dDevice, int nConstantBufferViews, int nShaderResourceViews)
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	d3dDescriptorHeapDesc.NumDescriptors = nConstantBufferViews + nShaderResourceViews; //CBVs + SRVs 
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dCbvSrvDescriptorHeap);

	m_d3dCbvCPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_d3dCbvGPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	m_d3dSrvCPUDescriptorStartHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize *  nConstantBufferViews);
	m_d3dSrvGPUDescriptorStartHandle.ptr = m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);

	m_d3dCbvCPUDescriptorNextHandle = m_d3dCbvCPUDescriptorStartHandle;
	m_d3dCbvGPUDescriptorNextHandle = m_d3dCbvGPUDescriptorStartHandle;
	m_d3dSrvCPUDescriptorNextHandle = m_d3dSrvCPUDescriptorStartHandle;
	m_d3dSrvGPUDescriptorNextHandle = m_d3dSrvGPUDescriptorStartHandle;
}

void Playing_Scene::CreateConstantBufferViews(ID3D12Device* pd3dDevice, int nConstantBufferViews, ID3D12Resource* pd3dConstantBuffers, UINT nStride)
{
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = pd3dConstantBuffers->GetGPUVirtualAddress();
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;

	d3dCBVDesc.SizeInBytes = nStride;

	for (int j = 0; j < nConstantBufferViews; j++)
	{
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (nStride * j);
		D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
		d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorNextHandle.ptr + ::gnCbvSrvDescriptorIncrementSize;
		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, m_d3dCbvCPUDescriptorNextHandle);
	}
}

D3D12_GPU_DESCRIPTOR_HANDLE Playing_Scene::CreateConstantBufferView(ID3D12Device* pd3dDevice, ID3D12Resource* pd3dConstantBuffer, UINT nStride)
{
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	d3dCBVDesc.SizeInBytes = nStride;
	d3dCBVDesc.BufferLocation = pd3dConstantBuffer->GetGPUVirtualAddress();

	pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, m_d3dCbvCPUDescriptorNextHandle);

	D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle = m_d3dCbvGPUDescriptorNextHandle;
	m_d3dCbvCPUDescriptorNextHandle.ptr = m_d3dCbvCPUDescriptorNextHandle.ptr + ::gnCbvSrvDescriptorIncrementSize;
	m_d3dCbvGPUDescriptorNextHandle.ptr = m_d3dCbvGPUDescriptorNextHandle.ptr + ::gnCbvSrvDescriptorIncrementSize;

	return(d3dCbvGPUDescriptorHandle);
}

D3D12_GPU_DESCRIPTOR_HANDLE Playing_Scene::CreateConstantBufferView(ID3D12Device* pd3dDevice, D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress, UINT nStride)
{
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	d3dCBVDesc.SizeInBytes = nStride;
	d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress;

	pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, m_d3dCbvCPUDescriptorNextHandle);

	D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle = m_d3dCbvGPUDescriptorNextHandle;
	m_d3dCbvCPUDescriptorNextHandle.ptr = m_d3dCbvCPUDescriptorNextHandle.ptr + ::gnCbvSrvDescriptorIncrementSize;
	m_d3dCbvGPUDescriptorNextHandle.ptr = m_d3dCbvGPUDescriptorNextHandle.ptr + ::gnCbvSrvDescriptorIncrementSize;

	return(d3dCbvGPUDescriptorHandle);
}

void Playing_Scene::CreateShaderResourceViews(ID3D12Device* pd3dDevice, CTexture* pTexture, UINT nDescriptorHeapIndex, UINT nRootParameterStartIndex)
{
	m_d3dSrvCPUDescriptorNextHandle.ptr += (::gnCbvSrvDescriptorIncrementSize * nDescriptorHeapIndex);
	m_d3dSrvGPUDescriptorNextHandle.ptr += (::gnCbvSrvDescriptorIncrementSize * nDescriptorHeapIndex);

	int nTextures = pTexture->GetTextures();
	for (int i = 0; i < nTextures; i++)
	{
		ID3D12Resource* pShaderResource = pTexture->GetResource(i);
		D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = pTexture->GetShaderResourceViewDesc(i);

		pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, m_d3dSrvCPUDescriptorNextHandle);
		m_d3dSrvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

		pTexture->SetGpuDescriptorHandle(i, m_d3dSrvGPUDescriptorNextHandle);
		m_d3dSrvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	}

	int nRootParameters = pTexture->GetRootParameters();

	for (int i = 0; i < nRootParameters; i++) 
		pTexture->SetRootParameterIndex(i, nRootParameterStartIndex + i);
}


void Playing_Scene::Remove_Unnecessary_Objects()
{
	// �ڽ� ��ü�� ������� �ϴ� ���� �߻��ϴ� ��� ���� �߻� ��
	//auto unactive_stone_range = std::remove_if(GameObject_Stone.begin(), GameObject_Stone.end(), [](CGameObject* stone) {
	//	if (!stone->active)
	//	{
	//		delete stone;
	//		return true;
	//	}
	//	return false; });
	//GameObject_Stone.erase(unactive_stone_range, GameObject_Stone.end());
}


// �浹 �� ã��
std::vector<std::pair<int, int>> Playing_Scene::FindCollisionPairs(const std::vector<StoneObject*>& stones_list)
{
	std::vector<std::pair<int, int>> collision_Pairs;
	int size = stones_list.size();

	for (int i = 0; i < size; ++i)
	{
		StoneObject* stone1 = stones_list[i];
		if (!stone1->active || stone1->used_item == Item_Type::Ghost)
			continue;

		for (int j = i + 1; j < size; ++j)
		{
			StoneObject* stone2 = stones_list[j];
			if (!stone2->active || stone2->used_item == Item_Type::Ghost)
				continue;

			if (stone1->Get_Collider().Intersects(stone2->Get_Collider()))
				collision_Pairs.push_back(std::make_pair(i, j));

		}
	}

	return collision_Pairs;
}

// �ӵ� ������Ʈ
void Playing_Scene::UpdateVelocities(StoneObject* stone1, StoneObject* stone2, XMVECTOR vel1, XMVECTOR vel2)
{
	XMFLOAT3 pos1 = stone1->GetPosition();
	XMFLOAT3 pos2 = stone2->GetPosition();

	XMVECTOR posVec1 = XMLoadFloat3(&pos1);
	XMVECTOR posVec2 = XMLoadFloat3(&pos2);

	XMVECTOR diffVec = XMVector3Normalize(posVec2 - posVec1);

	//-------------------------------------------------------------

	float overlapDistance = (stone1->Get_Collider().Radius + stone2->Get_Collider().Radius) - XMVectorGetX(XMVector3Length(posVec2 - posVec1));

	if (overlapDistance > 0)
	{
		XMVECTOR separationVec = diffVec * (overlapDistance / 2.0f);
		posVec1 -= separationVec;
		posVec2 += separationVec;

		XMStoreFloat3(&pos1, posVec1);
		XMStoreFloat3(&pos2, posVec2);

		stone1->SetPosition(pos1);
		stone2->SetPosition(pos2);
	}

	//-------------------------------------------------------------

	XMVECTOR velDiffVec = vel2 - vel1;
	float power = XMVectorGetX(XMVector3Dot(velDiffVec, diffVec));
	XMVECTOR bumpVec = power * diffVec;


	auto update_velocity = [](CGameObject* stone, XMVECTOR newVel)
		{
			XMFLOAT3 finalVel_f;
			XMStoreFloat3(&finalVel_f, XMVector3Normalize(newVel));
			XMStoreFloat3(&stone->m_xmf3MovingDirection, newVel);
			stone->m_fMovingSpeed = sqrtf(finalVel_f.x * finalVel_f.x + finalVel_f.y * finalVel_f.y + finalVel_f.z * finalVel_f.z);
		};

	update_velocity(stone1, vel1 + bumpVec);
	update_velocity(stone2, vel2 - bumpVec);
}

void Playing_Scene::Check_Stones_Collisions()
{
	std::vector<std::pair<int, int>> collision_Pairs = FindCollisionPairs(GameObject_Stone);

	// �浹 �� ���� ���� �ݺ����� ����Ͽ� ó��
	for (const std::pair<int, int>& pair : collision_Pairs)
	{
		StoneObject* stone1 = GameObject_Stone[pair.first];
		StoneObject* stone2 = GameObject_Stone[pair.second];

		XMVECTOR vel1 = XMLoadFloat3(&stone1->m_xmf3MovingDirection) * stone1->m_fMovingSpeed;
		XMVECTOR vel2 = XMLoadFloat3(&stone2->m_xmf3MovingDirection) * stone2->m_fMovingSpeed;

		UpdateVelocities(stone1, stone2, vel1, vel2);
	}
}

void Playing_Scene::Check_Board_and_Stone_Collisions(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// �浹 ��ü �ʱ�ȭ
	for (CGameObject* stone_ptr : GameObject_Stone)
	{
		if (!stone_ptr->active)
			continue;

		XMFLOAT3 stone_pos = stone_ptr->GetPosition();
		XMVECTOR pos_xz = XMVectorSet(stone_pos.x, 0.0f, stone_pos.z, 0.0f);
		ContainmentType containType = m_pBoards->Get_Collider().Contains(pos_xz);

		switch (containType)
		{
		case DISJOINT:
		{
			stone_ptr->SetMovingSpeed(0.0f);
			stone_ptr->GetPosition();

			if (stone_ptr->active == true)
			{
				if (stone_ptr->player_team)
					Setting_Particle(pd3dDevice, pd3dCommandList, stone_ptr->GetPosition(), material_color_white_stone, Particle_Type::Explosion);
				else if (!stone_ptr->player_team)
					Setting_Particle(pd3dDevice, pd3dCommandList, stone_ptr->GetPosition(), material_color_black_stone, Particle_Type::Explosion);
			}
			stone_ptr->active = false;
		}
		break;

		case INTERSECTS:
		case CONTAINS:
			break;
		}
	}
}


void Playing_Scene::Check_Item_and_Stone_Collisions(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (Item* item : Game_Items)
	{
		if (!item->active)
			continue;

		for (CGameObject* stone_ptr : player1.stone_list)
		{
			if (!stone_ptr->active)
				continue;

			if (stone_ptr->Get_Collider().Intersects(item->Get_Collider()))
			{
				item->SetActive(false);
				Setting_Particle(pd3dDevice, pd3dCommandList, item->GetPosition(), material_color_black_particle, Particle_Type::Firework);
				player1.Item_Inventory[item->item_type] += 1;
			}
		}
	}
}


bool Playing_Scene::Update_Item_Manager(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// ��ǻ�� �Ͽ��� ghost�� �˻��ϱ�
	if (Com_Turn)
	{
		if (item_manager->Get_Active_Stone_Num(Item_Type::Ghost))
		{
			std::vector<Stone_Item_Info*>* ghost_stones = item_manager->Get_Stone_Iist(Item_Type::Ghost);
			std::transform(ghost_stones->begin(), ghost_stones->end(), ghost_stones->begin(), [](Stone_Item_Info* ghost_info) {
				if (ghost_info->stone != NULL && ghost_info->stone->active)
					ghost_info->turn += 1;
				return ghost_info; });
		}
		return true;
	}



	bool Change_Turn = true;

	// Double Shot
	if (item_manager->Get_Stone(Item_Type::Double_Power))
	{
		Player_Shot = false;
		if (item_manager->Get_Stone(Item_Type::Double_Power)->turn == 0)
		{
			item_manager->Get_Stone(Item_Type::Double_Power)->turn += 1;
			m_pPlayer->SetPosition(player1.select_Stone->GetPosition());

			// �ι�° �����̹Ƿ�, �ٽ� ī�޶� ����
			m_pPlayer->SetCamera(m_pPlayer->ChangeCamera(THIRD_PERSON_CAMERA, 0.005f));
			player1.selected_Item_Type = Item_Type::None;
			Mark_selected_stone();
			Change_Turn = false;
		}
		else
		{
			item_manager->Set_Clear(Item_Type::Double_Power);
		}
	}

	// Frozen Time
	if (item_manager->Get_Active_Stone_Num(Item_Type::Frozen_Time))
	{
		std::vector<Stone_Item_Info*>* frozen_stones = item_manager->Get_Stone_Iist(Item_Type::Frozen_Time);
		for (Stone_Item_Info* stone_info : *frozen_stones)
		{
			if (stone_info->turn < 3)
			{
				if (stone_info->turn == 0)
				{
					XMFLOAT3 pos = stone_info->stone->GetPosition();
					pos.y = 30.0f;
					stone_info->particle = new Snow_Particle(pd3dDevice, pd3dCommandList, pos, Snow_Area_Radius, material_color_white_stone, Particle_Type::Snow);
					stone_info->particle->SetActive(true);
					((Snow_Particle*)stone_info->particle)->Set_Center(pos);
				}
				stone_info->turn += 1;
			}
		}
	}

	//Ghost
	if (item_manager->Get_Active_Stone_Num(Item_Type::Ghost))
	{
		std::vector<Stone_Item_Info*>* ghost_stones = item_manager->Get_Stone_Iist(Item_Type::Ghost);
		std::transform(ghost_stones->begin(), ghost_stones->end(), ghost_stones->begin(), [](Stone_Item_Info* ghost_info) {
			if (ghost_info->stone != NULL && ghost_info->stone->active)
				ghost_info->turn += 1;
			return ghost_info; });
	}

	return Change_Turn;
}

bool Playing_Scene::Change_Turn()
{
	if (Player_Turn)
	{
		Player_Turn = false;
		Com_Turn = true;
		Com_Shot = false;
		Set_BackGround_Color(XMFLOAT4(0.2f, 0.2f, 0.2f, 0.0f));

		if (player1.select_Stone != NULL && player1.select_Stone->active)
			player1.select_Stone->ChangeMaterial(1);

		player1.select_Stone = NULL;
		player1.selected_Item_Type = Item_Type::None;
		Charge_Effect->ChangeMaterial(2);
	}
	else if (Com_Turn)
	{
		Player_Turn = true;
		Player_Shot = false;
		Com_Turn = false;
		Set_BackGround_Color(XMFLOAT4(0.8f, 0.8f, 0.8f, 0.0f));

		if (computer.select_Stone->active)
		{
			computer.select_Stone->ChangeMaterial(1);
			computer.select_Stone = NULL;
			computer.target_Stone = NULL;
		
		}
		Charge_Effect->ChangeMaterial(1);
	}
	
	Mark_selected_stone();
	Update_Item_Inventory();

	power_charge = false;
	((BAR_UI*)ui_player_power)->Set_Bar_Charge_Mode(false);
	Charge_Effect->Reset();
	ui_player_power->Reset();
	ui_com_power->Reset();

	m_pPlayer->SetCamera(m_pPlayer->ChangeCamera(TOP_VIEW_CAMERA, 5.0f));
	return true;
}

bool Playing_Scene::Check_Turn()
{
	if (!Player_Shot && Player_Turn)
		return false;

	if (!Com_Shot && Com_Turn)
		return false;

	int stop_stone_n = 0;

	for (const CGameObject* obj_ptr : GameObject_Stone)
	{
		const StoneObject* stone_ptr = static_cast<const StoneObject*>(obj_ptr);
		if (0.1f >= stone_ptr->m_fMovingSpeed || !stone_ptr->active)
			stop_stone_n += 1;
		else
			stone_ptr = stone_ptr;
	}

	if (stop_stone_n == GameObject_Stone.size())
		return true;
	else
		return false;
	
}

bool Playing_Scene::Check_GameOver()
{
	int dead_White_Stone = 0;
	int dead_Black_Stone = 0;
	for (CGameObject* obj_ptr : GameObject_Stone)
	{
		if (obj_ptr->active == false)
		{
			if (obj_ptr->player_team)
				dead_White_Stone += 1;
			else
				dead_Black_Stone += 1;
		}
	}

	if (dead_White_Stone == 6 || dead_Black_Stone == 6)
		return true;
	else
		return false;
}

void Playing_Scene::Setting_Item(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT3 pos, Item_Type type)
{
	Item* item = NULL;

	switch (type)
	{
	case Item_Type::Double_Power:
		item = new Item(pd3dDevice, pd3dCommandList, Item_Type::Double_Power);
	break;

	case Item_Type::Ghost:
		item = new Item(pd3dDevice, pd3dCommandList, Item_Type::Ghost);
	break;

	case Item_Type::Taunt:
		item = new Item(pd3dDevice, pd3dCommandList, Item_Type::Taunt);
		break;

	case Item_Type::Fire_Shot:
		item = new Item(pd3dDevice, pd3dCommandList, Item_Type::Fire_Shot);
		break;

	case Item_Type::Frozen_Time:
		item = new Item(pd3dDevice, pd3dCommandList, Item_Type::Frozen_Time);
		break;

	case Item_Type::Max_Power:
		item = new Item(pd3dDevice, pd3dCommandList, Item_Type::Max_Power);
		break;

	case Item_Type::ETC:
	case Item_Type::None:
	default:
		break;
	}

	if (item != NULL)
	{
		item->SetActive(true);
		item->SetPosition(pos);
		item->outer_frame->Set_MaterialShader(&Object_Shader[0], 0);
		item->outer_frame->SetMaterial(material_color_none, true);
		item->outer_frame->m_ppMaterials[1].second = true;

		item->inner_frame->Set_MaterialShader(&Object_Shader[0], 0);
		item->inner_frame->SetMaterial(material_color_none, true);
		item->inner_frame->m_ppMaterials[1].second = true;

		Game_Items.push_back(item);
	}
}

void Playing_Scene::Setting_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT3 pos, CMaterial* material, Particle_Type particle_type)
{
	bool Done = false;
	for (Particle* particle : m_particle)
	{
		if (particle->p_type == particle_type && particle->active == false)
		{
			particle->SetActive(true);
			particle->SetPosition(pos);
			particle->SetMaterial(material);
			Done = true;
			break;
		}
	}

	if (Done == false)
	{
		Particle* particle = NULL;
		switch (particle_type)
		{

		case Particle_Type::Explosion:
		{
			particle = new Explosion_Particle(pd3dDevice, pd3dCommandList, material, Particle_Type::Explosion);
			particle->SetActive(true);
			particle->SetPosition(pos);
			m_particle.push_back(particle);
		}
		break;

		case Particle_Type::Charge:
		{
			particle = new Charge_Particle(pd3dDevice, pd3dCommandList, 50.0f, 5.0f, material, Particle_Type::Charge);
			particle->SetActive(true);
			particle->SetPosition(pos);
			((Charge_Particle*)particle)->Set_Center_Position(pos);
			m_particle.push_back(particle);
		}
		break;

		case Particle_Type::Firework:
		{
			particle = new Firework_Particle(pd3dDevice, pd3dCommandList, 3.0f, material, Particle_Type::Firework);
			particle->SetActive(true);
			particle->SetPosition(pos);

			particle->SetMaterial(material_color_item_inner_red);
			particle->AddMaterial(material_color_item_outer);
			particle->AddMaterial(material_color_item_inner_blue);
			particle->AddMaterial(material_color_item_inner_green);

			m_particle.push_back(particle);
			
		}
		break;

		case Particle_Type::Snow:
			particle = new Snow_Particle(pd3dDevice, pd3dCommandList, pos, Snow_Area_Radius, material_color_white_stone, Particle_Type::Snow);
			particle->SetActive(true);
			particle->SetPosition(pos);
			((Snow_Particle*)particle)->Set_Center(pos);
			m_particle.push_back(particle);
			break;

		case Particle_Type::None:
		default:
			break;
		}
	}

}

void Playing_Scene::AnimateObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed)
{
	for (CGameObject* stone_obj : GameObject_Stone)
		stone_obj->Animate(fTimeElapsed, NULL);


	for (Item* item : Game_Items)
		item->Animate(fTimeElapsed, NULL);

	for (Particle* particle : m_particle)
		particle->Animate(fTimeElapsed);

	for (UI* ui_ptr : UI_list)
		ui_ptr->AnimateObjects(fTimeElapsed);

	//if (m_pBoards)
	//	m_pBoards->Animate(fTimeElapsed, NULL);

	if (Charge_Effect)
		Charge_Effect->Animate(fTimeElapsed);



	if (m_pLights)
	{
		m_pLights->m_pLights[2].m_bEnable = true;
		m_pLights->m_pLights[2].m_xmf3Position.y = 30.0f;

		if (player1.select_Stone != NULL)
			m_pLights->m_pLights[2].m_xmf3Position = player1.select_Stone->GetPosition();
		else if (computer.select_Stone)
			m_pLights->m_pLights[2].m_xmf3Position = computer.select_Stone->GetPosition();
		else
			m_pLights->m_pLights[2].m_bEnable = false;
	}

	item_manager->Animate(fTimeElapsed);
	item_manager->Check_Stone_Item_Effect(&GameObject_Stone);

	Scene_Update(pd3dDevice, pd3dCommandList, fTimeElapsed);
}

void Playing_Scene::Scene_Update(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed)
{
	if (Player_Turn) // �÷��̾� ��
	{
		ui_player_power->Active = true;
		ui_com_power->Active = false;
		
		power_degree = ((BAR_UI*)ui_player_power)->Get_Degree();
		
		if (power_charge && player1.select_Stone != NULL)
		{
			Charge_Effect->active = true;
			Charge_Effect->Set_Center_Position(player1.select_Stone->GetPosition());
		}

	}
	else if (Com_Turn && Com_Shot == false) // ��ǻ�� ��
	{
		ui_player_power->Active = false;

		ui_com_power->Active = true;


		if (Game_Over == false) // ��ǻ�� ���ʶ��
		{
			if (computer.select_Stone == NULL) // ���� ���� ���ߴٸ�
			{
				//std::pair<StoneObject*, StoneObject*> hard_version = Find_Nearest_Enemy_Stone();

				std::pair<StoneObject*, StoneObject*> normal_version = Select_Stone_Com();
				if (normal_version.first == NULL || normal_version.second == NULL)
					return;

				computer.select_Stone = normal_version.first;
				computer.select_Stone->ChangeMaterial(2);

				computer.target_Stone = normal_version.second;

				// ��¡ �ð� ����
				power_charge = true;
				((BAR_UI*)ui_com_power)->Set_Bar_Charge_Mode(true);
				computer.random_time = 0.5f + (uid(dre) / 1200);
			}
			else
			{
				if (computer.random_time < computer.sum_time) // ������ �ð��� ��ٸ� ����
				{
					computer.random_time = -1;
					computer.sum_time = 0;

					Shoot_Stone_Com(power_degree);
					Com_Shot = true;

					power_charge = false;
					((BAR_UI*)ui_com_power)->Set_Bar_Charge_Mode(false);
				}
				else // �ð��� �ɶ����� ���� �ִϸ��̼� �� UI ������Ʈ
				{
					power_degree = ((BAR_UI*)ui_com_power)->Get_Degree();
					
					if (power_charge && computer.select_Stone != NULL)
					{
						Charge_Effect->active = true;
						Charge_Effect->Set_Center_Position(computer.select_Stone->GetPosition());
					}

					computer.sum_time += fTimeElapsed;
				}

			}
		}
	}

	//--------------- �� ����--------------
	
	// �� ���� üũ
	if (Limit_time > TURN_MAX_TIME)
		Need_to_change_turn = true;

	if (Need_to_change_turn)
	{
		if (Delay_time >= TURN_DELAY)
		{
			if (Update_Item_Manager(pd3dDevice, pd3dCommandList))
			{
				Limit_time = 0.0f;
				Delay_time = 0.0f;
				Need_to_change_turn = false;
				Change_Turn();
			}
			else
				Need_to_change_turn = false;
		}
		else
			Delay_time += fTimeElapsed;
	}
	else
	{
		Need_to_change_turn = Check_Turn();
		Limit_time += fTimeElapsed;
	}

	Check_Item_and_Stone_Collisions(pd3dDevice, pd3dCommandList);
	Check_Board_and_Stone_Collisions(pd3dDevice, pd3dCommandList);
	Check_Stones_Collisions();

	Remove_Unnecessary_Objects();


	if (m_pPlayer->GetCamera()->GetMode() == TOP_VIEW_CAMERA)
	{
		Update_Camera_Zoom(fTimeElapsed, 0.5f);
		Update_Player_pos_Oribit(fTimeElapsed, 0.5f);
	}


}

void Playing_Scene::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	// Ŀ��� ����Ʈ�� PSO ����, ��Ʈ �ñ״�ó�� PSO�� �̹� ���ε� �Ǿ� ���� == ���� Ŀ��� ����Ʈ���� ���ε� ���ص� ������, ������ ����
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	Object_Shader[0].Setting_PSO(pd3dCommandList, 1); 

	pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);


	// ī�޶� ���� �� ���� ������Ʈ
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->Update_Shader_Resource(pd3dCommandList);


	// ���� ������Ʈ
	Update_Shader_Resource(pd3dCommandList);
	
	m_pBoards->Render(pd3dCommandList, pCamera, &Object_Shader[0]);

	if (m_pTerrain)
		m_pTerrain->Render(pd3dCommandList, pCamera, &Object_Shader[0]);

	for (CGameObject* gameobject : GameObject_Stone)
	{
		gameobject->UpdateTransform(NULL);
		gameobject->Render(pd3dCommandList, pCamera, &Object_Shader[0]);
	}
	
	Item_Render(pd3dDevice, pd3dCommandList, pCamera);
	Particle_Render(pd3dDevice, pd3dCommandList, pCamera);



	UI_Render(pd3dDevice, pd3dCommandList);
}
void Playing_Scene::Particle_Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{	
	// Ŀ��� ����Ʈ�� PSO ����, ��Ʈ �ñ״�ó�� PSO�� �̹� ���ε� �Ǿ� ���� == ���� Ŀ��� ����Ʈ���� ���ε� ���ص� ������, ������ ����
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	Object_Shader[0].Setting_PSO(pd3dCommandList);

	// ī�޶� ���� �� ���� ������Ʈ
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->Update_Shader_Resource(pd3dCommandList);


	if(power_charge)
		Charge_Effect->Particle_Render(pd3dCommandList, pCamera, &Object_Shader[0]);
	
	for (Particle* particle : m_particle)
		particle->Particle_Render(pd3dCommandList, pCamera);

	item_manager->Particle_Render(pd3dCommandList, pCamera);
}
void Playing_Scene::UI_Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// Ŀ��� ����Ʈ�� PSO ����, ��Ʈ �ñ״�ó�� PSO�� �̹� ���ε� �Ǿ� ���� == ���� Ŀ��� ����Ʈ���� ���ε� ���ص� ������, ������ ����
	pd3dCommandList->SetGraphicsRootSignature(UI_GraphicsRootSignature);
	UI_Shader[0].Setting_PSO(pd3dCommandList);


	for (UI* ui_ptr : UI_list)
		if (ui_ptr->Active)
			ui_ptr->UI_Render(pd3dDevice, pd3dCommandList, UI_Shader);

}
void Playing_Scene::Item_Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	// Ŀ��� ����Ʈ�� PSO ����, ��Ʈ �ñ״�ó�� PSO�� �̹� ���ε� �Ǿ� ���� == ���� Ŀ��� ����Ʈ���� ���ε� ���ص� ������, ������ ����
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	Object_Shader[0].Setting_PSO(pd3dCommandList);

	// ī�޶� ���� �� ���� ������Ʈ
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->Update_Shader_Resource(pd3dCommandList);


	for (Item* item : Game_Items)
	{
		item->UpdateTransform(NULL);
		item->Render(pd3dCommandList, pCamera, &Object_Shader[0]);
	}


}
void Playing_Scene::Message_Render(ID2D1DeviceContext2* pd2dDevicecontext)
{
	// �ð� ���� ���
	std::wstring wsTimeLimit = std::to_wstring(TURN_MAX_TIME - static_cast<int>(Limit_time));
	D2D1_RECT_F player_Time_Limit = D2D1::RectF(350, 0, 450, 100);

	pd2dDevicecontext->DrawTextW(wsTimeLimit.c_str(), (UINT32)wcslen(wsTimeLimit.c_str()), write_font_list[0], &player_Time_Limit, brush_list[0]);

	
	// �κ��丮 ���� ��� ���� ���
	if (player_inventory->Is_Num_Render())
	{
		for (int i = 0; i < Item_Type_Num; ++i)
		{
			std::wstring item_n = std::to_wstring(player_inventory->item_list[i].second);
			pd2dDevicecontext->DrawTextW(item_n.c_str(), (UINT32)wcslen(item_n.c_str()), write_font_list[1], &player_inventory->text_area[i], brush_list[0]);
		}
	}

}

StoneObject* Playing_Scene::Pick_Stone_Pointed_By_Cursor(int xClient, int yClient, CCamera* pCamera)
{
	if (!pCamera) 
		return(NULL);
	XMFLOAT4X4 xmf4x4View = pCamera->GetViewMatrix();
	XMFLOAT4X4 xmf4x4Projection = pCamera->GetProjectionMatrix();
	D3D12_VIEWPORT d3dViewport = pCamera->GetViewport();

	// ȭ�� ��ǥ���� �� (xClient, yClient)�� ȭ�� ��ǥ ��ȯ�� ����ȯ�� ���� ��ȯ�� ����ȯ�� �Ѵ�. 
	// �� ����� ī�޶� ��ǥ���� ���̴�. ���� ����� ī�޶󿡼� z-������ �Ÿ��� 1�̹Ƿ� z-��ǥ�� 1�� �����Ѵ�.
	XMFLOAT3 xmf3PickPosition;
	xmf3PickPosition.x = (((2.0f * xClient) / d3dViewport.Width) - 1) / xmf4x4Projection._11;
	xmf3PickPosition.y = -(((2.0f * yClient) / d3dViewport.Height) - 1) / xmf4x4Projection._22;
	xmf3PickPosition.z = 1.0f;
	DebugOutput("x: " + std::to_string(xmf3PickPosition.x) + "\t y: " + std::to_string(xmf3PickPosition.y));

	int nIntersected = 0;

	float fHitDistance = FLT_MAX;
	float fNearestHitDistance = FLT_MAX;

	CGameObject* pIntersectedObject = NULL;
	CGameObject* pNearestObject = NULL;
	
	pIntersectedObject = Pick_Stone_By_RayIntersection(xmf3PickPosition, xmf4x4View, &fHitDistance);
	if (pIntersectedObject && (fHitDistance < fNearestHitDistance))
	{
		fNearestHitDistance = fHitDistance;
		pNearestObject = pIntersectedObject;
	}

	return((StoneObject*)pNearestObject);
}

StoneObject* Playing_Scene::Pick_Stone_By_RayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfNearHitDistance)
{
	int nIntersected = 0;
	*pfNearHitDistance = FLT_MAX;
	float fHitDistance = FLT_MAX;
	CGameObject* pSelectedObject = NULL;
	for (CGameObject* obj_stone : GameObject_Stone) 
	{
		if (player1.select_Stone == obj_stone)
			continue;
		
		nIntersected = obj_stone->Pick_Object_By_Ray_Intersection_Projection(xmf3PickPosition,xmf4x4View, &fHitDistance); 
		if ((nIntersected > 0) && (fHitDistance < *pfNearHitDistance))
		{
			*pfNearHitDistance = fHitDistance;
			pSelectedObject = obj_stone;
		}
	}
	return((StoneObject*)pSelectedObject);
}

bool Playing_Scene::is_Object_Selectable(CGameObject* now_picked)
{
	if (now_picked == player1.select_Stone || now_picked == NULL)
		return false;

	if (now_picked->player_team == false)
		return false;
	
	if (power_charge)
		return false;

	if (now_picked->used_item == Item_Type::Ghost)
		return false;

	if (item_manager->Get_Stone(Item_Type::Double_Power) != NULL)
		if(item_manager->Get_Stone(Item_Type::Double_Power)->turn == 1)
			return false;

	return true;
}

bool Playing_Scene::is_Player_Turn()
{
	return (Player_Turn && Player_Shot == false);
}

void Playing_Scene::Shoot_Stone(float power)
{ 
	if (player1.select_Stone != NULL)
	{
		if (player1.select_Stone->used_item == Item_Type::Max_Power)
			power = 1000;
		XMFLOAT3 direction = m_pPlayer->GetLookVector();
		player1.select_Stone->SetMovingDirection(direction);
		player1.select_Stone->SetMovingSpeed(power);

		Player_Shot = true;
	}
}

void Playing_Scene::Shoot_Stone_Com(float power)
{
	StoneObject* c_stone;
	StoneObject* p_stone;


	c_stone = computer.select_Stone;
	p_stone = computer.target_Stone;

	XMFLOAT3 c_position = c_stone->GetPosition();
	XMFLOAT3 p_position = p_stone->GetPosition();

	XMFLOAT3 direction;
	XMStoreFloat3(&direction, XMVectorSubtract(XMLoadFloat3(&p_position), XMLoadFloat3(&c_position)));
	XMStoreFloat3(&direction, XMVector3Normalize(XMLoadFloat3(&direction))); // ����ȭ���� �ϱ�

	// ���� ���� : -5 ~ 5
	float random_angle = 5 - uid(dre) / 300.0f;

	// ������ �������� ��ȯ
	float angleInRadians = XMConvertToRadians(random_angle);

	// Y ���� �������� ȸ���ϴ� ȸ�� ����� ����
	XMMATRIX rotationMatrix = XMMatrixRotationY(angleInRadians);

	// XMFLOAT3 ���͸� XMVECTOR�� ��ȯ
	XMVECTOR dir = XMLoadFloat3(&direction);

	// ȸ�� ����� �����Ͽ� ���� ���͸� ȸ��
	XMVECTOR rotatedDir = XMVector3TransformNormal(dir, rotationMatrix);

	// ȸ���� ���͸� XMFLOAT3�� ��ȯ
	XMFLOAT3 newDirection;
	XMStoreFloat3(&newDirection, rotatedDir);


	// �� ������
	c_stone->SetMovingDirection(newDirection);

	if (power < 100)
		power = 100;
	else if (power > 300)
		power = 500;

	c_stone->SetMovingSpeed(power);

}

std::pair<StoneObject*, StoneObject*> Playing_Scene::Select_Stone_Com()
{
	// 1. Find_Nearest_Enemy_Stone�� �ݺ��Ͽ� ���� ã�� ���� ����
	std::vector<std::pair<StoneObject*, StoneObject*>> pairs;
	std::vector<StoneObject*> Living_C_Stone;
	std::vector<StoneObject*> Living_Player_Stone;

	for (CGameObject* obj_ptr : GameObject_Stone)
	{
		if ((obj_ptr->active == true) && (obj_ptr->player_team == false))
		{
			StoneObject* com_stone = (StoneObject*)obj_ptr;
			Living_C_Stone.push_back(com_stone);
		}
		else if ((obj_ptr->active == true) && (obj_ptr->player_team == true))
		{
			StoneObject* player_stone = (StoneObject*)obj_ptr;
			Living_Player_Stone.push_back(player_stone);
		}
	}

	for (auto com_stone : Living_C_Stone)
	{
		for (auto player_stone : Living_Player_Stone)
		{
			pairs.push_back(std::make_pair(com_stone, player_stone));
		}
	}

	if (pairs.empty())
	{
		DebugOutput("No Pair to Shoot Target");
		::PostQuitMessage(0);
		return std::pair<StoneObject*, StoneObject*>{NULL, NULL};
	}

	// 3. �Ÿ� ������ ����
	std::sort(pairs.begin(), pairs.end(), [](const auto& lhs, const auto& rhs) {
		float lhs_D = XMVectorGetX(XMVector3Length(XMLoadFloat3(&lhs.first->GetPosition()) - XMLoadFloat3(&lhs.second->GetPosition())));
		float rhs_D = XMVectorGetX(XMVector3Length(XMLoadFloat3(&rhs.first->GetPosition()) - XMLoadFloat3(&rhs.second->GetPosition())));
		return lhs_D < rhs_D;
		});

	// 4. �迭�� ũ�Ⱑ 0~3�� �������� �ľ�
	int maxIndex = std::min<int>(3, static_cast<int>(pairs.size()) - 1);

	if (maxIndex < 0)
	{
		DebugOutput("No Pair to Shoot Target");
		::PostQuitMessage(0);
		return std::pair<StoneObject*, StoneObject*>{NULL, NULL};
	}

	// 5. 0���� maxIndex ������ ���� �ε����� ���Ͽ� ���� ����
	int randomIndex = rand() % (maxIndex + 1);
	auto selected_pair = pairs[randomIndex];
	return selected_pair;

}

std::pair<StoneObject*, StoneObject*> Playing_Scene::Find_Nearest_Enemy_Stone()
{


	std::vector<StoneObject*>Living_C_Stone;
	std::vector< StoneObject*>Living_Player_Stone;
	for (CGameObject* obj_ptr : GameObject_Stone)
	{
		if ((obj_ptr->active == true) && (obj_ptr->player_team == false))
		{
			StoneObject* com_stone = (StoneObject*)obj_ptr;
			Living_C_Stone.push_back(com_stone);
		}
		else if ((obj_ptr->active == true) && (obj_ptr->player_team == true))
		{
			StoneObject* player_stone = (StoneObject*)obj_ptr;
			Living_Player_Stone.push_back(player_stone);
		}
	}

	float Closest_Distance = 100000; // ���� �Ÿ� �ִ� 10��

	StoneObject* closest_com_stone = NULL;
	StoneObject* closest_player_stone = NULL;

	// ��ǻ�� ���� �÷��̾� �� ���� ���� ����� �Ÿ��� ã��
	for (auto com_stone : Living_C_Stone)
	{
		for (auto player_stone : Living_Player_Stone)
		{
			float distance = XMVectorGetX(XMVector3Length(XMLoadFloat3(&com_stone->GetPosition()) - XMLoadFloat3(&player_stone->GetPosition())));


			if (distance < Closest_Distance)
			{
				Closest_Distance = distance;
				closest_com_stone = com_stone;
				closest_player_stone = player_stone;
			}
		}
	}
	return std::make_pair(closest_com_stone, closest_player_stone);
}



void Playing_Scene::Mark_selected_stone()
{
	// ������ ������ Ghost stone�� ����Ǹ� �ȵ�
	std::vector<Stone_Item_Info*>* ghost_stones = item_manager->Get_Stone_Iist(Item_Type::Ghost);

	for (StoneObject* stone_obj : player1.stone_list)
	{
		if (stone_obj->active)
		{
			if (stone_obj == player1.select_Stone)
			{
				stone_obj->ChangeMaterial(2);
				if(player1.Item_Inventory[player1.selected_Item_Type]
					|| player1.selected_Item_Type == Item_Type::ETC 
					|| player1.selected_Item_Type == Item_Type::None)
					stone_obj->Apply_Item(player1.selected_Item_Type);
			}
			else
			{
				auto iter = std::find_if(ghost_stones->begin(), ghost_stones->end(), [stone_obj](Stone_Item_Info* info)
					{ return (info->stone != NULL && info->stone == stone_obj); });

				if (iter == ghost_stones->end()) // ghost ��ü�� �ƴ϶��
				{
					stone_obj->ChangeMaterial(1);
					stone_obj->Apply_Item(Item_Type::None);
				}
				else // ghost ��ü���
				{
					stone_obj->ChangeMaterial(1);
				}
			}
		}
	}
}

bool Playing_Scene::Check_Item(Item_Type i_type)
{
	if (player1.selected_Item_Type == Item_Type::ETC || player1.selected_Item_Type == Item_Type::None)
		return false;

	// �÷��̾ �ش� �������� �����ϰ� �ִٸ�
	if (player1.Item_Inventory[i_type])
		return true;

	else
		return false;
}
void Playing_Scene::Update_Item_Inventory()
{
	for (std::pair<Item_Type, int>& info : player_inventory->item_list)
		info.second = player1.Item_Inventory[info.first];

}
CGameObject* Playing_Scene::Pick_Item_Pointed_By_Cursor(int xClient, int yClient, CCamera* pCamera)
{
	if (!pCamera)
		return(NULL);

	D3D12_VIEWPORT d3dViewport = pCamera->GetViewport();

	// ȭ�� ��ǥ���� �� (xClient, yClient)�� ȭ�� ��ǥ ��ȯ�� ����ȯ�� ���� ��ȯ�� ����ȯ�� �Ѵ�. 
	// �� ����� ī�޶� ��ǥ���� ���̴�. ���� ����� ī�޶󿡼� z-������ �Ÿ��� 1�̹Ƿ� z-��ǥ�� 1�� �����Ѵ�.
	XMFLOAT3 xmf3PickPosition;
	xmf3PickPosition.x = (((2.0f * xClient) / d3dViewport.Width) - 1) * 8;
	xmf3PickPosition.y = -(((2.0f * (yClient - 500)) / d3dViewport.Height) - 1);
	xmf3PickPosition.z = 0.0f; 

	DebugOutput("x: " + std::to_string(xmf3PickPosition.x) + "\t y: " + std::to_string(xmf3PickPosition.y));

	int nIntersected = 0;
	float fHitDistance = FLT_MAX;
	float fNearestHitDistance = FLT_MAX;

	CGameObject* pIntersectedObject = NULL;
	CGameObject* pNearestObject = NULL;

	pIntersectedObject = Pick_Item_By_RayIntersection(xmf3PickPosition, pCamera, &fHitDistance);

	if (pIntersectedObject && (fHitDistance < fNearestHitDistance))
	{
		fNearestHitDistance = fHitDistance;
		pNearestObject = pIntersectedObject;
	}

	return(pNearestObject);
}

CGameObject* Playing_Scene::Pick_Item_By_RayIntersection(XMFLOAT3& xmf3PickPosition, CCamera* pCamera, float* pfNearHitDistance)
{
	XMFLOAT4X4 xmf4x4View = pCamera->GetViewMatrix();
	XMFLOAT4X4 xmf4x4Projection = pCamera->GetProjectionMatrix();

	int N = 1;
	int nIntersected = 0;
	*pfNearHitDistance = FLT_MAX;
	float fHitDistance = FLT_MAX;
	CGameObject* pSelected_item = NULL;

	for (CGameObject* item_obj : player_inventory->Get_Inventory_board_obj()->m_pChild)
	{
		//DebugOutput("Item info : " + std::to_string(N));
		nIntersected = item_obj->Pick_Object_By_Ray_Intersection_Orthographic(xmf3PickPosition, xmf4x4View, xmf4x4Projection, &fHitDistance);

		if ((nIntersected > 0) && (fHitDistance < *pfNearHitDistance))
		{
			DebugOutput("-------------------------------");
			DebugOutput("Picked Item: " + std::to_string(N));
			DebugOutput("-------------------------------");
			*pfNearHitDistance = fHitDistance;
			pSelected_item = item_obj;
		}
		N++;
	}
	return(pSelected_item);
}

bool Playing_Scene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		if (is_Player_Turn())
		{
			int clientX = LOWORD(lParam);
			int clientY = HIWORD(lParam);

			const int xMin = 0;
			const int xMax = 800;
			const int yMin = 0;
			const int yMax = 500;

			// ��ǥ�� ���� ���� �ִ��� Ȯ���ϴ� ���ǹ�
			if (clientX >= xMin && clientX <= xMax && clientY >= yMin && clientY <= yMax)
			{
				//���콺 ��ġ�� ������� �����ɽ����Ͽ� �� ����
				StoneObject* picked_obj = Pick_Stone_Pointed_By_Cursor(LOWORD(lParam), HIWORD(lParam), m_pPlayer->GetCamera());

				if (picked_obj)
				{
					if (is_Object_Selectable(picked_obj))
					{
						player1.select_Stone = picked_obj;
						m_pPlayer->SetPosition(picked_obj->GetPosition());

						m_pPlayer->SetCamera(m_pPlayer->ChangeCamera(THIRD_PERSON_CAMERA, 0.0f));
					}
				}
			}
		}

		if (player1.inventory_open && !power_charge)
		{
			int clientX = LOWORD(lParam);
			int clientY = HIWORD(lParam);

			const int xMin = 0;
			const int xMax = 800;
			const int yMin = 500;
			const int yMax = 600;

			// ��ǥ�� ���� ���� �ִ��� Ȯ���ϴ� ���ǹ�
			if (clientX >= xMin && clientX <= xMax && clientY >= yMin && clientY <= yMax) {

				//���콺 ��ġ�� ������� �����ɽ����Ͽ� ������ ����
				CGameObject* selected_item = Pick_Item_Pointed_By_Cursor(clientX, clientY, player_inventory);

				if (selected_item != NULL)
					player1.selected_Item_Type = ((Item*)selected_item)->item_type;
			}
		}
		break;

	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		Mark_selected_stone();
		Charge_Effect->Apply_Item(player1.selected_Item_Type);
		break;

	case WM_MOUSEWHEEL:
	{
		if (m_pPlayer->GetCamera()->GetMode() == TOP_VIEW_CAMERA)
		{
			zooming = true;

			int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

			if (zDelta > 0) // ��ũ�� ����
				zoom_value -= 2;
			else // ��ũ�� �Ʒ���
				zoom_value += 2;

			zoom_value = std::clamp(zoom_value, 0, 99); 
		}
	}
	break;

	default:
		break;
	}
	return(false);
}

bool Playing_Scene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_SPACE:
			if (is_Player_Turn() && player1.select_Stone != NULL) {
				power_charge = true;
				((BAR_UI*)ui_player_power)->Set_Bar_Charge_Mode(true);
			}
			break;

		case VK_LEFT:
			orbit_value += 1.0f;
			break;

		case VK_RIGHT:
			orbit_value -= 1.0f;
			break;



		default:
			break;
		}
	}
	break;


	case WM_KEYUP:
		switch (wParam)
		{
		case 'Q':
		case 'q':
			if (player1.select_Stone && power_charge == false)
			{
				m_pPlayer->SetCamera(m_pPlayer->ChangeCamera(TOP_VIEW_CAMERA, 0.005f));
				player1.select_Stone = NULL;
				player1.selected_Item_Type = Item_Type::None;
				Mark_selected_stone();

				player1.inventory_open = false;
				player_inventory->Set_Visualize(player1.inventory_open);
			}
			break;

		case VK_SPACE:
			if (is_Player_Turn() && player1.select_Stone != NULL) {
				power_charge = false;
				((BAR_UI*)ui_player_power)->Set_Bar_Charge_Mode(false);
				

				Shoot_Stone(power_degree);

				if (Check_Item(player1.selected_Item_Type))
				{
					player1.Item_Inventory[player1.selected_Item_Type] -= 1;
					item_manager->Add_Stone_Item_Applied(player1.select_Stone);
				}
				
				orbit_value = 0;
				zoom_value = 0;
				m_pPlayer->SetCamera(m_pPlayer->ChangeCamera(TOP_VIEW_CAMERA, 5.0f));
				Charge_Effect->Reset();
				ui_player_power->Reset();
				ui_com_power->Reset();
			}
			break;

		case VK_TAB:
			if (player1.select_Stone)
			{
				Camera_First_Person_View = !Camera_First_Person_View;
				if (Camera_First_Person_View)
				{
					if (m_pPlayer)
					{
						m_pPlayer->SetCamera(m_pPlayer->ChangeCamera(STONE_CAMERA, 0.05f));
					}
				}
				else
				{
					if (m_pPlayer)
					{
						m_pPlayer->SetCamera(m_pPlayer->ChangeCamera(THIRD_PERSON_CAMERA, 0.05f));
					}
				}
			}
			break;


		case VK_OEM_3:
		{
			player1.inventory_open = !player1.inventory_open;
			player_inventory->Set_Visualize(player1.inventory_open);
		}
			break;

		case VK_F9:
			break;

		case VK_ESCAPE:
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	return(false);
}

void Playing_Scene::ProcessInput(UCHAR* pKeysBuffer, XMFLOAT3 rotate, float fTimeElapsed)
{
	float cxDelta = rotate.y;
	float cyDelta = rotate.x;

	//���콺 �Ǵ� Ű �Է��� ������ �÷��̾ �̵��ϰų�(dwDirection) ȸ���Ѵ�(cxDelta �Ǵ� cyDelta).
	if ((cxDelta != 0.0f) || (cyDelta != 0.0f))
		m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);

	m_pPlayer->Update(fTimeElapsed);

	return;
}