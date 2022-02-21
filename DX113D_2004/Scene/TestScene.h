#pragma once

class TestScene : public Scene
{
private:

public:
	TestScene();
	~TestScene();

	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;


private:
	Collider* mBoxCollider;
	Collider* mTestCollider;
	Cube* mCube;
};