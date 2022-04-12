#pragma once

class Frustum;

class Models : public Model
{
protected:
	struct InstanceData
	{
		Matrix world;
		UINT index;
	};

	
public:
	Models();
	virtual ~Models();

	void Update();
	void Render() override;

	Transform* Add();
	InstanceData* GetInstanceData() { return mInstanceData; }
	Transform* GetTransform(UINT instance) { return mTransforms[instance]; }
	void SetCameraForCulling(Camera* camera) { mCameraForFrustumCulling = camera; }
	void SetBoxForFrustumCulling() { SetBox(&mMinBox, &mMaxBox); }

protected:
	vector<Transform*> mTransforms;
	//Matrix worlds[MAX_INSTANCE];
	InstanceData mInstanceData[MAX_INSTANCE];

	VertexBuffer* mInstanceBuffer;

	UINT mDrawCount;

	Vector3 mMinBox;
	Vector3 mMaxBox;
	Camera* mCameraForFrustumCulling;
	
};