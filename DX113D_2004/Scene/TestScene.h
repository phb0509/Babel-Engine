#pragma once

class TestScene : public Scene
{
private:

public:
	TestScene();
	~TestScene();

	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;


private:
	

private:
	Collider* mBoxCollider;
	Cube* mCube;
	bool mbIsPicked = false;

	RenderTarget* mPreRenderTargets[1];
	DepthStencil* mPreRenderTargetDSV;


	Vector3 mMouseScreenUVPosition;
	ComputeShader* mColorPickingComputeShader;
	ComputeStructuredBuffer* mComputeStructuredBuffer;
	ColorPickingInputBuffer* mInputBuffer;
	ColorPickingOutputBuffer* mOutputBuffer;
	Vector3 mMousePositionColor;

	float objectTransformMatrix[16] = {};

	float matrixTranslation[3] = {};
	float matrixRotation[3] = {};
	float matrixScale[3] = {};
};