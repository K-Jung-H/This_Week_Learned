#pragma once
#include "GameObject.h"
#include "Camera.h"

//플레이어 객체도 게임 객체이므로 CGameObject 클래스에서 상속하여 클래스를 생성한다.
class CPlayer : public CGameObject
{
public:
	CPlayer() { }
	virtual ~CPlayer() { if (m_pCamera) delete m_pCamera; }
private:
	//플레이어 객체에 포함된 카메라이다.
	CCamera* m_pCamera = NULL;
public:
	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void Move(float x, float y, float z);
	void Rotate(float x, float y, float z);
	void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; }
	CCamera* GetCamera() { return(m_pCamera); }
};