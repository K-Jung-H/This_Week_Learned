//-----------------------------------------------------------------------------
// File: Shader.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Shader.h"


float Random(float fMin, float fMax)
{
	float fRandomValue = (float)rand();
	if (fRandomValue < fMin) fRandomValue = fMin;
	if (fRandomValue > fMax) fRandomValue = fMax;
	return(fRandomValue);
}

float Random()
{
	return(rand() / float(RAND_MAX));
}

XMFLOAT3 RandomPositionInSphere(XMFLOAT3 xmf3Center, float fRadius, int nColumn, int nColumnSpace)
{
	float fAngle = Random() * 360.0f * (2.0f * 3.14159f / 360.0f);

	XMFLOAT3 xmf3Position;
	xmf3Position.x = xmf3Center.x + fRadius * sin(fAngle);
	xmf3Position.y = xmf3Center.y - (nColumn * float(nColumnSpace) / 2.0f) + (nColumn * nColumnSpace) + Random();
	xmf3Position.z = xmf3Center.z + fRadius * cos(fAngle);

	return(xmf3Position);
}
//============================================================
CShader::CShader()
{
}

CShader::~CShader()
{
	ReleaseShaderVariables();

	if (m_ppd3dPipelineStates)
	{
		for (int i = 0; i < m_nPipelineStates; i++) if (m_ppd3dPipelineStates[i]) m_ppd3dPipelineStates[i]->Release();
		delete[] m_ppd3dPipelineStates;
	}
}

D3D12_SHADER_BYTECODE CShader::CreateVertexShader()
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

D3D12_SHADER_BYTECODE CShader::CreatePixelShader()
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

D3D12_SHADER_BYTECODE CShader::CompileShaderFromFile(WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob **ppd3dShaderBlob)
{
	UINT nCompileFlags = 0;
#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob *pd3dErrorBlob = NULL;
	HRESULT hResult = ::D3DCompileFromFile(pszFileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, pszShaderName, pszShaderProfile, nCompileFlags, 0, ppd3dShaderBlob, &pd3dErrorBlob);
	char *pErrorString = NULL;
	if (pd3dErrorBlob)
	{
		pErrorString = (char*)pd3dErrorBlob->GetBufferPointer();
		DebugOutput(pErrorString);
	}
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
	d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();

	return(d3dShaderByteCode);
}

#define _WITH_WFOPEN
//#define _WITH_STD_STREAM

#ifdef _WITH_STD_STREAM
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#endif

D3D12_SHADER_BYTECODE CShader::ReadCompiledShaderFromFile(WCHAR *pszFileName, ID3DBlob **ppd3dShaderBlob)
{
	UINT nReadBytes = 0;
#ifdef _WITH_WFOPEN
	FILE *pFile = NULL;
	::_wfopen_s(&pFile, pszFileName, L"rb");
	::fseek(pFile, 0, SEEK_END);
	int nFileSize = ::ftell(pFile);
	BYTE *pByteCode = new BYTE[nFileSize];
	::rewind(pFile);
	nReadBytes = (UINT)::fread(pByteCode, sizeof(BYTE), nFileSize, pFile);
	::fclose(pFile);
#endif
#ifdef _WITH_STD_STREAM
	std::ifstream ifsFile;
	ifsFile.open(pszFileName, std::ios::in | std::ios::ate | std::ios::binary);
	nReadBytes = (int)ifsFile.tellg();
	BYTE *pByteCode = new BYTE[*pnReadBytes];
	ifsFile.seekg(0);
	ifsFile.read((char *)pByteCode, nReadBytes);
	ifsFile.close();
#endif

	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	if (ppd3dShaderBlob)
	{
		*ppd3dShaderBlob = NULL;
		HRESULT hResult = D3DCreateBlob(nReadBytes, ppd3dShaderBlob);
		memcpy((*ppd3dShaderBlob)->GetBufferPointer(), pByteCode, nReadBytes);
		d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
		d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();
	}
	else
	{
		d3dShaderByteCode.BytecodeLength = nReadBytes;
		d3dShaderByteCode.pShaderBytecode = pByteCode;
	}

	return(d3dShaderByteCode);
}

D3D12_INPUT_LAYOUT_DESC CShader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = NULL;
	d3dInputLayoutDesc.NumElements = 0;

	return(d3dInputLayoutDesc);
}

D3D12_RASTERIZER_DESC CShader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	//	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
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

	return(d3dRasterizerDesc);
}

D3D12_DEPTH_STENCIL_DESC CShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = TRUE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_BLEND_DESC CShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;

	d3dBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

void CShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	::ZeroMemory(&m_d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	m_d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
	m_d3dPipelineStateDesc.VS = CreateVertexShader();
	m_d3dPipelineStateDesc.PS = CreatePixelShader();
	m_d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
	m_d3dPipelineStateDesc.BlendState = CreateBlendState();
	m_d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	m_d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	m_d3dPipelineStateDesc.SampleMask = UINT_MAX;
	m_d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	m_d3dPipelineStateDesc.NumRenderTargets = 1;
	m_d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	m_d3dPipelineStateDesc.SampleDesc.Count = 1;
	m_d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&m_d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void **)&m_ppd3dPipelineStates[0]);
}

void CShader::OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList, int nPipelineState)
{
	if (m_ppd3dPipelineStates) pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[nPipelineState]);
}

void CShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState)
{
	OnPrepareRender(pd3dCommandList, nPipelineState);
}
//========================================================================

OOBBShader::OOBBShader()
{
}

OOBBShader::~OOBBShader()
{
}

D3D12_RASTERIZER_DESC OOBBShader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(d3dRasterizerDesc);
}

D3D12_INPUT_LAYOUT_DESC OOBBShader::CreateInputLayout()
{
	UINT nInputElementDescs = 1;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE OOBBShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_OOBB", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE OOBBShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_OOBB", "ps_5_1", &m_pd3dPixelShaderBlob));
}

void OOBBShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();

	if (m_d3dPipelineStateDesc.InputLayout.pInputElementDescs) 
		delete[] m_d3dPipelineStateDesc.InputLayout.pInputElementDescs;

	OOBB_Drawer::oobb_Mesh = new OOBBMesh(pd3dDevice, pd3dCommandList);
	OOBB_Drawer::oobb_shader = this;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
ScreenShader::ScreenShader()
{

}
ScreenShader::~ScreenShader()
{
}

D3D12_INPUT_LAYOUT_DESC ScreenShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "SCREEN_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}


