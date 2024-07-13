#include "stdafx.h"
#include "UI.h"

//=================================================================================
// UICamera ������
UICamera::UICamera(RECT& monitor_area) : CCamera()
{
	Monitor_Area = monitor_area;
	float ui_width = monitor_area.right - monitor_area.left;
	float ui_height = monitor_area.bottom - monitor_area.top; 


	// UI�� ����Ʈ
	// ����Ʈ ����: ȭ�鿡 UI�� ��ġ�� ���� ���� [UI �׷��� ��ġ �� ũ��] 
	SetViewport(monitor_area.left, monitor_area.top, ui_width, ui_height, 0.0f, 1.0f);
	
	// UI�� ���� ��Ʈ
	// ���� ��Ʈ ����: ����� �������� ������ �� ���� [���� ���� ������ X]
	m_d3dScissorRect = monitor_area;

	// UI�� ���� ���� ��� ����
	// UI�� �ִ� ���� ����: ui_width
	// UI�� �ִ� ���� ����: ui_height
	SetOrthographicProjection(ui_width, ui_height, 0.0f, 100.0f);
}

int UICamera::Update(float fTimeElapsed, bool sign)
{
	return 0;
}

void UICamera::Reset()
{
}

// UI �������� ���� ���� ���� ��� ����
void UICamera::SetOrthographicProjection(float viewWidth, float viewHeight, float nearZ, float farZ)
{
	m_xmf4x4Projection = Matrix4x4::OrthographicLH(viewWidth, viewHeight, nearZ, farZ);
}

//=================================================================================
// BAR_UI ������
BAR_UI::BAR_UI(RECT& monitor_area) : UICamera(monitor_area)
{
	Max_Width = monitor_area.right - monitor_area.left;
	Max_Height = monitor_area.bottom - monitor_area.top;
}

BAR_UI::~BAR_UI()
{
}

int BAR_UI::Update(float fTimeElapsed, bool power_charge)
{
	if (!Active)
		return 0;

	DebugOutput(std::to_string(Degree));

	if (power_charge)
	{
		if (Degree_increase) {
			if (Degree < 400) 
				Degree += 3;			
			else {
				// 400�� �����ϸ� ���� ���¸� false�� ����
				Degree_increase = false;  
			}
		}
		else {
			if (Degree > 300)
				Degree -= 3;
			else if (Degree > 200)
				Degree -= 2;
			else if (Degree > 100)
				Degree -= 1;
			else
				Degree = 100;  // Degree 100 ���� ����
		}
		SetScissorRect(Monitor_Area.right - Degree/2, 0, Monitor_Area.right, FRAME_BUFFER_HEIGHT);
	}
	else
		SetScissorRect(Monitor_Area.left, 0, Monitor_Area.right, FRAME_BUFFER_HEIGHT);

	if( 380 <= Degree && Degree <= 400)
		return 500;

	if (Degree_increase == false && Degree == 100)
	{
		return 100;
	}

	return Degree;
}

void BAR_UI::Reset()
{
	Degree = 0;
	Degree_increase = true;
}