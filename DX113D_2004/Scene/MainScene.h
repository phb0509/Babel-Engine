#pragma once

class MainScene : public Scene
{
public:
	enum class eCamera
	{
		WorldCamera,
		TargetCamera
	};

	MainScene();
	~MainScene();

	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;

private:
	void printToCSV();
	void setIdle(int instance);
	void moveWorldCamera();

private:
	Player* mPlayer;
	Monster* mMonster;
	Terrain* mTerrain;
	vector<Monster*> mMutants;

	ModelObject* mObstacle1;
	ModelObject* mObstacle2;

	Frustum* mTargetCameraFrustum;
	ModelAnimators* mInstancedMutants;
	vector<Matrix> boneMatrix;

	InstanceMutant* mInstanceMutant;
	vector<Monster*> mInstanceMutants;
	int mMutantInstanceCount;

	Camera* mWorldCamera;
	Camera* mTargetCamera;
	Camera* mTargetCameraForShow;
	bool mbIsWorldMode;

	eCamera mMainCamera;
	Vector3 mPreFrameMousePosition;
};