D3D12_SHADER_BYTECODE ScreenShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSTextureToScreen", "vs_5_1", &m_pd3dVertexShaderBlob));

}
D3D12_SHADER_BYTECODE ScreenShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSTextureToScreen", "ps_5_1", &m_pd3dPixelShaderBlob));

}
D3D12_BLEND_DESC ScreenShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA; // 소스 알파
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA; // 1 - 소스 알파
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD; // 더하기
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE; // 소스 알파(모드 변경 가능)
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO; // 대상 알파
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD; // 알파 블렌드 연산
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP; // 로직 연산 없음
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // 모든 채널에 쓰기 가능


	return(d3dBlendDesc);
}


void ScreenShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();

	if (m_d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] m_d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

void ScreenShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext)
{
	m_nObjects = 5;
	screen_Objects = new Screen_Rect * [m_nObjects];

	CTexture* main_screen_texture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	CTexture* start_button_texture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	CTexture* menu_button_texture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	CTexture* info_icon_button_texture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	CTexture* play_info_texture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);


	CTexture* alpha_screen_texture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	CTexture* pause_menu_texture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);

	CTexture* option_button_texture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	CTexture* continue_button_texture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	CTexture* exit_button_texture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	CTexture* option_icon_texture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);

	CTexture* text_box_texture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	CTexture* left_icon_texture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	CTexture* right_icon_texture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	CTexture* x_icon_texture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	CTexture* check_icon_texture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);

	CTexture* Power_Bar_texture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	CTexture* Time_Bar_texture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);

	

	main_screen_texture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"texture/background_image.dds", RESOURCE_TEXTURE2D, 0);
	start_button_texture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"texture/play_button_red.dds", RESOURCE_TEXTURE2D, 0);
	option_icon_texture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"texture/option_icon.dds", RESOURCE_TEXTURE2D, 0);
	
	info_icon_button_texture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"texture/info_icon_button.dds", RESOURCE_TEXTURE2D, 0);
	play_info_texture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"texture/how_to_play.dds", RESOURCE_TEXTURE2D, 0); // Play_Info
	
	menu_button_texture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"texture/menu_button_red.dds", RESOURCE_TEXTURE2D, 0);

	pause_menu_texture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"texture/menu_background_red.dds", RESOURCE_TEXTURE2D, 0);
	alpha_screen_texture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"texture/pause_background.dds", RESOURCE_TEXTURE2D, 0);
	option_button_texture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"texture/option_button_red.dds", RESOURCE_TEXTURE2D, 0);
	continue_button_texture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"texture/continue_button_red.dds", RESOURCE_TEXTURE2D, 0);
	exit_button_texture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"texture/exit_button_red.dds", RESOURCE_TEXTURE2D, 0);

	text_box_texture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"texture/TextBox.dds", RESOURCE_TEXTURE2D, 0);
	left_icon_texture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"texture/Left_icon_button.dds", RESOURCE_TEXTURE2D, 0);
	right_icon_texture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"texture/Right_icon_button.dds", RESOURCE_TEXTURE2D, 0);
	check_icon_texture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"texture/check_button.dds", RESOURCE_TEXTURE2D, 0);
	x_icon_texture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"texture/x_button.dds", RESOURCE_TEXTURE2D, 0);

	Power_Bar_texture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"texture/Power_Bar.dds", RESOURCE_TEXTURE2D, 0);
	Time_Bar_texture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"texture/Time_Bar.dds", RESOURCE_TEXTURE2D, 0);

	
	//================================================================================
	const char* main_screen_txt = "main_screen";

	main_screen_ptr = new Screen_Rect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this, main_screen_texture, XMFLOAT2(-1.0f, 1.0f), XMFLOAT2(1.0f, -1.0f), 10);
	std::copy(main_screen_txt, main_screen_txt + std::strlen(main_screen_txt) + 1, main_screen_ptr->m_pstrFrameName);

	screen_Objects[0] = main_screen_ptr;

	//================================================================================
	const char* start_txt = "start_button";

	Screen_Rect* start_button = new Screen_Rect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this, start_button_texture, XMFLOAT2(0.3f, -0.6f), XMFLOAT2(0.9f, -0.9f), 1);
	std::copy(start_txt, start_txt + std::strlen(start_txt) + 1, start_button->m_pstrFrameName);

	main_screen_ptr->SetChild(start_button);
	start_button_ptr = start_button;

	//================================================================================
	const char* option_icon_txt = "option_icon_button";

	Screen_Rect* option_icon_button = new Screen_Rect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this, option_icon_texture, XMFLOAT2(0.7f, 0.9f), XMFLOAT2(0.9f, 0.7f), 1);

	std::copy(option_icon_txt, option_icon_txt + std::strlen(option_icon_txt) + 1, option_icon_button->m_pstrFrameName);

	main_screen_ptr->SetChild(option_icon_button);
	option_icon_button_ptr = option_icon_button;

	//================================================================================
	const char* info_icon_txt = "info_icon_button";

	info_icon_button_ptr = new Screen_Rect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this, info_icon_button_texture, XMFLOAT2(-0.9f, 0.9f), XMFLOAT2(-0.7f, 0.7f), 1);

	std::copy(info_icon_txt, info_icon_txt + std::strlen(info_icon_txt) + 1, info_icon_button_ptr->m_pstrFrameName);

	main_screen_ptr->SetChild(info_icon_button_ptr);
	{
		//================================================================================
		const char* info_screen_txt = "info_screen";

		info_screen_ptr = new Screen_Rect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this, alpha_screen_texture, XMFLOAT2(-1.0f, 1.0f), XMFLOAT2(1.0f, -1.0f), 10);

		std::copy(info_screen_txt, info_screen_txt + std::strlen(info_screen_txt) + 1, info_screen_ptr->m_pstrFrameName);

		screen_Objects[1] = info_screen_ptr;

		//================================================================================
		const char* play_info_txt = "play_info";

		play_info_ptr = new Screen_Rect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this, play_info_texture, XMFLOAT2(-0.75f, 1.5f), XMFLOAT2(0.75f, -1.5f), 8);

		play_info_ptr->sissor_rect_screen_size.left = static_cast<UINT>((-0.75f + 1.0f) * 0.5f * FRAME_BUFFER_WIDTH);
		play_info_ptr->sissor_rect_screen_size.top = static_cast<UINT>((1.0f - 0.7f) * 0.5f * FRAME_BUFFER_HEIGHT);
		play_info_ptr->sissor_rect_screen_size.right = static_cast<UINT>((0.75f + 1.0f) * 0.5f * FRAME_BUFFER_WIDTH);
		play_info_ptr->sissor_rect_screen_size.bottom = static_cast<UINT>((1.0f - (-0.7f)) * 0.5f * FRAME_BUFFER_HEIGHT);


		std::copy(play_info_txt, play_info_txt + std::strlen(play_info_txt) + 1, play_info_ptr->m_pstrFrameName);

		info_screen_ptr->SetChild(play_info_ptr);

	}

	{
		//================================================================================
		const char* pause_screen_txt = "pause_screen";

		pause_screen_ptr = new Screen_Rect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this, alpha_screen_texture, XMFLOAT2(-1.0f, 1.0f), XMFLOAT2(1.0f, -1.0f), 9);
		std::copy(pause_screen_txt, pause_screen_txt + std::strlen(pause_screen_txt) + 1, pause_screen_ptr->m_pstrFrameName);

		screen_Objects[2] = pause_screen_ptr;

		//================================================================================
		const char* pause_menu_txt = "pause_menu";

		pause_menu_box_ptr = new Screen_Rect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this, pause_menu_texture, XMFLOAT2(-0.75f, 1.0f), XMFLOAT2(0.75f, -0.7f), 8);
		std::copy(pause_menu_txt, pause_menu_txt + std::strlen(pause_menu_txt) + 1, pause_screen_ptr->m_pstrFrameName);

		pause_screen_ptr->SetChild(pause_menu_box_ptr);

		//================================================================================
		const char* continue_txt = "continue_button";

		Screen_Rect* continue_button = new Screen_Rect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this, continue_button_texture, XMFLOAT2(-0.4f, 0.6f), XMFLOAT2(0.4f, 0.3f), 1);
		std::copy(continue_txt, continue_txt + std::strlen(continue_txt) + 1, continue_button->m_pstrFrameName);

		pause_menu_box_ptr->SetChild(continue_button);

		//================================================================================
		const char* option_txt = "option_button";

		Screen_Rect* option_button = new Screen_Rect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this, option_button_texture, XMFLOAT2(-0.4f, 0.25f), XMFLOAT2(0.4f, -0.05f), 1);
		std::copy(option_txt, option_txt + std::strlen(option_txt) + 1, option_button->m_pstrFrameName);

		pause_menu_box_ptr->SetChild(option_button);
		//================================================================================
		const char* exit_txt = "exit_button";

		Screen_Rect* exit_button = new Screen_Rect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this, exit_button_texture, XMFLOAT2(-0.4f, -0.1f), XMFLOAT2(0.4f, -0.4f), 1);
		std::copy(exit_txt, exit_txt + std::strlen(exit_txt) + 1, exit_button->m_pstrFrameName);

		pause_menu_box_ptr->SetChild(exit_button);
	}
		//================================================================================
		const char* menu_txt = "menu_icon_button";

		Screen_Rect* menu_button = new Screen_Rect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this, menu_button_texture, XMFLOAT2(0.7f, 0.9f), XMFLOAT2(0.9f, 0.7f), 1);

		std::copy(menu_txt, menu_txt + std::strlen(menu_txt) + 1, menu_button->m_pstrFrameName);

		screen_Objects[3] = menu_button;
		menu_button_ptr = menu_button;
	
	//================================================================================
	const char* option_screen_txt = "option_screen";

	option_screen_ptr = new Screen_Rect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this, alpha_screen_texture, XMFLOAT2(-1.0f, 1.0f), XMFLOAT2(1.0f, -1.0f), 9);

	std::copy(option_screen_txt, option_screen_txt + std::strlen(option_screen_txt) + 1, option_screen_ptr->m_pstrFrameName);

	screen_Objects[4] = option_screen_ptr;

	//================================================================================
	const char* option_menu_txt = "option_menu";

	option_menu_box_ptr = new Screen_Rect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this, pause_menu_texture, XMFLOAT2(-0.95f, 1.0f), XMFLOAT2(0.95f, -0.7f), 8);

	std::copy(option_menu_txt, option_menu_txt + std::strlen(option_menu_txt) + 1, option_menu_box_ptr->m_pstrFrameName);

	option_screen_ptr->SetChild(option_menu_box_ptr);
	//================================================================================
	{
		const char* option_player_speed_txt = "option_player_speed";

		option_player_speed_ptr = new Screen_Rect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this, text_box_texture, XMFLOAT2(-0.4f, 0.5f), XMFLOAT2(0.4f, 0.3f), 1);
		option_player_speed_ptr->Is_Text_Screen = true;
		option_player_speed_ptr->Text_List.push_back(_T("Slow"));
		option_player_speed_ptr->Text_List.push_back(_T("Normal"));
		option_player_speed_ptr->Text_List.push_back(_T("Fast"));

		std::copy(option_player_speed_txt, option_player_speed_txt + std::strlen(option_player_speed_txt) + 1, option_player_speed_ptr->m_pstrFrameName);

		option_menu_box_ptr->SetChild(option_player_speed_ptr);

		//================================================================================
		const char* option_player_speed_minus_txt = "option_player_speed_minus";

		option_player_speed_minus_button_ptr = new Screen_Rect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this, left_icon_texture, XMFLOAT2(-0.6f, 0.5f), XMFLOAT2(-0.4f, 0.3f), 1);

		std::copy(option_player_speed_minus_txt, option_player_speed_minus_txt + std::strlen(option_player_speed_minus_txt) + 1, option_player_speed_minus_button_ptr->m_pstrFrameName);

		option_player_speed_ptr->SetChild(option_player_speed_minus_button_ptr);

		//================================================================================
		const char* option_player_speed_plus_txt = "option_player_speed_plus";

		option_player_speed_plus_button_ptr = new Screen_Rect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this, right_icon_texture, XMFLOAT2(0.4f, 0.5f), XMFLOAT2(0.6f, 0.3f), 1);

		std::copy(option_player_speed_plus_txt, option_player_speed_plus_txt + std::strlen(option_player_speed_plus_txt) + 1, option_player_speed_plus_button_ptr->m_pstrFrameName);

		option_player_speed_ptr->SetChild(option_player_speed_plus_button_ptr);
	}
	{
		//================================================================================
		const char* option_stone_speed_txt = "option_stone_speed";

		option_stone_speed_ptr = new Screen_Rect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this, text_box_texture, XMFLOAT2(-0.4f, 0.1f), XMFLOAT2(0.4f, -0.1f), 1);
		option_stone_speed_ptr->Is_Text_Screen = true;
		option_stone_speed_ptr->Text_List.push_back(_T("Very Slow"));
		option_stone_speed_ptr->Text_List.push_back(_T("Slow"));
		option_stone_speed_ptr->Text_List.push_back(_T("Normal"));
		option_stone_speed_ptr->Text_List.push_back(_T("Fast"));
		option_stone_speed_ptr->Text_List.push_back(_T("Very Fast"));

		std::copy(option_stone_speed_txt, option_stone_speed_txt + std::strlen(option_stone_speed_txt) + 1, option_stone_speed_ptr->m_pstrFrameName);

		option_menu_box_ptr->SetChild(option_stone_speed_ptr);

		//================================================================================
		const char* option_stone_speed_minus_txt = "option_stone_speed_minus";

		option_stone_speed_minus_button_ptr = new Screen_Rect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this, left_icon_texture, XMFLOAT2(-0.6f, 0.1f), XMFLOAT2(-0.4f, -0.1f), 1);

		std::copy(option_stone_speed_minus_txt, option_stone_speed_minus_txt + std::strlen(option_stone_speed_minus_txt) + 1, option_stone_speed_minus_button_ptr->m_pstrFrameName);

		option_stone_speed_ptr->SetChild(option_stone_speed_minus_button_ptr);

		//================================================================================
		const char* option_stone_speed_plus_txt = "option_stone_speed_plus";

		option_stone_speed_plus_button_ptr = new Screen_Rect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this, right_icon_texture, XMFLOAT2(0.4f, 0.1f), XMFLOAT2(0.6f, -0.1f), 1);

		std::copy(option_stone_speed_plus_txt, option_stone_speed_plus_txt + std::strlen(option_stone_speed_plus_txt) + 1, option_stone_speed_plus_button_ptr->m_pstrFrameName);

		option_stone_speed_ptr->SetChild(option_stone_speed_plus_button_ptr);
	}
	{
		//================================================================================
		const char* option_difficulty_txt = "option_difficulty";

		option_difficulty_ptr = new Screen_Rect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this, text_box_texture, XMFLOAT2(-0.4f, -0.3f), XMFLOAT2(0.4f, -0.5f), 1);
		option_difficulty_ptr->Is_Text_Screen = true;
		option_difficulty_ptr->Text_List.push_back(_T("Easy"));
		option_difficulty_ptr->Text_List.push_back(_T("Normal"));
		option_difficulty_ptr->Text_List.push_back(_T("Hard"));

		std::copy(option_difficulty_txt, option_difficulty_txt + std::strlen(option_difficulty_txt) + 1, option_difficulty_ptr->m_pstrFrameName);

		option_menu_box_ptr->SetChild(option_difficulty_ptr);

		//================================================================================
		const char* option_difficulty_minus_txt = "option_difficulty_minus";

		option_difficulty_minus_button_ptr = new Screen_Rect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this, left_icon_texture, XMFLOAT2(-0.6f, -0.3f), XMFLOAT2(-0.4f, -0.5f), 1);

		std::copy(option_difficulty_minus_txt, option_difficulty_minus_txt + std::strlen(option_difficulty_minus_txt) + 1, option_difficulty_minus_button_ptr->m_pstrFrameName);

		option_difficulty_ptr->SetChild(option_difficulty_minus_button_ptr);

		//================================================================================
		const char* option_difficulty_plus_txt = "option_difficulty_plus";

		option_difficulty_plus_button_ptr = new Screen_Rect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this, right_icon_texture, XMFLOAT2(0.4f, -0.3f), XMFLOAT2(0.6f, -0.5f), 1);

		std::copy(option_difficulty_plus_txt, option_difficulty_plus_txt + std::strlen(option_difficulty_plus_txt) + 1, option_difficulty_plus_button_ptr->m_pstrFrameName);

		option_difficulty_ptr->SetChild(option_difficulty_plus_button_ptr);
	}
	//================================================================================

	const char* x_button_txt = "x_button";

	Screen_Rect* x_button = new Screen_Rect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this, x_icon_texture, XMFLOAT2(-0.3f, -0.6f), XMFLOAT2(-0.1f, -0.8f), 1);

	std::copy(x_button_txt, x_button_txt + std::strlen(x_button_txt) + 1, x_button->m_pstrFrameName);

	option_menu_box_ptr->SetChild(x_button);
	//================================================================================

	const char* check_button_txt = "check_button";

	Screen_Rect* check_button = new Screen_Rect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this, check_icon_texture, XMFLOAT2(0.1f, -0.6f), XMFLOAT2(0.3f, -0.8f), 1);

	std::copy(check_button_txt, check_button_txt + std::strlen(check_button_txt) + 1, check_button->m_pstrFrameName);

	option_menu_box_ptr->SetChild(check_button);
	//================================================================================
	const char* Power_Bar_txt = "Power_Bar_Rect";

	Screen_Rect* Power_Bar = new Screen_Rect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this, Power_Bar_texture, XMFLOAT2(-1.0f, -0.9f), XMFLOAT2(1.0f, -1.0f), 1);
	Power_Bar->right_scale_move_value = 0.0f;
	std::copy(Power_Bar_txt, Power_Bar_txt + std::strlen(Power_Bar_txt) + 1, Power_Bar->m_pstrFrameName);
	Power_Bar_ptr = Power_Bar;

	const char* Time_Bar_txt = "Time_Bar_Rect";
	Screen_Rect* Time_Bar = new Screen_Rect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this, Time_Bar_texture, XMFLOAT2(-1.0f, -0.8f), XMFLOAT2(1.0f, -1.0f), 1);
	Time_Bar->right_scale_move_value = 10.0f;
	std::copy(Time_Bar_txt, Time_Bar_txt + std::strlen(Time_Bar_txt) + 1, Time_Bar->m_pstrFrameName);
	Time_Bar_ptr = Time_Bar;
	//================================================================================

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void ScreenShader::Set_Start_Sceen_UI()
{
	for (int j = 0; j < m_nObjects; j++)
		if (screen_Objects[j])
			screen_Objects[j]->active = false;

	main_screen_ptr->active = true;
	option_icon_button_ptr->active = true;
}

