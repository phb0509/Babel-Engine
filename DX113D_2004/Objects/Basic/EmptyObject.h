#pragma once

class EmptyObject : public Transform
{
public:
	EmptyObject();
	~EmptyObject();

	void Update();
	void Render();

private:
};