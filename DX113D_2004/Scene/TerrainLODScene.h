#pragma once

class TerrainLODScene : public Scene
{
private:
	TerrainLOD* terrain;
	RasterizerState* rsState;
public:
	TerrainLODScene();
	~TerrainLODScene();

	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
};