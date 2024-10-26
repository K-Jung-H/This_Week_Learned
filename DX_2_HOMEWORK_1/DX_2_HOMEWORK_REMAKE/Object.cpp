//-----------------------------------------------------------------------------
// File: CGameObject.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Object.h"
#include "Shader.h"
#include "Scene.h"


void ApplyScalingTransform(ID2D1DeviceContext2* pd2dDevicecontext, const D2D1_RECT_F& area, float scale, const D2D1_MATRIX_3X2_F& oldTransform)
{
	float cx = area.left + (area.right - area.left) / 2.0f;
	float cy = area.top + (area.bottom - area.top) / 2.0f;

	D2D1_MATRIX_3X2_F Matrix_T1 = D2D1::Matrix3x2F::Translation(-cx, -cy);
	D2D1_MATRIX_3X2_F Matrix_S = D2D1::Matrix3x2F::Scale(scale, scale);
	D2D1_MATRIX_3X2_F Matrix_T2 = D2D1::Matrix3x2F::Translation(cx, cy);
	D2D1_MATRIX_3X2_F finalTransform = Matrix_T1 * Matrix_S * Matrix_T2 * oldTransform;

	pd2dDevicecontext->SetTransform(finalTransform);
}

XMFLOAT3 GetOrthogonalVector(const XMFLOAT3& axis)
{
	// axis를 XMVECTOR로 변환
	XMVECTOR axisVec = XMLoadFloat3(&axis);

	// 벡터가 영벡터인지 확인
	if (XMVector3Equal(axisVec, XMVectorZero()))
	{
		// 영벡터인 경우 기본 수직 벡터를 반환
		return XMFLOAT3(1.0f, 0.0f, 0.0f); // 기본 수직 벡터
	}

	// 기준 벡터 선택 (여기서는 Y축을 선택)
	XMVECTOR basisVec = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	// axis와 기준 벡터가 같은 방향인 경우 X축을 기준으로 사용
	if (XMVector3Equal(axisVec, basisVec))
	{
		basisVec = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	}

	// 외적을 사용하여 수직 벡터를 구함
	XMVECTOR orthogonalVec = XMVector3Cross(axisVec, basisVec);

	// 수직 벡터를 정규화
	orthogonalVec = XMVector3Normalize(orthogonalVec);

	// 결과가 영벡터인지 확인
	if (XMVector3Equal(orthogonalVec, XMVectorZero()))
	{
		// 영벡터인 경우 기본 수직 벡터를 반환
		return XMFLOAT3(1.0f, 0.0f, 0.0f); // 기본 수직 벡터
	}

	// 결과를 XMFLOAT3로 변환하여 반환
	XMFLOAT3 result;
	XMStoreFloat3(&result, orthogonalVec);
	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CTexture::CTexture(int nTextures, UINT nTextureType, int nSamplers, int nRootParameters)
{
	m_nTextureType = nTextureType;

	m_nTextures = nTextures;
	if (m_nTextures > 0)
	{
		m_ppd3dTextureUploadBuffers = new ID3D12Resource * [m_nTextures];
		m_ppd3dTextures = new ID3D12Resource * [m_nTextures];
		for (int i = 0; i < m_nTextures; i++) m_ppd3dTextureUploadBuffers[i] = m_ppd3dTextures[i] = NULL;

		m_ppstrTextureNames = new _TCHAR[m_nTextures][64];
		for (int i = 0; i < m_nTextures; i++) m_ppstrTextureNames[i][0] = '\0';

		m_pd3dSrvGpuDescriptorHandles = new D3D12_GPU_DESCRIPTOR_HANDLE[m_nTextures];
		for (int i = 0; i < m_nTextures; i++) m_pd3dSrvGpuDescriptorHandles[i].ptr = NULL;

		m_pnResourceTypes = new UINT[m_nTextures];
		m_pdxgiBufferFormats = new DXGI_FORMAT[m_nTextures];
		m_pnBufferElements = new int[m_nTextures];
	}
	m_nRootParameters = nRootParameters;
	if (nRootParameters > 0) m_pnRootParameterIndices = new int[nRootParameters];
	for (int i = 0; i < m_nRootParameters; i++) m_pnRootParameterIndices[i] = -1;

	m_nSamplers = nSamplers;
	if (m_nSamplers > 0) m_pd3dSamplerGpuDescriptorHandles = new D3D12_GPU_DESCRIPTOR_HANDLE[m_nSamplers];
}

CTexture::~CTexture()
{
	if (m_ppd3dTextures)
	{
		for (int i = 0; i < m_nTextures; i++) if (m_ppd3dTextures[i]) m_ppd3dTextures[i]->Release();
		delete[] m_ppd3dTextures;
	}

	if (m_ppstrTextureNames) delete[] m_ppstrTextureNames;

	if (m_pnResourceTypes) delete[] m_pnResourceTypes;
	if (m_pdxgiBufferFormats) delete[] m_pdxgiBufferFormats;
	if (m_pnBufferElements) delete[] m_pnBufferElements;

	if (m_pnRootParameterIndices) delete[] m_pnRootParameterIndices;
	if (m_pd3dSrvGpuDescriptorHandles) delete[] m_pd3dSrvGpuDescriptorHandles;

	if (m_pd3dSamplerGpuDescriptorHandles) delete[] m_pd3dSamplerGpuDescriptorHandles;
}

void CTexture::SetRootParameterIndex(int nIndex, UINT nRootParameterIndex)
{
	m_pnRootParameterIndices[nIndex] = nRootParameterIndex;
}

void CTexture::SetGpuDescriptorHandle(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle)
{
	m_pd3dSrvGpuDescriptorHandles[nIndex] = d3dSrvGpuDescriptorHandle;
}

void CTexture::SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle)
{
	m_pd3dSamplerGpuDescriptorHandles[nIndex] = d3dSamplerGpuDescriptorHandle;
}

void CTexture::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_nRootParameters == m_nTextures)
	{
		for (int i = 0; i < m_nRootParameters; i++)
		{
			if (m_pd3dSrvGpuDescriptorHandles[i].ptr && (m_pnRootParameterIndices[i] != -1)) pd3dCommandList->SetGraphicsRootDescriptorTable(m_pnRootParameterIndices[i], m_pd3dSrvGpuDescriptorHandles[i]);
		}
	}
	else
	{
		if (m_pd3dSrvGpuDescriptorHandles[0].ptr) pd3dCommandList->SetGraphicsRootDescriptorTable(m_pnRootParameterIndices[0], m_pd3dSrvGpuDescriptorHandles[0]);
	}
}

void CTexture::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, int nParameterIndex, int nTextureIndex)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(m_pnRootParameterIndices[nParameterIndex], m_pd3dSrvGpuDescriptorHandles[nTextureIndex]);
}

void CTexture::ReleaseShaderVariables()
{
}

void CTexture::ReleaseUploadBuffers()
{
	if (m_ppd3dTextureUploadBuffers)
	{
		for (int i = 0; i < m_nTextures; i++) if (m_ppd3dTextureUploadBuffers[i]) m_ppd3dTextureUploadBuffers[i]->Release();
		delete[] m_ppd3dTextureUploadBuffers;
		m_ppd3dTextureUploadBuffers = NULL;
	}
}

void CTexture::LoadTextureFromDDSFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszFileName, UINT nResourceType, UINT nIndex)
{
	m_pnResourceTypes[nIndex] = nResourceType;
	m_ppd3dTextures[nIndex] = ::CreateTextureResourceFromDDSFile(pd3dDevice, pd3dCommandList, pszFileName, &m_ppd3dTextureUploadBuffers[nIndex], D3D12_RESOURCE_STATE_GENERIC_READ/*D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE*/);
}

