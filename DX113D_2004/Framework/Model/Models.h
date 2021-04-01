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

	vector<Transform*> transforms;
	//Matrix worlds[MAX_INSTANCE];
	InstanceData instanceData[MAX_INSTANCE];

	VertexBuffer* instanceBuffer;

	Frustum* frustum;
	UINT drawCount;

	Vector3 minBox, maxBox;
public:
	Models(string file);
	virtual ~Models();

	void Update();
	void Render() override;

	Transform* Add();

	InstanceData* GetInstanceData() { return instanceData; }

	Transform* GetTransform(UINT instance) { return transforms[instance]; }
};