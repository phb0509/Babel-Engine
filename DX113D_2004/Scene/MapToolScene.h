#pragma once

class MapToolScene : public Scene
{
public:
	MapToolScene();
	~MapToolScene();

	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;


private:
	TerrainEditor* terrainEditor = nullptr;
	SkyBox* skyBox = nullptr;
	Cube* cube = nullptr;

	RasterizerState* mRasterizerState = nullptr;
};