void CTexture::LoadTextureFromWICFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszFileName, UINT nResourceType, UINT nIndex)
{
	m_pnResourceTypes[nIndex] = nResourceType;
	m_ppd3dTextures[nIndex] = ::CreateTextureResourceFromWICFile(pd3dDevice, pd3dCommandList, pszFileName, &m_ppd3dTextureUploadBuffers[nIndex], D3D12_RESOURCE_STATE_GENERIC_READ/*D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE*/);
}


void CTexture::LoadBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pData, UINT nElements, UINT nStride, DXGI_FORMAT ndxgiFormat, UINT nIndex)
{
	m_pnResourceTypes[nIndex] = RESOURCE_BUFFER;
	m_pdxgiBufferFormats[nIndex] = ndxgiFormat;
	m_pnBufferElements[nIndex] = nElements;
	m_ppd3dTextures[nIndex] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pData, nElements * nStride, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_GENERIC_READ, &m_ppd3dTextureUploadBuffers[nIndex]);
}

ID3D12Resource* CTexture::CreateTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT nIndex, UINT nResourceType, UINT nWidth, UINT nHeight, UINT nElements, UINT nMipLevels, DXGI_FORMAT dxgiFormat, D3D12_RESOURCE_FLAGS d3dResourceFlags, D3D12_RESOURCE_STATES d3dResourceStates, D3D12_CLEAR_VALUE* pd3dClearValue)
{
	m_pnResourceTypes[nIndex] = nResourceType;
	m_ppd3dTextures[nIndex] = ::CreateTexture2DResource(pd3dDevice, pd3dCommandList, nWidth, nHeight, nElements, nMipLevels, dxgiFormat, d3dResourceFlags, d3dResourceStates, pd3dClearValue);
	return(m_ppd3dTextures[nIndex]);
}

int CTexture::LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CGameObject* pParent, FILE* pInFile, CShader* pShader, UINT nIndex)
{
	char pstrTextureName[64] = { '\0' };

	BYTE nStrLength = 64;
	UINT nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
	nReads = (UINT)::fread(pstrTextureName, sizeof(char), nStrLength, pInFile);
	pstrTextureName[nStrLength] = '\0';

	bool bDuplicated = false;
	bool bLoaded = false;
	if (strcmp(pstrTextureName, "null"))
	{
		bLoaded = true;
		char pstrFilePath[64] = { '\0' };
		strcpy_s(pstrFilePath, 64, "Model/Textures/");

		bDuplicated = (pstrTextureName[0] == '@');
		strcpy_s(pstrFilePath + 15, 64 - 15, (bDuplicated) ? (pstrTextureName + 1) : pstrTextureName);
		strcpy_s(pstrFilePath + 15 + ((bDuplicated) ? (nStrLength - 1) : nStrLength), 64 - 15 - ((bDuplicated) ? (nStrLength - 1) : nStrLength), ".dds");

		size_t nConverted = 0;
		mbstowcs_s(&nConverted, m_ppstrTextureNames[nIndex], 64, pstrFilePath, _TRUNCATE);

#define _WITH_DISPLAY_TEXTURE_NAME

#ifdef _WITH_DISPLAY_TEXTURE_NAME
		static int nTextures = 0, nRepeatedTextures = 0;
		TCHAR pstrDebug[256] = { 0 };
		_stprintf_s(pstrDebug, 256, _T("Texture Name: %d %c %s\n"), (pstrTextureName[0] == '@') ? nRepeatedTextures++ : nTextures++, (pstrTextureName[0] == '@') ? '@' : ' ', m_ppstrTextureNames[nIndex]);
		OutputDebugString(pstrDebug);
#endif
		if (!bDuplicated)
		{
			LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, m_ppstrTextureNames[nIndex], RESOURCE_TEXTURE2D, nIndex);
#ifdef _WITH_STANDARD_TEXTURE_MULTIPLE_PARAMETERS
			CScene::CreateShaderResourceView(pd3dDevice, this, nIndex, PARAMETER_STANDARD_TEXTURE);
#else
			CScene::CreateShaderResourceView(pd3dDevice, this, nIndex);
#endif
		}
		else
		{
			if (pParent)
			{
				CGameObject* pRootGameObject = pParent;
				while (pRootGameObject)
				{
					if (!pRootGameObject->m_pParent) break;
					pRootGameObject = pRootGameObject->m_pParent;
				}
				D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle;
				int nParameterIndex = pRootGameObject->FindReplicatedTexture(m_ppstrTextureNames[nIndex], &d3dSrvGpuDescriptorHandle);
				if (nParameterIndex >= 0)
				{
					m_pd3dSrvGpuDescriptorHandles[nIndex] = d3dSrvGpuDescriptorHandle;
#ifdef _WITH_STANDARD_TEXTURE_MULTIPLE_PARAMETERS
					m_pnRootParameterIndices[nIndex] = nParameterIndex;
#endif
				}
			}
		}
	}
	return(bLoaded);
}

D3D12_SHADER_RESOURCE_VIEW_DESC CTexture::GetShaderResourceViewDesc(int nIndex)
{
	ID3D12Resource* pShaderResource = GetResource(nIndex);
	D3D12_RESOURCE_DESC d3dResourceDesc = pShaderResource->GetDesc();

	D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc;
	d3dShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	int nTextureType = GetTextureType(nIndex);
	switch (nTextureType)
	{
	case RESOURCE_TEXTURE2D: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 1)
	case RESOURCE_TEXTURE2D_ARRAY: //[]
		d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		d3dShaderResourceViewDesc.Texture2D.MipLevels = -1;
		d3dShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
		d3dShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_TEXTURE2DARRAY: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize != 1)
		d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
		d3dShaderResourceViewDesc.Texture2DArray.MipLevels = -1;
		d3dShaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.Texture2DArray.PlaneSlice = 0;
		d3dShaderResourceViewDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
		d3dShaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
		d3dShaderResourceViewDesc.Texture2DArray.ArraySize = d3dResourceDesc.DepthOrArraySize;
		break;
	case RESOURCE_TEXTURE_CUBE: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 6)
		d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		d3dShaderResourceViewDesc.TextureCube.MipLevels = 1;
		d3dShaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_BUFFER: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
		d3dShaderResourceViewDesc.Format = m_pdxgiBufferFormats[nIndex];
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		d3dShaderResourceViewDesc.Buffer.FirstElement = 0;
		d3dShaderResourceViewDesc.Buffer.NumElements = m_pnBufferElements[nIndex];
		d3dShaderResourceViewDesc.Buffer.StructureByteStride = 0;
		d3dShaderResourceViewDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		break;
	}
	return(d3dShaderResourceViewDesc);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CMaterial::CMaterial()
{
}

CMaterial::~CMaterial()
{
	if (m_pTexture) m_pTexture->Release();
	if (m_pShader) m_pShader->Release();
}

void CMaterial::SetShader(CShader *pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}

void CMaterial::SetTexture(CTexture* pTexture)
{
	if (m_pTexture) m_pTexture->Release();
	m_pTexture = pTexture;
	if (m_pTexture) m_pTexture->AddRef();
}

void CMaterial::ReleaseUploadBuffers()
{
//	if (m_pShader) m_pShader->ReleaseUploadBuffers();
	if (m_pTexture) m_pTexture->ReleaseUploadBuffers();
}

void CMaterial::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 4, &m_xmf4AmbientColor, 16);
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 4, &m_xmf4AlbedoColor, 20);
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 4, &m_xmf4SpecularColor, 24);
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 4, &m_xmf4EmissiveColor, 28);

	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 1, &m_nType, 32);

	if (m_pTexture) m_pTexture->UpdateShaderVariables(pd3dCommandList);
}

