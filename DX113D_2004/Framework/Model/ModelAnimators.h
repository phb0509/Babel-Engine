#pragma once

class Frustum;

class ModelAnimators : public ModelAnimator
{
private:
	struct InstanceData
	{
		Matrix world;
		UINT index;
	};

	vector<Transform*> transforms;
	InstanceData instanceData[MAX_INSTANCE];

	VertexBuffer* instanceBuffer;

	Frustum* frustum;
	UINT drawCount;

	Vector3 minBox, maxBox;

	vector<map<UINT, CallBackParam>> EndEvents;
	vector<map<UINT, int>> params;
public:
	ModelAnimators(string file);
	~ModelAnimators();

	void Update() override;
	void Render() override;
	void PostRender();

	Transform* Add();

	void PlayClip(UINT instance, UINT clip,
		float speed = 1.0f, float takeTime = 0.2f);

	void UpdateTransforms();

	Transform* GetTransform(UINT instance) { return transforms[instance]; }

	void SetEndEvents(UINT instance, UINT clip, CallBackParam Event) { EndEvents[instance][clip] = Event; }
	void SetParams(UINT instance, UINT clip, int param) { params[instance][clip] = param; }
};