#pragma once

#include "Timer.h"
#include "Player.h"
#include "Scene.h"

struct CB_FRAMEWORK_INFO
{
	float					m_fCurrentTime;
	float					m_fElapsedTime;
	int Scene_Type;
};

class CGameFramework
{
public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	void CreateSwapChain();
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();

	void CreateRtvAndDsvDescriptorHeaps();

	void CreateRenderTargetViews();
	void CreateDepthStencilView();

	void ChangeSwapChainState();

    void BuildObjects();
    void ReleaseObjects();

	void CreateShaderVariables();
	void UpdateShaderVariables();
	void ReleaseShaderVariables();

    void ProcessInput();
    void AnimateObjects();
    void FrameAdvance();

	void WaitForGpuComplete();
	void MoveToNextFrame();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

private:
	HINSTANCE					m_hInstance;
	HWND						m_hWnd; 

	int							m_nWndClientWidth;
	int							m_nWndClientHeight;
        
	IDXGIFactory4				*m_pdxgiFactory = NULL;
	IDXGISwapChain3				*m_pdxgiSwapChain = NULL;
	ID3D12Device				*m_pd3dDevice = NULL;

	bool						m_bMsaa4xEnable = false;
	UINT						m_nMsaa4xQualityLevels = 0;

	static const UINT			m_nSwapChainBuffers = 2;
	UINT						m_nSwapChainBufferIndex;

	ID3D12Resource				*m_ppd3dSwapChainBackBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap		*m_pd3dRtvDescriptorHeap = NULL;

	ID3D12Resource				*m_pd3dDepthStencilBuffer = NULL;
	ID3D12DescriptorHeap		*m_pd3dDsvDescriptorHeap = NULL;

	ID3D12CommandAllocator		*m_pd3dCommandAllocator = NULL;
	ID3D12CommandQueue			*m_pd3dCommandQueue = NULL;
	ID3D12GraphicsCommandList	*m_pd3dCommandList = NULL;

	ID3D12Fence					*m_pd3dFence = NULL;
	UINT64						m_nFenceValues[m_nSwapChainBuffers];
	HANDLE						m_hFenceEvent;

#if defined(_DEBUG)
	ID3D12Debug					*m_pd3dDebugController;
#endif

	CGameTimer					m_GameTimer;

	CScene						*rendering_scene = NULL;
	Start_Scene					*start_scene = NULL;

	Game_Scene				*game_scene = NULL;
	CPlayer						*m_pPlayer = NULL;
	CCamera						*m_pCamera = NULL;

	POINT						m_ptOldCursorPos;

	_TCHAR						m_pszFrameRate[70];

	bool Game_Start = false;
	int Game_Scene_Type = 0;
private:
	// DX2DÀü¿ë
#ifdef _WITH_DIRECT2D
	ID3D11On12Device* m_pd3d11On12Device = NULL;
	ID3D11DeviceContext* m_pd3d11DeviceContext = NULL;
	ID2D1Factory3* m_pd2dFactory = NULL;
	IDWriteFactory* m_pdWriteFactory = NULL;
	ID2D1Device2* m_pd2dDevice = NULL;
	ID2D1DeviceContext2* m_pd2dDeviceContext = NULL;

	ID3D11Resource* m_ppd3d11WrappedBackBuffers[m_nSwapChainBuffers];
	ID2D1Bitmap1* m_ppd2dRenderTargets[m_nSwapChainBuffers];

	ID2D1SolidColorBrush* m_pd2dbrBackground = NULL;
	ID2D1SolidColorBrush* m_pd2dbrBorder = NULL;
	ID2D1SolidColorBrush* m_pd2dbrText = NULL;
	ID2D1SolidColorBrush* m_pd2dbrScore_Text = NULL;

	IDWriteTextFormat* m_pdw_Timer_Font = NULL;
	IDWriteTextFormat* m_pdw_UI_Text_Font = NULL;
	IDWriteTextFormat* m_pdw_Score_Font = NULL;

	IDWriteTextLayout* m_pdwTextLayout = NULL;

	void CreateDirect2DDevice();
#endif

protected:
	ID3D12Resource* m_pd3dcbFrameworkInfo = NULL;
	CB_FRAMEWORK_INFO* m_pcbMappedFrameworkInfo = NULL;
};

