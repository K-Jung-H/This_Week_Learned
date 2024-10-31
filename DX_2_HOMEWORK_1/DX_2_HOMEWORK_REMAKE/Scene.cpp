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
	D3D12_ROOT_PARAMETER pd3dRootParameters[14];
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
		pd3dRootParameters[10].Descriptor.ShaderRegister = 6; //Bounding Box
		pd3dRootParameters[10].Descriptor.RegisterSpace = 0;
		pd3dRootParameters[10].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		//==================================================
		pd3dRootParameters[11].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		pd3dRootParameters[11].Constants.ShaderRegister = 7;  // outline_color
		pd3dRootParameters[11].Constants.RegisterSpace = 0; 
		pd3dRootParameters[11].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		//==================================================
		pd3dRootParameters[12].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		pd3dRootParameters[12].Constants.Num32BitValues = 8;
		pd3dRootParameters[12].Constants.ShaderRegister = 8;  // black_hole_wave
		pd3dRootParameters[12].Constants.RegisterSpace = 0;
		pd3dRootParameters[12].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		//==================================================
		pd3dRootParameters[13].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		pd3dRootParameters[13].Constants.Num32BitValues = 2;
		pd3dRootParameters[13].Constants.ShaderRegister = 9;  // sprite_index
		pd3dRootParameters[13].Constants.RegisterSpace = 0;
		pd3dRootParameters[13].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
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
	b = pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&pd3dGraphicsRootSignature);
	if (FAILED(b))
	{
		if (pd3dErrorBlob)
		{
			OutputDebugStringA((char*)pd3dErrorBlob->GetBufferPointer());
			pd3dErrorBlob->Release();
		}
	}
	
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

void  CScene::Check_Click_Button(string button_info)
{

	if (button_info == "option_player_speed_minus" || button_info == "option_player_speed_plus")
	{
		screen_shader->option_player_speed_ptr->Text_Scale = 1.5f;

		if (button_info == "option_player_speed_minus")
			screen_shader->option_player_speed_ptr->Change_Text(-1);

		if (button_info == "option_player_speed_plus")
			screen_shader->option_player_speed_ptr->Change_Text(+1);
	}

	if (button_info == "option_stone_speed_minus" || button_info == "option_stone_speed_plus")
	{
		screen_shader->option_stone_speed_ptr->Text_Scale = 1.5f;

		if (button_info == "option_stone_speed_minus")
			screen_shader->option_stone_speed_ptr->Change_Text(-1);

		if (button_info == "option_stone_speed_plus")
			screen_shader->option_stone_speed_ptr->Change_Text(+1);
	}

	if (button_info == "option_difficulty_minus" || button_info == "option_difficulty_plus")
	{
		screen_shader->option_difficulty_ptr->Text_Scale = 1.5f;

		if (button_info == "option_difficulty_minus")
			screen_shader->option_difficulty_ptr->Change_Text(-1);

		if (button_info == "option_difficulty_plus")
			screen_shader->option_difficulty_ptr->Change_Text(+1);
	}

	if (button_info == "check_button" || button_info == "x_button")
	{
		if (button_info == "check_button")
		{
			PLAYER_SPEED_VALUE = screen_shader->option_player_speed_ptr->Text_Index + 1;
			STONE_SPEED_VALUE = screen_shader->option_stone_speed_ptr->Text_Index + 1;
			DIFFICULTY_VALUE = screen_shader->option_difficulty_ptr->Text_Index + 1;
		}
		else if (button_info == "x_button")
		{
			screen_shader->option_player_speed_ptr->Text_Index = PLAYER_SPEED_VALUE - 1;
			screen_shader->option_stone_speed_ptr->Text_Index = STONE_SPEED_VALUE - 1;
			screen_shader->option_difficulty_ptr->Text_Index = DIFFICULTY_VALUE - 1;
		}
	}

}

void CScene::AnimateObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed)
{
	for (int i = 0; i < m_nGameObjects; i++) if (m_ppGameObjects[i]) m_ppGameObjects[i]->Animate(fTimeElapsed, NULL);
	for (int i = 0; i < m_nGameObjects; i++) if (m_ppGameObjects[i]) m_ppGameObjects[i]->UpdateTransform(NULL);

	for (int i = 0; i < m_nShaders; i++) if (m_ppShaders[i]) m_ppShaders[i]->AnimateObjects(fTimeElapsed);
	
}

void CScene::Scene_Update()
{
}

void CScene::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
}

void CScene::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
}

void CScene::Message_Render(ID2D1DeviceContext2* pd2dDevicecontext)
{
}

