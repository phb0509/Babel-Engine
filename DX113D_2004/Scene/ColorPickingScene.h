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
	Monster* mMonster;
	Player* mPlayer;
	Terrain* mTerrain;

	Collider* collider;
	
	DepthStencil* mDepthStencil;
	RenderTarget* mRenderTargets[1];
	RenderTarget* mRenderTarget;
	Texture* texture;



	Vector3 mMouseScreenUVPosition;
	ComputeShader* mComputeShader;
	ComputeStructuredBuffer* mComputeStructuredBuffer;
	ColorPickingInputBuffer* mInputBuffer;
	ColorPickingOutputBuffer* mOutputBuffer;

	Cube* mCube;
	Collider* mBoxCollider;
	Collider* mSphereCollider;
	Collider* mCapsuleCollider;


};