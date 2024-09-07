#include "stdafx.h"
#include "UI.h"

//=================================================================================
// UICamera ������
UICamera::UICamera(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, RECT& monitor_area) : CCamera()
{
	//UI�� ������
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
	SetOrthographicProjection(ui_width, ui_height, 0.1f, 100.0f);

	CCamera::Create_Shader_Resource(pd3dDevice, pd3dCommandList);

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

UI_Object::UI_Object(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	Create_Object_Buffer(pd3dDevice, pd3dCommandList);
	Create_UI_Info_Buffer(pd3dDevice, pd3dCommandList);
	//Update_UI_Info(pd3dCommandList);

}

UI_Object::~UI_Object()
{
}

void UI_Object::Create_UI_Info_Buffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// ���� ���� ���� ���� �� ����
	UINT ncbElementBytes = ((sizeof(CB_UI_INFO) + 255) & ~255); //256�� ���
	UI_Constant_Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	UI_Constant_Buffer->Map(0, NULL, (void**)&Mapped_UI_info);
}

void UI_Object::Update_UI_Info(ID3D12GraphicsCommandList* pd3dCommandList)
{
	int default_stick_side = 0;
	float default_Degree = 100;

	CB_UI_INFO* mapped_bar_info = (CB_UI_INFO*)Mapped_UI_info; 
	::memcpy(&mapped_bar_info->fixType, &default_stick_side, sizeof(int));
	::memcpy(&mapped_bar_info->scale, &default_Degree, sizeof(float));

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbGameObjectGpuVirtualAddress = UI_Constant_Buffer->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(0, d3dcbGameObjectGpuVirtualAddress);
}

void UI_Object::Animate(float fElapsedTime)
{
	CGameObject::Animate(fElapsedTime, NULL);
}

void UI_Object::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader)
{
	if (!active)
		return;

	// �ٸ� Ÿ���̶�� �ʿ��� ������ �̹� Render ȣ�� �������� ������Ʈ ��
	if (type == UI_Type::Standard)
		Update_UI_Info(pd3dCommandList);

	CGameObject::Update_Shader_Resource(pd3dCommandList, Resource_Buffer_Type::GameObject_info);

	for (CMesh* p_mesh : mesh_list)
		p_mesh->Render(pd3dCommandList);

	for (CGameObject* sibling_ptr : m_pSibling)
		sibling_ptr->Render(pd3dCommandList, pCamera, pShader);

	for (CGameObject* child_ptr : m_pChild)
		child_ptr->Render(pd3dCommandList, pCamera, pShader);

}

//=================================================================================

BAR_UI_Object::BAR_UI_Object(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int stick_type)
	: UI_Object(pd3dDevice, pd3dCommandList)
{
	type = UI_Type::Bar;

	sticked_side = stick_type;

}

BAR_UI_Object::~BAR_UI_Object()
{
}

void BAR_UI_Object::Update_BAR_UI_Info(ID3D12GraphicsCommandList* pd3dCommandList)
{
	CB_UI_INFO* mapped_bar_info = (CB_UI_INFO*)Mapped_UI_info;
	::memcpy(&mapped_bar_info->fixType, &sticked_side, sizeof(int));
	::memcpy(&mapped_bar_info->scale, &Degree, sizeof(float));

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbGameObjectGpuVirtualAddress = UI_Constant_Buffer->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(0, d3dcbGameObjectGpuVirtualAddress);
}

void BAR_UI_Object::Animate(float fElapsedTime)
{
	if (Charging)
	{
		if (Degree_increase) 
		{
			if (Degree < 300)
				Degree += 8;
			else if (Degree < 500)
				Degree += 6;
			else if (Degree < 600)
				Degree += 4;
			else 
				Degree_increase = false;
		}
		else 
		{
			if (Degree > 500)
				Degree -= 2;
			else if (Degree > 300)
				Degree -= 4;
			else if (Degree > 100)
				Degree -= 6;
			else
				Degree = 100;
		}
	}

	if (500 <= Degree && Degree <= 600)
	{
		std::string str = "Degree is " + std::to_string(Degree);
		DebugOutput(str);
	}
}

void BAR_UI_Object::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader)
{
	Update_BAR_UI_Info(pd3dCommandList);
	UI_Object::Render(pd3dCommandList, pCamera, pShader);
}

