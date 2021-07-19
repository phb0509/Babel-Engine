#include "Framework.h"



#include "Scene/TerrainEditorScene.h"
#include "Scene/ModelExportScene.h"
#include "Scene/TessellationScene.h"
#include "Scene/TerrainLODScene.h"
#include "Scene/DeferredRenderingScene.h"
#include "Scene/MainScene.h"
#include "Scene/ColliderSettingScene.h"
#include "Scene/MapToolScene.h"
#include "Scene/TestScene.h"


Program::Program()
{
	Create();

	//SceneManager::Get()->Add("export", new ModelExportScene());
	//SceneManager::Get()->Add("start", new MainScene());
	SceneManager::Get()->Add("start", new MapToolScene());
	//SceneManager::Get()->Add("start", new ColliderSettingScene());
//	SceneManager::Get()->Add("start", new TestScene());
	//SceneManager::Get()->Add("start", new TerrainEditorScene());
	//SceneManager::Get()->Add("start", new TessellationScene());
	//SceneManager::Get()->Add("start", new TerrainLODScene());
	//SceneManager::Get()->AddScene("export");
	SceneManager::Get()->AddScene("start");
}

Program::~Program()
{
	Delete();
}

void Program::Update()
{
	if (KEY_DOWN(VK_F1))
		Transform::mbIsAxisDrawing = !Transform::mbIsAxisDrawing;

	Control::Get()->Update();
	Timer::Get()->Update();

	SceneManager::Get()->Update();

	//if (Environment::Get()->GetIsTargetCamera())
	//{
	//	Environment::Get()->GetTargetCamera()->Update();
	//}
	//else
	//{
	//	Environment::Get()->GetWorldCamera()->Update();
	//}


	Environment::Get()->GetTargetCamera()->Update();
	Environment::Get()->GetWorldCamera()->Update();
	
	Control::Get()->SetWheel(0.0f);
}

void Program::PreRender()
{
	Environment::Get()->Set();
	SceneManager::Get()->PreRender();
}

void Program::Render()
{
	Device::Get()->SetRenderTarget();
	Environment::Get()->GetTargetCamera()->Render();
	Environment::Get()->GetWorldCamera()->Render();
	Environment::Get()->Set();

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

	Environment::Get()->PostRender();

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

	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(DEVICE, DEVICECONTEXT);
}

void Program::Delete()
{
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
