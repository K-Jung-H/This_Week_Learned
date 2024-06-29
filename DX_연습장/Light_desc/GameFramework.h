#pragma once
#include "Timer.h"
#include "Scene.h"
#include "Camera.h"
//#include "Player.h"

class CPlayer;

class CGameFramework
{
private:
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	int m_nWndClientWidth;
	int m_nWndClientHeight;

	IDXGIFactory4* m_pdxgiFactory; // DXGI 팩토리 인터페이스에 대한 포인터
	IDXGISwapChain3* m_pdxgiSwapChain; // 스왑 체인 인터페이스에 대한 포인터 - 주로 디스플레이를 제어하기 위하여 필요
	ID3D12Device* m_pd3dDevice; // Direct3D 디바이스 인터페이스에 대한 포인터 - 주로 리소스를 생성하기 위하여 필요

	bool m_bMsaa4xEnable = false; // MSAA 다중 샘플링을 활성화
	UINT m_nMsaa4xQualityLevels = 0; // 다중 샘플링 레벨을 설정

	//=================스왑 체인=================
	static const UINT m_nSwapChainBuffers = 2; // 스왑 체인 후면 버퍼의 개수
	UINT m_nSwapChainBufferIndex; // 현재 스왑 체인의 후면 버퍼 인덱스

	//=================(RTV)렌더 타겟=================
	ID3D12Resource* m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers]; // 렌더 타겟 (Rtv)버퍼
	ID3D12DescriptorHeap* m_pd3dRtvDescriptorHeap; // Rtv 서술자 힙 인터페이스 포인터
	UINT m_nRtvDescriptorIncrementSize; // Rtv 서술자 원소 크기

	//=================(DSV)깊이-스텐실=================
	ID3D12Resource* m_pd3dDepthStencilBuffer; // 깊이-스텐실 (Dsv)버퍼
	ID3D12DescriptorHeap* m_pd3dDsvDescriptorHeap; // Dsv 서술자 힙 인터페이스 포인터
	UINT m_nDsvDescriptorIncrementSize; // Dsv 서술자 원소 크기

	//=================명령어 큐/리스트=================
	ID3D12CommandQueue* m_pd3dCommandQueue; // 명령 큐 인터페이스 포인터
	ID3D12CommandAllocator* m_pd3dCommandAllocator; // 명령 할당자 인터페이스 포인터
	ID3D12GraphicsCommandList* m_pd3dCommandList; // 명령 리스트 인터페이스 포인터

	//=================펜스=================
	ID3D12Fence* m_pd3dFence; // 펜스 포인터
	UINT64 m_nFenceValues[m_nSwapChainBuffers]; // 펜스 값
	HANDLE m_hFenceEvent; // 펜스 이벤트 핸들


	ID3D12PipelineState* m_pd3dPipelineState; // 그래픽스 파이프라인 상태 객체 인터페이스 포인터


	//=================프레임 주기 타이머=================
	CGameTimer m_GameTimer; // 게임 프레임워크에서 사용할 타이머
	_TCHAR m_pszFrameRate[50]; // 프레임 레이트를 주 윈도우의 캡션에 출력하기 위한 문자열

	CScene* m_pScene;

public:
	CCamera* m_pCamera = NULL;
	CPlayer* m_pPlayer = NULL; 	// 플레이어 객체에 대한 포인터
	
	POINT m_ptOldCursorPos; // 마지막으로 마우스 버튼을 클릭할 때 마우스 커서 위치

public:
	CGameFramework();
	~CGameFramework();
	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd); // 프레임워크 초기화 - 주 윈도우가 생성되면 호출

	void OnDestroy(); // 프레임워크 종료
	void CreateSwapChain(); // 스왑 체인 생성
	void ChangeSwapChainState();

	void CreateRtvAndDsvDescriptorHeaps(); // 서술자 힙 생성
	void CreateDirect3DDevice(); // 디바이스 생성
	void CreateCommandQueueAndList(); // 명령 큐 / 명령 할당자 / 명령 리스트 생성 

	void CreateRenderTargetViews(); // 렌더 타겟 뷰를 생성하는 함수

	void CreateDepthStencilView(); // 깊이 - 스텐실 뷰를 생성하는 함수

	void BuildObjects(); // 렌더링할 메쉬와 게임 객체 생성
	void ReleaseObjects(); // 렌더링할 메쉬와 게임 객체 소멸

	void ProcessInput(); // 프레임 워크 - 사용자 입력 관리
	void AnimateObjects(); // 프레임 워크 - 애니메이션 관리
	void FrameAdvance(); // 프레임 워크 - 렌더링

	void WaitForGpuComplete(); 	// CPU와 GPU를 동기화
	void MoveToNextFrame();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam); // 마우스 입력 처리
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam); // 키보드 입력 처리
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam); // 윈도우 메세지 처리



protected:
	

};