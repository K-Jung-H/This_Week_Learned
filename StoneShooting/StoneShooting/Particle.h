#pragma once
#include "GameObject.h"
#include "Shader.h"

inline float RandF(float fMin, float fMax);
XMVECTOR RandomUnitVectorOnSphere();
XMVECTOR GetRandomRotatedVector(float angle);
XMVECTOR RotateVector(const XMVECTOR& vec, const XMVECTOR& axis, float angle);


class Particle : public CRotatingObject
{
public:
	Particle_Type p_type = Particle_Type::None;

	float						m_fElapsedTimes = 0.0f;
	float						m_fDuration = 2.0f;
	float						Particle_Rotation = 720.0f;
	
public:
	UINT8* particles_info = NULL;
	ID3D12Resource* m_pConstant_Buffer; // �ѹ��� ���� ������ ��� ������ �Ϲ� GameObject�� ���ۿ� ũ�Ⱑ �޶�� ��

	Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, Particle_Type p_type = Particle_Type::None);
	virtual ~Particle();

	static void Prepare_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void Create_Shader_Resource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Release_Shader_Resource();

	virtual void Animate(float fElapsedTime);
	virtual void Particle_Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	void Reset();
};


#define EXPLOSION_DEBRISES 240

class Explosion_Particle : public Particle
{
	static XMFLOAT3				Explosion_Sphere_Vectors[EXPLOSION_DEBRISES];
	static bool Setting;
	static CMesh* m_ExplosionMesh; 
	XMFLOAT4X4					m_pxmf4x4Transforms[EXPLOSION_DEBRISES];

	float						m_fExplosionSpeed = 10.0f;
	

public:
	static void Prepare_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	
	Explosion_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
		CMaterial* material, Particle_Type p_type = Particle_Type::Explosion);
	virtual ~Explosion_Particle();


	virtual void Create_Shader_Resource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Release_Shader_Resource();

	virtual void Animate(float fElapsedTime);
	virtual void Particle_Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	virtual void Reset();
};



#define CHARGE_DEBRISES 160

class Charge_Particle : public Particle
{
	static bool Setting;
	static CMesh* m_ChargeMesh;
	static XMFLOAT3				Charge_Sphere_Vectors[CHARGE_DEBRISES];
	XMFLOAT4X4					m_pxmf4x4Transforms[CHARGE_DEBRISES];

	std::vector<XMFLOAT3> Particle_Start_Position; // �� ������ ���� ��ġ
	std::vector<float> Particle_ElapsedTime; // �� ������ ��� �ð�
	std::vector<float> Particle_Speed; // ���ں� �ӵ�

	float						Max_Range = 100.0f;
	float						m_fChargeSpeed = 10.0f;
	float						Rotation_Orbit = 200.0f;

	float						Active_Particle = 0;
public:
	static void Prepare_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	Charge_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		float Range, float cycle_time, CMaterial* material, Particle_Type p_type = Particle_Type::Charge);

	virtual ~Charge_Particle();


	virtual void Create_Shader_Resource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Release_Shader_Resource();

	void Set_Center_Position(XMFLOAT3 pos);

	virtual void Animate(float fElapsedTime);
	void Particle_Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader);
	virtual void Reset();
};

#define Firework_DEBRISES 300

class Firework_Particle : public Particle
{
	UINT8* firework_material_info = NULL;
	ID3D12Resource* firework_Constant_Buffer;


	static bool					Setting;
	static CMesh*				m_FireworkMesh;
	static XMFLOAT3				Firework_Vectors[Firework_DEBRISES];
	XMFLOAT4X4					m_pxmf4x4Transforms[Firework_DEBRISES];

	std::vector<float>			Particle_Speed; // ���ں� �ӵ� == �ð��� ���������� ���� ��������, �ְ����� ������ ���� �������� ��
	std::vector<XMFLOAT3>		Particle_Direction; // ���ڴ� �ð��� �������� ������ �Ʒ��� ���ؾ� ��
	std::vector<float>			Particle_ElapsedTime; // �� ������ ��� �ð�

	float						Active_Particle = 0;

	XMFLOAT3					Default_Direction = { 0.0f,1.0f,0.0f };
	float						Max_Height = 60.0f;
	float						slope = 10.0f; // ���� ����
	

public:
	static void Prepare_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	Firework_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		float cycle_time, CMaterial* material, Particle_Type p_type = Particle_Type::Firework);

	virtual ~Firework_Particle();


	virtual void Create_Shader_Resource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Release_Shader_Resource();


	void Set_Main_Direction(XMFLOAT3 Direction);

	virtual void Animate(float fElapsedTime);

	virtual void Particle_Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	virtual void Reset();
};


#define Snow_DEBRISES 100

class Snow_Particle : public Particle
{
	UINT8* snow_material_info = NULL;
	ID3D12Resource* Snow_Constant_Buffer;


	static bool Setting;
	static XMFLOAT3 Particle_Pos_XZ[Snow_DEBRISES]; // �� ������ ���� ��ǥ
	static XMFLOAT3 Particle_Rotation_Vector[Snow_DEBRISES]; // ���ڸ��� ȸ���ϴ� ����
	static CMesh* m_SnowMesh;

	XMFLOAT4X4					m_pxmf4x4Transforms[Snow_DEBRISES];

	std::vector<float>			Particle_ElapsedTime; // �� ������ ��� �ð�
	std::vector<float>			Particle_Speed; // ���� ������ �ӵ��� ��� �ٸ� ��


	float						Active_Particle = 0;
	float						range = 10.0f;
	XMFLOAT3					center_pos = { 0.0f,0.0f,0.0f };
	XMFLOAT3					Default_Direction = { 0.0f,-1.0f,0.0f };
	float						Max_Move_Y = 40.0f;


public:
	static void Prepare_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	Snow_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		XMFLOAT3 start_pos, float range, CMaterial* material, Particle_Type p_type = Particle_Type::Snow);

	virtual ~Snow_Particle();


	virtual void Create_Shader_Resource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Release_Shader_Resource();


	void Set_Center(XMFLOAT3 Direction);

	virtual void Animate(float fElapsedTime);

	virtual void Particle_Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	virtual void Reset();
};