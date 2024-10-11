//-----------------------------------------------------------------------------
// File: CScene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Scene.h"

CDescriptorHeap* CScene::m_pDescriptorHeap = NULL;

CDescriptorHeap::CDescriptorHeap()
{
	m_d3dSrvCPUDescriptorStartHandle.ptr = NULL;
	m_d3dSrvGPUDescriptorStartHandle.ptr = NULL;
}

CDescriptorHeap::~CDescriptorHeap()
{
	if (m_pd3dCbvSrvDescriptorHeap) m_pd3dCbvSrvDescriptorHeap->Release();
}

void CScene::CreateCbvSrvDescriptorHeaps(ID3D12Device* pd3dDevice, int nConstantBufferViews, int nShaderResourceViews)
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	d3dDescriptorHeapDesc.NumDescriptors = nConstantBufferViews + nShaderResourceViews; //CBVs + SRVs 
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pDescriptorHeap->m_pd3dCbvSrvDescriptorHeap);

	m_pDescriptorHeap->m_d3dCbvCPUDescriptorStartHandle = m_pDescriptorHeap->m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pDescriptorHeap->m_d3dCbvGPUDescriptorStartHandle = m_pDescriptorHeap->m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	m_pDescriptorHeap->m_d3dSrvCPUDescriptorStartHandle.ptr = m_pDescriptorHeap->m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);
	m_pDescriptorHeap->m_d3dSrvGPUDescriptorStartHandle.ptr = m_pDescriptorHeap->m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);

	m_pDescriptorHeap->m_d3dCbvCPUDescriptorNextHandle = m_pDescriptorHeap->m_d3dCbvCPUDescriptorStartHandle;
	m_pDescriptorHeap->m_d3dCbvGPUDescriptorNextHandle = m_pDescriptorHeap->m_d3dCbvGPUDescriptorStartHandle;
	m_pDescriptorHeap->m_d3dSrvCPUDescriptorNextHandle = m_pDescriptorHeap->m_d3dSrvCPUDescriptorStartHandle;
	m_pDescriptorHeap->m_d3dSrvGPUDescriptorNextHandle = m_pDescriptorHeap->m_d3dSrvGPUDescriptorStartHandle;
}

void CScene::CreateConstantBufferViews(ID3D12Device* pd3dDevice, int nConstantBufferViews, ID3D12Resource* pd3dConstantBuffers, UINT nStride)
{
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = pd3dConstantBuffers->GetGPUVirtualAddress();
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	d3dCBVDesc.SizeInBytes = nStride;
	for (int j = 0; j < nConstantBufferViews; j++)
	{
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (nStride * j);
		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, m_pDescriptorHeap->m_d3dCbvCPUDescriptorNextHandle);
		m_pDescriptorHeap->m_d3dCbvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
		m_pDescriptorHeap->m_d3dCbvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	}
}

D3D12_GPU_DESCRIPTOR_HANDLE CScene::CreateConstantBufferView(ID3D12Device* pd3dDevice, ID3D12Resource* pd3dConstantBuffer, UINT nStride)
{
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	d3dCBVDesc.SizeInBytes = nStride;
	d3dCBVDesc.BufferLocation = pd3dConstantBuffer->GetGPUVirtualAddress();
	pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, m_pDescriptorHeap->m_d3dCbvCPUDescriptorNextHandle);
	D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle = m_pDescriptorHeap->m_d3dCbvGPUDescriptorNextHandle;
	m_pDescriptorHeap->m_d3dCbvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	m_pDescriptorHeap->m_d3dCbvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

	return(d3dCbvGPUDescriptorHandle);
}

D3D12_GPU_DESCRIPTOR_HANDLE CScene::CreateConstantBufferView(ID3D12Device* pd3dDevice, D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress, UINT nStride)
{
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	d3dCBVDesc.SizeInBytes = nStride;
	d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress;
	pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, m_pDescriptorHeap->m_d3dCbvCPUDescriptorNextHandle);
	D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle = m_pDescriptorHeap->m_d3dCbvGPUDescriptorNextHandle;
	m_pDescriptorHeap->m_d3dCbvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	m_pDescriptorHeap->m_d3dCbvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

	return(d3dCbvGPUDescriptorHandle);
}

