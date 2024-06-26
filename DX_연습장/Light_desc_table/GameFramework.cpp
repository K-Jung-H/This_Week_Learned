#include "stdafx.h"
#include "GameFramework.h"

CGameFramework::CGameFramework()
{
	m_pdxgiFactory = NULL;
	m_pdxgiSwapChain = NULL;
	m_pd3dDevice = NULL;
	m_pd3dCommandAllocator = NULL;
	m_pd3dCommandQueue = NULL;
	m_pd3dPipelineState = NULL;
	m_pd3dCommandList = NULL;
	m_pd3dRtvDescriptorHeap = NULL;
	m_nRtvDescriptorIncrementSize = 0;
	m_pd3dDepthStencilBuffer = NULL;
	m_pd3dDsvDescriptorHeap = NULL;
	m_nDsvDescriptorIncrementSize = 0;
	m_nSwapChainBufferIndex = 0;
	m_hFenceEvent = NULL;
	m_pd3dFence = NULL;

	m_nWndClientWidth = FRAME_BUFFER_WIDTH;
	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;

	m_pScene = NULL;

	for (int i = 0; i < m_nSwapChainBuffers; i++)
		m_ppd3dRenderTargetBuffers[i] = NULL;

	for (int i = 0; i < m_nSwapChainBuffers; i++)
		m_nFenceValues[i] = 0;

	_tcscpy_s(m_pszFrameRate, _T("LapProject ("));
}
CGameFramework::~CGameFramework()
{
}

//다음 함수는 응용 프로그램이 실행되어 주 윈도우가 생성되면 호출된다는 것에 유의하라. 
bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	CreateDirect3DDevice(); // Direct3D 디바이스 생성
	CreateCommandQueueAndList(); // 명령 큐, 리스트 생성
	CreateRtvAndDsvDescriptorHeaps(); // RTV, DSV 힙 생성
	CreateSwapChain(); // 스왑 체인 생성

	CreateDepthStencilView(); // DSV 생성

	BuildObjects(); //렌더링할 게임 객체를 생성한다. 
	return(true);
}
void CGameFramework::OnDestroy()
{
	WaitForGpuComplete(); 	//GPU가 모든 명령 리스트를 실행할 때 까지 기다린다. 

	ReleaseObjects(); 	//게임 객체(게임 월드 객체)를 소멸한다. 

	::CloseHandle(m_hFenceEvent);

	for (int i = 0; i < m_nSwapChainBuffers; i++) 
		if (m_ppd3dRenderTargetBuffers[i])
			m_ppd3dRenderTargetBuffers[i]->Release();

	if (m_pd3dRtvDescriptorHeap) m_pd3dRtvDescriptorHeap->Release();
	if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
	if (m_pd3dDsvDescriptorHeap) m_pd3dDsvDescriptorHeap->Release();
	if (m_pd3dCommandAllocator) m_pd3dCommandAllocator->Release();
	if (m_pd3dCommandQueue) m_pd3dCommandQueue->Release();
	if (m_pd3dPipelineState) m_pd3dPipelineState->Release();
	if (m_pd3dCommandList) m_pd3dCommandList->Release();
	if (m_pd3dFence) m_pd3dFence->Release();

	m_pdxgiSwapChain->SetFullscreenState(FALSE, NULL);

	if (m_pdxgiSwapChain) m_pdxgiSwapChain->Release();
	if (m_pd3dDevice) m_pd3dDevice->Release();
	if (m_pdxgiFactory) m_pdxgiFactory->Release();

#if defined(_DEBUG)
	IDXGIDebug1* pdxgiDebug = NULL;
	DXGIGetDebugInterface1(0, __uuidof(IDXGIDebug1), (void**)&pdxgiDebug);
	HRESULT hResult = pdxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
	pdxgiDebug->Release();
#endif
}