void CScene::Reset()
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
	//현재 버튼 텍스쳐 개수 : 15개 // 지형 텍스쳐 : 3개 // 플라잉 박스 텍스쳐 : 1 // 소행성 텍스쳐 : 1 // 블랙홀 텍스쳐 : 1// 폭발 스프라이트 : 1
	CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 17 + 50 + 1 + 1 +15 + 3 + 1 + 1 + 1); // 다른 씬에서 쓸 SRV 개수까지 미리 생성 

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
		{
			DebugOutput(selected_screen_info);
			Check_Click_Button(selected_screen_info);
		}

	}
	break;

	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		selected_screen_info = "";
		break;

	case WM_MOUSEWHEEL:
	{
		// wParam에서 스크롤 방향 및 휠 회전 값을 추출
		short delta = GET_WHEEL_DELTA_WPARAM(wParam);

		if (delta > 0)// 위로 스크롤
		{
			scroll_value -= 0.1f;
		}
		else if (delta < 0)// 아래로 스크롤
		{
			scroll_value += 0.1f;
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
		case VK_ESCAPE:
			::PostQuitMessage(0);
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

void Start_Scene::Check_Click_Button(string button_info)
{
	if (button_info == "start_button")
		start_button_down = true;

	if (button_info == "menu_icon_button")
		screen_shader->pause_screen_ptr->active = !screen_shader->pause_screen_ptr->active;

	if (button_info == "option_button" || button_info == "option_icon_button")
	{
		screen_shader->option_screen_ptr->active = true;
		screen_shader->start_button_ptr->active = false;
		screen_shader->option_icon_button_ptr->active = false;
		screen_shader->info_icon_button_ptr->active = false;
	}

	if (button_info == "check_button" || button_info == "x_button")
	{
		screen_shader->option_screen_ptr->active = false;
		screen_shader->option_icon_button_ptr->active = true;
		screen_shader->info_icon_button_ptr->active = true;
		screen_shader->start_button_ptr->active = true;
	}

	if (button_info == "info_icon_button")
	{
		screen_shader->info_screen_ptr->active = !screen_shader->info_screen_ptr->active;
		screen_shader->option_icon_button_ptr->active = !screen_shader->info_screen_ptr->active;
		if (screen_shader->info_screen_ptr->active)
		{
			scroll_value = -1.0f;
			screen_shader->info_screen_ptr->scroll_value = -1.0f;
		}
	}


	CScene::Check_Click_Button(button_info);

}


void Start_Scene::AnimateObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed)
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

void Start_Scene::Message_Render(ID2D1DeviceContext2* pd2dDevicecontext)
{
	if (screen_shader)
	{
		Screen_Rect** Text_Objects = (Screen_Rect**)screen_shader->screen_Objects;
		int Obj_N = screen_shader->m_nObjects;

		for (int i = 0; i < Obj_N; i++)
		{
			if (Text_Objects[i]->active == false)
				continue;

			Text_Objects[i]->Message_Render(pd2dDevicecontext, write_font_list[0], brush_list[0]);
		}
	}
	


	if (screen_shader->option_screen_ptr->active == true)
	{
		wstring text_player_speed = _T("Player speed");
		wstring text_stone_speed = _T("Stone speed");
		wstring text_difficulty = _T("Difficulty");
		D2D1_RECT_F text_player_speed_area = { 200, 120, 600, 130 };
		D2D1_RECT_F text_stone_speed_area = { 200, 240, 600, 250 };
		D2D1_RECT_F text_difficulty_speed_area = { 200, 360, 600, 370 };
		pd2dDevicecontext->DrawTextW(text_player_speed.c_str(), (UINT32)wcslen(text_player_speed.c_str()), write_font_list[1], &text_player_speed_area, brush_list[0]);
		pd2dDevicecontext->DrawTextW(text_stone_speed.c_str(), (UINT32)wcslen(text_stone_speed.c_str()), write_font_list[1], &text_stone_speed_area, brush_list[0]);
		pd2dDevicecontext->DrawTextW(text_difficulty.c_str(), (UINT32)wcslen(text_difficulty.c_str()), write_font_list[1], &text_difficulty_speed_area, brush_list[0]);
	}

}

void Start_Scene::Reset()
{
	start_button_down = false;
	scroll_value = 0;
	m_pPlayer->SetPosition(XMFLOAT3(100.0f, 250.0f, 500.0f));
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

		float scale_value = 5.0f;
		float height_scale = 5.0f;

		XMFLOAT3 xmf3Scale(scale_value, height_scale, scale_value);
		XMFLOAT4 xmf4Color(1.0f, 1.0f, 0.5f, 1.0f);
		m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("Hight_Map/handmade_moon_surface.raw"), 513, 513, 513, 513, xmf3Scale, xmf4Color);
	}



	//============================================================
	m_nShaders = 5;
	m_ppShaders = new CShader * [m_nShaders];


	CObjectsShader::Object_map = new CRawFormatImage(L"Hight_Map/ObjectsMap.raw", 257, 257, true);
	CObjectsShader* pObjectsShader = new CObjectsShader();
	pObjectsShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	pObjectsShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, NULL);

	
	
	BOX_Shader* pBoxShader = new BOX_Shader();
	pBoxShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	pBoxShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, m_pTerrain);

	box_shader = pBoxShader;

	//============================================================
	
	Black_Hole_Shader* pBlack_Hole_Shader = new Black_Hole_Shader();
	pBlack_Hole_Shader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	pBlack_Hole_Shader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, m_pTerrain);

	black_hole_shader = pBlack_Hole_Shader;

	//============================================================

	Sprite_Billboard_Shader* pSprite_Billboard_Shader = new Sprite_Billboard_Shader();
	pSprite_Billboard_Shader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	pSprite_Billboard_Shader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, m_pTerrain);

	effect_shader = pSprite_Billboard_Shader;

	//============================================================
		
	Diffuse_Shader* diffuse_shader = new Diffuse_Shader();
	diffuse_shader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	diffuse_shader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, NULL);

	bullet_shader = diffuse_shader;

	//============================================================
	
	OOBBShader* pOOBBShader = new OOBBShader();
	pOOBBShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	Outline_Shader* outline_shader = new Outline_Shader();
	outline_shader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	Asteroid_Shader* pAsteroidShader = new Asteroid_Shader();
	pAsteroidShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	pAsteroidShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, m_pTerrain);
	pAsteroidShader->Set_Outline_Shader(outline_shader);

	enemy_shader = pAsteroidShader;

	//============================================================
	m_ppShaders[0] = pObjectsShader;
	m_ppShaders[1] = pBoxShader;
	m_ppShaders[2] = pBlack_Hole_Shader;
	m_ppShaders[3] = pSprite_Billboard_Shader;
	m_ppShaders[4] = diffuse_shader;
	//============================================================
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
		for (int i = 0; i < m_nGameObjects; i++) 
			if (m_ppGameObjects[i]) m_ppGameObjects[i]->Release();
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
		{
			DebugOutput(selected_screen_info);
			Check_Click_Button(selected_screen_info);
		}
	}
	break;

	case WM_LBUTTONUP:
	case WM_RBUTTONUP:

		selected_screen_info = "";
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
		case '1':
			CObjectsShader::Show_Collider = !CObjectsShader::Show_Collider;
			if (CObjectsShader::Show_Collider)
			{
				CObjectsShader::Show_Attack_Collider = false;
				CObjectsShader::Show_ETC_Collider = false;
			}
			break;

		case '2':
			CObjectsShader::Show_Attack_Collider = !CObjectsShader::Show_Attack_Collider;
			if (CObjectsShader::Show_Attack_Collider)
			{
				CObjectsShader::Show_Collider = false;
				CObjectsShader::Show_ETC_Collider = false;
			}
			break;
		case '3':
			CObjectsShader::Show_ETC_Collider = !CObjectsShader::Show_ETC_Collider;
			if (CObjectsShader::Show_ETC_Collider)
			{
				CObjectsShader::Show_Attack_Collider = false;
				CObjectsShader::Show_Collider = false;
			}
			break;

		case VK_SPACE:
			if (m_pPlayer->skill_charged && m_pPlayer->skill_cool_down)
			{
				if (bullet_shader->m_ppObjects[0]->active == false)
				{
					bullet_shader->m_ppObjects[0]->active = true;
					bullet_shader->m_ppObjects[0]->SetPosition(m_pPlayer->GetPosition());
					bullet_shader->m_ppObjects[0]->SetMovingDirection(m_pPlayer->GetLookVector());
					bullet_shader->m_ppObjects[0]->SetMovingSpeed(300.0f);
					Update_Power_Bar(-100.0f);
				}
			}
			break;

		case 'z':
		case 'Z':
			Update_Power_Bar(10.0f);
			break;

		case 'x':
		case 'X':
			Update_Time_Bar(10.0f);
			break;

		case 'c':
		case 'C':
			Update_Time_Bar(-10.0f);
			break;

		case VK_OEM_PLUS:
			m_pPlayer->Life += 1;
			break;

		case VK_OEM_MINUS:
			m_pPlayer->Life -= 1;
			break;

		case VK_TAB:
			Pause_Mode = !Pause_Mode;
			break;

		case VK_ESCAPE:
			Check_Click_Button("menu_icon_button");
			break;

		default:
			break;
		}
		break;

	case WM_KEYUP:
		switch (wParam)
		{
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

void  Game_Scene::Check_Click_Button(string button_info)
{
	
	if (button_info == "menu_icon_button")
	{
		screen_shader->pause_screen_ptr->active = !screen_shader->pause_screen_ptr->active;
		screen_shader->menu_button_ptr->active = false;
		Pause_Mode = true;
	}

	if (button_info == "option_button" || button_info == "option_icon_button")
	{
		screen_shader->option_screen_ptr->active = !screen_shader->option_screen_ptr->active;
		screen_shader->pause_screen_ptr->active = !screen_shader->option_screen_ptr->active;
	}


	if (button_info == "check_button" || button_info == "x_button")
	{
		screen_shader->option_screen_ptr->active = false;
		screen_shader->pause_screen_ptr->active = true;
	}

	if (button_info == "continue_button")
	{
		screen_shader->pause_screen_ptr->active = false;
		screen_shader->menu_button_ptr->active = true;
		Pause_Mode = false;
	}

	if (button_info == "exit_button")
	{
		screen_shader->pause_screen_ptr->active = false;
		screen_shader->menu_button_ptr->active = true;
		Pause_Mode = false;
		Reset_Signal = true;
	}
	CScene::Check_Click_Button(button_info);
}

void Game_Scene::AnimateObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed)
{
	m_fElapsedTime += fTimeElapsed;
	time_to_score += fTimeElapsed;

	if (time_to_score > 1.0f)
	{
		time_to_score = 0.0f;
		game_score += 10 * DIFFICULTY_VALUE;
		if (black_hole_shader->Get_Player_Black_Hole()->active == false)
			Update_Power_Bar(1.0f);
	}

	for (int i = 0; i < m_nGameObjects; i++)
	{
		if (m_ppGameObjects[i])
		{
			m_ppGameObjects[i]->Animate(fTimeElapsed, NULL);
			m_ppGameObjects[i]->UpdateTransform(NULL);
		}
	}
			

	for (int i = 0; i < m_nShaders; i++) 
		if (m_ppShaders[i]) 
			m_ppShaders[i]->AnimateObjects(fTimeElapsed);

	
	enemy_shader->AnimateObjects(fTimeElapsed);
	
	
	if (m_pLights)
	{
		m_pLights[1].m_xmf3Position = m_pPlayer->GetPosition();
		m_pLights[1].m_xmf3Direction = m_pPlayer->GetLookVector();
	}

	if (black_hole_shader->Get_Player_Black_Hole()->active)
	{
		gravity_hole_runtime += fTimeElapsed;
		if (gravity_hole_runtime >= 1.0f)
		{
			gravity_hole_runtime = 0.0f;
			Update_Time_Bar(-10.0f);
		}
	}

	Scene_Update(pd3dDevice, pd3dCommandList);
}

