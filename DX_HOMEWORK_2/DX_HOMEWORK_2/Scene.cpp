#include "stdafx.h"
#include "Scene.h"
#include "Player.h"

XMFLOAT3& Get_Random_Normalize_Direction()
{
	float randX = static_cast<float>(rand()) / RAND_MAX; 
	float randY = static_cast<float>(rand()) / RAND_MAX; 
	float randZ = static_cast<float>(rand()) / RAND_MAX; 

	XMFLOAT3 random_D(randX, randY, randZ);

	XMVECTOR normalized_D= XMVector3Normalize(XMLoadFloat3(&random_D));

	XMStoreFloat3(&random_D, normalized_D);

	return random_D;
}

CScene::CScene()
{
}
CScene::~CScene()
{
}

void CScene::CreateGraphicsPipelineState(ID3D12Device* pd3dDevice)
{
	//정점 셰이더와 픽셀 셰이더를 생성한다.
	ID3DBlob* pd3dVertexShaderBlob = NULL;
	ID3DBlob* pd3dPixelShaderBlob = NULL;
	UINT nCompileFlags = 0;
#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	D3DCompileFromFile(L"Shaders.hlsl", NULL, NULL, "VSMain", "vs_5_1", nCompileFlags, 0,
		&pd3dVertexShaderBlob, NULL);
	D3DCompileFromFile(L"Shaders.hlsl", NULL, NULL, "PSMain", "ps_5_1", nCompileFlags, 0,
		&pd3dPixelShaderBlob, NULL);
	//래스터라이저 상태를 설정한다.
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
	//블렌드 상태를 설정한다.
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
	//그래픽 파이프라인 상태를 설정한다.
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

void CScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int mode)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	m_pShaders = new CObjectsShader[m_nShaders];
	m_pShaders[0].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);


	float fHalfWidth = 100.0f, fHalfHeight = 100.0f, fHalfDepth = 400.0f;

	CWallMesh* pWallMesh = new CWallMesh(pd3dDevice, pd3dCommandList, fHalfWidth * 2.0f, fHalfHeight * 2.0f, fHalfDepth * 2.0f, XMFLOAT4(0.0f, 0.0f, 0.3f, 0.0f));

	m_pWallsObject = new CWallsObject();
	m_pWallsObject->SetPosition(0.0f, 0.0f, 0.0f);
	m_pWallsObject->SetMesh(pWallMesh);
	m_pWallsObject->m_pxmf4WallPlanes[0] = XMFLOAT4(+1.0f, 0.0f, 0.0f, fHalfWidth);
	m_pWallsObject->m_pxmf4WallPlanes[1] = XMFLOAT4(-1.0f, 0.0f, 0.0f, fHalfWidth);
	m_pWallsObject->m_pxmf4WallPlanes[2] = XMFLOAT4(0.0f, +1.0f, 0.0f, fHalfHeight);
	m_pWallsObject->m_pxmf4WallPlanes[3] = XMFLOAT4(0.0f, -1.0f, 0.0f, fHalfHeight);
	m_pWallsObject->m_pxmf4WallPlanes[4] = XMFLOAT4(0.0f, 0.0f, +1.0f, fHalfDepth);
	m_pWallsObject->m_pxmf4WallPlanes[5] = XMFLOAT4(0.0f, 0.0f, -1.0f, fHalfDepth);
	m_pWallsObject->m_xmOOBBPlayerMoveCheck = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth -1.0f, fHalfHeight - 1.0f, fHalfDepth - 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	CExplosiveObject::PrepareExplosion(pd3dDevice, pd3dCommandList);



	CCubeMeshDiffused* pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 12.0f, 12.0f, 12.0f);
	CSphereMeshDiffused* pSphereMesh = new CSphereMeshDiffused(pd3dDevice, pd3dCommandList, 6.0f, 20, 20, XMFLOAT4(0.0f, 0.0f, 0.5f, 0.0f));
	CMesh* pAirplaneMesh = new CAirplaneMeshDiffused(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 4.0f, XMFLOAT4(0.2f, 0.0f, 0.0f, 0.0f));

	if (mode == 1)
	{
		m_nGameObjects = 2;
		m_ppGameObjects = new CGameObject * [m_nGameObjects];
		
		CExplosiveObject* FloatingCube = new CExplosiveObject();
		FloatingCube->SetMesh((CMesh*)pCubeMesh);
		FloatingCube->SetPosition(12.0f, 0.0f, 0.0f);

		FloatingCube->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 1.0f));
		FloatingCube->SetRotationSpeed(30.0f);
		FloatingCube->SetMovingDirection(Get_Random_Normalize_Direction());
		FloatingCube->SetMovingSpeed(10.5f);
		m_ppGameObjects[0] = FloatingCube;

		CExplosiveObject* FloatingSphere = new CExplosiveObject();
		FloatingSphere->SetMesh((CMesh*)pSphereMesh);
		FloatingSphere->SetPosition(36.0f, 0.0f, 0.0f);

		FloatingSphere->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 1.0f));
		FloatingSphere->SetRotationSpeed(30.0f);
		FloatingSphere->SetMovingDirection(Get_Random_Normalize_Direction());
		FloatingSphere->SetMovingSpeed(10.5f);
		m_ppGameObjects[1] = FloatingSphere;

	}
	else if (mode == 2)
	{
		
		int NNN = 1;
		int xObjects = NNN, yObjects = NNN, zObjects = NNN, i = 0;
		m_nGameObjects = (xObjects * 2 + 1) * (yObjects * 2 + 1) * (zObjects * 2 + 1);
		m_ppGameObjects = new CGameObject * [m_nGameObjects];

		float fxPitch = 12.0f * 2.5f;
		float fyPitch = 12.0f * 2.5f;
		float fzPitch = 12.0f * 2.5f;

		CEnemyObject* pEnemyObject = NULL;
		for (int x = -xObjects; x <= xObjects; x++)
		{
			for (int y = -yObjects; y <= yObjects; y++)
			{
				for (int z = -zObjects; z <= zObjects; z++)
				{
					pEnemyObject = new CEnemyObject(pd3dDevice, pd3dCommandList);
					pEnemyObject->SetMesh((CMesh*)pAirplaneMesh);
					pEnemyObject->SetPosition(fxPitch * x, fyPitch * y, fzPitch * z);

					pEnemyObject->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 1.0f));
					pEnemyObject->SetRotationSpeed(30.0f);
					pEnemyObject->SetMovingDirection(Get_Random_Normalize_Direction());
					pEnemyObject->SetMovingSpeed(10.5f);
					m_ppGameObjects[i++] = pEnemyObject;
				}
			}
		}
	}

	((CAirplaneMeshDiffused*)pAirplaneMesh)->Make_Another_Color_Mesh(pd3dDevice, pd3dCommandList);


}

void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) 
		m_pd3dGraphicsRootSignature->Release();


	for (int i = 0; i < m_nShaders; i++)
	{
		m_pShaders[i].ReleaseShaderVariables();
	}

	if (m_pShaders)
		delete[] m_pShaders;


	for (int i = 0; i < m_nGameObjects; i++)
		m_ppGameObjects[i]->Release();


}

void CScene::ReleaseUploadBuffers()
{
	for (int j = 0; j < m_nGameObjects; j++)
		m_ppGameObjects[j]->ReleaseUploadBuffers();
}




ID3D12RootSignature* CScene::GetGraphicsRootSignature()
{
	return(m_pd3dGraphicsRootSignature);
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
	lParam)
{
	return(false);
}
bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam,
	LPARAM lParam)
{
	return(false);
}

bool ProcessInput(UCHAR* pKeysBuffer)
{
	return(false);
}