void CScene::CreateShaderResourceViews(ID3D12Device* pd3dDevice, CTexture* pTexture, UINT nDescriptorHeapIndex, UINT nRootParameterStartIndex)
{
	m_pDescriptorHeap->m_d3dSrvCPUDescriptorNextHandle.ptr += (::gnCbvSrvDescriptorIncrementSize * nDescriptorHeapIndex);
	m_pDescriptorHeap->m_d3dSrvGPUDescriptorNextHandle.ptr += (::gnCbvSrvDescriptorIncrementSize * nDescriptorHeapIndex);

	int nTextures = pTexture->GetTextures();
	for (int i = 0; i < nTextures; i++)
	{
		ID3D12Resource* pShaderResource = pTexture->GetResource(i);
		D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = pTexture->GetShaderResourceViewDesc(i);
		pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, m_pDescriptorHeap->m_d3dSrvCPUDescriptorNextHandle);
		m_pDescriptorHeap->m_d3dSrvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

		pTexture->SetGpuDescriptorHandle(i, m_pDescriptorHeap->m_d3dSrvGPUDescriptorNextHandle);
		m_pDescriptorHeap->m_d3dSrvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	}
	int nRootParameters = pTexture->GetRootParameters();
	for (int i = 0; i < nRootParameters; i++) pTexture->SetRootParameterIndex(i, nRootParameterStartIndex + i);
}

void CScene::CreateShaderResourceView(ID3D12Device* pd3dDevice, CTexture* pTexture, int nIndex, UINT nRootParameterStartIndex)
{
	ID3D12Resource* pShaderResource = pTexture->GetResource(nIndex);
	D3D12_GPU_DESCRIPTOR_HANDLE d3dGpuDescriptorHandle = pTexture->GetGpuDescriptorHandle(nIndex);
	if (pShaderResource && !d3dGpuDescriptorHandle.ptr)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = pTexture->GetShaderResourceViewDesc(nIndex);
		pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, m_pDescriptorHeap->m_d3dSrvCPUDescriptorNextHandle);
		m_pDescriptorHeap->m_d3dSrvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

		pTexture->SetGpuDescriptorHandle(nIndex, m_pDescriptorHeap->m_d3dSrvGPUDescriptorNextHandle);
		m_pDescriptorHeap->m_d3dSrvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

		pTexture->SetRootParameterIndex(nIndex, nRootParameterStartIndex + nIndex);
	}
}

void CScene::CreateShaderResourceView(ID3D12Device* pd3dDevice, CTexture* pTexture, int nIndex)
{
	ID3D12Resource* pShaderResource = pTexture->GetResource(nIndex);
	D3D12_GPU_DESCRIPTOR_HANDLE d3dGpuDescriptorHandle = pTexture->GetGpuDescriptorHandle(nIndex);
	if (pShaderResource && !d3dGpuDescriptorHandle.ptr)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = pTexture->GetShaderResourceViewDesc(nIndex);
		pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, m_pDescriptorHeap->m_d3dSrvCPUDescriptorNextHandle);
		m_pDescriptorHeap->m_d3dSrvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

		pTexture->SetGpuDescriptorHandle(nIndex, m_pDescriptorHeap->m_d3dSrvGPUDescriptorNextHandle);
		m_pDescriptorHeap->m_d3dSrvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	}
}

//=============================================================================================

CScene::CScene()
{
}

CScene::~CScene()
{
}

void CScene::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
}

void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();
	if (m_pDescriptorHeap) delete m_pDescriptorHeap;

	if (m_ppShaders)
	{
		for (int i = 0; i < m_nShaders; i++)
		{
			m_ppShaders[i]->ReleaseShaderVariables();
			m_ppShaders[i]->ReleaseObjects();
			m_ppShaders[i]->Release();
		}
		delete[] m_ppShaders;
	}

	if (m_pSkyBox) delete m_pSkyBox;

	if (m_ppGameObjects)
	{
		for (int i = 0; i < m_nGameObjects; i++) if (m_ppGameObjects[i]) m_ppGameObjects[i]->Release();
		delete[] m_ppGameObjects;
	}

	ReleaseShaderVariables();

}

