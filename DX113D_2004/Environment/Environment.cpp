#include "Framework.h"

Environment::Environment()
{
	mSamplerState = new SamplerState();
	mSamplerState->SetState();

	mBlendStates[0] = new BlendState();
	mBlendStates[1] = new BlendState();
	mBlendStates[1]->SetAlpha(true);

	mDepthStates[0] = new DepthStencilState();
	mDepthStates[1] = new DepthStencilState();
	mDepthStates[1]->DepthEnable(false);
}

Environment::~Environment()
{
	GM->SafeDelete(mSamplerState);

	for (auto blendState : mBlendStates)
	{
		GM->SafeDelete(blendState);
	}

	for (auto depthState : mDepthStates)
	{
		GM->SafeDelete(depthState);
	}
}

void Environment::PostRender()
{
	showLightInformation();
	mDepthStates[1]->SetState();
}

void Environment::showLightInformation()
{
}

void Environment::Set()
{
	SetViewport();
	mBlendStates[0]->SetState();
	mDepthStates[0]->SetState();
}

void Environment::SetViewport(UINT width, UINT height)
{
	mViewport.Width = (float)width;
	mViewport.Height = (float)height;
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;
	mViewport.TopLeftX = 0;
	mViewport.TopLeftY = 0;

	DEVICECONTEXT->RSSetViewports(1, &mViewport);
}