void CGameFramework::CreateSwapChain()
{
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	m_nWndClientWidth = rcClient.right - rcClient.left;
	m_nWndClientHeight = rcClient.bottom - rcClient.top;

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
	dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers;
	dxgiSwapChainDesc.BufferDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.BufferDesc.Height = m_nWndClientHeight;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.OutputWindow = m_hWnd;
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	dxgiSwapChainDesc.Windowed = TRUE;

	// 전체화면 모드에서 바탕화면의 해상도를 후면버퍼의 크기에 맞게 변경
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; 	

	HRESULT hResult = m_pdxgiFactory->CreateSwapChain(m_pd3dCommandQueue,
		&dxgiSwapChainDesc, (IDXGISwapChain**)&m_pdxgiSwapChain);
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

	hResult = m_pdxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);

#ifndef _WITH_SWAPCHAIN_FULLSCREEN_STATE
	CreateRenderTargetViews();

#endif
}


void CGameFramework::ChangeSwapChainState()
{
	WaitForGpuComplete();

	BOOL bFullScreenState = FALSE;
	m_pdxgiSwapChain->GetFullscreenState(&bFullScreenState, NULL);
	m_pdxgiSwapChain->SetFullscreenState(!bFullScreenState, NULL);

	DXGI_MODE_DESC dxgiTargetParameters{};
	dxgiTargetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiTargetParameters.Width = m_nWndClientWidth;
	dxgiTargetParameters.Height = m_nWndClientHeight;
	dxgiTargetParameters.RefreshRate.Numerator = 60;
	dxgiTargetParameters.RefreshRate.Denominator = 1;
	dxgiTargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiTargetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	m_pdxgiSwapChain->ResizeTarget(&dxgiTargetParameters);

	for (int i = 0; i < m_nSwapChainBuffers; i++)
		if (m_ppd3dRenderTargetBuffers[i])
			m_ppd3dRenderTargetBuffers[i]->Release();

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	m_pdxgiSwapChain->GetDesc(&dxgiSwapChainDesc);
	m_pdxgiSwapChain->ResizeBuffers(m_nSwapChainBuffers, m_nWndClientWidth,
		m_nWndClientHeight, dxgiSwapChainDesc.BufferDesc.Format, dxgiSwapChainDesc.Flags);
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
	CreateRenderTargetViews();

}

void CGameFramework::CreateDirect3DDevice()
{
	HRESULT hResult;
	UINT nDXGIFactoryFlags = 0;
#if defined(_DEBUG)
	ID3D12Debug* pd3dDebugController = NULL;
	hResult = D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void**)&pd3dDebugController);
	if (pd3dDebugController)
	{
		pd3dDebugController->EnableDebugLayer();
		pd3dDebugController->Release();
	}
	nDXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
	hResult = ::CreateDXGIFactory2(nDXGIFactoryFlags, __uuidof(IDXGIFactory4), (void**)&m_pdxgiFactory);
	IDXGIAdapter1* pd3dAdapter = NULL;
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_pdxgiFactory->EnumAdapters1(i,&pd3dAdapter); i++)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
		pd3dAdapter->GetDesc1(&dxgiAdapterDesc);
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) 
			continue;
		if (SUCCEEDED(D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_12_0,_uuidof(ID3D12Device), (void**)&m_pd3dDevice))) 
			break;
	}
	//모든 하드웨어 어댑터 대하여 특성 레벨 12.0을 지원하는 하드웨어 디바이스를 생성
	if (!pd3dAdapter)
	{
		m_pdxgiFactory->EnumWarpAdapter(_uuidof(IDXGIAdapter1), (void**)&pd3dAdapter);
		D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), (void**)&m_pd3dDevice);
	}
	//특성 레벨 12.0을 지원하는 하드웨어 디바이스를 생성할 수 없으면 WARP 디바이스를 생성
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount = 4; //Msaa4x 다중 샘플링
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;

	d3dMsaaQualityLevels.NumQualityLevels = 0;
	m_pd3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels; 	// 디바이스가 지원하는 다중 샘플의 품질 수준을 확인

	m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false; // 다중 샘플의 품질 수준이 1보다 크면 다중 샘플링을 활성화

	hResult = m_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&m_pd3dFence);

	for (UINT i = 0; i < m_nSwapChainBuffers; i++) // 펜스를 생성하고 펜스 값을 0으로 설정
		m_nFenceValues[i] = 0; 

	// 펜스와 동기화를 위한 이벤트 객체를 생성 - 이벤트 객체의 초기값 FALSE 
	// 이벤트가 실행되면(Signal) -> 이벤트의 값을 자동적으로 FALSE가 되도록 생성
	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL); 

	if (pd3dAdapter) 
		pd3dAdapter->Release();

	::gnCbvSrvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	m_nRtvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_nDsvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

}