ID3D12RootSignature* CScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;

	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[5];
	{
		pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		pd3dDescriptorRanges[0].NumDescriptors = 1;
		pd3dDescriptorRanges[0].BaseShaderRegister = 0; //t0: Default Texture
		pd3dDescriptorRanges[0].RegisterSpace = 0;
		pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		pd3dDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		pd3dDescriptorRanges[1].NumDescriptors = 7;
		pd3dDescriptorRanges[1].BaseShaderRegister = 1; //t1: gtxtStandardTextures[7] //0:Albedo, 1:Specular, 2:Metallic, 3:Normal, 4:Emission, 5:DetailAlbedo, 6:DetailNormal
		pd3dDescriptorRanges[1].RegisterSpace = 0;
		pd3dDescriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		pd3dDescriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		pd3dDescriptorRanges[2].NumDescriptors = 1;
		pd3dDescriptorRanges[2].BaseShaderRegister = 8; //t2: gtxtSkyBoxTexture
		pd3dDescriptorRanges[2].RegisterSpace = 0;
		pd3dDescriptorRanges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		//=======================================================================
		pd3dDescriptorRanges[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		pd3dDescriptorRanges[3].NumDescriptors = 3;
		pd3dDescriptorRanges[3].BaseShaderRegister = 9; //t9: gtxtTerrainBaseTexture
		pd3dDescriptorRanges[3].RegisterSpace = 0;
		pd3dDescriptorRanges[3].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		pd3dDescriptorRanges[4].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		pd3dDescriptorRanges[4].NumDescriptors = 2;
		pd3dDescriptorRanges[4].BaseShaderRegister = 12; //t12: gtxtBox_Albedo
		pd3dDescriptorRanges[4].RegisterSpace = 0;
		pd3dDescriptorRanges[4].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	}
	D3D12_ROOT_PARAMETER pd3dRootParameters[11];
	{
		pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		pd3dRootParameters[0].Descriptor.ShaderRegister = 1; //Camera
		pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
		pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		pd3dRootParameters[1].Constants.Num32BitValues = 33;
		pd3dRootParameters[1].Constants.ShaderRegister = 2; //GameObject
		pd3dRootParameters[1].Constants.RegisterSpace = 0;
		pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		pd3dRootParameters[2].Descriptor.ShaderRegister = 3; //Lights
		pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
		pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		pd3dRootParameters[3].DescriptorTable.NumDescriptorRanges = 1;
		pd3dRootParameters[3].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[0]); // 기본 텍스쳐 
		pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		pd3dRootParameters[4].DescriptorTable.NumDescriptorRanges = 1;
		pd3dRootParameters[4].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[1]); // Standard Mesh 의 텍스쳐 
		pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		pd3dRootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		pd3dRootParameters[5].DescriptorTable.NumDescriptorRanges = 1;
		pd3dRootParameters[5].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[2]); // SkyBox의 텍스쳐
		pd3dRootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		//==================================================
		pd3dRootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		pd3dRootParameters[6].DescriptorTable.NumDescriptorRanges = 1;
		pd3dRootParameters[6].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[3]; // 터레인 텍스쳐/디테일/알파
		pd3dRootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		//==================================================

		pd3dRootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		pd3dRootParameters[7].Descriptor.ShaderRegister = 4; //Framework Info
		pd3dRootParameters[7].Descriptor.RegisterSpace = 0;
		pd3dRootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		pd3dRootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		pd3dRootParameters[8].Descriptor.ShaderRegister = 5; // Screen_Info
		pd3dRootParameters[8].Descriptor.RegisterSpace = 0;
		pd3dRootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		//==================================================
		pd3dRootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		pd3dRootParameters[9].DescriptorTable.NumDescriptorRanges = 1;
		pd3dRootParameters[9].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[4]; // 박스
		pd3dRootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		//==================================================
		pd3dRootParameters[10].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		pd3dRootParameters[10].Descriptor.ShaderRegister = 7; //Bounding Box
		pd3dRootParameters[10].Descriptor.RegisterSpace = 0;
		pd3dRootParameters[10].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		//==================================================
	}
	D3D12_STATIC_SAMPLER_DESC pd3dSamplerDescs[3];

	pd3dSamplerDescs[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].MipLODBias = 0;
	pd3dSamplerDescs[0].MaxAnisotropy = 1;
	pd3dSamplerDescs[0].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pd3dSamplerDescs[0].MinLOD = 0;
	pd3dSamplerDescs[0].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[0].ShaderRegister = 0;
	pd3dSamplerDescs[0].RegisterSpace = 0;
	pd3dSamplerDescs[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dSamplerDescs[1].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].MipLODBias = 0;
	pd3dSamplerDescs[1].MaxAnisotropy = 1;
	pd3dSamplerDescs[1].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pd3dSamplerDescs[1].MinLOD = 0;
	pd3dSamplerDescs[1].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[1].ShaderRegister = 1;
	pd3dSamplerDescs[1].RegisterSpace = 0;
	pd3dSamplerDescs[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dSamplerDescs[2].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[2].AddressU = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
	pd3dSamplerDescs[2].AddressV = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
	pd3dSamplerDescs[2].AddressW = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
	pd3dSamplerDescs[2].MipLODBias = 0;
	pd3dSamplerDescs[2].MaxAnisotropy = 1;
	pd3dSamplerDescs[2].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pd3dSamplerDescs[2].MinLOD = 0;
	pd3dSamplerDescs[2].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[2].ShaderRegister = 2;
	pd3dSamplerDescs[2].RegisterSpace = 0;
	pd3dSamplerDescs[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = _countof(pd3dSamplerDescs);
	d3dRootSignatureDesc.pStaticSamplers = pd3dSamplerDescs;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;
	HRESULT b = D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	if (FAILED(b))
	{
		if (pd3dErrorBlob)
		{
			OutputDebugStringA((char*)pd3dErrorBlob->GetBufferPointer());
			pd3dErrorBlob->Release();
		}
		return nullptr;
	}
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

void CScene::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	
}

void CScene::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{

}

void CScene::ReleaseShaderVariables()
{

}

void CScene::ReleaseUploadBuffers()
{
	if (m_pSkyBox) m_pSkyBox->ReleaseUploadBuffers();

	for (int i = 0; i < m_nShaders; i++) m_ppShaders[i]->ReleaseUploadBuffers();
	for (int i = 0; i < m_nGameObjects; i++) m_ppGameObjects[i]->ReleaseUploadBuffers();
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 'W': m_ppGameObjects[0]->MoveForward(+1.0f); break;
		case 'S': m_ppGameObjects[0]->MoveForward(-1.0f); break;
		case 'A': m_ppGameObjects[0]->MoveStrafe(-1.0f); break;
		case 'D': m_ppGameObjects[0]->MoveStrafe(+1.0f); break;
		case 'Q': m_ppGameObjects[0]->MoveUp(+1.0f); break;
		case 'R': m_ppGameObjects[0]->MoveUp(-1.0f); break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	return(false);
}

bool CScene::ProcessInput(UCHAR *pKeysBuffer)
{
	return(false);
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	for (int i = 0; i < m_nGameObjects; i++) if (m_ppGameObjects[i]) m_ppGameObjects[i]->Animate(fTimeElapsed, NULL);
	for (int i = 0; i < m_nGameObjects; i++) if (m_ppGameObjects[i]) m_ppGameObjects[i]->UpdateTransform(NULL);

	for (int i = 0; i < m_nShaders; i++) if (m_ppShaders[i]) m_ppShaders[i]->AnimateObjects(fTimeElapsed);
	
}

void CScene::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
}

