#pragma once

class ColorPickingScene : public Scene
{

public:
	struct InputDesc
	{
		UINT index;
		Float3 v0, v1, v2;
	};

	struct OutputDesc
	{
		int picked;
		float u, v, distance;
	};

	ColorPickingScene();
	~ColorPickingScene();

	// Scene을(를) 통해 상속됨
	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;

private:
	void colorPicking();
	void changePickedObject();

private:
	Monster* mMonster;
	Player* mPlayer;
	Terrain* mTerrain;

	RenderTarget* mPreRenderTargets[1];
	DepthStencil* mPreRenderTargetDSV;


	Vector3 mMouseScreenUVPosition;
	ComputeShader* mColorPickingComputeShader;
	ComputeStructuredBuffer* mComputeStructuredBuffer;
	ColorPickingInputBuffer* mInputBuffer;
	ColorPickingOutputBuffer* mOutputBuffer;

	Cube* mCube;
	Collider* mBoxCollider;
	Collider* mSphereCollider;
	Collider* mCapsuleCollider;

	vector<Collider*> mColliders;
	Vector3 mMousePositionColor;
	
	Collider* mCurrentPickedCollider;
	Collider* mPreviousPickedCollider;

	bool mbIsGizmosRender = false;
	int mPickedGizmo = 0;
	Vector3 mPreviousMousePosition;

	Player* player;
	bool useWindow = true;

	float objectTransformMatrix[16] = {};

	Vector3 matrixTranslation;
	Vector3 matrixRotation;
	Vector3 matrixScale;

	ImGuizmo::OPERATION currentGizmo = ImGuizmo::NONE;
};