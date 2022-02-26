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

	// Scene��(��) ���� ��ӵ�
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

	DepthStencil* mDepthStencil;
	RenderTarget* mRenderTargets[1];
	RenderTarget* mRenderTargetTexture;
	Texture* texture;


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
	
	Collider* mPickedCollider;
	Gizmos* mGizmos;

	bool mbIsGizmosRender = false;
	int mPickedGizmo = 0;
	Vector3 mPreviousMousePosition;
};