void Game_Scene::Scene_Update(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	Collision_Defender(m_pPlayer, (CObjectsShader*)box_shader);
	Check_Enemy_Collision();
	Check_Enemy_Black_Hole();
	Check_Enemy_Overlap();
	Check_Bullet_Collision();
	Update_Power_Bar();
	Update_Time_Bar();

	if (m_pPlayer->Life <= 0)
	{
		Reset_Signal = true;
	}

	if (m_fElapsedTime > 3.0f)
	{
		m_fElapsedTime = 0.0f;
		for(int i = 0; i < DIFFICULTY_VALUE; ++i)
			enemy_shader->Add_Object(pd3dDevice, pd3dCommandList, XMFLOAT3(m_pTerrain->GetWidth() / 2, 100.0f, m_pTerrain->GetLength() / 2), m_pPlayer);
	}
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
	
	//======================================
	for (CGameObject* obj_ptr : enemy_shader->m_ppObjects)
	{
		if (obj_ptr && obj_ptr->active)
		{
			obj_ptr->UpdateTransform(NULL);
			obj_ptr->Render(pd3dCommandList, pCamera);
		}
	}

	if (Show_Collider)
	{
		for (CGameObject* obj_ptr : enemy_shader->m_ppObjects)
			obj_ptr->Collider_Render(pd3dCommandList, pCamera);
	}
	//======================================
	for (int i = 0; i < m_nGameObjects; i++) 
		if (m_ppGameObjects[i]) 
			m_ppGameObjects[i]->Render(pd3dCommandList, pCamera);

	for (int i = 0; i < m_nShaders; i++) 
		if (m_ppShaders[i]) 
			m_ppShaders[i]->Render(pd3dCommandList, pCamera);


	// 블랙홀 충전량, 블랙홀 유지 시간 UI
	screen_shader->Bar_Render(pd3dCommandList, pCamera);

	if (m_pPlayer)
	{
		m_pPlayer->Render(pd3dCommandList, m_pPlayer->GetCamera());

		OOBB_Drawer* player_oobb_drawer = m_pPlayer->oobb_drawer;
		player_oobb_drawer->oobb_shader->Render(pd3dCommandList, pCamera);
		if (CObjectsShader::Show_Collider)
		{
			if (m_pPlayer->GetCollider())
			{
				player_oobb_drawer->Render(pd3dCommandList, pCamera);
				player_oobb_drawer->UpdateOOBB_Data(pd3dCommandList, m_pPlayer, XMFLOAT4(0.0f,1.0f,0.0f,1.0f));
				player_oobb_drawer->Render(pd3dCommandList, pCamera);
			}
		}
		
		if (CObjectsShader::Show_Attack_Collider)
			if (m_pPlayer->Get_Light_Collider())
			{
				player_oobb_drawer->Render(pd3dCommandList, pCamera);
				player_oobb_drawer->UpdateOOBB_Data(pd3dCommandList, &m_pPlayer->m_xmf4x4World, m_pPlayer->Get_Light_Collider(), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
				player_oobb_drawer->Render(pd3dCommandList, pCamera);
			}

		if(CObjectsShader::Show_ETC_Collider)
			if (m_pPlayer->Get_Navi_Collider())
			{
				player_oobb_drawer->Render(pd3dCommandList, pCamera);
				player_oobb_drawer->UpdateOOBB_Data(pd3dCommandList, &m_pPlayer->m_xmf4x4World, m_pPlayer->Get_Navi_Collider(), XMFLOAT4(1.0f, 0.5f, 0.0f, 1.0f));
				player_oobb_drawer->Render(pd3dCommandList, pCamera);
			}
	}
	if (screen_shader)
		screen_shader->Render(pd3dCommandList, pCamera);


}

void Game_Scene::Message_Render(ID2D1DeviceContext2* pd2dDevicecontext)
{
	if (screen_shader)
	{
		Screen_Rect** Text_Objects = (Screen_Rect**)screen_shader->screen_Objects;
		int Obj_N = screen_shader->m_nObjects;

		for (int i = 0; i < Obj_N; i++)
		{
			if (Text_Objects[i]->active == false)
				continue;

			Text_Objects[i]->Message_Render(pd2dDevicecontext, write_font_list[0], brush_list[0]);
		}

		if (screen_shader->option_screen_ptr->active == true)
		{
			wstring text_player_speed = _T("Player speed");
			wstring text_stone_speed = _T("Stone speed");
			wstring text_difficulty = _T("Difficulty");
			D2D1_RECT_F text_player_speed_area = { 200, 120, 600, 130 };
			D2D1_RECT_F text_stone_speed_area = { 200, 240, 600, 250 };
			D2D1_RECT_F text_difficulty_speed_area = { 200, 360, 600, 370 };
			pd2dDevicecontext->DrawTextW(text_player_speed.c_str(), (UINT32)wcslen(text_player_speed.c_str()), write_font_list[1], &text_player_speed_area, brush_list[0]);
			pd2dDevicecontext->DrawTextW(text_stone_speed.c_str(), (UINT32)wcslen(text_stone_speed.c_str()), write_font_list[1], &text_stone_speed_area, brush_list[0]);
			pd2dDevicecontext->DrawTextW(text_difficulty.c_str(), (UINT32)wcslen(text_difficulty.c_str()), write_font_list[1], &text_difficulty_speed_area, brush_list[0]);
		}
		else if(screen_shader->pause_screen_ptr->active == false)
		{
			// 게임 화면인 경우

			wstring text_HP = _T("HP: ") + std::to_wstring(m_pPlayer->Life);
			wstring text_Black_Hole = _T("Skill Status: "); 
			if (m_pPlayer->skill_charged)
				text_Black_Hole += _T("Charged");
			else
				text_Black_Hole += _T("Not Prepared");

			wstring text_Score = _T("Score: ") + std::to_wstring(game_score);
			D2D1_RECT_F text_HP_area = { 0, 0, 300, 30 };
			D2D1_RECT_F text_Black_Hole_area = { 0, 30, 300, 60 };
			D2D1_RECT_F text_Score_area = { 0, 60, 300, 90 };
			pd2dDevicecontext->DrawTextW(text_HP.c_str(), (UINT32)wcslen(text_HP.c_str()), write_font_list[2], &text_HP_area, brush_list[1]);
			pd2dDevicecontext->DrawTextW(text_Black_Hole.c_str(), (UINT32)wcslen(text_Black_Hole.c_str()), write_font_list[2], &text_Black_Hole_area, brush_list[1]);
			pd2dDevicecontext->DrawTextW(text_Score.c_str(), (UINT32)wcslen(text_Score.c_str()), write_font_list[2], &text_Score_area, brush_list[1]);


			return;
		}
	}
	

}


void Game_Scene::Check_Bullet_Collision()
{
	Bullet_Object* bullet_ptr = (Bullet_Object*)bullet_shader->m_ppObjects[0];
	CGameObject* gravity_hole_obj = black_hole_shader->m_ppObjects[1];

	XMFLOAT3 bullet_pos = { 0.0f,0.0f,0.0f };
	if (bullet_ptr->active)
		bullet_pos = bullet_ptr->GetPosition();
	else
		return;

	if (m_pTerrain)
	{
		float terrain_y = m_pTerrain->GetHeight(bullet_pos.x, bullet_pos.z);

		if (abs(bullet_pos.y - terrain_y) < 5.0f)
		{
			gravity_hole_obj->active = true;
			bullet_ptr->active = false;

			bullet_pos.y += 5.0f;
			gravity_hole_obj->SetPosition(bullet_pos);
			Update_Time_Bar(100.0f);
		}
	}

	for (CGameObject* box_ptr : box_shader->m_ppObjects)
	{
		if (!box_ptr->is_render) 
			continue;

		BoundingOrientedBox* box_obb = box_ptr->GetCollider();
		if (bullet_ptr->GetCollider()->Intersects(*box_obb))
		{
			bullet_ptr->active = false;
			gravity_hole_obj->active = true;
			gravity_hole_obj->SetPosition(bullet_pos);
			Update_Time_Bar(100.0f);
			break;
		}
	}

	// 총알의 생존 시간 만들어서, 3초 이상 살아있으면 그자리에 생성하기
	// 총알 속도 빠르게 하기
	if (bullet_ptr->active_time > 3.0f)
	{
		bullet_ptr->active = false;
		bullet_ptr->active_time = false;

		gravity_hole_obj->active = true;
		gravity_hole_obj->SetPosition(bullet_pos);
		Update_Time_Bar(100.0f);
	}
}

void Game_Scene::Collision_Defender(CPlayer* player_ptr, CObjectsShader* object_shader)
{
	XMFLOAT3 now_speed = player_ptr->GetVelocity();
	if (Vector3::Length(now_speed) < 0.5f)
		return;  // 속도가 매우 느리면 충돌 검사 중단

	BoundingOrientedBox player_OBB = BoundingOrientedBox(*player_ptr->Get_Navi_Collider());
	BoundingOrientedBox* obj_OBB = NULL;

	for (int i = 0; i < object_shader->m_ppObjects.size(); ++i)
	{
		obj_OBB = object_shader->m_ppObjects[i]->GetCollider();
		if (player_OBB.Intersects(*obj_OBB))
		{
			XMFLOAT3 moving_vector = player_ptr->GetVelocity();
			XMVECTOR direction_vector = XMLoadFloat3(&moving_vector);
			direction_vector = XMVector3Normalize(direction_vector); // 현재 이동 방향 정규화

			// 광선의 시작점과 방향 설정
			XMVECTOR rayOrigin = XMLoadFloat3(&player_ptr->GetPosition()); // 플레이어의 현재 위치
			XMVECTOR rayDirection = direction_vector; // 정규화된 이동 방향


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

						// 회피 지점 계산 = 충돌 지점 + (법선 벡터 * 회피 거리)
						float avoidDistance = 20.0f; // 회피할 거리 
						XMVECTOR collisionPoint = rayOrigin + rayDirection * f_distance;
						XMVECTOR avoidTarget = collisionPoint + (wallNormal * avoidDistance);


						XMVECTOR playerPosition = XMLoadFloat3(&player_ptr->GetPosition());

						XMVECTOR newDirection = XMVector3Normalize(avoidTarget - playerPosition);

						XMVECTOR currentLook = XMLoadFloat3(&player_ptr->GetLook());

						// 두 벡터 사이의 회전 방향 계산 (외적 사용)
						XMVECTOR crossProduct = XMVector3Cross(currentLook, newDirection);
						float crossY = XMVectorGetY(crossProduct); // Y축 회전 기준 (좌/우)
						float crossX = XMVectorGetX(crossProduct); // X축 회전 기준 (상/하)

						// 회전 적용 (상/하 또는 좌/우로 결정)
						if (fabs(crossY) > 0.01f) // 좌/우 회전
						{
							float rotateAmountY = XMConvertToDegrees(crossY) * 0.1f; // 회전할 각도 조정
							player_ptr->Rotate(0.0f, rotateAmountY, 0.0f); // Y축 회전 (좌/우)
						}
						else if (fabs(crossX) > 0.01f) // 상/하 회전
						{
							float rotateAmountX = XMConvertToDegrees(crossX) * 0.1f; // 회전할 각도 조정
							player_ptr->Rotate(rotateAmountX, 0.0f, 0.0f); // X축 회전 (상/하)
						}

						// 회피 지점으로 향하는 방향으로 속도 설정
						XMFLOAT3 player_new_velocity;
						XMStoreFloat3(&player_new_velocity, newDirection);
						player_ptr->SetVelocity(player_new_velocity);

						// 새롭게 계산된 속도에 따라 이동 적용
						XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(player_new_velocity, Vector3::Length(now_speed), false);
						player_ptr->Move(xmf3Velocity, true);
					}
				}
			}
		}
	}
}