void CScene::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
}


//=============================================================================================

Start_Scene::Start_Scene()
{
}

Start_Scene::~Start_Scene()
{
}

void Start_Scene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	m_pDescriptorHeap = new CDescriptorHeap();
	//현재 버튼 텍스쳐 개수 : 15개 // 지형 텍스쳐 : 3개 // 플라잉 박스 텍스쳐 : 1 // 소행성 텍스쳐 : 1
	CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 17 + 50 + 1 + 1 +15 + 3 + 1 + 1); // 다른 씬에서 쓸 SRV 개수까지 미리 생성 

	m_nShaders = 0;
	m_ppShaders = new CShader * [m_nShaders];


	screen_shader = new ScreenShader();
	screen_shader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	screen_shader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	screen_shader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, NULL);


	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void Start_Scene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();
	if (m_pDescriptorHeap) delete m_pDescriptorHeap;

	if (m_ppShaders)
	{
		for (int i = 0; i < m_nShaders; i++)
		{
			m_ppShaders[i]->ReleaseShaderVariables();
			m_ppShaders[i]->ReleaseObjects();
			m_ppShaders[i]->Release();
		}
		delete[] m_ppShaders;
	}

	if (screen_shader)
	{
		screen_shader->ReleaseShaderVariables();
		screen_shader->ReleaseObjects();
		screen_shader->Release();

	}

	if (m_pSkyBox) 
		delete m_pSkyBox;

	if (m_ppGameObjects)
	{
		for (int i = 0; i < m_nGameObjects; i++)
			if (m_ppGameObjects[i]) m_ppGameObjects[i]->Release();

		delete[] m_ppGameObjects;
	}

	ReleaseShaderVariables();

}

void Start_Scene::ReleaseUploadBuffers()
{
	if (m_pSkyBox) m_pSkyBox->ReleaseUploadBuffers();

	for (int i = 0; i < m_nShaders; i++) m_ppShaders[i]->ReleaseUploadBuffers();
	for (int i = 0; i < m_nGameObjects; i++) m_ppGameObjects[i]->ReleaseUploadBuffers();
}