void CMaterial::ReleaseShaderVariables()
{
	if (m_pShader) m_pShader->ReleaseShaderVariables();
	if (m_pTexture) m_pTexture->ReleaseShaderVariables();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CGameObject::CGameObject()
{
	m_xmf4x4Transform = Matrix4x4::Identity();
	m_xmf4x4World = Matrix4x4::Identity();
	

}

CGameObject::CGameObject(int nMaterials) : CGameObject()
{
	m_nMaterials = nMaterials;
	if (m_nMaterials > 0)
	{
		m_ppMaterials = new CMaterial*[m_nMaterials];
		for(int i = 0; i < m_nMaterials; i++) m_ppMaterials[i] = NULL;
	}
}

CGameObject::~CGameObject()
{
	if (m_pMesh) m_pMesh->Release();

	if (m_nMaterials > 0)
	{
		for (int i = 0; i < m_nMaterials; i++)
		{
			if (m_ppMaterials[i]) m_ppMaterials[i]->Release();
		}
	}
	if (m_ppMaterials) delete[] m_ppMaterials;
}

void CGameObject::AddRef() 
{ 
	m_nReferences++; 

	if (m_pSibling) m_pSibling->AddRef();
	if (m_pChild) m_pChild->AddRef();
}

void CGameObject::Release() 
{ 
	if (m_pSibling) m_pSibling->Release();
	if (m_pChild) m_pChild->Release();

	if (--m_nReferences <= 0) delete this; 
}

void CGameObject::SetChild(CGameObject *pChild)
{
	if (m_pChild)
	{
		if (pChild) pChild->m_pSibling = m_pChild->m_pSibling;
		m_pChild->m_pSibling = pChild;
	}
	else
	{
		m_pChild = pChild;
	}
	if (pChild)
	{
		pChild->m_pParent = this;
	}
}

void CGameObject::SetMesh(CMesh *pMesh)
{
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
}

void CGameObject::SetShader(CShader *pShader)
{
	if (!m_nMaterials)
	{
		m_nMaterials = 1;
		m_ppMaterials = new CMaterial * [m_nMaterials];
		m_ppMaterials[0] = new CMaterial();
	}
	m_ppMaterials[0]->SetShader(pShader);
}

void CGameObject::SetShader(int nMaterial, CShader *pShader)
{
	if (m_ppMaterials[nMaterial]) m_ppMaterials[nMaterial]->SetShader(pShader);
}

void CGameObject::SetMaterial(int nMaterial, CMaterial *pMaterial)
{
	if (m_ppMaterials[nMaterial]) m_ppMaterials[nMaterial]->Release();
	m_ppMaterials[nMaterial] = pMaterial;
	if (m_ppMaterials[nMaterial]) m_ppMaterials[nMaterial]->AddRef();
}

void CGameObject::Animate(float fTimeElapsed, XMFLOAT4X4 *pxmf4x4Parent)
{
	if (m_fMovingSpeed != 0.0f && active)
		Move(m_xmf3MovingDirection, m_fMovingSpeed * fTimeElapsed);

	if (m_pSibling) m_pSibling->Animate(fTimeElapsed, pxmf4x4Parent);
	if (m_pChild) m_pChild->Animate(fTimeElapsed, &m_xmf4x4World);
}

CGameObject *CGameObject::FindFrame(char *pstrFrameName)
{
	CGameObject *pFrameObject = NULL;
	if (!strncmp(m_pstrFrameName, pstrFrameName, strlen(pstrFrameName))) return(this);

	if (m_pSibling) if (pFrameObject = m_pSibling->FindFrame(pstrFrameName)) return(pFrameObject);
	if (m_pChild) if (pFrameObject = m_pChild->FindFrame(pstrFrameName)) return(pFrameObject);

	return(NULL);
}

bool CGameObject::IsVisible(CCamera* pCamera)
{
	OnPrepareRender();
	if (pCamera == NULL)
		return false;

	if (m_pMesh == NULL)
			return false;

	bool bIsVisible = false;
	BoundingOrientedBox* xmBoundingBox = m_pMesh->GetBoundingBox();
	if (xmBoundingBox != NULL)
	{
		//모델 좌표계의 바운딩 박스를 월드 좌표계로 변환한다. 
		BoundingOrientedBox world_bounding_box;
		xmBoundingBox->Transform(world_bounding_box, XMLoadFloat4x4(&m_xmf4x4World));
		if (pCamera) 
			bIsVisible = pCamera->IsInFrustum(world_bounding_box);
		return(bIsVisible);
	}
	else
		return false;
}

void CGameObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	OnPrepareRender();

	if (culling_type == Culling_Type::Need_Culling)
		if (!IsVisible(pCamera))
			return;

	UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);

	if (m_nMaterials > 0)
	{
		for (int i = 0; i < m_nMaterials; i++)
		{
			if (m_ppMaterials[i])
			{
				if (m_ppMaterials[i]->m_pShader) 
					m_ppMaterials[i]->m_pShader->Render(pd3dCommandList, pCamera);

				m_ppMaterials[i]->UpdateShaderVariables(pd3dCommandList);
			}

			if (m_pMesh) 
				m_pMesh->Render(pd3dCommandList, i);
		}
	}
	if (m_pSibling) m_pSibling->Render(pd3dCommandList, pCamera);
	if (m_pChild) m_pChild->Render(pd3dCommandList, pCamera);


}

void CGameObject::Collider_Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (oobb_drawer) // 충돌 처리하는 객체라면 == 충돌체가 있나?
	{
		oobb_drawer->oobb_shader->Render(pd3dCommandList, pCamera);
		oobb_drawer->UpdateOOBB_Data(pd3dCommandList, this, XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
		oobb_drawer->Render(pd3dCommandList, pCamera);
	}
}

void CGameObject::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{

}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
}

void CGameObject::UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4 *pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));

	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);
}

void CGameObject::UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, CMaterial *pMaterial)
{
}

void CGameObject::ReleaseShaderVariables()
{
}

void CGameObject::ReleaseUploadBuffers()
{
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();

	for (int i = 0; i < m_nMaterials; i++)
	{
		if (m_ppMaterials[i]) m_ppMaterials[i]->ReleaseUploadBuffers();
	}

	if (m_pSibling) m_pSibling->ReleaseUploadBuffers();
	if (m_pChild) m_pChild->ReleaseUploadBuffers();
}

void CGameObject::UpdateTransform(XMFLOAT4X4 *pxmf4x4Parent)
{
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4Transform, *pxmf4x4Parent) : m_xmf4x4Transform;

	if (m_pSibling) m_pSibling->UpdateTransform(pxmf4x4Parent);
	if (m_pChild) m_pChild->UpdateTransform(&m_xmf4x4World);
}

void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4Transform._41 = x;
	m_xmf4x4Transform._42 = y;
	m_xmf4x4Transform._43 = z;

	UpdateTransform(NULL);
}

void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

void CGameObject::SetScale(float x, float y, float z)
{
	XMMATRIX mtxScale = XMMatrixScaling(x, y, z);
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxScale, m_xmf4x4Transform);

	UpdateTransform(NULL);
}

XMFLOAT3 CGameObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}

XMFLOAT3 CGameObject::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33)));
}

XMFLOAT3 CGameObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23)));
}

XMFLOAT3 CGameObject::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13)));
}

void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::Move(XMFLOAT3& vDirection, float fSpeed)
{
	SetPosition(m_xmf4x4World._41 + vDirection.x * fSpeed, m_xmf4x4World._42 + vDirection.y * fSpeed, m_xmf4x4World._43 + vDirection.z * fSpeed);
}

void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4Transform);

	UpdateTransform(NULL);
}

void CGameObject::Rotate(XMFLOAT3 *pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4Transform);

	UpdateTransform(NULL);
}

