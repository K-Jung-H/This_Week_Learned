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

	XMFLOAT3 m_xmf3Position; // �÷��̾� ��ġ ����
	XMFLOAT3 m_xmf3Right;// �÷��̾� x��(Right)
	XMFLOAT3 m_xmf3Up; // �÷��̾� y��(Up)
	XMFLOAT3 m_xmf3Look; // �÷��̾� z��(Look)

	float m_fPitch; // �÷��̾� ���� x��(Right) ȸ�� ��
	float m_fYaw; // �÷��̾� ���� y��(Up) ȸ�� ��
	float m_fRoll; // �÷��̾� ���� z��(Look) ȸ�� ��

	XMFLOAT3 m_xmf3Velocity; // �÷��̾��� �̵� �ӵ� ����
	XMFLOAT3 m_xmf3Gravity; // �÷��̾ �ۿ��ϴ� �߷� ����
	float m_fFriction; // �÷��̾ �ۿ��ϴ� ������


	float m_fMaxVelocityXZ;	// �÷��̾� xz��� �̵� �ӷ��� �ִ밪 - �� ������ ����
	float m_fMaxVelocityY; // �÷��̾� y�� ���� �̵� �ӷ� �ִ밪 - �� ������ ����


	// �÷��̾� ��ġ�� �ٲ� ������ ȣ��Ǵ� OnPlayerUpdateCallback() �Լ����� ����ϴ� ������
	LPVOID m_pPlayerUpdatedContext;

	// ī�޶� ��ġ�� �ٲ� ������ ȣ��Ǵ� OnCameraUpdateCallback() �Լ����� ����ϴ� ������
	LPVOID m_pCameraUpdatedContext;

	CCamera* m_pCamera = NULL; //���� �÷��̾ ������ ī�޶�

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

	// �÷��̾��� ��ġ�� xmf3Position ��ġ�� ����
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

	// �÷��̾ �̵��ϴ� �Լ�
	void Move(ULONG nDirection, float fDistance, bool bVelocity = false);
	void Move(const XMFLOAT3& xmf3Shift, bool bVelocity = false);
	void Move(float fxOffset = 0.0f, float fyOffset = 0.0f, float fzOffset = 0.0f);

	// �÷��̾ ȸ���ϴ� �Լ�
	void Rotate(float x, float y, float z);

	//�÷��̾��� ��ġ�� ȸ�� ������ ��� �ð��� ���� �����ϴ� �Լ�
	void Update(float fTimeElapsed);


	//�÷��̾��� ��ġ�� �ٲ� ������ ȣ��Ǵ� �Լ��� �� �Լ����� ����ϴ� ������ �����ϴ� �Լ�
	virtual void OnPlayerUpdateCallback(float fTimeElapsed) { }
	void SetPlayerUpdatedContext(LPVOID pContext) { m_pPlayerUpdatedContext = pContext; }


	//ī�޶��� ��ġ�� �ٲ� ������ ȣ��Ǵ� �Լ��� �� �Լ����� ����ϴ� ������ �����ϴ� �Լ�
	virtual void OnCameraUpdateCallback(float fTimeElapsed) { }
	void SetCameraUpdatedContext(LPVOID pContext) { m_pCameraUpdatedContext = pContext; }


	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);


	// ī�޶� �����ϴ� �Լ�
	CCamera* OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode);
	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed) { return(NULL); }

	//�÷��̾��� ��ġ�� ȸ�������κ��� ���� ��ȯ ����� �����ϴ� �Լ�
	virtual void OnPrepareRender();

	//�÷��̾��� ī�޶� 3��Ī ī�޶��� �� �÷��̾�(�޽�)�� ������
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
