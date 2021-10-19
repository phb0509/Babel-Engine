#pragma once

class ModelAnimator : public ModelReader
{

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
	// 본을 텍스쳐로 만듬 (Shader로 넘길 수 있는 최대 한계는 16바이트 * 5000.
	// 본 하나 매프레임마다 수많은 키프레임이 있기 때문에 한계를 넘는다.
	// 그래서 텍스쳐로 만들어서 대량의 데이터를 넘겨줘야함.
	void CreateTexture();
	void CreateClipTransform(UINT index);


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
};