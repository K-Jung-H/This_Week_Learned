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

	IDXGIFactory4* m_pdxgiFactory; // DXGI ���丮 �������̽��� ���� ������
	IDXGISwapChain3* m_pdxgiSwapChain; // ���� ü�� �������̽��� ���� ������ - �ַ� ���÷��̸� �����ϱ� ���Ͽ� �ʿ�
	ID3D12Device* m_pd3dDevice; // Direct3D ����̽� �������̽��� ���� ������ - �ַ� ���ҽ��� �����ϱ� ���Ͽ� �ʿ�

	bool m_bMsaa4xEnable = false; // MSAA ���� ���ø��� Ȱ��ȭ
	UINT m_nMsaa4xQualityLevels = 0; // ���� ���ø� ������ ����

	//=================���� ü��=================
	static const UINT m_nSwapChainBuffers = 2; // ���� ü�� �ĸ� ������ ����
	UINT m_nSwapChainBufferIndex; // ���� ���� ü���� �ĸ� ���� �ε���

	//=================(RTV)���� Ÿ��=================
	ID3D12Resource* m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers]; // ���� Ÿ�� (Rtv)����
	ID3D12DescriptorHeap* m_pd3dRtvDescriptorHeap; // Rtv ������ �� �������̽� ������
	UINT m_nRtvDescriptorIncrementSize; // Rtv ������ ���� ũ��

	//=================(DSV)����-���ٽ�=================
	ID3D12Resource* m_pd3dDepthStencilBuffer; // ����-���ٽ� (Dsv)����
	ID3D12DescriptorHeap* m_pd3dDsvDescriptorHeap; // Dsv ������ �� �������̽� ������
	UINT m_nDsvDescriptorIncrementSize; // Dsv ������ ���� ũ��

	//=================��ɾ� ť/����Ʈ=================
	ID3D12CommandQueue* m_pd3dCommandQueue; // ��� ť �������̽� ������
	ID3D12CommandAllocator* m_pd3dCommandAllocator; // ��� �Ҵ��� �������̽� ������
	ID3D12GraphicsCommandList* m_pd3dCommandList; // ��� ����Ʈ �������̽� ������

	//=================�潺=================
	ID3D12Fence* m_pd3dFence; // �潺 ������
	UINT64 m_nFenceValues[m_nSwapChainBuffers]; // �潺 ��
	HANDLE m_hFenceEvent; // �潺 �̺�Ʈ �ڵ�


	ID3D12PipelineState* m_pd3dPipelineState; // �׷��Ƚ� ���������� ���� ��ü �������̽� ������


	//=================������ �ֱ� Ÿ�̸�=================
	CGameTimer m_GameTimer; // ���� �����ӿ�ũ���� ����� Ÿ�̸�
	_TCHAR m_pszFrameRate[50]; // ������ ����Ʈ�� �� �������� ĸ�ǿ� ����ϱ� ���� ���ڿ�

	CScene* m_pScene;

public:
	CCamera* m_pCamera = NULL;
	CPlayer* m_pPlayer = NULL; 	// �÷��̾� ��ü�� ���� ������
	
	POINT m_ptOldCursorPos; // ���������� ���콺 ��ư�� Ŭ���� �� ���콺 Ŀ�� ��ġ

public:
	CGameFramework();
	~CGameFramework();
	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd); // �����ӿ�ũ �ʱ�ȭ - �� �����찡 �����Ǹ� ȣ��

	void OnDestroy(); // �����ӿ�ũ ����
	void CreateSwapChain(); // ���� ü�� ����
	void ChangeSwapChainState();

	void CreateRtvAndDsvDescriptorHeaps(); // ������ �� ����
	void CreateDirect3DDevice(); // ����̽� ����
	void CreateCommandQueueAndList(); // ��� ť / ��� �Ҵ��� / ��� ����Ʈ ���� 

	void CreateRenderTargetViews(); // ���� Ÿ�� �並 �����ϴ� �Լ�

	void CreateDepthStencilView(); // ���� - ���ٽ� �並 �����ϴ� �Լ�

	void BuildObjects(); // �������� �޽��� ���� ��ü ����
	void ReleaseObjects(); // �������� �޽��� ���� ��ü �Ҹ�

	void ProcessInput(); // ������ ��ũ - ����� �Է� ����
	void AnimateObjects(); // ������ ��ũ - �ִϸ��̼� ����
	void FrameAdvance(); // ������ ��ũ - ������

	void WaitForGpuComplete(); 	// CPU�� GPU�� ����ȭ
	void MoveToNextFrame();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam); // ���콺 �Է� ó��
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam); // Ű���� �Է� ó��
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam); // ������ �޼��� ó��



protected:
	

};