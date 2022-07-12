#include "Framework.h"

SamplerState::SamplerState()
	: mDesc{}, mState(nullptr)
{
	mDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	mDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	mDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	mDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	mDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	mDesc.MinLOD = 0;
	mDesc.MaxLOD = D3D11_FLOAT32_MAX;

	Changed();
}

SamplerState::~SamplerState()
{
	mState->Release();
}

void SamplerState::SetState(UINT slot)
{
	DEVICECONTEXT->PSSetSamplers(slot, 1, &mState);
	DEVICECONTEXT->DSSetSamplers(slot, 1, &mState);
}

void SamplerState::Filter(D3D11_FILTER value)
{
	mDesc.Filter = value;
	Changed();
}

void SamplerState::Address(D3D11_TEXTURE_ADDRESS_MODE value)
{
	mDesc.AddressU = value;
	mDesc.AddressV = value;
	mDesc.AddressW = value;
	Changed();
}

void SamplerState::Changed()
{
	if (mState != nullptr)
		mState->Release();

	V(DEVICE->CreateSamplerState(&mDesc, &mState));
}