void ScreenShader::Set_Game_Sceen_UI()
{
	for (int j = 0; j < m_nObjects; j++)
		if (screen_Objects[j])
			screen_Objects[j]->active = false;

	menu_button_ptr->active = true;
	
}

bool ScreenShader::Scroll_Update(float fTimeElapsed, float value)
{
	static float currentScrollValue = 0.0f;
	float lerpSpeed = 2.0f;                   

	float t = lerpSpeed * fTimeElapsed;

	std::vector<Screen_Rect*> scroll_list;
	scroll_list.push_back(play_info_ptr);

	for(Screen_Rect* sr_ptr : scroll_list)
		if (sr_ptr)
		{
			currentScrollValue += (value - sr_ptr->scroll_value) * t;
			sr_ptr->scroll_value = currentScrollValue;
			sr_ptr->scroll_value = std::clamp(sr_ptr->scroll_value, -1.2f, 1.2f);

			sr_ptr->sissor_rect_clip = true; // 시저렉트 적용할 객체들

			if (abs(sr_ptr->scroll_value) == 1.2f)
				return false;
		}
	return true;
}
void ScreenShader::AnimateObjects(float fTimeElapsed)
{
}
void ScreenShader::ReleaseObjects()
{
	if (screen_Objects)
	{
		for (int j = 0; j < m_nObjects; j++) 
			if (screen_Objects[j]) 
				screen_Objects[j]->Release();
		delete[] screen_Objects;
	}
}