void Game_Scene::Check_Enemy_Collision()
{
	BoundingOrientedBox* player_light_obb = m_pPlayer->Get_Light_Collider();
	BoundingOrientedBox* player_body_obb = m_pPlayer->GetCollider();
	BoundingOrientedBox* bullet_obb = NULL;
	
	if (bullet_shader->m_ppObjects[0]->active)
		bullet_obb = bullet_shader->m_ppObjects[0]->GetCollider();

	for (CGameObject* asteroid_ptr : enemy_shader->m_ppObjects)
	{
		if (!asteroid_ptr->active) 
			continue;

		BoundingOrientedBox* asteroid_obb = asteroid_ptr->GetCollider();
		bool isDestroyed = false;

		// Wall 충돌 검사
		for (CGameObject* box_ptr : box_shader->m_ppObjects)
		{
			if (!box_ptr->is_render) 
				continue;

			BoundingOrientedBox* box_obb = box_ptr->GetCollider();
			if (asteroid_obb->Intersects(*box_obb))
			{
				isDestroyed = true;
				break;
			}
		}

		if (isDestroyed)
		{
			asteroid_ptr->active = false;
			Add_Boom_Effect(asteroid_ptr->GetPosition());
			DebugOutput("Boom!");
			continue; // 다음 asteroid로 넘어감
		}

		// Player light 충돌 검사
		if (asteroid_obb->Intersects(*player_light_obb))
		{
			auto asteroid = static_cast<Asteroid*>(asteroid_ptr);
			if (asteroid->life > 0)
			{
				asteroid->life -= 1;
			}
			else
			{
				asteroid_ptr->active = false;
				Update_Power_Bar(10.0f);
				Add_Boom_Effect(asteroid_ptr->GetPosition(), true);
				DebugOutput("Boom!");
			}
			continue; // 다음 asteroid로 넘어감
		}

		// Player body 충돌 검사
		if (asteroid_obb->Intersects(*player_body_obb))
		{
			asteroid_ptr->active = false;
			Add_Boom_Effect(asteroid_ptr->GetPosition(), false);
			DebugOutput("Boom!");
			continue; // 다음 asteroid로 넘어감
		}
		
		if (bullet_obb != NULL)
		{
			if (asteroid_obb->Intersects(*bullet_obb))
			{
				asteroid_ptr->active = false;
				bullet_shader->m_ppObjects[0]->active = false;
				Add_Boom_Effect(asteroid_ptr->GetPosition(), true);
				DebugOutput("Boom!");
			}
		}
	}
}