bool Start_Scene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	bool isClicked = false;

	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	{
		if (selected_screen_info != "")
			break;

		int mouseX = LOWORD(lParam);
		int mouseY = HIWORD(lParam);

		int width = FRAME_BUFFER_WIDTH;
		int height = FRAME_BUFFER_HEIGHT;

		float screen_x = (static_cast<float>(mouseX) / (width / 2)) - 1.0f;
		float screen_y = 1.0f - (static_cast<float>(mouseY) / (height / 2));

		selected_screen_info = screen_shader->PickObjectPointedByCursor(screen_x, screen_y, NULL);
		if (selected_screen_info != "")
			DebugOutput(selected_screen_info);

		if (selected_screen_info == "menu_icon_button")
			screen_shader->pause_screen_ptr->active = !screen_shader->pause_screen_ptr->active;

		if (selected_screen_info == "option_button" || selected_screen_info == "option_icon_button")
		{
			screen_shader->option_screen_ptr->active = true;
			screen_shader->option_icon_button_ptr->active = false;
			screen_shader->info_icon_button_ptr->active = false;

		}
		if (selected_screen_info == "check_button" || selected_screen_info == "x_button")
		{
			screen_shader->option_screen_ptr->active = false;
			screen_shader->option_icon_button_ptr->active = true;
			screen_shader->info_icon_button_ptr->active = true;
		}

		if (selected_screen_info == "info_icon_button")
		{
			screen_shader->info_screen_ptr->active = !screen_shader->info_screen_ptr->active;
			screen_shader->option_icon_button_ptr->active = !screen_shader->info_screen_ptr->active;
		}
		if (selected_screen_info == "start_button")
			start_button_down = true;
	}
	break;

	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		::ReleaseCapture();
		selected_screen_info = "";
		break;

	case WM_MOUSEWHEEL:
	{
		// wParam에서 스크롤 방향 및 휠 회전 값을 추출
		short delta = GET_WHEEL_DELTA_WPARAM(wParam);

		if (delta > 0)// 위로 스크롤
		{
			scroll_value += 0.1f;
		}
		else if (delta < 0)// 아래로 스크롤
		{
			scroll_value -= 0.1f;
		}
		
	}
	break;

	case WM_MOUSEMOVE:
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
		case 'W': m_ppGameObjects[0]->MoveForward(+1.0f); break;
		case 'S': m_ppGameObjects[0]->MoveForward(-1.0f); break;
		case 'A': m_ppGameObjects[0]->MoveStrafe(-1.0f); break;
		case 'D': m_ppGameObjects[0]->MoveStrafe(+1.0f); break;
		case 'Q': m_ppGameObjects[0]->MoveUp(+1.0f); break;
		case 'R': m_ppGameObjects[0]->MoveUp(-1.0f); break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	return(false);
}

bool Start_Scene::ProcessInput(UCHAR* pKeysBuffer)
{
	return(false);
}

void Start_Scene::AnimateObjects(float fTimeElapsed)
{
	for (int i = 0; i < m_nGameObjects; i++) if (m_ppGameObjects[i]) 
		m_ppGameObjects[i]->Animate(fTimeElapsed, NULL);

	for (int i = 0; i < m_nGameObjects; i++) if (m_ppGameObjects[i]) 
		m_ppGameObjects[i]->UpdateTransform(NULL);

	for (int i = 0; i < m_nShaders; i++) if (m_ppShaders[i])
		m_ppShaders[i]->AnimateObjects(fTimeElapsed);

	if (screen_shader->Scroll_Update(fTimeElapsed, scroll_value) == false)
		scroll_value = 0.0f;

}

void Start_Scene::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (m_pd3dGraphicsRootSignature)
		pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	pd3dCommandList->SetDescriptorHeaps(1, &m_pDescriptorHeap->m_pd3dCbvSrvDescriptorHeap);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);
}

void Start_Scene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	pCamera = m_pPlayer->GetCamera();
	OnPrepareRender(pd3dCommandList, pCamera);

	UpdateShaderVariables(pd3dCommandList);


	for (int i = 0; i < m_nGameObjects; i++)
		if (m_ppGameObjects[i])
			m_ppGameObjects[i]->Render(pd3dCommandList, pCamera);

	for (int i = 0; i < m_nShaders; i++)
		if (m_ppShaders[i])
			m_ppShaders[i]->Render(pd3dCommandList, pCamera);


	if (m_pPlayer)
		m_pPlayer->Render(pd3dCommandList, pCamera);
	
	if (screen_shader)
		screen_shader->Render(pd3dCommandList, pCamera);
}

//=============================================================================================

Game_Scene::Game_Scene()
{
}

Game_Scene::~Game_Scene()
{
}