ID3D12RootSignature* CScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;
	D3D12_ROOT_PARAMETER pd3dRootParameters[2];
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[0].Constants.Num32BitValues = 16;
	pd3dRootParameters[0].Constants.ShaderRegister = 0;
	pd3dRootParameters[0].Constants.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues = 32;
	pd3dRootParameters[1].Constants.ShaderRegister = 1;
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
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

void CScene::CheckObjectByObjectCollisions()
{
	for (int i = 0; i < m_nGameObjects; i++)
		m_ppGameObjects[i]->m_pObjectCollided = NULL;

	for (int i = 0; i < m_nGameObjects; i++)
	{
		for (int j = (i + 1); j < m_nGameObjects; j++)
		{
			if (m_ppGameObjects[i]->m_xmOOBB.Intersects(m_ppGameObjects[j]->m_xmOOBB))
			{
				m_ppGameObjects[i]->m_pObjectCollided = m_ppGameObjects[j];
				m_ppGameObjects[j]->m_pObjectCollided = m_ppGameObjects[i];
			}
		}
	}

	for (int i = 0; i < m_nGameObjects; i++)
	{
		if (m_ppGameObjects[i]->m_pObjectCollided)
		{
			XMFLOAT3 xmf3MovingDirection = m_ppGameObjects[i]->m_xmf3MovingDirection;
			float fMovingSpeed = m_ppGameObjects[i]->m_fMovingSpeed;

			m_ppGameObjects[i]->m_xmf3MovingDirection = m_ppGameObjects[i]->m_pObjectCollided->m_xmf3MovingDirection;
			m_ppGameObjects[i]->m_fMovingSpeed = m_ppGameObjects[i]->m_pObjectCollided->m_fMovingSpeed;
			m_ppGameObjects[i]->m_pObjectCollided->m_xmf3MovingDirection = xmf3MovingDirection;
			m_ppGameObjects[i]->m_pObjectCollided->m_fMovingSpeed = fMovingSpeed;
			m_ppGameObjects[i]->m_pObjectCollided->m_pObjectCollided = NULL;
			m_ppGameObjects[i]->m_pObjectCollided = NULL;
		}
	}
}

void CScene::CheckObjectByWallCollisions()
{
	for (int i = 0; i < m_nGameObjects; i++)
	{
		ContainmentType containType = m_pWallsObject->m_xmOOBB.Contains(m_ppGameObjects[i]->m_xmOOBB);
		switch (containType)
		{
		case DISJOINT:
		{
			int nPlaneIndex = -1;
			for (int j = 0; j < 6; j++)
			{
				PlaneIntersectionType intersectType = m_ppGameObjects[i]->m_xmOOBB.Intersects(XMLoadFloat4(&m_pWallsObject->m_pxmf4WallPlanes[j]));
				if (intersectType == BACK)
				{
					nPlaneIndex = j;
					break;
				}
			}
			if (nPlaneIndex != -1)
			{
				XMVECTOR xmvNormal = XMVectorSet(m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].x, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].y, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
				XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(&m_ppGameObjects[i]->m_xmf3MovingDirection), xmvNormal);
				XMStoreFloat3(&m_ppGameObjects[i]->m_xmf3MovingDirection, xmvReflect);
			}
			break;
		}
		case INTERSECTS:
		{
			int nPlaneIndex = -1;
			for (int j = 0; j < 6; j++)
			{
				PlaneIntersectionType intersectType = m_ppGameObjects[i]->m_xmOOBB.Intersects(XMLoadFloat4(&m_pWallsObject->m_pxmf4WallPlanes[j]));
				if (intersectType == INTERSECTING)
				{
					nPlaneIndex = j;
					break;
				}
			}
			if (nPlaneIndex != -1)
			{
				XMVECTOR xmvNormal = XMVectorSet(m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].x, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].y, m_pWallsObject->m_pxmf4WallPlanes[nPlaneIndex].z, 0.0f);
				XMVECTOR xmvReflect = XMVector3Reflect(XMLoadFloat3(&m_ppGameObjects[i]->m_xmf3MovingDirection), xmvNormal);
				XMStoreFloat3(&m_ppGameObjects[i]->m_xmf3MovingDirection, xmvReflect);
			}
			break;
		}
		case CONTAINS:
			break;
		}
	}
}

