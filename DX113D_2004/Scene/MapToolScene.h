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
	void moveWorldCamera();
private:
	TerrainEditor* mTerrainEditor = nullptr;

	RasterizerState* mRasterizerState = nullptr;
	Camera* mWorldCamera;
	Vector3 mPreFrameMousePosition;
};