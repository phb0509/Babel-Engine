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

private:
	Monster* mMonster;
	Player* mPlayer;
	Terrain* mTerrain;

	RenderTarget* mPreRenderTargets[2];
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
	Gizmos* mGizmos;

	bool mbIsGizmosRender = false;
	int mPickedGizmo = 0;
	Vector3 mPreviousMousePosition;

	Player* player;
	bool useWindow = true;

	float objectTransformMatrix[16] = {};
	float matrixTranslation[3] = {};
	float matrixRotation[3] = {};
	float matrixScale[3] = {};
};