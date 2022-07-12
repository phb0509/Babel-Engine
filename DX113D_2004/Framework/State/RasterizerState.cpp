#include "Framework.h"

RasterizerState::RasterizerState()
	: mDesc{}, mState(nullptr)
{
	mDesc.CullMode = D3D11_CULL_BACK;
	mDesc.FillMode = D3D11_FILL_SOLID;

	Changed();
}

RasterizerState::~RasterizerState()
{
	mState->Release();
}

void RasterizerState::SetState()
{
	DEVICECONTEXT->RSSetState(mState);
}

void RasterizerState::FillMode(D3D11_FILL_MODE value)
{
	mDesc.FillMode = value;

	Changed();
}

void RasterizerState::FrontCounterClockwise(bool value)
{
	mDesc.FrontCounterClockwise = value;

	Changed();
}

void RasterizerState::Changed()
{
	if (mState != nullptr)
		mState->Release();

	V(DEVICE->CreateRasterizerState(&mDesc, &mState));
}