void CGameObject::Rotate(XMFLOAT4 *pxmf4Quaternion)
{
	XMMATRIX mtxRotate = XMMatrixRotationQuaternion(XMLoadFloat4(pxmf4Quaternion));
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4Transform);

	UpdateTransform(NULL);
}

void CGameObject::SetLookAt(XMFLOAT3& xmf3Target, XMFLOAT3& xmf3Up)
{
	//m_xmf4x4Transform
	//XMFLOAT3 xmf3Position(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
	//XMFLOAT4X4 mtxLookAt = Matrix4x4::LookAtLH(xmf3Position, xmf3Target, xmf3Up);
	//m_xmf4x4World._11 = mtxLookAt._11; m_xmf4x4World._12 = mtxLookAt._21; m_xmf4x4World._13 = mtxLookAt._31;
	//m_xmf4x4World._21 = mtxLookAt._12; m_xmf4x4World._22 = mtxLookAt._22; m_xmf4x4World._23 = mtxLookAt._32;
	//m_xmf4x4World._31 = mtxLookAt._13; m_xmf4x4World._32 = mtxLookAt._23; m_xmf4x4World._33 = mtxLookAt._33;

	
	XMFLOAT3 xmf3Position(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
	XMFLOAT4X4 mtxLookAt = Matrix4x4::LookAtLH(xmf3Position, xmf3Target, xmf3Up);
	m_xmf4x4Transform._11 = mtxLookAt._11; m_xmf4x4Transform._12 = mtxLookAt._21; m_xmf4x4Transform._13 = mtxLookAt._31;
	m_xmf4x4Transform._21 = mtxLookAt._12; m_xmf4x4Transform._22 = mtxLookAt._22; m_xmf4x4Transform._23 = mtxLookAt._32;
	m_xmf4x4Transform._31 = mtxLookAt._13; m_xmf4x4Transform._32 = mtxLookAt._23; m_xmf4x4Transform._33 = mtxLookAt._33;
}

int CGameObject::FindReplicatedTexture(_TCHAR* pstrTextureName, D3D12_GPU_DESCRIPTOR_HANDLE* pd3dSrvGpuDescriptorHandle)
{
	int nParameterIndex = -1;

	for (int i = 0; i < m_nMaterials; i++)
	{
		if (m_ppMaterials[i] && m_ppMaterials[i]->m_pTexture)
		{
			int nTextures = m_ppMaterials[i]->m_pTexture->GetTextures();
			for (int j = 0; j < nTextures; j++)
			{
				if (!_tcsncmp(m_ppMaterials[i]->m_pTexture->GetTextureName(j), pstrTextureName, _tcslen(pstrTextureName)))
				{
					*pd3dSrvGpuDescriptorHandle = m_ppMaterials[i]->m_pTexture->GetGpuDescriptorHandle(j);
					nParameterIndex = m_ppMaterials[i]->m_pTexture->GetRootParameter(j);
					return(nParameterIndex);
				}
			}
		}
	}
	if (m_pSibling) if ((nParameterIndex = m_pSibling->FindReplicatedTexture(pstrTextureName, pd3dSrvGpuDescriptorHandle)) > 0) return(nParameterIndex);
	if (m_pChild) if ((nParameterIndex = m_pChild->FindReplicatedTexture(pstrTextureName, pd3dSrvGpuDescriptorHandle)) > 0) return(nParameterIndex);

	return(nParameterIndex);
}

void CGameObject::LoadMaterialsFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, CGameObject *pParent, FILE *pInFile, CShader *pShader)
{
	char pstrToken[64] = { '\0' };

	int nMaterial = 0;
	BYTE nStrLength = 0;

	UINT nReads = (UINT)::fread(&m_nMaterials, sizeof(int), 1, pInFile);

	m_ppMaterials = new CMaterial*[m_nMaterials];
	for (int i = 0; i < m_nMaterials; i++) m_ppMaterials[i] = NULL;

	CMaterial *pMaterial = NULL;
	CTexture* pTexture = NULL;

	for ( ; ; )
	{
		nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
		nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile); 
		pstrToken[nStrLength] = '\0';

		if (!strcmp(pstrToken, "<Material>:"))
		{
			nReads = (UINT)::fread(&nMaterial, sizeof(int), 1, pInFile);

			pMaterial = new CMaterial(); 

#ifdef _WITH_STANDARD_TEXTURE_MULTIPLE_PARAMETERS
			pTexture = new CTexture(7, RESOURCE_TEXTURE2D, 0, 7); //0:Albedo, 1:Specular, 2:Metallic, 3:Normal, 4:Emission, 5:DetailAlbedo, 6:DetailNormal
#else
			pTexture = new CTexture(7, RESOURCE_TEXTURE2D, 0, 1); //0:Albedo, 1:Specular, 2:Metallic, 3:Normal, 4:Emission, 5:DetailAlbedo, 6:DetailNormal
			pTexture->SetRootParameterIndex(0, PARAMETER_STANDARD_TEXTURE);
#endif
			pMaterial->SetTexture(pTexture);
//			pMaterial->SetShader(pShader);
			SetMaterial(nMaterial, pMaterial); //m_ppMaterials[i] = pMaterial

			UINT nMeshType = GetMeshType();
			//if (nMeshType & VERTEXT_NORMAL_TEXTURE) pMaterial->SetStandardShader();
		}
		else if (!strcmp(pstrToken, "<AlbedoColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_xmf4AlbedoColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<EmissiveColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_xmf4EmissiveColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<SpecularColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_xmf4SpecularColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<Glossiness>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fGlossiness), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Smoothness>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fSmoothness), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Metallic>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fSpecularHighlight), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<SpecularHighlight>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fMetallic), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<GlossyReflection>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fGlossyReflection), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<AlbedoMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, pShader, 0)) pMaterial->SetMaterialType(MATERIAL_ALBEDO_MAP);
		}
		else if (!strcmp(pstrToken, "<SpecularMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, pShader, 1)) pMaterial->SetMaterialType(MATERIAL_SPECULAR_MAP);
		}
		else if (!strcmp(pstrToken, "<NormalMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, pShader, 2)) pMaterial->SetMaterialType(MATERIAL_NORMAL_MAP);
		}
		else if (!strcmp(pstrToken, "<MetallicMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, pShader, 3)) pMaterial->SetMaterialType(MATERIAL_METALLIC_MAP);
		}
		else if (!strcmp(pstrToken, "<EmissionMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, pShader, 4)) pMaterial->SetMaterialType(MATERIAL_EMISSION_MAP);
		}
		else if (!strcmp(pstrToken, "<DetailAlbedoMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, pShader, 5)) pMaterial->SetMaterialType(MATERIAL_DETAIL_ALBEDO_MAP);
		}
		else if (!strcmp(pstrToken, "<DetailNormalMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, pShader, 6)) pMaterial->SetMaterialType(MATERIAL_DETAIL_NORMAL_MAP);
		}
		else if (!strcmp(pstrToken, "</Materials>"))
		{
			break;
		}
	}
}

CGameObject *CGameObject::LoadFrameHierarchyFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, CGameObject *pParent, FILE *pInFile, CShader *pShader)
{
	char pstrToken[64] = { '\0' };

	BYTE nStrLength = 0;
	UINT nReads = 0;

	int nFrame = 0, nTextures = 0;

	CGameObject *pGameObject = NULL;

	for ( ; ; )
	{
		nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
		nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
		pstrToken[nStrLength] = '\0';

		if (!strcmp(pstrToken, "<Frame>:"))
		{
			pGameObject = new CGameObject();

			nReads = (UINT)::fread(&nFrame, sizeof(int), 1, pInFile);
			nReads = (UINT)::fread(&nTextures, sizeof(int), 1, pInFile);

			nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
			nReads = (UINT)::fread(pGameObject->m_pstrFrameName, sizeof(char), nStrLength, pInFile);
			pGameObject->m_pstrFrameName[nStrLength] = '\0';
		}
		else if (!strcmp(pstrToken, "<Transform>:"))
		{
			XMFLOAT3 xmf3Position, xmf3Rotation, xmf3Scale;
			XMFLOAT4 xmf4Rotation;
			nReads = (UINT)::fread(&xmf3Position, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf3Rotation, sizeof(float), 3, pInFile); //Euler Angle
			nReads = (UINT)::fread(&xmf3Scale, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf4Rotation, sizeof(float), 4, pInFile); //Quaternion
		}
		else if (!strcmp(pstrToken, "<TransformMatrix>:"))
		{
			nReads = (UINT)::fread(&pGameObject->m_xmf4x4Transform, sizeof(float), 16, pInFile);
		}
		else if (!strcmp(pstrToken, "<Mesh>:"))
		{
			CStandardMesh *pMesh = new CStandardMesh(pd3dDevice, pd3dCommandList);
			pMesh->LoadMeshFromFile(pd3dDevice, pd3dCommandList, pInFile);
			pGameObject->SetMesh(pMesh);
		}
		else if (!strcmp(pstrToken, "<Materials>:"))
		{
			pGameObject->LoadMaterialsFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, pShader);
		}
		else if (!strcmp(pstrToken, "<Children>:"))
		{
			int nChilds = 0;
			nReads = (UINT)::fread(&nChilds, sizeof(int), 1, pInFile);
			if (nChilds > 0)
			{
				for (int i = 0; i < nChilds; i++)
				{
					CGameObject *pChild = CGameObject::LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pGameObject, pInFile, pShader);
					if (pChild) pGameObject->SetChild(pChild);
#ifdef _WITH_DEBUG_FRAME_HIERARCHY
					TCHAR pstrDebug[256] = { 0 };
					_stprintf_s(pstrDebug, 256, _T("(Frame: %p) (Parent: %p)\n"), pChild, pGameObject);
					OutputDebugString(pstrDebug);
#endif
				}
			}
		}
		else if (!strcmp(pstrToken, "</Frame>"))
		{
			break;
		}
	}
	return(pGameObject);
}

void CGameObject::PrintFrameInfo(CGameObject *pGameObject, CGameObject *pParent)
{
	TCHAR pstrDebug[256] = { 0 };

	_stprintf_s(pstrDebug, 256, _T("(Frame: %p) (Parent: %p)\n"), pGameObject, pParent);
	OutputDebugString(pstrDebug);

	if (pGameObject->m_pSibling) CGameObject::PrintFrameInfo(pGameObject->m_pSibling, pParent);
	if (pGameObject->m_pChild) CGameObject::PrintFrameInfo(pGameObject->m_pChild, pGameObject);
}

CGameObject *CGameObject::LoadGeometryFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, char *pstrFileName, CShader *pShader)
{
	FILE *pInFile = NULL;
	::fopen_s(&pInFile, pstrFileName, "rb");
	::rewind(pInFile);

	CGameObject *pGameObject = CGameObject::LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, NULL, pInFile, pShader);

#ifdef _WITH_DEBUG_FRAME_HIERARCHY
	TCHAR pstrDebug[256] = { 0 };
	_stprintf_s(pstrDebug, 256, _T("Frame Hierarchy\n"));
	OutputDebugString(pstrDebug);

	CGameObject::PrintFrameInfo(pGameObject, NULL);
#endif

	return(pGameObject);
}

void CGameObject::GenerateRayForPicking(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4&
	xmf4x4View, XMFLOAT3* pxmf3PickRayOrigin, XMFLOAT3* pxmf3PickRayDirection)
{
	XMFLOAT4X4 xmf4x4WorldView = Matrix4x4::Multiply(m_xmf4x4World, xmf4x4View);
	XMFLOAT4X4 xmf4x4Inverse = Matrix4x4::Inverse(xmf4x4WorldView);
	XMFLOAT3 xmf3CameraOrigin(0.0f, 0.0f, 0.0f);
	//카메라 좌표계의 원점을 모델 좌표계로 변환한다. 
	*pxmf3PickRayOrigin = Vector3::TransformCoord(xmf3CameraOrigin, xmf4x4Inverse);
	//카메라 좌표계의 점(마우스 좌표를 역변환하여 구한 점)을 모델 좌표계로 변환한다. 
	*pxmf3PickRayDirection = Vector3::TransformCoord(xmf3PickPosition, xmf4x4Inverse);
	//광선의 방향 벡터를 구한다. 
	*pxmf3PickRayDirection = Vector3::Normalize(Vector3::Subtract(*pxmf3PickRayDirection, *pxmf3PickRayOrigin));
}

int CGameObject::PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfHitDistance)
{
	int nIntersected = 0;
	if(m_pMesh)
	{
		XMFLOAT3 xmf3PickRayOrigin, xmf3PickRayDirection;
		//모델 좌표계의 광선을 생성한다. 
		GenerateRayForPicking(xmf3PickPosition, xmf4x4View, &xmf3PickRayOrigin, &xmf3PickRayDirection);
		//모델 좌표계의 광선과 메쉬의 교차를 검사한다. 
		nIntersected = m_pMesh->CheckRayIntersection(xmf3PickRayOrigin, xmf3PickRayDirection, pfHitDistance);
	}


	return(nIntersected);

}