void CGameFramework::CreateCommandQueueAndList()
{
	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;
	::ZeroMemory(&d3dCommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3dCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	HRESULT hResult = m_pd3dDevice->CreateCommandQueue(&d3dCommandQueueDesc, // 직접(Direct) 명령 큐를 생성 
		_uuidof(ID3D12CommandQueue), (void**)&m_pd3dCommandQueue);
	
	hResult = m_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, // 직접(Direct) 명령 할당자를 생성
		__uuidof(ID3D12CommandAllocator), (void**)&m_pd3dCommandAllocator);

	hResult = m_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, // 직접(Direct) 명령 리스트를 생성 
	m_pd3dCommandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&m_pd3dCommandList);
		
	hResult = m_pd3dCommandList->Close(); // 명령 리스트는 생성시 열린(Open) 상태, 닫힌(Closed) 상태로 전환 
}

void CGameFramework::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDescriptorHeapDesc.NumDescriptors = m_nSwapChainBuffers;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask = 0;

	HRESULT hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, 	// RTV 힙(서술자의 개수 = 스왑체인 버퍼의 개수)을 생성
		__uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dRtvDescriptorHeap);

	m_nRtvDescriptorIncrementSize =
		m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV); // RTV 힙의 원소의 크기 저장

	d3dDescriptorHeapDesc.NumDescriptors = 1;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc,  	// DSV 힙(서술자의 개수 = 1)을 생성
		__uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dDsvDescriptorHeap);

	m_nDsvDescriptorIncrementSize =
		m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV); // DSV 힙의 원소의 크기 저장

}

void CGameFramework::CreateRenderTargetViews()
{
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle =
		m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < m_nSwapChainBuffers; i++)
	{
		m_pdxgiSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&m_ppd3dRenderTargetBuffers[i]);
		m_pd3dDevice->CreateRenderTargetView(m_ppd3dRenderTargetBuffers[i], NULL, d3dRtvCPUDescriptorHandle);
		d3dRtvCPUDescriptorHandle.ptr += m_nRtvDescriptorIncrementSize;
	}
}

void CGameFramework::CreateDepthStencilView()
{
	D3D12_RESOURCE_DESC d3dResourceDesc;
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = m_nWndClientWidth;
	d3dResourceDesc.Height = m_nWndClientHeight;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dResourceDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	d3dResourceDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES d3dHeapProperties;
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapProperties.CreationNodeMask = 1;
	d3dHeapProperties.VisibleNodeMask = 1;

	D3D12_CLEAR_VALUE d3dClearValue;
	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;
	
	m_pd3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE, // DSV 버퍼를 생성
		&d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue,
		__uuidof(ID3D12Resource), (void**)&m_pd3dDepthStencilBuffer);
	
	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle =
		m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, NULL, d3dDsvCPUDescriptorHandle); //DSV 버퍼 뷰를 생성	
}

void CGameFramework::BuildObjects()
{
	m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);

	m_pScene = new CScene();
	if (m_pScene) 
		m_pScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList); // 씬 객체를 생성, 씬에 포함될 게임 객체 생성

	CAirplanePlayer* pAirplanePlayer = new CAirplanePlayer(m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature());
	
	m_pScene->m_pPlayer = m_pPlayer = pAirplanePlayer;
	m_pCamera = m_pPlayer->GetCamera();

	m_pd3dCommandList->Close();
	ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	WaitForGpuComplete(); // 그래픽 명령 리스트들이 모두 실행될 때까지 대기

	if (m_pScene) 
		m_pScene->ReleaseUploadBuffers(); // 그래픽 리소스들을 생성하는 과정에 생성된 업로드 버퍼들을 소멸

	if (m_pPlayer) 
		m_pPlayer->ReleaseUploadBuffers(); // 그래픽 리소스들을 생성하는 과정에 생성된 업로드 버퍼들을 소멸

	m_GameTimer.Reset();
}


