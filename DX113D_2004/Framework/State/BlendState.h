#pragma once

class BlendState
{

public:
	BlendState();
	~BlendState();

	void SetState();
	void Alpha(bool value);
	void AlphaToCoverage(bool value);
	void Additive();
	void Changed();

private:
	ID3D11BlendState* mState;
	D3D11_BLEND_DESC mDesc;

};