void CScene::CheckPlayerByWallCollision(DWORD camera_mode)
{
	BoundingOrientedBox xmOOBBPlayerMoveCheck;
	m_pWallsObject->m_xmOOBBPlayerMoveCheck.Transform(xmOOBBPlayerMoveCheck, XMLoadFloat4x4(&m_pWallsObject->m_xmf4x4World));
	XMStoreFloat4(&xmOOBBPlayerMoveCheck.Orientation, XMQuaternionNormalize(XMLoadFloat4(&xmOOBBPlayerMoveCheck.Orientation)));

	if (camera_mode == THIRD_PERSON_CAMERA)
	{
		BoundingOrientedBox xmPlayerBoundingBox = m_pPlayer->m_pMesh->GetBoundingBox();

		//모델 좌표계의 바운딩 박스를 월드 좌표계로 변환한다. 
		xmPlayerBoundingBox.Transform(xmPlayerBoundingBox, XMLoadFloat4x4(&m_pPlayer->m_xmf4x4World));

		if (!xmOOBBPlayerMoveCheck.Intersects(xmPlayerBoundingBox))
		{
			XMFLOAT3 p_pos = m_pPlayer->GetPosition();
			XMFLOAT3 w_pos = m_pWallsObject->GetPosition();

			// 벽이 이동할 거리
			XMVECTOR moveDistance = XMLoadFloat3(&p_pos) - XMLoadFloat3(&w_pos);

			// 모든 게임 객체의 위치를 해당 거리만큼 이동
			for (int i = 0; i < m_nGameObjects; i++)
			{
				if (m_pWallsObject->m_xmOOBB.Contains(m_ppGameObjects[i]->m_xmOOBB))
				{
					XMFLOAT3& ObjectPos = m_ppGameObjects[i]->GetPosition();

					XMFLOAT3 new_ObjectPos;
					XMStoreFloat3(&new_ObjectPos, XMLoadFloat3(&ObjectPos) + moveDistance);

					m_ppGameObjects[i]->SetPosition(new_ObjectPos);
				}
			}

			// 벽 이동
			m_pWallsObject->SetPosition(p_pos);
			m_pWallsObject->m_xmOOBB.Center = p_pos;

		}
	}
	else
	{
		if (!xmOOBBPlayerMoveCheck.Contains(XMLoadFloat3(&m_pPlayer->GetCamera_Pos())))
		{
			XMFLOAT3 c_pos = m_pPlayer->GetCamera_Pos();
			XMFLOAT3 w_pos = m_pWallsObject->GetPosition();

			// 벽이 이동할 거리
			XMVECTOR moveDistance = XMLoadFloat3(&c_pos) - XMLoadFloat3(&w_pos);

			// 모든 게임 객체의 위치를 해당 거리만큼 이동
			for (int i = 0; i < m_nGameObjects; i++)
			{
				if (m_pWallsObject->m_xmOOBB.Contains(m_ppGameObjects[i]->m_xmOOBB))
				{
					XMFLOAT3& ObjectPos = m_ppGameObjects[i]->GetPosition();

					XMFLOAT3 new_ObjectPos;
					XMStoreFloat3(&new_ObjectPos, XMLoadFloat3(&ObjectPos) + moveDistance);

					m_ppGameObjects[i]->SetPosition(new_ObjectPos);
				}
			}

			// 벽 이동
			m_pWallsObject->SetPosition(c_pos);
			m_pWallsObject->m_xmOOBB.Center = c_pos;

		}
	}
}