void CGameFramework::ReleaseObjects()
{
	if (m_pScene) m_pScene->ReleaseObjects();
	if (m_pScene) delete m_pScene;
}



void CGameFramework::ProcessInput()
{
	static UCHAR pKeyBuffer[256];
	DWORD dwDirection = 0;

	// 키보드의 상태 정보를 반환 
	// 화살표 키를 누르면 플레이어를 오른쪽/왼쪽/앞/뒤로 이동
	// Page Up / Page Down 키를 누르면 플레이어를 위/아래로 이동
	if (::GetKeyboardState(pKeyBuffer))
	{
		if (pKeyBuffer[VK_UP] & 0xF0) dwDirection |= DIR_FORWARD;
		if (pKeyBuffer[VK_DOWN] & 0xF0) dwDirection |= DIR_BACKWARD;
		if (pKeyBuffer[VK_LEFT] & 0xF0) dwDirection |= DIR_LEFT;
		if (pKeyBuffer[VK_RIGHT] & 0xF0) dwDirection |= DIR_RIGHT;
		if (pKeyBuffer[VK_PRIOR] & 0xF0) dwDirection |= DIR_UP;
		if (pKeyBuffer[VK_NEXT] & 0xF0) dwDirection |= DIR_DOWN;
	}

	float cxDelta = 0.0f, cyDelta = 0.0f;
	POINT ptCursorPos;

	// 마우스를 캡쳐했다면 마우스가 얼마만큼 이동하였는 가를 계산 
	// 마우스 캡쳐 = 마우스 버튼이 눌려진 상태
	// 마우스 버튼이 눌려진 상태에서 마우스를 좌우 또는 상하로 움직이면 플레이어를 x축 또는 y축으로 회전
	if (::GetCapture() == m_hWnd)
	{
		// 마우스 커서를 화면에서 없앰
		::SetCursor(NULL);
		
		// 현재 마우스 커서의 위치를 가져옴 
		::GetCursorPos(&ptCursorPos);

		// 마우스 버튼이 눌린 상태에서 마우스가 움직인 양을 구함 
		cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;

		//마우스 커서의 위치를 마우스가 눌려졌던 위치로 설정
		::SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
	}


	// 마우스 또는 키 입력이 있다면 플레이어를 이동 회전
	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
	{
		if (cxDelta || cyDelta)
		{
			// cxDelta는 y축의 회전
			// cyDelta는 x축의 회전
			// cxDelta는 z축의 회전 - 오른쪽 마우스 버튼이 눌려진 경우 

			if (pKeyBuffer[VK_RBUTTON] & 0xF0)
				m_pPlayer->Rotate(cyDelta, 0.0f, -cxDelta);
			else
				m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
		}
		
		// 플레이어를 dwDirection 방향으로 이동
		// 이동 거리는 시간에 비례 
		if (dwDirection) 
			m_pPlayer->Move(dwDirection, 50.0f * m_GameTimer.GetTimeElapsed(), true);
	}

	// 플레이어를 실제로 이동하고 카메라를 갱신 
	// 중력과 마찰력의 영향을 속도 벡터에 적용
	m_pPlayer->Update(m_GameTimer.GetTimeElapsed());
}

void CGameFramework::AnimateObjects()
{
	if (m_pScene) m_pScene->AnimateObjects(m_GameTimer.GetTimeElapsed());
}