void Game_Scene::Check_Enemy_Black_Hole()
{
	if (black_hole_shader->Get_Player_Black_Hole()->active == false)
		return;
	
	Black_Hole_Object* Gravity_black_hole = black_hole_shader->Get_Player_Black_Hole();
	BoundingOrientedBox* Gravity_black_hole_obb = Gravity_black_hole->Get_Gravity_Collider();

	for (CGameObject* asteroid_ptr : enemy_shader->m_ppObjects)
	{
		if (asteroid_ptr->active == false)
			continue;

		auto asteroid = static_cast<Asteroid*>(asteroid_ptr);

		BoundingOrientedBox* asteroid_obb = asteroid->GetCollider();

		if (asteroid_obb->Intersects(*Gravity_black_hole_obb))
		{
			// 점점 빨려 들어가기
			asteroid->SetTarget(Gravity_black_hole);
			
			float new_speed = asteroid->GetMovingSpeed() * 1.2f;
			asteroid->SetMovingSpeed(new_speed);

			asteroid->In_Gravity = true;

			// 블랙홀 중심에 도달하면
			if (asteroid_obb->Intersects(*Gravity_black_hole->GetCollider()))
			{
				asteroid_ptr->active = false;
				Add_Boom_Effect(asteroid_ptr->GetPosition(), true);
				DebugOutput("Boom!");
			}
		}
		else
		{
			asteroid->SetTarget(m_pPlayer);
			asteroid->In_Gravity = false;
		}
	}
}

