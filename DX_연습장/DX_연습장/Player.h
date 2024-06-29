#define DIR_FORWARD 0x01
#define DIR_BACKWARD 0x02
#define DIR_LEFT 0x04
#define DIR_RIGHT 0x08
#define DIR_UP 0x10
#define DIR_DOWN 0x20

#include "GameObject.h"
#include "Camera.h"


class CPlayer : public CGameObject
{
protected:

	XMFLOAT3 m_xmf3Position; // 플레이어 위치 벡터
	XMFLOAT3 m_xmf3Right;// 플레이어 x축(Right)
	XMFLOAT3 m_xmf3Up; // 플레이어 y축(Up)
	XMFLOAT3 m_xmf3Look; // 플레이어 z축(Look)

	float m_fPitch; // 플레이어 로컬 x축(Right) 회전 각
	float m_fYaw; // 플레이어 로컬 y축(Up) 회전 각
	float m_fRoll; // 플레이어 로컬 z축(Look) 회전 각

	XMFLOAT3 m_xmf3Velocity; // 플레이어의 이동 속도 벡터
	XMFLOAT3 m_xmf3Gravity; // 플레이어에 작용하는 중력 벡터
	float m_fFriction; // 플레이어에 작용하는 마찰력


	float m_fMaxVelocityXZ;	// 플레이어 xz평면 이동 속력의 최대값 - 한 프레임 동안
	float m_fMaxVelocityY; // 플레이어 y축 방향 이동 속력 최대값 - 한 프레임 동안


	// 플레이어 위치가 바뀔 때마다 호출되는 OnPlayerUpdateCallback() 함수에서 사용하는 데이터
	LPVOID m_pPlayerUpdatedContext;

	// 카메라 위치가 바뀔 때마다 호출되는 OnCameraUpdateCallback() 함수에서 사용하는 데이터
	LPVOID m_pCameraUpdatedContext;

	CCamera* m_pCamera = NULL; //현재 플레이어에 설정된 카메라

public:
	CPlayer();
	virtual ~CPlayer();

	XMFLOAT3 GetPosition() { return(m_xmf3Position); }
	XMFLOAT3 GetLookVector() { return(m_xmf3Look); }
	XMFLOAT3 GetUpVector() { return(m_xmf3Up); }
	XMFLOAT3 GetRightVector() { return(m_xmf3Right); }

	void SetFriction(float fFriction) { m_fFriction = fFriction; }
	void SetGravity(XMFLOAT3& xmf3Gravity) { m_xmf3Gravity = xmf3Gravity; }
	void SetVelocity(XMFLOAT3& xmf3Velocity) { m_xmf3Velocity = xmf3Velocity; }


	void SetMaxVelocityXZ(float fMaxVelocity) { m_fMaxVelocityXZ = fMaxVelocity; }
	void SetMaxVelocityY(float fMaxVelocity) { m_fMaxVelocityY = fMaxVelocity; }

	// 플레이어의 위치를 xmf3Position 위치로 설정
	void SetPosition(XMFLOAT3& xmf3Position) {
		Move(XMFLOAT3(
			xmf3Position.x - m_xmf3Position.x,
			xmf3Position.y - m_xmf3Position.y,
			xmf3Position.z - m_xmf3Position.z),
			false);
	}
	XMFLOAT3& GetVelocity() { return(m_xmf3Velocity); }
	float GetYaw() { return(m_fYaw); }
	float GetPitch() { return(m_fPitch); }
	float GetRoll() { return(m_fRoll); }
	CCamera* GetCamera() { return(m_pCamera); }
	void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; }

	// 플레이어를 이동하는 함수
	void Move(ULONG nDirection, float fDistance, bool bVelocity = false);
	void Move(const XMFLOAT3& xmf3Shift, bool bVelocity = false);
	void Move(float fxOffset = 0.0f, float fyOffset = 0.0f, float fzOffset = 0.0f);

	// 플레이어를 회전하는 함수
	void Rotate(float x, float y, float z);

	//플레이어의 위치와 회전 정보를 경과 시간에 따라 갱신하는 함수
	void Update(float fTimeElapsed);


	//플레이어의 위치가 바뀔 때마다 호출되는 함수와 그 함수에서 사용하는 정보를 설정하는 함수
	virtual void OnPlayerUpdateCallback(float fTimeElapsed) { }
	void SetPlayerUpdatedContext(LPVOID pContext) { m_pPlayerUpdatedContext = pContext; }


	//카메라의 위치가 바뀔 때마다 호출되는 함수와 그 함수에서 사용하는 정보를 설정하는 함수
	virtual void OnCameraUpdateCallback(float fTimeElapsed) { }
	void SetCameraUpdatedContext(LPVOID pContext) { m_pCameraUpdatedContext = pContext; }


	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);


	// 카메라 변경하는 함수
	CCamera* OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode);
	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed) { return(NULL); }

	//플레이어의 위치와 회전축으로부터 월드 변환 행렬을 생성하는 함수
	virtual void OnPrepareRender();

	//플레이어의 카메라가 3인칭 카메라일 때 플레이어(메쉬)를 렌더링
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
};


class CAirplanePlayer : public CPlayer
{
public:
	CAirplanePlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~CAirplanePlayer();

	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);
	virtual void OnPrepareRender();
};