BoundingOrientedBox* CGameObject::GetCollider()
{
	BoundingOrientedBox* pOriginalBoundingBox = m_pMesh->GetBoundingBox();
	if (pOriginalBoundingBox == NULL)
		return NULL;

	BoundingOrientedBox* pWorldBoundingBox = new BoundingOrientedBox(*pOriginalBoundingBox);
	pWorldBoundingBox->Center = GetPosition();

	XMVECTOR quaternionRotation = XMQuaternionRotationMatrix(XMLoadFloat4x4(&m_xmf4x4World));
	XMStoreFloat4(&pWorldBoundingBox->Orientation, quaternionRotation);

	return pWorldBoundingBox;
}

bool CGameObject::Check_Polygon_Ray_Normal(XMFLOAT3 start_pos, XMFLOAT3 dir, XMFLOAT3* polygon_normal_vector)
{
	if (m_pMesh)
	{
		// 1. 월드 행렬의 역행렬을 계산
		XMMATRIX worldMatrix = XMLoadFloat4x4(&m_xmf4x4World); // 월드 행렬을 로드
		XMMATRIX invWorldMatrix = XMMatrixInverse(nullptr, worldMatrix); // 월드 행렬의 역행렬 계산

		// 2. 광선의 시작점과 방향을 모델 좌표계로 변환
		XMVECTOR rayOrigin = XMLoadFloat3(&start_pos); // 시작점을 XMVECTOR로 로드
		XMVECTOR rayDir = XMLoadFloat3(&dir);          // 방향 벡터를 XMVECTOR로 로드

		rayOrigin = XMVector3TransformCoord(rayOrigin, invWorldMatrix); // 시작점의 변환
		rayDir = XMVector3TransformNormal(rayDir, invWorldMatrix);      // 방향 벡터의 변환
		rayDir = XMVector3Normalize(rayDir); // 방향 벡터 정규화

		if (m_pMesh->Check_Polygon_Ray_Normal(rayOrigin, rayDir, polygon_normal_vector))
			return true;
	}
	return false; 
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
CMesh* OOBB_Drawer::oobb_Mesh = NULL;
CShader* OOBB_Drawer::oobb_shader = NULL;

void OOBB_Drawer::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(OOBB_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbOOBBInfo = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbOOBBInfo->Map(0, NULL, (void**)&m_pcbMappedOOBBInfo);
}

bool OOBB_Drawer::UpdateOOBB_Data(ID3D12GraphicsCommandList* pd3dCommandList, CGameObject* g_obj, XMFLOAT4 line_color)
{
	// 게임 오브젝트에서 BoundingOrientedBox를 가져오고, NULL 체크
	BoundingOrientedBox* pBoundingBox = g_obj->GetCollider();
	if (pBoundingBox == NULL)
		return false;

	// BoundingOrientedBox의 Extents 값을 가져옵니다 (필요한 값만 사용)
	XMVECTOR extents = XMLoadFloat3(&pBoundingBox->Extents);

	// 게임 오브젝트의 위치와 회전값을 로드
	XMVECTOR center = XMLoadFloat3(&pBoundingBox->Center);
	XMMATRIX worldMatrix = XMLoadFloat4x4(&g_obj->m_xmf4x4World);

	// 월드 행렬에서 회전 행렬 추출
	XMVECTOR scale, rotation, translation;
	XMMatrixDecompose(&scale, &rotation, &translation, worldMatrix);

	// 회전 행렬을 기준으로 BoundingOrientedBox에 적용할 변환 행렬을 구성
	XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(rotation);
	XMMATRIX scaleMatrix = XMMatrixScaling(
		2.0f * XMVectorGetX(extents),
		2.0f * XMVectorGetY(extents),
		2.0f * XMVectorGetZ(extents));

	XMMATRIX translationMatrix = XMMatrixTranslationFromVector(center);

	// 최종 월드 행렬 계산 (스케일, 회전, 이동 순서로 적용)
	XMMATRIX finalWorldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

	// 행렬을 XMFLOAT4X4 형식으로 저장 (HLSL에서 사용하는 행렬은 행 우선(row-major) 형태이므로 전치 필요)
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(finalWorldMatrix));

	// 상수 버퍼에 적용할 데이터를 설정
	m_pcbMappedOOBBInfo->line_color = line_color;  // OBB의 색상
	m_pcbMappedOOBBInfo->world_matrix = xmf4x4World;  // 계산된 월드 행렬

	// GPU 상수 버퍼에 적용
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbOOBBInfo->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(PARAMETER_OOBB_CUBE_CBV, d3dGpuVirtualAddress);

	return true;
}
bool OOBB_Drawer::UpdateOOBB_Data(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* matrix, BoundingOrientedBox* pBoundingBox, XMFLOAT4 line_color)
{
	// 게임 오브젝트에서 BoundingOrientedBox를 가져오고, NULL 체크
	if (pBoundingBox == NULL)
		return false;

	// BoundingOrientedBox의 Extents 값을 가져옵니다 (필요한 값만 사용)
	XMVECTOR extents = XMLoadFloat3(&pBoundingBox->Extents);

	// 게임 오브젝트의 위치와 회전값을 로드
	XMVECTOR center = XMLoadFloat3(&pBoundingBox->Center);
	XMMATRIX worldMatrix = XMLoadFloat4x4(matrix);

	// 월드 행렬에서 회전 행렬 추출
	XMVECTOR scale, rotation, translation;
	XMMatrixDecompose(&scale, &rotation, &translation, worldMatrix);

	// 회전 행렬을 기준으로 BoundingOrientedBox에 적용할 변환 행렬을 구성
	XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(rotation);
	XMMATRIX scaleMatrix = XMMatrixScaling(
		2.0f * XMVectorGetX(extents),
		2.0f * XMVectorGetY(extents),
		2.0f * XMVectorGetZ(extents));

	XMMATRIX translationMatrix = XMMatrixTranslationFromVector(center);

	// 최종 월드 행렬 계산 (스케일, 회전, 이동 순서로 적용)
	XMMATRIX finalWorldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

	// 행렬을 XMFLOAT4X4 형식으로 저장 (HLSL에서 사용하는 행렬은 행 우선(row-major) 형태이므로 전치 필요)
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(finalWorldMatrix));

	// 상수 버퍼에 적용할 데이터를 설정
	m_pcbMappedOOBBInfo->line_color = line_color;  // OBB의 색상
	m_pcbMappedOOBBInfo->world_matrix = xmf4x4World;  // 계산된 월드 행렬

	// GPU 상수 버퍼에 적용
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbOOBBInfo->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(PARAMETER_OOBB_CUBE_CBV, d3dGpuVirtualAddress);

	return true;
}

