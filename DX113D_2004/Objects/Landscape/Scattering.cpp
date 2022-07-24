#include "Framework.h"

Scattering::Scattering()	
	: mRadius(10), mSlices(32), mStacks(16)
{
	mRenderTargets[0] = new RenderTarget();
	mRenderTargets[1] = new RenderTarget();

	mDepthStencil = new DepthStencil();

	mRayleighTexture = new UIImage(L"Texture");
	mRayleighTexture->mPosition = { 100, 150, 0 };
	mRayleighTexture->mScale = { 200, 100, 0 };
	mRayleighTexture->SetSRV(mRenderTargets[0]->GetSRV());

	mMieTexture = new UIImage(L"Texture");
	mMieTexture->mPosition = { 100, 50, 0 };
	mMieTexture->mScale = { 200, 100, 0 };
	mMieTexture->SetSRV(mRenderTargets[1]->GetSRV());

	mTargetBuffer = new TargetBuffer();

	mQuadMaterial = new Material(L"ScatteringTarget");

	CreateQuad();
	CreateSphere();

	mMaterial = new Material(L"Scattering");
	mMaterial->SetDiffuseMap(L"Textures/Landscape/NightSky.png");

	mStarBuffer = new TimeBuffer();

	mDepthMode[0] = new DepthStencilState();
	mDepthMode[1] = new DepthStencilState();
	mDepthMode[1]->DepthEnable(false);	
}

Scattering::~Scattering()
{
	delete mRenderTargets[0];
	delete mRenderTargets[1];

	delete mDepthStencil;

	delete mRayleighTexture;
	delete mMieTexture;

	delete mTargetBuffer;
	delete mQuadBuffer;

	delete mQuadMaterial;
	delete mMaterial;

	delete mMesh;

	delete mDepthMode[0];
	delete mDepthMode[1];
}

void Scattering::Update()
{
	mStarBuffer->data.time = mLight->GetLightInfo().direction.y;
}

void Scattering::PreRender()
{
	RenderTarget::ClearAndSetWithDSV(mRenderTargets, 2, mDepthStencil);

	mQuadBuffer->SetIA();
	DEVICECONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	mTargetBuffer->SetPSBuffer(10);
	mQuadMaterial->Set();

	DEVICECONTEXT->Draw(6, 0);
}

void Scattering::Render()
{
	mMesh->SetIA();

	DEVICECONTEXT->PSSetShaderResources(10, 1, &mRenderTargets[0]->GetSRV());
	DEVICECONTEXT->PSSetShaderResources(11, 1, &mRenderTargets[1]->GetSRV());

	mStarBuffer->SetPSBuffer(10);

	mMaterial->Set();

	mDepthMode[1]->SetState();
	DEVICECONTEXT->DrawIndexed(mIndexCount, 0, 0);
	mDepthMode[0]->SetState();
}

void Scattering::PostRener()
{
	mRayleighTexture->Render();
	mMieTexture->Render();

	ImGui::SliderInt("SampleCount", &mTargetBuffer->data.sampleCount, 1, 50);
}

void Scattering::CreateQuad()
{
	mQuadVertices = new VertexUV[6];

	mQuadVertices[0].position = Float3(-1, -1, 0);
	mQuadVertices[1].position = Float3(-1, +1, 0);
	mQuadVertices[2].position = Float3(+1, -1, 0);
	mQuadVertices[3].position = Float3(+1, -1, 0);
	mQuadVertices[4].position = Float3(-1, +1, 0);
	mQuadVertices[5].position = Float3(+1, +1, 0);

	mQuadVertices[0].uv = Float2(0, 1);
	mQuadVertices[1].uv = Float2(0, 0);
	mQuadVertices[2].uv = Float2(1, 1);
	mQuadVertices[3].uv = Float2(1, 1);
	mQuadVertices[4].uv = Float2(0, 0);
	mQuadVertices[5].uv = Float2(1, 0);

	mQuadBuffer = new VertexBuffer(mQuadVertices, sizeof(VertexUV), 6);
	delete[] mQuadVertices;
}

void Scattering::CreateSphere()
{
	UINT domeCount = mSlices;
	UINT latitude = mStacks;
	UINT longitude = domeCount;

	mVertexCount = longitude * latitude * 2;
	mIndexCount = (longitude - 1) * (latitude - 1) * 2 * 6;

	VertexUV* vertices = new VertexUV[mVertexCount];

	UINT index = 0;
	for (UINT i = 0; i < longitude; i++)
	{
		float xz = 100.0f * (i / (longitude - 1.0f)) * XM_PI / 180.0f;

		for (UINT j = 0; j < latitude; j++)
		{
			float y = XM_PI * j / (latitude - 1);

			vertices[index].position.x = sin(xz) * cos(y);
			vertices[index].position.y = cos(xz);
			vertices[index].position.z = sin(xz) * sin(y);
			Vector3 temp = vertices[index].position;
			temp *= mRadius;
			vertices[index].position = temp;

			vertices[index].uv.x = 0.5f / (float)longitude + i / (float)longitude;
			vertices[index].uv.y = 0.5f / (float)latitude + j / (float)latitude;

			index++;
		}
	}

	for (UINT i = 0; i < longitude; i++)
	{
		float xz = 100.0f * (i / (longitude - 1.0f)) * XM_PI / 180.0f;

		for (UINT j = 0; j < latitude; j++)
		{
			float y = (XM_PI * 2.0f) - (XM_PI * j / (latitude - 1));

			vertices[index].position.x = sin(xz) * cos(y);
			vertices[index].position.y = cos(xz);
			vertices[index].position.z = sin(xz) * sin(y);
			Vector3 temp = vertices[index].position;
			temp *= mRadius;
			vertices[index].position = temp;

			vertices[index].uv.x = 0.5f / (float)longitude + i / (float)longitude;
			vertices[index].uv.y = 0.5f / (float)latitude + j / (float)latitude;

			index++;
		}
	}

	index = 0;
	UINT* indices = new UINT[mIndexCount];

	for (UINT i = 0; i < longitude - 1; i++)
	{
		for (UINT j = 0; j < latitude - 1; j++)
		{
			indices[index++] = i * latitude + j;
			indices[index++] = (i + 1) * latitude + j;
			indices[index++] = (i + 1) * latitude + j + 1;

			indices[index++] = (i + 1) * latitude + j + 1;
			indices[index++] = i * latitude + j + 1;
			indices[index++] = i * latitude + j;
		}
	}

	UINT offset = latitude * longitude;
	for (UINT i = 0; i < longitude - 1; i++)
	{
		for (UINT j = 0; j < latitude - 1; j++)
		{
			indices[index++] = offset + i * latitude + j;
			indices[index++] = offset + (i + 1) * latitude + j + 1;
			indices[index++] = offset + (i + 1) * latitude + j;
							   
			indices[index++] = offset + i * latitude + j + 1;
			indices[index++] = offset + (i + 1) * latitude + j + 1;
			indices[index++] = offset + i * latitude + j;
		}
	}

	mMesh = new Mesh(vertices, sizeof(VertexUV), mVertexCount,
		indices, mIndexCount);

	delete[] vertices;
	delete[] indices;
}
