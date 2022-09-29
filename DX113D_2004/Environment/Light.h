#pragma once

class Sphere;

class Light : public Transform
{
public:
	Light();
	Light(LightType lightType);
	~Light();

	void Update();
	void Render();
	void PostRender();
	void Delete();

	LightInfo GetLightInfo() { return mLightInfo; }
	Sphere* GetSphere() { return mSphere; }

	void SetIndex(int index) { mIndex = index; }
	void SetLightBuffer(LightBuffer* lightBuffer) { mLightBuffer = lightBuffer; }
	Vector3 GetDirection() { return mDirection; }

private:
	void updateLightInfo();

private:
	Float4 mColor;
	LightType mType;

	Float3 mDirection;
	float mRange;
	float mInner;
	float mOuter;
	float mLength;
	int mActive;

	LightInfo mLightInfo;
	Sphere* mSphere;
	int mIndex;
	LightBuffer* mLightBuffer;
};