void OOBB_Drawer::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (oobb_Mesh)
		oobb_Mesh->Render(pd3dCommandList, 0);
	
};
//==================================================================

CSkyBox::CSkyBox(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature) : CGameObject(1)
{
	CSkyBoxMesh *pSkyBoxMesh = new CSkyBoxMesh(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 20.0f);
	SetMesh(pSkyBoxMesh);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CTexture *pSkyBoxTexture = new CTexture(1, RESOURCE_TEXTURE_CUBE, 0, 1);
	pSkyBoxTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"SkyBox/SkyBox_space.dds", RESOURCE_TEXTURE_CUBE, 0);

	CSkyBoxShader *pSkyBoxShader = new CSkyBoxShader();
	pSkyBoxShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pSkyBoxShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CScene::CreateShaderResourceViews(pd3dDevice, pSkyBoxTexture, 0, PARAMETER_SKYBOX_CUBE_TEXTURE);

	CMaterial *pSkyBoxMaterial = new CMaterial();
	pSkyBoxMaterial->SetTexture(pSkyBoxTexture);
	pSkyBoxMaterial->SetShader(pSkyBoxShader);

	SetMaterial(0, pSkyBoxMaterial);
}

CSkyBox::~CSkyBox()
{
}

void CSkyBox::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	XMFLOAT3 xmf3CameraPos = pCamera->GetPosition();
	SetPosition(xmf3CameraPos.x, xmf3CameraPos.y, xmf3CameraPos.z);

	CGameObject::Render(pd3dCommandList, pCamera);
}

CHeightMapTerrain::CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color) : CGameObject(1)
{
	m_nWidth = nWidth;
	m_nLength = nLength;

	int cxQuadsPerBlock = nBlockWidth - 1;
	int czQuadsPerBlock = nBlockLength - 1;

	m_xmf3Scale = xmf3Scale;

	m_pHeightMapImage = new CHeightMapImage(pFileName, nWidth, nLength, xmf3Scale);

	long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;
	long czBlocks = (m_nLength - 1) / czQuadsPerBlock;



	CHeightMapGridMesh* pHeightMapGridMesh = NULL;
	for (int z = 0, zStart = 0; z < czBlocks; z++)
	{
		for (int x = 0, xStart = 0; x < cxBlocks; x++)
		{
			xStart = x * (nBlockWidth - 1);
			zStart = z * (nBlockLength - 1);
			pHeightMapGridMesh = new CHeightMapGridMesh(pd3dDevice, pd3dCommandList, xStart, zStart, nBlockWidth, nBlockLength, xmf3Scale, xmf4Color, m_pHeightMapImage);
			SetMesh(pHeightMapGridMesh);
		}
	}

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CTexture* pTerrainTexture = new CTexture(3, RESOURCE_TEXTURE2D, 0, 1); // 텍스쳐는 리소스 3개 사용 + 루트 파라메터에 서술자 3개 사용하겠다
	
	pTerrainTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Hight_Map/Moon_Texture.dds", RESOURCE_TEXTURE2D, 0); // Base_Texture
	pTerrainTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Hight_Map/moon_detail_texture.dds", RESOURCE_TEXTURE2D, 1); //Detail_Texture_7
	pTerrainTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Hight_Map/HeightMap(Alpha).dds", RESOURCE_TEXTURE2D, 2); 
	CScene::CreateShaderResourceViews(pd3dDevice, pTerrainTexture, 0, 6); // 루트 파라메터 서술자 인덱스 6번부터 차례대로 연결하겠다 // 위에서 3개 사용하겟다고 선언


	CTerrainShader* pTerrainShader = new CTerrainShader();
	pTerrainShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pTerrainShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	//UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	//D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle = CScene::CreateConstantBufferView(pd3dDevice, m_pd3dcbGameObject, ncbElementBytes);
	//SetCbvGPUDescriptorHandle(d3dCbvGPUDescriptorHandle);

	CMaterial* pTerrainMaterial = new CMaterial();
	pTerrainMaterial->SetTexture(pTerrainTexture);
	pTerrainMaterial->SetShader(pTerrainShader);
	pTerrainMaterial->m_xmf4AlbedoColor = XMFLOAT4(0.5f, 0.5f, 0.0f, 1.0f);
	SetMaterial(0, pTerrainMaterial);

}