void Game_Scene::Check_Enemy_Overlap() {
	std::vector<CollisionInfo> collisions;

	// 충돌 체크
	for (CGameObject* asteroid_ptr : enemy_shader->m_ppObjects) {
		if (!asteroid_ptr->active)
			continue;

		BoundingOrientedBox* asteroid_obb = asteroid_ptr->GetCollider();

		for (CGameObject* other_asteroid_ptr : enemy_shader->m_ppObjects) {
			if (other_asteroid_ptr == asteroid_ptr || !other_asteroid_ptr->active)
				continue;

			BoundingOrientedBox* other_asteroid_obb = other_asteroid_ptr->GetCollider();

			// 두 OBB가 겹치는지 확인
			if (asteroid_obb->Intersects(*other_asteroid_obb)) {
				// 충돌 정보를 저장
				collisions.push_back({ asteroid_ptr, other_asteroid_ptr });
			}
		}
	}

	// 충돌 처리
	for (const CollisionInfo& collision : collisions) {
		CGameObject* objA = collision.collided_obj_A;
		CGameObject* objB = collision.collided_obj_B;

		// 객체 A와 B의 진행 방향 가져오기
		XMFLOAT3 dirA = objA->GetMovingDirection();
		XMFLOAT3 dirB = objB->GetMovingDirection();

		// 진행 방향을 반대 방향으로 설정하여 충돌을 피하게 함
		XMFLOAT3 newDirA = { -dirA.x, -dirA.y, -dirA.z };
		XMFLOAT3 newDirB = { -dirB.x, -dirB.y, -dirB.z };

		// 새로운 방향 설정
		objA->SetMovingDirection(newDirA);
		objB->SetMovingDirection(newDirB);

		// Optional: 두 객체의 위치를 살짝 변경하여 중첩되지 않도록 설정 (필요한 경우)
		float offset = 0.1f; // 간격을 조정하여 객체가 겹치지 않도록 함
		XMFLOAT3 posA = objA->GetPosition();
		XMFLOAT3 posB = objB->GetPosition();
		XMFLOAT3 separationVector = { posB.x - posA.x, posB.y - posA.y, posB.z - posA.z };
		XMVECTOR separation = XMLoadFloat3(&separationVector);
		separation = XMVector3Normalize(separation);

		// 두 객체를 반대 방향으로 약간씩 이동
		XMFLOAT3 adjustment;
		XMStoreFloat3(&adjustment, separation * offset);
		posA.x -= adjustment.x;
		posA.y -= adjustment.y;
		posA.z -= adjustment.z;
		posB.x += adjustment.x;
		posB.y += adjustment.y;
		posB.z += adjustment.z;

		// 새로운 위치 설정
		objA->SetPosition(posA);
		objB->SetPosition(posB);
	}

	// 충돌 정보 초기화 (다음 프레임을 위해)
	collisions.clear();
}

