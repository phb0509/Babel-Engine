#pragma once

class TerrainEditorScene : public Scene
{
private:
	TerrainEditor* terrainEditor;
	SkyBox* skyBox;
public:
	TerrainEditorScene();
	~TerrainEditorScene();

	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
};