CHeightMapTerrain::~CHeightMapTerrain(void)
{
	if (m_pHeightMapImage) delete m_pHeightMapImage;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///

Screen_Rect::Screen_Rect(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, 
	ScreenShader* screen_shader, CTexture* screen_texture, XMFLOAT2 l_t, XMFLOAT2 r_b, int camera_distance) : CGameObject(1)
{
	Textured_Screen_Mesh* screen_mesh = new Textured_Screen_Mesh(pd3dDevice, pd3dCommandList, l_t, r_b, camera_distance);
	SetMesh(screen_mesh);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CTexture* pScreen_Texture = screen_texture;

	
	CScene::CreateShaderResourceViews(pd3dDevice, pScreen_Texture, 0, PARAMETER_DEFAULT_TEXTURE);
	
	CMaterial* pScreenMaterial = new CMaterial();
	pScreenMaterial->SetTexture(pScreen_Texture);

	SetMaterial(0, pScreenMaterial);

	sissor_rect_screen_size = D3D12_RECT();
	sissor_rect_screen_size.left = static_cast<UINT>((l_t.x + 1.0f) * 0.5f * FRAME_BUFFER_WIDTH);
	sissor_rect_screen_size.top = static_cast<UINT>((1.0f - l_t.y) * 0.5f * FRAME_BUFFER_HEIGHT);
	sissor_rect_screen_size.right = static_cast<UINT>((r_b.x + 1.0f) * 0.5f * FRAME_BUFFER_WIDTH);
	sissor_rect_screen_size.bottom = static_cast<UINT>((1.0f - r_b.y) * 0.5f * FRAME_BUFFER_HEIGHT);
}

Screen_Rect::~Screen_Rect()
{
}

void Screen_Rect::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_Screen_Rect_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbScreen_Rect_Info = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbScreen_Rect_Info->Map(0, NULL, (void**)&m_pcbMappedScreen_Rect_Info);
}

void Screen_Rect::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT4X4 matrix_temp = Matrix4x4::Identity();
	matrix_temp._24 = scroll_value;
	m_pcbMappedScreen_Rect_Info->transform_info = matrix_temp;

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbScreen_Rect_Info->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(8, d3dGpuVirtualAddress);
}

void Screen_Rect::ReleaseShaderVariables()
{
	if (m_pd3dcbScreen_Rect_Info)
	{
		m_pd3dcbScreen_Rect_Info->Unmap(0, NULL);
		m_pd3dcbScreen_Rect_Info->Release();
	}
}

void Screen_Rect::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (active == false)
		return;

	XMFLOAT3 xmf3CameraPos = pCamera->GetPosition();
	SetPosition(xmf3CameraPos.x, xmf3CameraPos.y, xmf3CameraPos.z);

	UpdateShaderVariables(pd3dCommandList); // for scroll
	
	if (sissor_rect_clip)
		pd3dCommandList->RSSetScissorRects(1, &sissor_rect_screen_size);

	CGameObject::Render(pd3dCommandList, pCamera);

	if (sissor_rect_clip)
		pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
}

void Screen_Rect::Message_Render(ID2D1DeviceContext2* pd2dDevicecontext, IDWriteTextFormat* font, ID2D1SolidColorBrush* brush)
{
	if (Is_Text_Screen)
	{
		if (Text_Scale > 1.0f)
			Text_Scale -= 0.05f;
		else
			Text_Scale = 1.0f;

		D2D1_RECT_F text_area =
		{
			static_cast<LONG>(sissor_rect_screen_size.left),
			static_cast<LONG>(sissor_rect_screen_size.top),
			static_cast<LONG>(sissor_rect_screen_size.right),
			static_cast<LONG>(sissor_rect_screen_size.bottom)
		};

		D2D1_MATRIX_3X2_F oldTransform;
		pd2dDevicecontext->GetTransform(&oldTransform);

		ApplyScalingTransform(pd2dDevicecontext, text_area, Text_Scale, oldTransform);

		std::wstring text_message = _T("");
		if (Text_List.size() > 0)
			text_message = Text_List[Text_Index];

		pd2dDevicecontext->DrawTextW(text_message.c_str(), (UINT32)wcslen(text_message.c_str()), font, &text_area, brush);
		pd2dDevicecontext->SetTransform(oldTransform);
	}

		if (m_pChild)
			((Screen_Rect*)m_pChild)->Message_Render(pd2dDevicecontext, font, brush);

		if (m_pSibling)
			((Screen_Rect*)m_pSibling)->Message_Render(pd2dDevicecontext, font, brush);
}

void Screen_Rect::Change_Text(int n)
{
	if (n > 0)
		if (Text_Index < Text_List.size() - 1)
			Text_Index += 1;

	if (n < 0)
		if (Text_Index > 0)
			Text_Index -= 1;
}

Screen_Rect* Screen_Rect::PickScreenObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, float* pfHitDistance)
{
	if (active == false)
		return NULL;

	float Distance = FLT_MAX; 
	Screen_Rect* nearest_rect = NULL;

	XMFLOAT3 xmf3PickRayOrigin = { xmf3PickPosition };
	XMFLOAT3 xmf3PickRayDirection = { 0.0f, 0.0f, 1.0f };

	// 모델 좌표계의 광선과 메쉬의 교차를 검사
	if (m_pMesh)
	{
		float hitDistance = FLT_MAX; 
		int nIntersected = m_pMesh->CheckRayIntersection(xmf3PickRayOrigin, xmf3PickRayDirection, &hitDistance);

		if (nIntersected && hitDistance < Distance)
		{
			nearest_rect = this; 
			Distance = hitDistance; 
		}
	}

	// 형제 노드에 대한 재귀 호출
	if (m_pSibling)
	{
		Screen_Rect* sibling_intersected = ((Screen_Rect*)m_pSibling)->PickScreenObjectByRayIntersection(xmf3PickPosition, pfHitDistance);

		if (sibling_intersected && *pfHitDistance < Distance)
		{
			nearest_rect = sibling_intersected; 
			Distance = *pfHitDistance; 
		}
	}

	// 자식 노드에 대한 재귀 호출
	if (m_pChild)
	{
		Screen_Rect* child_intersected = ((Screen_Rect*)m_pChild)->PickScreenObjectByRayIntersection(xmf3PickPosition, pfHitDistance);

		if (child_intersected && *pfHitDistance < Distance)
		{
			nearest_rect = child_intersected; 
			Distance = *pfHitDistance; 
		}
	}

	if (nearest_rect)
		*pfHitDistance = Distance; 

	return nearest_rect; 
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
CMi24Object::CMi24Object(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	culling_type = Culling_Type::Need_Culling;
}

CMi24Object::~CMi24Object()
{
}

void CMi24Object::PrepareAnimate()
{
	m_pMainRotorFrame = FindFrame("Top_Rotor");
	m_pTailRotorFrame = FindFrame("Tail_Rotor");
}

void CMi24Object::Animate(float fTimeElapsed, XMFLOAT4X4 *pxmf4x4Parent)
{
	if (m_pMainRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationY(XMConvertToRadians(360.0f * 2.0f) * fTimeElapsed);
		m_pMainRotorFrame->m_xmf4x4Transform = Matrix4x4::Multiply(xmmtxRotate, m_pMainRotorFrame->m_xmf4x4Transform);
	}
	if (m_pTailRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationX(XMConvertToRadians(360.0f * 4.0f) * fTimeElapsed);
		m_pTailRotorFrame->m_xmf4x4Transform = Matrix4x4::Multiply(xmmtxRotate, m_pTailRotorFrame->m_xmf4x4Transform);
	}

	CGameObject::Animate(fTimeElapsed, pxmf4x4Parent);
}


//======================================================

CRotatingObject::CRotatingObject() : CGameObject()
{
	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 10.0f;
}

CRotatingObject::CRotatingObject(int nmaterial) : CGameObject(nmaterial)
{
	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 10.0f;
}

CRotatingObject::~CRotatingObject()
{
}

void CRotatingObject::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	CGameObject::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);

	CGameObject::Animate(fTimeElapsed, pxmf4x4Parent);
}