void ScreenShader::ReleaseUploadBuffers()
{
	for (int j = 0; j < m_nObjects; j++) 
		if (screen_Objects[j]) 
			screen_Objects[j]->ReleaseUploadBuffers();
}

void ScreenShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	CShader::Render(pd3dCommandList, pCamera, nPipelineState);

	for (int j = 0; j < m_nObjects; j++)
	{
		if (screen_Objects[j])
		{
				screen_Objects[j]->Render(pd3dCommandList, pCamera);
		}
	}
}

void ScreenShader::Bar_Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	CShader::Render(pd3dCommandList, pCamera, nPipelineState);
	if (Power_Bar_ptr->right_scale_move_value > 0.0f)
		Power_Bar_ptr->Render(pd3dCommandList, pCamera);

	if (Time_Bar_ptr->right_scale_move_value > 0.0f)
		Time_Bar_ptr->Render(pd3dCommandList, pCamera);
}

std::string ScreenShader::PickObjectPointedByCursor(float xClient, float yClient, CCamera* pCamera)
{
	XMFLOAT3 xmf3PickPosition = { (float)xClient, (float)yClient, -1.0f };

	float fHitDistance = FLT_MAX, fNearestHitDistance = FLT_MAX;

	Screen_Rect* pNearestObject = NULL;
	Screen_Rect* pPicked_Object = NULL;


	Screen_Rect** Check_Objects = (Screen_Rect**)screen_Objects;
	int Obj_N = m_nObjects;

	for (int i = 0; i < Obj_N; i++)
	{
		if (Check_Objects[i]->active == false)
			continue;

		pPicked_Object = Check_Objects[i]->PickScreenObjectByRayIntersection(xmf3PickPosition, &fHitDistance);

		if (pPicked_Object != NULL && (fHitDistance < fNearestHitDistance))
		{
			fNearestHitDistance = fHitDistance;
			pNearestObject = pPicked_Object;
		}
	}

	if (pNearestObject != NULL)
	{
		std::string name(pNearestObject->m_pstrFrameName);
		DebugOutput(name);
		return name;
	}
	else
		return std::string{ "" };
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CSkyBoxShader::CSkyBoxShader()
{
}

CSkyBoxShader::~CSkyBoxShader()
{
}

D3D12_INPUT_LAYOUT_DESC CSkyBoxShader::CreateInputLayout()
{
	UINT nInputElementDescs = 1;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_DEPTH_STENCIL_DESC CSkyBoxShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0xff;
	d3dDepthStencilDesc.StencilWriteMask = 0xff;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_INCR;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_DECR;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CSkyBoxShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSSkyBox", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CSkyBoxShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSSkyBox", "ps_5_1", &m_pd3dPixelShaderBlob));
}

void CSkyBoxShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();

	if (m_d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] m_d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}


CTerrainShader::CTerrainShader()
{
}

CTerrainShader::~CTerrainShader()
{
}

D3D12_INPUT_LAYOUT_DESC CTerrainShader::CreateInputLayout()
{
	UINT nInputElementDescs = 4;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,				2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[3] = { "TEXCOORD",	1, DXGI_FORMAT_R32G32_FLOAT,				3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CTerrainShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSTerrain", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CTerrainShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSTerrain", "ps_5_1", &m_pd3dPixelShaderBlob));
}



void CTerrainShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
}


//================================================

Outline_Shader::Outline_Shader()
{
}

Outline_Shader::~Outline_Shader()
{
}

D3D12_DEPTH_STENCIL_DESC Outline_Shader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));

	//깊이-검사를 하지 않으므로 여러 개의 객체들이 겹쳐지는 것처럼 그려진다.
	d3dDepthStencilDesc.DepthEnable = TRUE;

	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	return(d3dDepthStencilDesc);
}


D3D12_INPUT_LAYOUT_DESC Outline_Shader::CreateInputLayout()
{
	UINT nInputElementDescs = 3;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE Outline_Shader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSOutline", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE Outline_Shader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSOutline", "ps_5_1", &m_pd3dPixelShaderBlob));
}

void Outline_Shader::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
}

void Outline_Shader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	CShader::Render(pd3dCommandList, pCamera, nPipelineState);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CStandardShader::CStandardShader()
{
}

CStandardShader::~CStandardShader()
{
}

D3D12_INPUT_LAYOUT_DESC CStandardShader::CreateInputLayout()
{
	UINT nInputElementDescs = 5;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[3] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[4] = { "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 4, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CStandardShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSStandard", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CStandardShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSStandard", "ps_5_1", &m_pd3dPixelShaderBlob));
}

void CStandardShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();

	if (m_d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] m_d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

CRawFormatImage* CObjectsShader::Object_map = NULL;


bool CObjectsShader::Show_Collider = false;
bool CObjectsShader::Show_Attack_Collider = false;
bool CObjectsShader::Show_ETC_Collider = false;
CObjectsShader::CObjectsShader()
{
}

CObjectsShader::~CObjectsShader()
{
}

void CObjectsShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext)
{
}

void CObjectsShader::ReleaseObjects()
{
	for (CGameObject* obj_ptr : m_ppObjects)
	{
		if (obj_ptr)
			obj_ptr->Release();
	}
	m_ppObjects.clear();
}

void CObjectsShader::AnimateObjects(float fTimeElapsed)
{
	for (CGameObject* obj_ptr : m_ppObjects)
	{
		if (obj_ptr)
			obj_ptr->Animate(fTimeElapsed);
	}
}

void CObjectsShader::ReleaseUploadBuffers()
{
	for (CGameObject* obj_ptr : m_ppObjects)
	{
		if (obj_ptr)
			obj_ptr->ReleaseUploadBuffers();
	}
}

void CObjectsShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState)
{
	CShader::Render(pd3dCommandList, pCamera, nPipelineState);

	for(CGameObject* obj_ptr : m_ppObjects)
	{
		if (obj_ptr && (obj_ptr->is_render && obj_ptr->active))
		{
			obj_ptr->UpdateTransform(NULL);
			obj_ptr->Render(pd3dCommandList, pCamera);
		}
	}

	if (Show_Collider)
	{
		for (CGameObject* obj_ptr : m_ppObjects)
		{
			if (obj_ptr && obj_ptr->active)
				obj_ptr->Collider_Render(pd3dCommandList, pCamera);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CPlayerShader::CPlayerShader()
{
}

CPlayerShader::~CPlayerShader()
{
}

D3D12_INPUT_LAYOUT_DESC CPlayerShader::CreateInputLayout()
{
	UINT nInputElementDescs = 5;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[3] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[4] = { "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 4, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CPlayerShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSStandard", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CPlayerShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSStandard", "ps_5_1", &m_pd3dPixelShaderBlob));
}

void CPlayerShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();

	if (m_d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] m_d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

//=========================================================
Diffuse_Shader::Diffuse_Shader()
{

}

Diffuse_Shader::~Diffuse_Shader()
{
}


D3D12_INPUT_LAYOUT_DESC Diffuse_Shader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE Diffuse_Shader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSDiffused", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE Diffuse_Shader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSDiffused", "ps_5_1", &m_pd3dPixelShaderBlob));
}

void Diffuse_Shader::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
}

void Diffuse_Shader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext)
{
	CGameObject* bullet = new Bullet_Object(pd3dDevice, pd3dCommandList, NULL);
	CMesh* pSphereMesh = new CSphereMeshDiffused(pd3dDevice, pd3dCommandList, 2.0f, 30, 30);
	CMaterial* bullet_material = new CMaterial();
	bullet->SetMesh(pSphereMesh);
	bullet->SetMaterial(0, bullet_material);
	bullet->SetPosition(0.0f, 50.0f, 0.0f);
	m_ppObjects.push_back(bullet);
}

void Diffuse_Shader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	CShader::Render(pd3dCommandList, pCamera, nPipelineState);
	CObjectsShader::Render(pd3dCommandList, pCamera, nPipelineState);

}

//===============================================================


BOX_Shader::BOX_Shader()
{
}

BOX_Shader::~BOX_Shader()
{
}

D3D12_INPUT_LAYOUT_DESC BOX_Shader::CreateInputLayout()
{
	UINT nInputElementDescs = 5;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[3] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[4] = { "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 4, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE BOX_Shader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_FLYING_BOX", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE BOX_Shader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_FLYING_BOX", "ps_5_1", &m_pd3dPixelShaderBlob));
}

void BOX_Shader::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
}

void BOX_Shader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext)
{
	CMesh* cube_mesh = new Textured_Cube_Mesh(pd3dDevice, pd3dCommandList, 40.0f, 40.0f, 40.0f);
	CTexture* cube_texture = new CTexture(2, RESOURCE_TEXTURE2D, 0, 1);
	cube_texture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"texture/box_texture.dds", RESOURCE_TEXTURE2D, 0);
	cube_texture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"texture/box_texture_normal_2.dds", RESOURCE_TEXTURE2D, 1);


	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CScene::CreateShaderResourceViews(pd3dDevice, cube_texture, 0, 9);


	CMaterial* pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(cube_texture);

	//===========================================
	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;
	XMFLOAT3 xmf3Scale = pTerrain->GetScale();

	float wall_size= pTerrain->GetWidth();

	//===========================================
	Flying_Box* wall_box_object = NULL;
	CMesh* wall_box_mesh = new Textured_Cube_Mesh(pd3dDevice, pd3dCommandList, wall_size, wall_size, 10.0f);

	XMFLOAT3 wall_pos_list[4] =
	{ 
		XMFLOAT3{0,				wall_size/2,		wall_size/2},
		XMFLOAT3{wall_size,		wall_size / 2,	wall_size/2},
		XMFLOAT3{wall_size/2,	wall_size / 2	,	wall_size},
		XMFLOAT3{wall_size/2,	wall_size / 2 ,	0			}
	};

	for (int i = 0; i < 4; ++i)
	{
		wall_box_object = new Flying_Box(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		wall_box_object->SetMesh(wall_box_mesh);
		wall_box_object->SetMaterial(0, pCubeMaterial);
		wall_box_object->SetPosition(wall_pos_list[i]);
		wall_box_object->SetRotationSpeed(0.0f);
		wall_box_object->is_render = false; // 이거 활성화 하면  벽면 안보이게 됨
		if (i < 2)
			wall_box_object->Rotate(0.0f, 90.0f, 0.0f);

		m_ppObjects.push_back(wall_box_object);
	}



	//===========================================


	Flying_Box* box_object = NULL;

	for (int nObjects = 0, z = 2; z <= 254; z+=20)
	{
		for (int x = 2; x <= 254; x+=20)
		{
			BYTE nPixel = Object_map->GetRawImagePixel(x, z);

			float fyOffset = 0.0f;

			switch (nPixel)
			{
			case 102:
				fyOffset = 30.0f * 0.5f;
				break;

			case 153:
				fyOffset = 20.0f * 0.5f;
				break;

			case 204:
				fyOffset = 40.0f * 0.5f;
				break;

			default:
				break;
			}
			
			if (fyOffset)
			{
				float xPosition = x * xmf3Scale.x;
				float zPosition = z * xmf3Scale.z;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition) * 5;

				box_object = new Flying_Box(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
				box_object->SetMesh(cube_mesh);
				box_object->SetMaterial(0, pCubeMaterial); 

				box_object->SetPosition(xPosition, fHeight + fyOffset, zPosition);

				m_ppObjects.push_back(box_object);
			}
		}
	}

}

void BOX_Shader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	CShader::Render(pd3dCommandList, pCamera, nPipelineState);
	CObjectsShader::Render(pd3dCommandList, pCamera, nPipelineState);

}

//===============================================================

Asteroid_Shader::Asteroid_Shader()
{
}

Asteroid_Shader::~Asteroid_Shader()
{
}

D3D12_INPUT_LAYOUT_DESC Asteroid_Shader::CreateInputLayout()
{
	UINT nInputElementDescs = 3;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE Asteroid_Shader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_Instancing_Asteroid", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE Asteroid_Shader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_Instancing_Asteroid", "ps_5_1", &m_pd3dPixelShaderBlob));
}

void Asteroid_Shader::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
}

