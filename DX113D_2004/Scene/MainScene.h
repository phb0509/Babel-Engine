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
	Player* player;
	Monster* monster;
	Terrain* terrain;
	vector<Monster*> monsters;

	ModelObject* obstacle1;
	ModelObject* obstacle2;

	int a = 0;

};