#include "Framework.h"

Environment::Environment()
{
	mSamplerState = new SamplerState();
	mSamplerState->SetState();

	mBlendState[0] = new BlendState();
	mBlendState[1] = new BlendState();
	mBlendState[1]->Alpha(true);

	mDepthState[0] = new DepthStencilState();
	mDepthState[1] = new DepthStencilState();
	mDepthState[1]->DepthEnable(false);
}

Environment::~Environment()
{
	delete mSamplerState;
	
}

void Environment::PostRender()
{
	showLightInformation();
	mDepthState[1]->SetState();
}

void Environment::showLightInformation()
{
	
}

void Environment::Set()
{
	SetViewport();
	mBlendState[0]->SetState();
	mDepthState[0]->SetState();
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

