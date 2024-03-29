#include "Framework.h"
#include "Scene/MainScene.h"
#include "Scene/ColliderSettingScene.h"
#include "Scene/MapToolScene.h"


Program::Program()
{
	GM->Get()->SetWindowDropEvent(bind(&Program::PostRender, this));

	Create();

	//SceneManager::Get()->Add("start", new ColliderSettingScene());
	//SceneManager::Get()->Add("start", new MainScene());
	SceneManager::Get()->Add("start", new MapToolScene());

	SceneManager::Get()->AddScene("start");
}

Program::~Program()
{
	Delete();
}

void Program::Update()
{
	Timer::Get()->Update();
	Control::Get()->Update();
	SceneManager::Get()->Update();
	Control::Get()->SetWheel(0.0f);
}

void Program::PreRender()
{
	SceneManager::Get()->PreRender();
}

void Program::Render()
{
	SceneManager::Get()->Render();
}

void Program::PostRender()
{
	DirectWrite::Get()->GetDC()->BeginDraw();

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	{ // 좌측상단 FPS,현재 시간(초) 출력
		wstring fps = L"FPS : " + to_wstring((int)Timer::Get()->GetFPS());
		RECT rect = { 0, 0, 100, 100 };
		DirectWrite::Get()->RenderText(fps, rect);
	}

	SceneManager::Get()->PostRender();

	DirectWrite::Get()->GetDC()->EndDraw();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void Program::Create()
{
	Device::Get();
	Environment::Get();
	Control::Get();
	Timer::Get();
	DirectWrite::Get();
	GameManager::Get();
	DebugManager::Get();

	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(DEVICE, DEVICECONTEXT);
}

void Program::Delete()
{
	DebugManager::Delete();
	GameManager::Delete();
	SceneManager::Delete();
	DirectWrite::Delete();
	Timer::Delete();
	Control::Delete();
	Environment::Delete();
	Texture::Delete();
	Shader::Delete();
	Device::Delete();
}