void Asteroid_Shader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext)
{
	pTerrain_info = (CHeightMapTerrain*)pContext;


	asteroid_mesh = new Textured_Cube_Mesh(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 20.0f);

	CTexture* asteroid_texture = new CTexture(2, RESOURCE_TEXTURE2D, 0, 1);
	asteroid_texture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"texture/Lava_Emissive.dds", RESOURCE_TEXTURE2D, 0);
	asteroid_texture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"texture/Lava_Normal.dds", RESOURCE_TEXTURE2D, 1);
	CScene::CreateShaderResourceViews(pd3dDevice, asteroid_texture, 0, 9);

	pAsteroidMaterial = new CMaterial();
	pAsteroidMaterial->SetTexture(asteroid_texture);
	pAsteroidMaterial->SetShader(this);

	outline_Material = new CMaterial();

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

}

void Asteroid_Shader::AnimateObjects(float fTimeElapsed)
{

	for (CGameObject* obj_ptr : m_ppObjects)
	{
		if (obj_ptr->active == false)
			continue;

		Asteroid* obj_info = (Asteroid*)obj_ptr;
		XMFLOAT3 Target_pos = obj_info->target_obj->GetPosition();
		XMFLOAT3 obj_pos = obj_ptr->GetPosition();

		if (obj_info->In_Gravity)
		{
			XMFLOAT3 newDirection = { 0.0f,0.0f,0.0f };
			XMStoreFloat3(&newDirection, XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&Target_pos), XMLoadFloat3(&obj_pos))));

			obj_ptr->SetMovingDirection(newDirection);
		}
		else
		{
			// 목표까지의 거리와 좌표 차이 계산
			float dx = Target_pos.x - obj_pos.x;
			float dy = Target_pos.y - obj_pos.y;
			float dz = Target_pos.z - obj_pos.z;
			float distance = sqrt(dx * dx + dy * dy + dz * dz);

			// `cool_down` 값 조정
			float cool_down = (distance > 500.0f) ? 3.0f : 1.0f;
			if (obj_info->move_time < cool_down)
			{
				obj_info->move_time += fTimeElapsed;
			}
			else
			{
				obj_info->move_time = 0.0f;

				// 방향과 속도 설정
				XMFLOAT3 direction = { 0.0f, 0.0f, 0.0f };
				float speed = 0.0f;

				if (distance > 500.0f)
				{
					// 무작위 축 선택
					int randomAxis = rand() % 3; // 0: x, 1: y, 2: z
					if (randomAxis == 0 && (fabs(dx) > 5.0f)) // X축 선택
						direction.x = (dx > 0) ? 1.0f : -1.0f;
					else if (randomAxis == 1 && (fabs(dy) > 5.0f)) // Y축 선택
						direction.y = (dy > 0) ? 1.0f : -1.0f;
					else if (randomAxis == 2 && (fabs(dz) > 5.0f)) // Z축 선택
						direction.z = (dz > 0) ? 1.0f : -1.0f;

					// 속도를 100으로 설정
					speed = 200.0f;

					obj_ptr->SetMovingDirection(direction);
					obj_ptr->SetMovingSpeed(speed);

				}
				else if (distance > 1.0f) // 거리 50 이하일 때 방향 합산 및 속도 설정
				{
					direction.x = (dx > 0) ? 1.0f : -1.0f;
					direction.y = (dy > 0) ? 1.0f : -1.0f;
					direction.z = (dz > 0) ? 1.0f : -1.0f;

					// 방향 벡터 정규화
					XMVECTOR dirVector = XMLoadFloat3(&direction);
					dirVector = XMVector3Normalize(dirVector);
					XMStoreFloat3(&direction, dirVector);

					// 속도를 30으로 설정
					speed = 100.0f;

					obj_ptr->SetMovingDirection(direction);
					obj_ptr->SetMovingSpeed(speed);

				}

				if (distance < 1.0f)
				{
					obj_ptr->SetMovingDirection(XMFLOAT3(0.0f, 1.0f, 0.0f));
					obj_ptr->SetMovingSpeed(0.0f);
				}
			}
		}
	}

	for (CGameObject* obj_ptr : m_ppObjects)
	{

		XMFLOAT3 pos = obj_ptr->GetPosition();
		float fHeight = pTerrain_info->GetHeight(pos.x, pos.z) + 10.0f;

		if (pos.y < fHeight)
		{
			pos.y = fHeight;
			obj_ptr->SetPosition(pos);
			((Asteroid*)obj_ptr)->move_time += 1.0f;
		}
	}


	CObjectsShader::AnimateObjects(fTimeElapsed);
}


void Asteroid_Shader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	CShader::Render(pd3dCommandList, pCamera, nPipelineState);
}

void Asteroid_Shader::Add_Object(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT3 pos, CGameObject* target_ptr)
{
	// 죽어있는 객체의 수를 세기 위한 변수
	int inactiveCount = std::count_if(m_ppObjects.begin(), m_ppObjects.end(), [](CGameObject* obj_ptr) {
		return obj_ptr->active == false;
		});

	// 죽어있는 객체의 수가 10개 이상인 경우
	if (inactiveCount >= 10) {
		// 죽어있는 객체를 삭제하고 살아있는 객체를 새로운 벡터로 복사
		m_ppObjects.erase(std::remove_if(m_ppObjects.begin(), m_ppObjects.end(), [](CGameObject* obj_ptr) {
			if (obj_ptr->active == false) {
				delete obj_ptr; // 죽어있는 객체 메모리 해제
				return true;    // 이 객체를 제거
			}
			return false; // 살아있는 객체는 유지
			}), m_ppObjects.end());
	}

	// 재활용 또는 새로운 객체 추가
	for (CGameObject* obj_ptr : m_ppObjects)
	{
		if (obj_ptr->active == false)
		{
			obj_ptr->SetPosition(pos);
			((Asteroid*)obj_ptr)->life = 100;
			((Asteroid*)obj_ptr)->SetTarget(target_ptr);
			((Asteroid*)obj_ptr)->In_Gravity = false;
			obj_ptr->active = true;
			return; // 재활용한 객체가 있으므로 함수 종료
		}
	}

	if (m_ppObjects.size() >= 60)
		return;

	// 모든 객체가 다 살아있어서 공간이 없다면 새로운 객체 생성
	CGameObject* asteroid_ptr = new Asteroid(pd3dDevice, pd3dCommandList, NULL);

	asteroid_ptr->SetMaterial(0, outline_Material);
	asteroid_ptr->SetMaterial(1, pAsteroidMaterial);
	asteroid_ptr->SetMesh(asteroid_mesh);
	asteroid_ptr->SetPosition(pos);
	((Asteroid*)asteroid_ptr)->SetTarget(target_ptr);

	m_ppObjects.push_back(asteroid_ptr);
}

CBillboardObjectsShader::CBillboardObjectsShader()
{
}

CBillboardObjectsShader::~CBillboardObjectsShader()
{
}


