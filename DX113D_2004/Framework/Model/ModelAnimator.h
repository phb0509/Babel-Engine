#pragma once

class ModelAnimator : public ModelReader
{

public:
	ModelAnimator();
	virtual ~ModelAnimator();

	virtual void Update();
	virtual void Render();
	void PostRender();
	void CreateTexture();

	void ReadClip(string modelName, string clipFileName);
	void PlayClip(UINT clip, float speed = 1.0f, float takeTime = 0.2f);
	void SetEndEvent(UINT clip, CallBack Event) { mEndEvent[clip] = Event; }
	void SetEndParamEvent(UINT clip, CallBackParam Event) { mEndParamEvent[clip] = Event; }
	void SetParam(UINT clip, int value) { mParam[clip] = value; }

	Matrix GetTransformByNode(int nodeIndex);
	Matrix GetTransformByNode(UINT instance, int nodeIndex);
	
	const vector<string>& GetClipNames() { return mClipNames; }
	const vector<ModelClip*>& GetClips() { return mClips; }

	int GetCurrentClipFrame() { return mCurrentClipFrame; }
	int GetCurrentClipFrameCount() { return mCurrentClipFrameCount; }

	void PlayAnimation() { mbIsPlayedAnimation = true; }
	void StopAnimation() { mbIsPlayedAnimation = false; }
	void TestEvent();
	FrameBuffer* GetFrameBuffer() { return mFrameBuffer; }


	// Model Function
	void MakeBoneTransform();
	void SetBoneTransforms();
	void ExecuteSetMeshEvent();

protected:
	// 본을 텍스쳐로 만듬 (Shader로 넘길 수 있는 최대 한계는 16바이트 * 5000.
	// 본 하나 매프레임마다 수많은 키프레임이 있기 때문에 한계를 넘는다.
	// 그래서 텍스쳐로 만들어서 대량의 데이터를 넘겨줘야함.

	void CreateClipTransform(UINT index);


protected:
	FrameBuffer* mFrameBuffer;
	ClipTransform* mClipTransform;
	ClipTransform* mNodeTransform;

	ID3D11Texture2D* mTexture;
	ID3D11ShaderResourceView* mSRV;

	vector<ModelClip*> mClips;
	vector<string> mClipNames;

	map<UINT, CallBack> mEndEvent;
	map<UINT, CallBackParam> mEndParamEvent;
	map<UINT, int> mParam;
	
	int mCurrentClipFrame;
	int mCurrentClipFrameCount;
	bool mbIsPlayedAnimation;

	//Model
	BoneBuffer* mBoneBuffer;
	map<int, Matrix> mBoneTransforms;
	Matrix* mNodeTransforms;
};