void Game_Scene::BuildDefaultLightsAndMaterials()
{
	m_nLights = 4;
	m_pLights = new LIGHT[m_nLights];
	::ZeroMemory(m_pLights, sizeof(LIGHT) * m_nLights);

	m_xmf4GlobalAmbient = XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f);

	m_pLights[0].m_bEnable = false;
	m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights[0].m_fRange = 1000.0f;
	m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f);
	m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_pLights[0].m_xmf4Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);
	m_pLights[0].m_xmf3Position = XMFLOAT3(30.0f, 30.0f, 30.0f);
	m_pLights[0].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);

	m_pLights[1].m_bEnable = true;
	m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights[1].m_fRange = 500.0f;
	m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.1f, 0.5f, 0.1f, 1.0f);
	m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.4f, 0.5f, 0.4f, 1.0f);
	m_pLights[1].m_xmf4Specular = XMFLOAT4(0.3f, 0.5f, 0.3f, 0.0f);
	m_pLights[1].m_xmf3Position = XMFLOAT3(-50.0f, 20.0f, -5.0f);
	m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights[1].m_fFalloff = 8.0f;
	m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(20.0f));

	m_pLights[2].m_bEnable = true;
	m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	m_pLights[2].m_xmf4Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 0.0f);
	m_pLights[2].m_xmf3Direction = XMFLOAT3(0.5f, -0.5f, 0.0f);

	m_pLights[3].m_bEnable = false;
	m_pLights[3].m_nType = SPOT_LIGHT;
	m_pLights[3].m_fRange = 600.0f;
	m_pLights[3].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights[3].m_xmf4Diffuse = XMFLOAT4(0.3f, 0.7f, 0.0f, 1.0f);
	m_pLights[3].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights[3].m_xmf3Position = XMFLOAT3(50.0f, 30.0f, 30.0f);
	m_pLights[3].m_xmf3Direction = XMFLOAT3(0.0f, 1.0f, 1.0f);
	m_pLights[3].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights[3].m_fFalloff = 8.0f;
	m_pLights[3].m_fPhi = (float)cos(XMConvertToRadians(90.0f));
	m_pLights[3].m_fTheta = (float)cos(XMConvertToRadians(30.0f));
}

void Game_Scene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	BuildDefaultLightsAndMaterials();
	
	{
		m_pSkyBox = new CSkyBox(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

		float scale_value = 3.0f;
		float height_scale = 0.3f;

		XMFLOAT3 xmf3Scale(scale_value, height_scale, scale_value);
		XMFLOAT4 xmf4Color(1.0f, 1.0f, 0.5f, 1.0f);

		m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("Hight_Map/HeightMap.raw"), 257, 257, 257, 257, xmf3Scale, xmf4Color);
	}
	m_nShaders = 4;
	m_ppShaders = new CShader * [m_nShaders];


	CObjectsShader::Object_map = new CRawFormatImage(L"Hight_Map/ObjectsMap.raw", 257, 257, true);
	CObjectsShader* pObjectsShader = new CObjectsShader();
	pObjectsShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	pObjectsShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, NULL);

	m_ppShaders[0] = pObjectsShader;
	
	BOX_Shader* pBoxShader = new BOX_Shader();
	pBoxShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	pBoxShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, m_pTerrain);

	m_ppShaders[1] = pBoxShader;

	Asteroid_Shader* pAsteroidShader = new Asteroid_Shader();
	pAsteroidShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	pAsteroidShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, m_pTerrain);

	m_ppShaders[2] = pAsteroidShader;

	//============================================================
	
	CBillboardObjectsShader* pBillboardShader = new CBillboardObjectsShader();
	pBillboardShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	pBillboardShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, m_pTerrain);

	m_ppShaders[3] = pBillboardShader;

	//============================================================

	OOBBShader* pOOBBShader = new OOBBShader();
	pOOBBShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);


	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void Game_Scene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();
	if (m_pDescriptorHeap) delete m_pDescriptorHeap;

	if (m_ppShaders)
	{
		for (int i = 0; i < m_nShaders; i++)
		{
			m_ppShaders[i]->ReleaseShaderVariables();
			m_ppShaders[i]->ReleaseObjects();
			m_ppShaders[i]->Release();
		}
		delete[] m_ppShaders;
	}

	if (m_pSkyBox) delete m_pSkyBox;
	if (m_pTerrain) delete m_pTerrain;

	if (m_ppGameObjects)
	{
		for (int i = 0; i < m_nGameObjects; i++) if (m_ppGameObjects[i]) m_ppGameObjects[i]->Release();
		delete[] m_ppGameObjects;
	}

	ReleaseShaderVariables();

	if (m_pLights) delete[] m_pLights;
}

