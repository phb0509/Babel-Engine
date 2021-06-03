#pragma once

class TestScene : public Scene
{
public:
	TestScene();
	~TestScene();

	// Scene을(를) 통해 상속됨
	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;


private:
	Collider* collider;
	Cube* cube;
	vector<Cube*> cubes;
};