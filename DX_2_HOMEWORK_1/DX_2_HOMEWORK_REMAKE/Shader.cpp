//-----------------------------------------------------------------------------
// File: Shader.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Shader.h"

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



	main_screen_texture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"texture/background_image.dds", RESOURCE_TEXTURE2D, 0);
	start_button_texture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"texture/play_button_red.dds", RESOURCE_TEXTURE2D, 0);
	option_icon_texture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"texture/option_icon.dds", RESOURCE_TEXTURE2D, 0);
	
	info_icon_button_texture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"texture/info_icon_button.dds", RESOURCE_TEXTURE2D, 0);
	play_info_texture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"texture/Play_Info.dds", RESOURCE_TEXTURE2D, 0);

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

		play_info_ptr = new Screen_Rect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this, play_info_texture, XMFLOAT2(-0.75f, 0.7f), XMFLOAT2(0.75f, -0.7f), 8);

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

		option_player_speed_ptr = new Screen_Rect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this, text_box_texture, XMFLOAT2(-0.4f, 0.6f), XMFLOAT2(0.4f, 0.3f), 1);

		std::copy(option_player_speed_txt, option_player_speed_txt + std::strlen(option_player_speed_txt) + 1, option_player_speed_ptr->m_pstrFrameName);

		option_menu_box_ptr->SetChild(option_player_speed_ptr);

		//================================================================================
		const char* option_player_speed_minus_txt = "option_player_speed_minus";

		option_player_speed_minus_button_ptr = new Screen_Rect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this, left_icon_texture, XMFLOAT2(-0.6f, 0.6f), XMFLOAT2(-0.4f, 0.3f), 1);

		std::copy(option_player_speed_minus_txt, option_player_speed_minus_txt + std::strlen(option_player_speed_minus_txt) + 1, option_player_speed_minus_button_ptr->m_pstrFrameName);

		option_player_speed_ptr->SetChild(option_player_speed_minus_button_ptr);

		//================================================================================
		const char* option_player_speed_plus_txt = "option_player_speed_plus";

		option_player_speed_plus_button_ptr = new Screen_Rect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this, right_icon_texture, XMFLOAT2(0.4f, 0.6f), XMFLOAT2(0.6f, 0.3f), 1);

		std::copy(option_player_speed_plus_txt, option_player_speed_plus_txt + std::strlen(option_player_speed_plus_txt) + 1, option_player_speed_plus_button_ptr->m_pstrFrameName);

		option_player_speed_ptr->SetChild(option_player_speed_plus_button_ptr);
	}
	{
		//================================================================================
		const char* option_stone_speed_txt = "option_stone_speed";

		option_stone_speed_ptr = new Screen_Rect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this, text_box_texture, XMFLOAT2(-0.4f, 0.25f), XMFLOAT2(0.4f, -0.05f), 1);

		std::copy(option_stone_speed_txt, option_stone_speed_txt + std::strlen(option_stone_speed_txt) + 1, option_stone_speed_ptr->m_pstrFrameName);

		option_menu_box_ptr->SetChild(option_stone_speed_ptr);

		//================================================================================
		const char* option_stone_speed_minus_txt = "option_stone_speed_minus";

		option_stone_speed_minus_button_ptr = new Screen_Rect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this, left_icon_texture, XMFLOAT2(-0.6f, 0.25f), XMFLOAT2(-0.4f, -0.05f), 1);

		std::copy(option_stone_speed_minus_txt, option_stone_speed_minus_txt + std::strlen(option_stone_speed_minus_txt) + 1, option_stone_speed_minus_button_ptr->m_pstrFrameName);

		option_stone_speed_ptr->SetChild(option_stone_speed_minus_button_ptr);

		//================================================================================
		const char* option_stone_speed_plus_txt = "option_stone_speed_plus";

		option_stone_speed_plus_button_ptr = new Screen_Rect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this, right_icon_texture, XMFLOAT2(0.4f, 0.25f), XMFLOAT2(0.6f, -0.05f), 1);

		std::copy(option_stone_speed_plus_txt, option_stone_speed_plus_txt + std::strlen(option_stone_speed_plus_txt) + 1, option_stone_speed_plus_button_ptr->m_pstrFrameName);

		option_stone_speed_ptr->SetChild(option_stone_speed_plus_button_ptr);
	}
	{
		//================================================================================
		const char* option_difficulty_txt = "option_difficulty";

		option_difficulty_ptr = new Screen_Rect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this, text_box_texture, XMFLOAT2(-0.4f, -0.1f), XMFLOAT2(0.4f, -0.4f), 1);

		std::copy(option_difficulty_txt, option_difficulty_txt + std::strlen(option_difficulty_txt) + 1, option_difficulty_ptr->m_pstrFrameName);

		option_menu_box_ptr->SetChild(option_difficulty_ptr);

		//================================================================================
		const char* option_difficulty_minus_txt = "option_difficulty_minus";

		option_difficulty_minus_button_ptr = new Screen_Rect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this, left_icon_texture, XMFLOAT2(-0.6f, -0.1f), XMFLOAT2(-0.4f, -0.4f), 1);

		std::copy(option_difficulty_minus_txt, option_difficulty_minus_txt + std::strlen(option_difficulty_minus_txt) + 1, option_difficulty_minus_button_ptr->m_pstrFrameName);

		option_difficulty_ptr->SetChild(option_difficulty_minus_button_ptr);

		//================================================================================
		const char* option_difficulty_plus_txt = "option_difficulty_plus";

		option_difficulty_plus_button_ptr = new Screen_Rect(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, this, right_icon_texture, XMFLOAT2(0.4f, -0.1), XMFLOAT2(0.6f, -0.4f), 1);

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
			sr_ptr->scroll_value = std::clamp(sr_ptr->scroll_value, -0.4f, 0.4f);

			sr_ptr->sissor_rect_clip = true; // 시저렉트 적용할 객체들

			if (abs(sr_ptr->scroll_value) == 0.4f)
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
CObjectsShader::CObjectsShader()
{
	m_nObjects = 120;
}

