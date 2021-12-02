#pragma once

class ModelAnimator : public ModelReader
{

public:
	ModelAnimator();
	virtual ~ModelAnimator();

	void ReadClip(string file);

	virtual void Update();
	virtual void Render();
	void PostRender();

	void PlayClip(UINT clip, float speed = 1.0f, float takeTime = 0.2f);
	void SetEndEvent(UINT clip, CallBack Event) { mEndEvent[clip] = Event; }
	void SetEndParamEvent(UINT clip, CallBackParam Event) { mEndParamEvent[clip] = Event; }	
	void SetParam(UINT clip, int value) { mParam[clip] = value; }

	Matrix GetTransformByNode(int nodeIndex);
	Matrix GetTransformByNode(UINT instance, int nodeIndex);
	int GetModelClipsSize() { return mModelClips.size(); }


	// Model Function
	void MakeBoneTransform();
	void SetBoneTransforms();
	void ExecuteSetMeshEvent();

protected:
	// ���� �ؽ��ķ� ���� (Shader�� �ѱ� �� �ִ� �ִ� �Ѱ�� 16����Ʈ * 5000.
	// �� �ϳ� �������Ӹ��� ������ Ű�������� �ֱ� ������ �Ѱ踦 �Ѵ´�.
	// �׷��� �ؽ��ķ� ���� �뷮�� �����͸� �Ѱ������.
	void CreateTexture();
	void CreateClipTransform(UINT index);
	




protected:
	FrameBuffer* mFrameBuffer;
	ClipTransform* mClipTransform;
	ClipTransform* mNodeTransform;

	ID3D11Texture2D* mTexture;
	ID3D11ShaderResourceView* mSRV;

	vector<ModelClip*> mModelClips;

	map<UINT, CallBack> mEndEvent;
	map<UINT, CallBackParam> mEndParamEvent;
	map<UINT, int> mParam;

	//Model
	BoneBuffer* mBoneBuffer;
	map<int, Matrix> mBoneTransforms;
	Matrix* mNodeTransforms;
};