void Game_Scene::Add_Boom_Effect(XMFLOAT3 exlposion_pos, bool color_change)
{
	if (color_change) // 플레이어가 터트린 경우
	{
		game_score += 100;
	}
	else // 플레이어가 맞은 경우
		m_pPlayer->Life -= 1;
	

	bool Done = false; 
	Sprite_Billboard_Shader* pshader = effect_shader;

	for (CGameObject* boom_billboard_obj : pshader->m_ppObjects)
	{
		if (boom_billboard_obj->active == false)
		{
			boom_billboard_obj->active = true;
			((Billboard_Animation_Object*)boom_billboard_obj)->sprite_index = 0;
			((Billboard_Animation_Object*)boom_billboard_obj)->blue_boom = color_change;
			boom_billboard_obj->SetPosition(exlposion_pos);
			Done = true;
			break;
		}
	}

	if (Done == false)
	{
		pshader->Add_Object(exlposion_pos);
		Billboard_Animation_Object* new_obj = (Billboard_Animation_Object*)pshader->m_ppObjects.back();
		new_obj->blue_boom = color_change;
	}
	return;
}

void Game_Scene::Update_Power_Bar(float add_point)
{
	Screen_Rect* power_ptr = screen_shader->Power_Bar_ptr;

	to_be_point += add_point;


	if (to_be_point < 0.0f)
		to_be_point = 0.0f;
	else if (to_be_point > 100.0f)
		to_be_point = 100.0f;

	float lerp_speed = 0.05f; 
	now_point = now_point + (to_be_point - now_point) * lerp_speed;


	power_ptr->right_scale_move_value = now_point / 10.0f;

	if (now_point >= 99.0f)
		m_pPlayer->skill_charged = true;
	else
		m_pPlayer->skill_charged = false;
}