void CScene::CheckObjectByBulletCollisions()
{
	CBulletObject** ppBullets = ((CAirplanePlayer*)m_pPlayer)->m_ppBullets;
	for (int i = 0; i < m_nGameObjects; i++)
	{
		CExplosiveObject* pExplosiveObject = (CExplosiveObject*)m_ppGameObjects[i];

		for (int j = 0; j < BULLETS; j++)
		{
			if (ppBullets[j]->m_bActive && !pExplosiveObject->m_bBlowingUp)
			{
				if (m_ppGameObjects[i]->m_xmOOBB.Intersects(ppBullets[j]->m_xmOOBB))
				{
					pExplosiveObject->m_bBlowingUp = true;
					ppBullets[j]->Reset();
				}
			}
		}
	}
}

void CScene::CheckPlayerByBulletCollisions()
{
	BoundingOrientedBox xmPlayerBoundingBox = m_pPlayer->m_pMesh->GetBoundingBox();

	//모델 좌표계의 바운딩 박스를 월드 좌표계로 변환한다. 
	xmPlayerBoundingBox.Transform(xmPlayerBoundingBox, XMLoadFloat4x4(&m_pPlayer->m_xmf4x4World));


	for (int i = 0; i < m_nGameObjects; i++)
	{
		for (int j = 0; j < E_BULLETS; j++)
		{
			CEnemyBulletObject** ppBullets = static_cast<CEnemyObject*>(m_ppGameObjects[i])->m_ppBullets;
			if (ppBullets[j]->m_bActive && !ppBullets[j]->crashed && xmPlayerBoundingBox.Intersects(ppBullets[j]->m_xmOOBB))
			{
				ppBullets[j]->crashed = true;
				ppBullets[j]->m_fExplosionSpeed = 50.0f;
				CAirplanePlayer* pPlayer = static_cast<CAirplanePlayer*>(m_pPlayer);
				
				if (pPlayer->Life > 0)
				{
					if (!pPlayer->m_Armored)
					{
						pPlayer->Life -= 1;
						pPlayer->m_Armored = true;
						pPlayer->Draw_Wire = true;
					}
				}
				else
				{
					pPlayer->Life = 0;
				}
			}
		}
	}
}


void CScene::CheckBarrierByBulletCollisions()
{
	for (int i = 0; i < m_nGameObjects; i++)
	{
		for (int j = 0; j < E_BULLETS; j++)
		{
			CEnemyBulletObject** ppBullets = static_cast<CEnemyObject*>(m_ppGameObjects[i])->m_ppBullets;
			CBarrierObject* pBarrier = static_cast<CAirplanePlayer*>(m_pPlayer)->m_pBarrier;
			if (ppBullets[j]->m_bActive && !ppBullets[j]->crashed && !pBarrier->m_bBlowingUp)
			{
				if (pBarrier->m_xmBSphere.Intersects(ppBullets[j]->m_xmOOBB))
				{

					ppBullets[j]->SetMovingDirection_Reverse();
					ppBullets[j]->crashed = true;
					ppBullets[j]->m_fExplosionSpeed = 30.0f;

					if (pBarrier->life > 1)
					{
						pBarrier->life -= 1;
						float new_speed = 200 - 20 * (10 - pBarrier->life);
						pBarrier->SetRotationSpeed(new_speed);
					}
					else
					{
						pBarrier->m_bBlowingUp = true;
						static_cast<CAirplanePlayer*>(m_pPlayer)->barrier_mode = false;
					}
				}
			}
		}
	}
}


