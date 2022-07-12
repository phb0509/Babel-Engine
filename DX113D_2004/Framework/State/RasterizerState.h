#pragma once

class RasterizerState
{

public:
	RasterizerState();
	~RasterizerState();

	void SetState();
	void FillMode(D3D11_FILL_MODE value);
	void FrontCounterClockwise(bool value);
	void Changed();

private:
	ID3D11RasterizerState* mState;
	D3D11_RASTERIZER_DESC mDesc;

};