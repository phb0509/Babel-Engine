#include "Framework.h"

Environment::Environment()
{
	mSamplerState = new SamplerState();
	mSamplerState->SetState();
}

Environment::~Environment()
{
	GM->SafeDelete(mSamplerState);
}

void Environment::Set()
{
	SetViewport();
}

void Environment::SetViewport(UINT width, UINT height)
{
	mViewport.Width = (float)width;
	mViewport.Height = (float)height;
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;
	//mViewport.TopLeftX = 0;
	//mViewport.TopLeftY = 0;

	DEVICECONTEXT->RSSetViewports(1, &mViewport);
}

