#include "Framework.h"

Particle::Particle(wstring diffuseFile)
	: particleCount(MAX_COUNT)
{
	isActive = false;

	material = new Material();
	material->SetDiffuseMap(diffuseFile);

	blendState[0] = new BlendState();
	blendState[1] = new BlendState();
	blendState[1]->Additive();

	depthState[0] = new DepthStencilState();
	depthState[1] = new DepthStencilState();
	depthState[1]->DepthWriteMask(D3D11_DEPTH_WRITE_MASK_ZERO);
}

Particle::~Particle()
{
	delete material;
	delete vertexBuffer;

	delete blendState[0];
	delete blendState[1];

	delete depthState[0];
	delete depthState[1];
}

void Particle::Render()
{
	if (!isActive)
		return;

	blendState[1]->SetState();
	depthState[1]->SetState();

	SetWorldBuffer();
	vertexBuffer->IASet();
	DEVICECONTEXT->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	CAMERA->GetViewBuffer()->SetGSBuffer(1);
	Environment::Get()->GetProjectionBuffer()->SetGSBuffer(2);

	material->Set();
	geometryShader->Set();

	DEVICECONTEXT->Draw(particleCount, 0);

	DEVICECONTEXT->GSSetShader(nullptr, nullptr, 0);

	blendState[0]->SetState();
	depthState[0]->SetState();
}

void Particle::Play(Vector3 position)
{
	isActive = true;
	this->mPosition = position;
}

void Particle::Stop()
{
	isActive = false;
}