void CGameFramework::WaitForGpuComplete()
{
	UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex]; // CPU 펜스의 값을 증가 
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue); // GPU가 펜스의 값을 설정하는 명령을 명령 큐에 추가
	if (m_pd3dFence->GetCompletedValue() < nFenceValue) // 펜스의 현재 값이 설정한 값이 될 때까지 기다림
	{
		hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void CGameFramework::MoveToNextFrame()
{
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

	UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);
	
	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

//#define _WITH_PLAYER_TOP

void CGameFramework::FrameAdvance()
{
	m_GameTimer.Tick(0.0f); //타이머의 시간이 갱신 + 프레임 레이트 계산
	ProcessInput();
	AnimateObjects();

	HRESULT hResult = m_pd3dCommandAllocator->Reset(); // 명령 할당자 리셋
	hResult = m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL); // 명령 리스트 리셋

	D3D12_RESOURCE_BARRIER d3dResourceBarrier;
	::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; // 현재 렌더 타겟에 대한 프리젠트가 끝나기를 기다린다.
	d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier.Transition.pResource = m_ppd3dRenderTargetBuffers[m_nSwapChainBufferIndex];
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; // 프리젠트가 끝나면 D3D12_RESOURCE_STATE_PRESENT 상태에서
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // D3D12_RESOURCE_STATE_RENDER_TARGET 상태로 전환
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);



	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle =
		m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(); // RTV 서술자 힙의 시작 주소 
	d3dRtvCPUDescriptorHandle.ptr += (m_nSwapChainBufferIndex * m_nRtvDescriptorIncrementSize);  // 현재의 RTV의 서술자의 CPU 주소(핸들)를 계산
	
	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle =
		m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(); // DSV 서술자 힙 시작 주소 -> DSV 1개 == 주소 계산 X



	float pfClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; //원하는 색상으로 렌더 타겟(뷰)을 지움
	m_pd3dCommandList->ClearRenderTargetView(d3dRtvCPUDescriptorHandle, pfClearColor/*Colors::Azure*/, 0, NULL);
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle,  D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

	m_pd3dCommandList->OMSetRenderTargets(1, &d3dRtvCPUDescriptorHandle, TRUE, &d3dDsvCPUDescriptorHandle); // RTV와 DSV를 OM에 연결

	if (m_pScene) 
		m_pScene->Render(m_pd3dCommandList, m_pCamera);

	//3인칭 카메라일 때 플레이어가 항상 보이도록 렌더링한다. 

#ifdef _WITH_PLAYER_TOP
	//렌더 타겟은 그대로 두고 깊이 버퍼를 1.0으로 지우고 플레이어를 렌더링하면 플레이어는 무조건 그려질 것
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, 
	D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
#endif
	//3인칭 카메라일 때 플레이어를 렌더링한다. 
	if (m_pPlayer) 
		m_pPlayer->Render(m_pd3dCommandList, m_pCamera);

	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);


	hResult = m_pd3dCommandList->Close(); // 명령 리스트를 닫힌 상태로 만듬
	ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList }; // 명령 리스트를 명령 큐에 추가
	m_pd3dCommandQueue->ExecuteCommandLists(1/*_countof(ppd3dCommandLists)*/, ppd3dCommandLists);  // 명령 리스트를 실행


	WaitForGpuComplete(); // GPU가 모든 명령 리스트를 실행할 때 까지 대기

	m_pdxgiSwapChain->Present(0, 0); // 스왑체인을 프리젠트

	MoveToNextFrame();

	// 현재의 프레임 레이트를 문자열로 가져와서 주 윈도우의 타이틀로 출력
	// m_pszBuffer 문자열이 "LapProject ("으로 초기화되었으므로 
	// (m_pszFrameRate+12)에서부터 프레임 레이트를 문자열로 출력하여 " FPS)" 문자열과 합침
	// ::_itow_s(m_nCurrentFrameRate, (m_pszFrameRate+12), 37, 10);
	// ::wcscat_s((m_pszFrameRate+12), 37, _T(" FPS)"));
	m_GameTimer.GetFrameRate(m_pszFrameRate + 12, 37);
	::SetWindowText(m_hWnd, m_pszFrameRate);
}

void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		//마우스 캡쳐를 하고 현재 마우스 위치를 저장
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		::ReleaseCapture();	//마우스 캡쳐를 해제 
		break;
	}
}

void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_F1: // 1인칭 카메라
		case VK_F2: // 스페이스-쉽 카메라
		case VK_F3: // 3인칭 카메라
			if (m_pPlayer)
				m_pCamera = m_pPlayer->ChangeCamera((wParam - VK_F1 + 1), m_GameTimer.GetTimeElapsed());
			break;

		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;

		case VK_RETURN:
			break;

		//윈도우 / 전체화면 모드 전환
		case VK_F9:
			ChangeSwapChainState();
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_SIZE:
	{
		m_nWndClientWidth = LOWORD(lParam);
		m_nWndClientHeight = HIWORD(lParam);
		break;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return(0);
}