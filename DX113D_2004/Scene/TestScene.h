#pragma once

class TestScene : public Scene
{

public:

	TestScene();
	~TestScene();

	// Scene��(��) ���� ��ӵ�
	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;

	HRESULT CDialogEventHandler_CreateInstance(REFIID riid, void** ppv);

	
private:
	//ModelExporter* mModelExporter;
	bool mCheck = true;

	string tempPath;
};