void BAR_UI_Object::Set_Charging_Mode(bool charge)
{
	Charging = charge;
}
float BAR_UI_Object::Get_Degree()
{
	if (!active)
		return 0;

	if (500 <= Degree && Degree <= 600)
		return Degree * 2;

	if (400 <= Degree && Degree <= 500)
		return Degree;

	if (Degree_increase == false && Degree == 100)
		return 100;

	return Degree * 1.5;
}

//=================================================================================

UI::UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, RECT& monitor_area) 
	: UICamera(pd3dDevice, pd3dCommandList, monitor_area)
{
}

void UI::AnimateObjects(float fTimeElapsed)
{
	for (UI_Object* game_obj : ui_object)
		game_obj->Animate(fTimeElapsed);
}

void UI::UI_Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader)
{
	SetViewportsAndScissorRects(pd3dCommandList);
	CCamera::Update_Shader_Resource(pd3dCommandList);

	for (UI_Object* ui_obj : ui_object)
	{
		if (ui_obj->active)
		{
			ui_obj->UpdateTransform(NULL);
			ui_obj->Render(pd3dCommandList, this, pShader);
		}
	}
}



void UI::Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void UI::Reset()
{
}

//=================================================================================
// BAR_UI ������
BAR_UI::BAR_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, RECT& monitor_area)
	: UI(pd3dDevice, pd3dCommandList, monitor_area)
{
	Max_Width = monitor_area.right - monitor_area.left;
	Max_Height = monitor_area.bottom - monitor_area.top;
	SetScissorRect(Monitor_Area.left, 0, Monitor_Area.right, FRAME_BUFFER_HEIGHT);
}

BAR_UI::~BAR_UI()
{
}



void BAR_UI::UI_Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader)
{
	UI::UI_Render(pd3dDevice, pd3dCommandList, pShader);
}

void BAR_UI::Set_Bar_obj(BAR_UI_Object* bar)
{
	if (bar_obj != NULL)
		delete bar_obj;

	bar_obj = bar;
}

BAR_UI_Object* BAR_UI::Get_Bar_obj()
{
	if (bar_obj != NULL)
		return bar_obj;

	return NULL;
}

void BAR_UI::Set_Bar_Charge_Mode(bool charge)
{
	if (bar_obj != NULL)
	{
		bar_obj->Set_Charging_Mode(charge);
	}
	else
	{
		DebugOutput("Bar_Obj is NULL");
	}
}

float BAR_UI::Get_Degree()
{
	if (bar_obj == NULL)
		return 0;

	return bar_obj->Degree;
}

void BAR_UI::Reset()
{
	if (bar_obj != NULL)
	{
		bar_obj->Degree = 0;
		bar_obj->Degree_increase = true;
	}
}

//=================================================================================
// Inventory_UI ������
Inventory_UI::Inventory_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, RECT& monitor_area)
	: UI(pd3dDevice, pd3dCommandList, monitor_area)
{
	SetScissorRect(Monitor_Area.left, 0, Monitor_Area.right, FRAME_BUFFER_HEIGHT);
	visualize = true;

}

Inventory_UI::~Inventory_UI()
{
}

void Inventory_UI::AnimateObjects(float fTimeElapsed)
{
	if (visualize)
	{
		if (inventory_board_obj != NULL)
		{
			XMFLOAT3 board_pos = inventory_board_obj->GetPosition();

			if (Drag_up)
			{
				if (board_pos.y > 0)
				{
					inventory_board_obj->SetPosition(0.0f, 0.1f, board_pos.z);
					hold = true;
				}
				else
					inventory_board_obj->Move(Drag_Direction, 180.0f * fTimeElapsed);
			}
			else if (Drag_up == false)
			{
				float board_height = (Monitor_Area.bottom - Monitor_Area.top);
				hold = false;
				if (board_pos.y < -board_height)
				{
					inventory_board_obj->SetPosition(0.0f, -board_height, board_pos.z);
					visualize = false;
				}
				else
					inventory_board_obj->Move(Drag_Direction, -180.0f * fTimeElapsed);
			}
		}
		inventory_board_obj->UpdateTransform(NULL);
		UI::AnimateObjects(fTimeElapsed);
	}
}

void Inventory_UI::UI_Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader)
{
	if(visualize)
		UI::UI_Render(pd3dDevice, pd3dCommandList, pShader);
}

void Inventory_UI::Set_Visualize(bool Bool)
{
	if (Bool)
	{
		Drag_up = true;
		visualize = true;
	}
	else
	{
		Drag_up = false;
	}
}

void Inventory_UI::Get_Info()
{
}

bool Inventory_UI::Is_Num_Render()
{
	return (visualize && hold);
}

void Inventory_UI::Reset()
{

}