void Game_Scene::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLights->Map(0, NULL, (void**)&m_pcbMappedLights);
}

void Game_Scene::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	::memcpy(m_pcbMappedLights->m_pLights, m_pLights, sizeof(LIGHT) * m_nLights);
	::memcpy(&m_pcbMappedLights->m_xmf4GlobalAmbient, &m_xmf4GlobalAmbient, sizeof(XMFLOAT4));
	::memcpy(&m_pcbMappedLights->m_nLights, &m_nLights, sizeof(int));
}

void Game_Scene::ReleaseShaderVariables()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}
}

void Game_Scene::ReleaseUploadBuffers()
{
	if (m_pSkyBox) m_pSkyBox->ReleaseUploadBuffers();
	if (m_pTerrain) m_pTerrain->ReleaseUploadBuffers();

	for (int i = 0; i < m_nShaders; i++) m_ppShaders[i]->ReleaseUploadBuffers();
	for (int i = 0; i < m_nGameObjects; i++) m_ppGameObjects[i]->ReleaseUploadBuffers();
}

bool Game_Scene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	bool isClicked = false;

	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	{
		if (selected_screen_info != "")
			break;

		int mouseX = LOWORD(lParam);
		int mouseY = HIWORD(lParam);

		int width = FRAME_BUFFER_WIDTH;
		int height = FRAME_BUFFER_HEIGHT;

		float screen_x = (static_cast<float>(mouseX) / (width / 2)) - 1.0f;
		float screen_y = 1.0f - (static_cast<float>(mouseY) / (height / 2));

		selected_screen_info = screen_shader->PickObjectPointedByCursor(screen_x, screen_y, NULL);
		if (selected_screen_info != "")
			DebugOutput(selected_screen_info);

		if (selected_screen_info == "menu_icon_button")
			screen_shader->pause_screen_ptr->active = !screen_shader->pause_screen_ptr->active;

		if (selected_screen_info == "option_button" || selected_screen_info == "option_icon_button")
		{
			screen_shader->option_screen_ptr->active = !screen_shader->option_screen_ptr->active;
			screen_shader->pause_screen_ptr->active = !screen_shader->option_screen_ptr->active;
		}

		if (selected_screen_info == "check_button" || selected_screen_info == "x_button")
		{
			screen_shader->option_screen_ptr->active = false;
		}
	}
	break;

	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		::ReleaseCapture();
		selected_screen_info = "";
		break;

	case WM_MOUSEWHEEL:
	{
		// wParam에서 스크롤 방향 및 휠 회전 값을 추출
		short delta = GET_WHEEL_DELTA_WPARAM(wParam);

		// delta 값이 양수이면 위로 스크롤, 음수이면 아래로 스크롤
		if (delta > 0)
		{
			// 위로 스크롤
			OutputDebugString(L"Mouse wheel scrolled up.\n");
		}
		else if (delta < 0)
		{
			// 아래로 스크롤
			OutputDebugString(L"Mouse wheel scrolled down.\n");
		}
	}
	break;

	case WM_MOUSEMOVE:
		break;

	default:
		break;
	}

	return(false);
}

bool Game_Scene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_TAB:
			CObjectsShader::Show_Collider = true;
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
	return(true);
}

bool Game_Scene::ProcessInput(UCHAR* pKeysBuffer)
{
	return(false);
}

void Game_Scene::AnimateObjects(float fTimeElapsed)
{
	for (int i = 0; i < m_nGameObjects; i++) if (m_ppGameObjects[i]) m_ppGameObjects[i]->Animate(fTimeElapsed, NULL);
	for (int i = 0; i < m_nGameObjects; i++) if (m_ppGameObjects[i]) m_ppGameObjects[i]->UpdateTransform(NULL);

	for (int i = 0; i < m_nShaders; i++) if (m_ppShaders[i]) m_ppShaders[i]->AnimateObjects(fTimeElapsed);

	if (m_pLights)
	{
		m_pLights[1].m_xmf3Position = m_pPlayer->GetPosition();
		m_pLights[1].m_xmf3Direction = m_pPlayer->GetLookVector();
	}
	Collision_Defender(m_pPlayer, (CObjectsShader*)m_ppShaders[1]);
}

void Game_Scene::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (m_pd3dGraphicsRootSignature)
		pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	pd3dCommandList->SetDescriptorHeaps(1, &m_pDescriptorHeap->m_pd3dCbvSrvDescriptorHeap);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);
}