void CScene::AnimateObjects(float fTimeElapsed)
{
	for (int j = 0; j < m_nGameObjects; j++)
	{
		((CRotatingObject*)m_ppGameObjects[j])->Rotate_to_Player(fTimeElapsed, XMFLOAT3{ m_pPlayer->GetPosition() });
		m_ppGameObjects[j]->Animate(fTimeElapsed);

		if (m_pSelectedObject != m_ppGameObjects[j])
		{
			m_ppGameObjects[j]->picked = false;
		}
	}

	m_pWallsObject->Animate(fTimeElapsed);

	if (scene_num == 1)
	{
		if (((CExplosiveObject*)m_ppGameObjects[0])->m_bBlowingUp)
			is_killed_1 = true;

		if (((CExplosiveObject*)m_ppGameObjects[1])->m_bBlowingUp)
			is_killed_2 = true;

		if (is_killed_1 && is_killed_2)
		{
			scene_change = true;
		}
	}
	else 
	{
		if (((CAirplanePlayer*)m_pPlayer)->game_over && !((CAirplanePlayer*)m_pPlayer)->m_bBlowingUp)
		{
			scene_change = true;
		}
	}

	((CAirplanePlayer*)m_pPlayer)->Animate(fTimeElapsed);
}

void CScene::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	m_pShaders[0].Render(pd3dCommandList, pCamera, false);

	
	m_pWallsObject->Render(pd3dCommandList, pCamera);


	for (int i = 0; i < m_nGameObjects; i++)
	{
		m_pShaders[0].Render(pd3dCommandList, pCamera, m_ppGameObjects[i]->Draw_Wire);
		m_ppGameObjects[i]->Render(pd3dCommandList, pCamera);
	}

	CAirplanePlayer* pPlayer = (CAirplanePlayer*)m_pPlayer;

	//=======================플레이어 렌더링 =======================

	if (m_pPlayer)
	{
		pPlayer->Render(pd3dCommandList, pCamera);
	}

	m_pShaders[0].Render(pd3dCommandList, pCamera, true);

	if (pPlayer->m_pBarrier->barrier_mode)
	{
		pPlayer->m_pBarrier->Render(pd3dCommandList, pCamera);
	}
}


CGameObject* CScene::PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera)
{
	if (!pCamera) return(NULL);
	XMFLOAT4X4 xmf4x4View = pCamera->GetViewMatrix();
	XMFLOAT4X4 xmf4x4Projection = pCamera->GetProjectionMatrix();
	D3D12_VIEWPORT d3dViewport = pCamera->GetViewport();
	XMFLOAT3 xmf3PickPosition;
	/*화면 좌표계의 점 (xClient, yClient)를 화면 좌표 변환의 역변환과 투영 변환의 역변환을 한다. 
	그 결과는 카메라 좌표계의 점이다. 투영 평면이 카메라에서 z-축으로 거리가 1이므로 z-좌표는 1로 설정한다.*/
	xmf3PickPosition.x = (((2.0f * xClient) / d3dViewport.Width) - 1) / xmf4x4Projection._11;
	xmf3PickPosition.y = -(((2.0f * yClient) / d3dViewport.Height) - 1) / xmf4x4Projection._22;
	xmf3PickPosition.z = 1.0f;
	int nIntersected = 0;
	float fHitDistance = FLT_MAX, fNearestHitDistance = FLT_MAX;
	CGameObject* pIntersectedObject = NULL, * pNearestObject = NULL;

	pIntersectedObject = PickObjectByRayIntersection(xmf3PickPosition,xmf4x4View, &fHitDistance);
	if (pIntersectedObject && (fHitDistance < fNearestHitDistance))
	{
		fNearestHitDistance = fHitDistance;
		pNearestObject = pIntersectedObject;
	}

	return(pNearestObject);
}


CGameObject* CScene::PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfNearHitDistance)
{
	int nIntersected = 0;
	*pfNearHitDistance = FLT_MAX;
	float fHitDistance = FLT_MAX;
	CGameObject* pSelectedObject = NULL;
	for (int j = 0; j < m_nGameObjects; j++)
	{
		nIntersected = m_ppGameObjects[j]->PickObjectByRayIntersection(xmf3PickPosition,
			xmf4x4View, &fHitDistance);
		if ((nIntersected > 0) && (fHitDistance < *pfNearHitDistance))
		{
			*pfNearHitDistance = fHitDistance;
			pSelectedObject = m_ppGameObjects[j];
		}
	}
	return(pSelectedObject);
}