void Game_Scene::Update_Time_Bar(float add_point)
{
	Screen_Rect* time_ptr = screen_shader->Time_Bar_ptr;

	to_be_time += add_point;


	if (to_be_time < 0.0f)
		to_be_time = 0.0f;
	else if (to_be_time > 100.0f)
		to_be_time = 100.0f;

	float lerp_speed = 0.1f;
	now_time = now_time + (to_be_time - now_time) * lerp_speed;


	time_ptr->right_scale_move_value = now_time / 10.0f;

	if (now_time >= 0.1f)
		m_pPlayer->skill_cool_down = false;
	else
	{
		m_pPlayer->skill_cool_down = true;
		if (black_hole_shader->Get_Player_Black_Hole()->active == true)
		{
			for (CGameObject* asteroid_ptr : enemy_shader->m_ppObjects)
				((Asteroid*)asteroid_ptr)->SetTarget(m_pPlayer);

			black_hole_shader->Get_Player_Black_Hole()->active = false;
		}
	}
	

}

void Game_Scene::Reset()
{
	m_pPlayer->SetPosition(XMFLOAT3(0.0f, 250.0f, 0.0f));
	Show_Collider = false;
	Pause_Mode = false;
	Reset_Signal = false;

	m_pPlayer->Life = 10;
	m_pPlayer->skill_cool_down = false;
	m_pPlayer->skill_charged = false;

	game_score = 0;
	to_be_point = 0.0f;
	now_point = 0.0f;

	gravity_hole_runtime = 0.0f;
	to_be_time = 0.0f;
	now_time = 0.0f;

	enemy_shader->m_ppObjects.clear();
}