void Game_Scene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	pCamera = m_pPlayer->GetCamera();
	OnPrepareRender(pd3dCommandList, pCamera);

	UpdateShaderVariables(pd3dCommandList);

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbLightsGpuVirtualAddress); //Lights

	if (m_pSkyBox) m_pSkyBox->Render(pd3dCommandList, pCamera);
	if (m_pTerrain) m_pTerrain->Render(pd3dCommandList, pCamera);

	for (int i = 0; i < m_nGameObjects; i++) 
		if (m_ppGameObjects[i]) 
			m_ppGameObjects[i]->Render(pd3dCommandList, pCamera);

	for (int i = 0; i < m_nShaders; i++) 
		if (m_ppShaders[i]) 
			m_ppShaders[i]->Render(pd3dCommandList, pCamera);


	if (m_pPlayer)
		m_pPlayer->Render(pd3dCommandList, m_pPlayer->GetCamera());


	if (screen_shader)
		screen_shader->Render(pd3dCommandList, pCamera);


}

void Game_Scene::Collision_Defender(CPlayer* player_ptr, CObjectsShader* object_shader)
{
	XMFLOAT3 now_speed = player_ptr->GetVelocity();
	if (Vector3::Length(now_speed) < 1.0f)
		return;

	BoundingOrientedBox player_OBB = BoundingOrientedBox(*player_ptr->GetCollider());
	BoundingOrientedBox* obj_OBB = NULL;
	for (int i = 0; i < object_shader->m_nObjects; ++i)
	{
		obj_OBB = object_shader->m_ppObjects[i]->GetCollider();
		if (player_OBB.Intersects(*obj_OBB))
		{
			XMFLOAT3 moving_vector = player_ptr->GetVelocity();
			XMVECTOR direction_vector = XMLoadFloat3(&moving_vector);
			direction_vector = XMVector3Normalize(direction_vector);


			// 광선의 시작점과 방향 설정
			XMVECTOR rayOrigin = XMLoadFloat3(&player_ptr->GetPosition()); // 플레이어의 현재 위치
			XMVECTOR rayDirection = direction_vector; // 정규화된 이동 방향


			// 가까운 충돌 거리 변수
			float f_distance = 0.0f;

			// 광선과 obj_OBB 간의 충돌 검사
			if (obj_OBB->Intersects(rayOrigin, direction_vector, f_distance))
			{
				XMFLOAT3 ray_start_pos, ray_dir, polygon_normal_vector;
				XMStoreFloat3(&ray_start_pos, rayOrigin);
				XMStoreFloat3(&ray_dir, rayDirection);

				// 메시와의 충돌 검사 및 폴리곤 법선 벡터 획득
				if (object_shader->m_ppObjects[i]->Check_Polygon_Ray_Normal(ray_start_pos, ray_dir, &polygon_normal_vector))
				{
					if (polygon_normal_vector.x != 0.0f || polygon_normal_vector.y != 0.0f || polygon_normal_vector.z != 0.0f)
					{
						// 벽의 법선 벡터 로드 및 정규화
						XMVECTOR wallNormal = XMLoadFloat3(&polygon_normal_vector);
						wallNormal = XMVector3Normalize(wallNormal); // 법선 벡터 정규화

						// 충돌 지점 계산: rayOrigin + rayDirection * 충돌 거리
						XMVECTOR collisionPoint = rayOrigin + rayDirection * f_distance;

						// 회피 거리 설정 (법선 방향으로 이동할 거리)
						float avoidDistance = 10.0f; // 예시 값, 필요한 경우 조정

						// 회피 지점(가상의 타겟) 계산: 충돌 지점 + (법선 벡터 * 회피 거리)
						XMVECTOR avoidTarget = collisionPoint + (wallNormal * avoidDistance);

						// 플레이어의 현재 위치 로드
						XMVECTOR playerPosition = XMLoadFloat3(&player_ptr->GetPosition());

						// 새로운 방향 벡터 계산: 플레이어 위치에서 회피 지점을 향한 방향
						XMVECTOR newDirection = XMVector3Normalize(avoidTarget - playerPosition);

						// 회피 지점으로 향하도록 이동 방향 설정 (Seek 알고리즘 적용)
						XMFLOAT3 player_new_velocity;
						XMStoreFloat3(&player_new_velocity, newDirection);
						player_ptr->SetVelocity(player_new_velocity);

						//// 실제 이동 적용 (새로운 속도에 따라)
						XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(player_new_velocity, Vector3::Length(now_speed), false);
						player_ptr->Move(xmf3Velocity, true);

					}
				}
			}
		}


	 }
}