CObjectsShader::~CObjectsShader()
{
}

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

void CObjectsShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext)
{
	m_ppObjects = new CGameObject*[m_nObjects];

	CGameObject *pSuperCobraModel = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/SuperCobra.bin", this);
	CGameObject* pGunshipModel = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Gunship.bin", this);

	int nColumnSpace = 5, nColumnSize = 30;           
    int nFirstPassColumnSize = (m_nObjects % nColumnSize) > 0 ? (nColumnSize - 1) : nColumnSize;

	int nObjects = 0;
    for (int h = 0; h < nFirstPassColumnSize; h++)
    {
        for (int i = 0; i < floor(float(m_nObjects) / float(nColumnSize)); i++)
        {
			if (nObjects % 2)
			{
				m_ppObjects[nObjects] = new CSuperCobraObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
				m_ppObjects[nObjects]->SetChild(pSuperCobraModel);
				pSuperCobraModel->AddRef();
			}
			else
			{
				m_ppObjects[nObjects] = new CGunshipObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
				m_ppObjects[nObjects]->SetChild(pGunshipModel);
				pGunshipModel->AddRef();
			}
			m_ppObjects[nObjects]->SetPosition(RandomPositionInSphere(XMFLOAT3(0.0f, 0.0f, 0.0f), Random(20.0f, 100.0f), h - int(floor(nColumnSize / 2.0f)), nColumnSpace));
			m_ppObjects[nObjects]->Rotate(0.0f, 90.0f, 0.0f);
			m_ppObjects[nObjects++]->PrepareAnimate();
		}
    }

    if (nFirstPassColumnSize != nColumnSize)
    {
        for (int i = 0; i < m_nObjects - int(floor(float(m_nObjects) / float(nColumnSize)) * nFirstPassColumnSize); i++)
        {
			if (nObjects % 2)
			{
				m_ppObjects[nObjects] = new CSuperCobraObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
				m_ppObjects[nObjects]->SetChild(pSuperCobraModel);
				pSuperCobraModel->AddRef();
			}
			else
			{
				m_ppObjects[nObjects] = new CGunshipObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
				m_ppObjects[nObjects]->SetChild(pGunshipModel);
				pGunshipModel->AddRef();
			}
			m_ppObjects[nObjects]->SetPosition(RandomPositionInSphere(XMFLOAT3(0.0f, 0.0f, 0.0f), Random(20.0f, 100.0f), nColumnSize - int(floor(nColumnSize / 2.0f)), nColumnSpace));
			m_ppObjects[nObjects]->Rotate(0.0f, 90.0f, 0.0f);
			m_ppObjects[nObjects++]->PrepareAnimate();
        }
    }

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CObjectsShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) m_ppObjects[j]->Release();
		delete[] m_ppObjects;
	}
}

void CObjectsShader::AnimateObjects(float fTimeElapsed)
{
}

void CObjectsShader::ReleaseUploadBuffers()
{
	for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) m_ppObjects[j]->ReleaseUploadBuffers();
}

void CObjectsShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState)
{
	CShader::Render(pd3dCommandList, pCamera, nPipelineState);

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
		{
			m_ppObjects[j]->Animate(0.16f);
			m_ppObjects[j]->UpdateTransform(NULL);
			m_ppObjects[j]->Render(pd3dCommandList, pCamera);
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

