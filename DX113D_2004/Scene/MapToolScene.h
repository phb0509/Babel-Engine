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
	void initLight();
	void initCamera();

	void updateLight();
	void updateCamera();
	void moveWorldCamera();

	

private:
	TerrainEditor* mTerrainEditor;

	RasterizerState* mRasterizerState;
	Camera* mWorldCamera;
	Vector3 mPreFrameMousePosition;

	LightBuffer* mLightBuffer;
	Light* mDirectionalLight;
};