#pragma once

class ModelAnimator : public ModelReader
{
protected:
	FrameBuffer* frameBuffer;
	ClipTransform* clipTransform;
	ClipTransform* nodeTransform;

	ID3D11Texture2D* texture;
	ID3D11ShaderResourceView* srv;

	vector<ModelClip*> clips;	

	map<UINT, CallBack> EndEvent;
	map<UINT, CallBackParam> EndParamEvent;	
	map<UINT, int> param;
public:
	ModelAnimator(string file);
	virtual ~ModelAnimator();

	void ReadClip(string file);

	virtual void Update();
	virtual void Render();
	void PostRender();

	void PlayClip(UINT clip, float speed = 1.0f, float takeTime = 0.2f);
	void SetEndEvent(UINT clip, CallBack Event) { EndEvent[clip] = Event; }
	void SetEndParamEvent(UINT clip, CallBackParam Event) { EndParamEvent[clip] = Event; }	
	void SetParam(UINT clip, int value) { param[clip] = value; }

	Matrix GetTransformByNode(int nodeIndex);
	Matrix GetTransformByNode(UINT instance, int nodeIndex);
protected:
	void CreateTexture();
	void CreateClipTransform(UINT index);
};