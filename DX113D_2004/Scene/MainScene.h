#pragma once

class MainScene : public Scene
{
public:
	MainScene();
	~MainScene();


	// Scene��(��) ���� ��ӵ�
	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;


private:
	Player* mPlayer;
	Monster* mMonster;
	Terrain* mTerrain;
	vector<Monster*> monsters;

	ModelObject* mObstacle1;
	ModelObject* mObstacle2;

	Cube* cube;
	vector<Cube*> cubes;
	Frustum* mTargetCameraFrustum;
};