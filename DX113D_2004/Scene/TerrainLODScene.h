#pragma once

class TerrainLODScene : public Scene
{
public:
	TerrainLODScene();
	~TerrainLODScene();

	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;


private:
	TerrainLOD* mTerrain;
	RasterizerState* mRSState;

	LightBuffer* mLightBuffer;
	Light* mDirectionalLight;
	Camera* mCamera;
};