D3D12_DEPTH_STENCIL_DESC CBillboardObjectsShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = TRUE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}


D3D12_BLEND_DESC CBillboardObjectsShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA; // 소스 알파
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA; // 1 - 소스 알파
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD; // 더하기
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE; // 소스 알파(모드 변경 가능)
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO; // 대상 알파
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD; // 알파 블렌드 연산
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP; // 로직 연산 없음
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // 모든 채널에 쓰기 가능

	return(d3dBlendDesc);
}

D3D12_RASTERIZER_DESC CBillboardObjectsShader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(d3dRasterizerDesc);
}

D3D12_INPUT_LAYOUT_DESC CBillboardObjectsShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CBillboardObjectsShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSBillboard", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CBillboardObjectsShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSBillboard", "ps_5_1", &m_pd3dPixelShaderBlob));
}

void CBillboardObjectsShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
}

void CBillboardObjectsShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext)
{

}

void CBillboardObjectsShader::ReleaseUploadBuffers()
{
	CObjectsShader::ReleaseUploadBuffers();
}

void CBillboardObjectsShader::ReleaseObjects()
{
	if (m_pRawFormatImage)
		delete m_pRawFormatImage;

	CObjectsShader::ReleaseObjects();
}

void CBillboardObjectsShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	XMFLOAT3 xmf3CameraPosition = pCamera->GetPosition();

	for (CGameObject* obj_ptr : m_ppObjects)
	{
		if (obj_ptr)
			obj_ptr->SetLookAt(xmf3CameraPosition, XMFLOAT3(0.0f, 1.0f, 0.0f));
	}


	CShader::Render(pd3dCommandList, pCamera, nPipelineState);
	CObjectsShader::Render(pd3dCommandList, pCamera, nPipelineState);
}


Black_Hole_Shader::Black_Hole_Shader()
{
}

Black_Hole_Shader::~Black_Hole_Shader()
{
}

D3D12_SHADER_BYTECODE Black_Hole_Shader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSBillboard_Black_Hole", "ps_5_1", &m_pd3dPixelShaderBlob));
}

D3D12_DEPTH_STENCIL_DESC Black_Hole_Shader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

void Black_Hole_Shader::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
}

void Black_Hole_Shader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext)
{
	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;
	//========================================================
	CGameObject* red_hole_ptr = new Black_Hole_Object(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	CMesh* black_hole_mesh = new Billboard_Mesh(pd3dDevice, pd3dCommandList, 100.0f, 100.0f);
	CTexture* black_hole_texture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	black_hole_texture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"texture/blue_hole.dds", RESOURCE_TEXTURE2D, 0);

	CMaterial* pBlackHoleMaterial = new CMaterial();
	pBlackHoleMaterial->SetTexture(black_hole_texture);

	CScene::CreateShaderResourceViews(pd3dDevice, black_hole_texture, 0, PARAMETER_DEFAULT_TEXTURE);

	red_hole_ptr->SetMesh(black_hole_mesh);
	red_hole_ptr->SetMaterial(0, pBlackHoleMaterial);
	red_hole_ptr->SetPosition(pTerrain->GetWidth() / 2, 100.0f, pTerrain->GetLength() / 2);
	((Black_Hole_Object*)red_hole_ptr)->wave_info.Color_Change = XMFLOAT4(1.0f, 0.0f, 1.0f, 0.0f);
	m_ppObjects.push_back(red_hole_ptr);

	spawner_hole = (Black_Hole_Object*)red_hole_ptr;
	//========================================================
	CGameObject* blue_hole_ptr = new Black_Hole_Object(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	blue_hole_ptr->SetMesh(black_hole_mesh);
	blue_hole_ptr->SetMaterial(0, pBlackHoleMaterial);
	blue_hole_ptr->SetPosition(300.0f, 200.0f, 300.0f);
	blue_hole_ptr->active = false;

	m_ppObjects.push_back(blue_hole_ptr);

	player_black_hole = (Black_Hole_Object*)blue_hole_ptr;
	//========================================================

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}


void Black_Hole_Shader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	CBillboardObjectsShader::Render(pd3dCommandList, pCamera, nPipelineState);

	if (CObjectsShader::Show_ETC_Collider)
		if (player_black_hole->Gravity_area)
		{
			player_black_hole->oobb_drawer->oobb_shader->Render(pd3dCommandList, pCamera);
			player_black_hole->Gravity_area->Center = player_black_hole->GetPosition();
			player_black_hole->oobb_drawer->UpdateOOBB_Data(pd3dCommandList, &player_black_hole->m_xmf4x4World, player_black_hole->Gravity_area, XMFLOAT4(0.0f, 0.5f, 1.0f, 1.0f));
			player_black_hole->oobb_drawer->Render(pd3dCommandList, pCamera);
		}
}


//==========================================================================

Sprite_Billboard_Shader::Sprite_Billboard_Shader()
{
}

Sprite_Billboard_Shader::~Sprite_Billboard_Shader()
{
}

D3D12_INPUT_LAYOUT_DESC Sprite_Billboard_Shader::CreateInputLayout()
{
	UINT nInputElementDescs = 3;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "VERTEX_NUM", 0, DXGI_FORMAT_R32_UINT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };


	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE Sprite_Billboard_Shader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_Billboard_Animation", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE Sprite_Billboard_Shader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_Billboard_Animation", "ps_5_1", &m_pd3dPixelShaderBlob));
}

void Sprite_Billboard_Shader::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
}

void Sprite_Billboard_Shader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext)
{
	CGameObject* boom_ptr = new Billboard_Animation_Object(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	boom_mesh = new Billboard_Mesh(pd3dDevice, pd3dCommandList, 100.0f, 100.0f);
	CTexture* boom_texture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	boom_texture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"texture/boom_sprite.dds", RESOURCE_TEXTURE2D, 0);

	boom_material = new CMaterial();
	boom_material->SetTexture(boom_texture);

	CScene::CreateShaderResourceViews(pd3dDevice, boom_texture, 0, PARAMETER_DEFAULT_TEXTURE);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void Sprite_Billboard_Shader::Add_Object(XMFLOAT3 pos)
{
	CGameObject* boom_obj = new Billboard_Animation_Object(NULL, NULL, NULL);
	boom_obj->SetMesh(boom_mesh);
	boom_obj->SetMaterial(0, boom_material);
	boom_obj->SetPosition(pos);

	m_ppObjects.push_back(boom_obj);
}

void Sprite_Billboard_Shader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	CBillboardObjectsShader::Render(pd3dCommandList, pCamera, nPipelineState);
}


