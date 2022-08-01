#pragma once

class BlendState
{

public:
	BlendState();
	~BlendState();

	void SetState();
	void SetAlpha(bool value);
	void SetAlphaToCoverage(bool value);
	void SetAdditive();
	void Change();

private:
	ID3D11BlendState* mState;
	D3D11_BLEND_DESC mDesc;

};