//======================================================

Flying_Box::Flying_Box(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) 
	: CRotatingObject(1)
{
	PrepareAnimate();
	oobb_drawer = new OOBB_Drawer();
	oobb_drawer->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	culling_type = Culling_Type::Need_Culling;
}

Flying_Box::~Flying_Box()
{
}

void Flying_Box::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	CRotatingObject::Animate(fTimeElapsed, pxmf4x4Parent);
}

void Flying_Box::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	CGameObject::Render(pd3dCommandList, pCamera);
}
//=======================================================
Asteroid::Asteroid(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) 
	: CRotatingObject(2)
{
	PrepareAnimate();
	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	oobb_drawer = new OOBB_Drawer();
	oobb_drawer->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	culling_type = Culling_Type::Need_Culling;

}
Asteroid::~Asteroid()
{
}

void Asteroid::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_Outline_INFO) + 255) & ~255); //256의 배수
	m_pd3dcb_outline_Info = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcb_outline_Info->Map(0, NULL, (void**)&m_pcbMapped_outline_Info);
}

void Asteroid::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pcbMapped_outline_Info->outline_color = outline_color;
	m_pcbMapped_outline_Info->thickness = 1.0f;
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcb_outline_Info->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(11, d3dGpuVirtualAddress);
}

void Asteroid::ReleaseShaderVariables()
{
	if (m_pd3dcb_outline_Info)
	{
		m_pd3dcb_outline_Info->Unmap(0, NULL);
		m_pd3dcb_outline_Info->Release();
	}
}

void Asteroid::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	XMFLOAT3 axis = GetMovingDirection();
	XMFLOAT3 new_axis = GetOrthogonalVector(axis);
	SetRotationAxis(new_axis);

	float speed = GetMovingSpeed();
	if (speed > 30.0f)
		SetRotationSpeed(30.0f);
	else
		SetRotationSpeed(60.0f);

	CRotatingObject::Animate(fTimeElapsed, pxmf4x4Parent);
}

void Asteroid::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	OnPrepareRender();

	if (culling_type == Culling_Type::Need_Culling)
		if (!IsVisible(pCamera))
			return;

	float red = (life)/100.0f;
	float green = (100.0f - life) / 100;
	outline_color = { red , green, 0 };

	UpdateShaderVariables(pd3dCommandList);
	CGameObject::UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);

	if (m_nMaterials > 0)
	{
		for (int i = 0; i < m_nMaterials; i++)
		{
			if (m_ppMaterials[i])
			{
				if (m_ppMaterials[i]->m_pShader)
					m_ppMaterials[i]->m_pShader->Render(pd3dCommandList, pCamera);

				m_ppMaterials[i]->UpdateShaderVariables(pd3dCommandList);
			}

			if (m_pMesh)
				((Asteroid_Mesh*)m_pMesh)->Asteroid_Mesh::Render(pd3dCommandList, 0, Overlaped_Box_N);

		}
	}
}

//==================================================

Bullet_Object::Bullet_Object(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
	: CRotatingObject(1)
{
	PrepareAnimate();
	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	oobb_drawer = new OOBB_Drawer();
	oobb_drawer->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	culling_type = Culling_Type::Need_Culling;

	SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
	SetRotationSpeed(90.0f);
	SetMovingSpeed(0.0f);
}
Bullet_Object::~Bullet_Object()
{
}

void Bullet_Object::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	CRotatingObject::Animate(fTimeElapsed, pxmf4x4Parent);
}

void Bullet_Object::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	CGameObject::Render(pd3dCommandList, pCamera);
}

//==================================================

Billboard_Object::Billboard_Object(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
	: CRotatingObject(1)
{
}

Billboard_Object::~Billboard_Object()
{
}

void Billboard_Object::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	CRotatingObject::Animate(fTimeElapsed, pxmf4x4Parent);

}

//==================================================

Black_Hole_Object::Black_Hole_Object(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
	: Billboard_Object(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	oobb_drawer = new OOBB_Drawer();
	oobb_drawer->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	Gravity_area = new BoundingOrientedBox(XMFLOAT3(0.0f,0.0f,0.0f), XMFLOAT3(100.0f,100.0f,100.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

Black_Hole_Object::~Black_Hole_Object()
{
}

void Black_Hole_Object::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	Billboard_Object::Animate(fTimeElapsed, pxmf4x4Parent);
}
void Black_Hole_Object::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	pd3dCommandList->SetGraphicsRoot32BitConstants(12, 1, &wave_info._Tiling, 0);
	pd3dCommandList->SetGraphicsRoot32BitConstants(12, 1, &wave_info._WaveSpeed, 1);
	pd3dCommandList->SetGraphicsRoot32BitConstants(12, 1, &wave_info._WaveFrequency, 2);
	pd3dCommandList->SetGraphicsRoot32BitConstants(12, 1, &wave_info._WaveAmplitude, 3);
	pd3dCommandList->SetGraphicsRoot32BitConstants(12, 4, &wave_info.Color_Change, 4);

	CGameObject::Render(pd3dCommandList, pCamera);
}


//==================================================

Billboard_Animation_Object::Billboard_Animation_Object(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
	: Billboard_Object(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	sprite_index = 0;
	active = true;
}

Billboard_Animation_Object::~Billboard_Animation_Object()
{
}

void Billboard_Animation_Object::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	sprite_index += 1; // sprite_index를 0 ~ 47 범위로 증가시킴
	if (sprite_index >= 48)
		active = false;
	
	CRotatingObject::Animate(fTimeElapsed, pxmf4x4Parent);
}

void Billboard_Animation_Object::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	pd3dCommandList->SetGraphicsRoot32BitConstants(13, 1, &sprite_index, 0); 
	pd3dCommandList->SetGraphicsRoot32BitConstants(13, 1, &blue_boom, 1);

	OnPrepareRender();

	if (culling_type == Culling_Type::Need_Culling)
		if (!IsVisible(pCamera))
			return;

	UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);

	if (m_nMaterials > 0)
	{
		for (int i = 0; i < m_nMaterials; i++)
		{
			if (m_ppMaterials[i])
			{
				if (m_ppMaterials[i]->m_pShader)
					m_ppMaterials[i]->m_pShader->Render(pd3dCommandList, pCamera);

				m_ppMaterials[i]->UpdateShaderVariables(pd3dCommandList);
			}

			if (m_pMesh)
				((Billboard_Mesh*)m_pMesh)->Render(pd3dCommandList, true);
		}
	}
}
