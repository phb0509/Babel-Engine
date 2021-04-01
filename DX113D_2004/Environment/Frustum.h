#pragma once

class Frustum
{
private:
	Vector4 planes[6];

	Matrix projection;
	Matrix view;

public:
	Frustum();
	~Frustum();

	void Update();

	bool ContainPoint(Vector3 position);
	bool ContainSphere(Vector3 center, float radius);
	bool ContainBox(Vector3 minBox, Vector3 maxBox);

	void GetPlanes(Float4* cullings);
};