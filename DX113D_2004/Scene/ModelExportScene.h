#pragma once

class ModelExportScene : public Scene
{
private:
	ModelExporter* exporter;

public:
	ModelExportScene();
	~ModelExportScene();

	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
};