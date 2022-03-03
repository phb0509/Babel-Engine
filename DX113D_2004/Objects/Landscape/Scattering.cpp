#include "Framework.h"

Scattering::Scattering()	
	: radius(10), slices(32), stacks(16)
{
	targets[0] = new RenderTarget();
	targets[1] = new RenderTarget();

	depthStencil = new DepthStencil();

	rayleighTexture = new UIImage(L"Texture");
	rayleighTexture->mPosition = { 100, 150, 0 };
	rayleighTexture->mScale = { 200, 100, 0 };
	rayleighTexture->SetSRV(targets[0]->GetSRV());

	mieTexture = new UIImage(L"Texture");
	mieTexture->mPosition = { 100, 50, 0 };
	mieTexture->mScale = { 200, 100, 0 };
	mieTexture->SetSRV(targets[1]->GetSRV());

	targetBuffer = new TargetBuffer();

	quadMaterial = new Material(L"ScatteringTarget");

	CreateQuad();
	CreateSphere();

	material = new Material(L"Scattering");
	material->SetDiffuseMap(L"Textures/Landscape/NightSky.png");

	starBuffer = new TimeBuffer();

	depthMode[0] = new DepthStencilState();
	depthMode[1] = new DepthStencilState();
	depthMode[1]->DepthEnable(false);	
}

Scattering::~Scattering()
{
	delete targets[0];
	delete targets[1];

	delete depthStencil;

	delete rayleighTexture;
	delete mieTexture;

	delete targetBuffer;
	delete quadBuffer;

	delete quadMaterial;
	delete material;

	delete mesh;

	delete depthMode[0];
	delete depthMode[1];
}

void Scattering::Update()
{
	starBuffer->data.time = LIGHT->data.lights[0].direction.y;
}

void Scattering::PreRender()
{
	RenderTarget::ClearAndSetWithDSV(targets, 2, depthStencil);

	quadBuffer->IASet();
	DEVICECONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	targetBuffer->SetPSBuffer(10);
	quadMaterial->Set();

	DEVICECONTEXT->Draw(6, 0);
}

void Scattering::Render()
{
	mesh->IASet();

	DEVICECONTEXT->PSSetShaderResources(10, 1, &targets[0]->GetSRV());
	DEVICECONTEXT->PSSetShaderResources(11, 1, &targets[1]->GetSRV());

	starBuffer->SetPSBuffer(10);

	material->Set();

	depthMode[1]->SetState();
	DEVICECONTEXT->DrawIndexed(indexCount, 0, 0);
	depthMode[0]->SetState();
}

void Scattering::PostRener()
{
	rayleighTexture->Render();
	mieTexture->Render();

	ImGui::SliderInt("SampleCount", &targetBuffer->data.sampleCount, 1, 50);
}

void Scattering::CreateQuad()
{
	quadVertices = new VertexUV[6];

	quadVertices[0].position = Float3(-1, -1, 0);
	quadVertices[1].position = Float3(-1, +1, 0);
	quadVertices[2].position = Float3(+1, -1, 0);
	quadVertices[3].position = Float3(+1, -1, 0);
	quadVertices[4].position = Float3(-1, +1, 0);
	quadVertices[5].position = Float3(+1, +1, 0);

	quadVertices[0].uv = Float2(0, 1);
	quadVertices[1].uv = Float2(0, 0);
	quadVertices[2].uv = Float2(1, 1);
	quadVertices[3].uv = Float2(1, 1);
	quadVertices[4].uv = Float2(0, 0);
	quadVertices[5].uv = Float2(1, 0);

	quadBuffer = new VertexBuffer(quadVertices, sizeof(VertexUV), 6);
	delete[] quadVertices;
}

void Scattering::CreateSphere()
{
	UINT domeCount = slices;
	UINT latitude = stacks;
	UINT longitude = domeCount;

	vertexCount = longitude * latitude * 2;
	indexCount = (longitude - 1) * (latitude - 1) * 2 * 6;

	VertexUV* vertices = new VertexUV[vertexCount];

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
			temp *= radius;
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
			temp *= radius;
			vertices[index].position = temp;

			vertices[index].uv.x = 0.5f / (float)longitude + i / (float)longitude;
			vertices[index].uv.y = 0.5f / (float)latitude + j / (float)latitude;

			index++;
		}
	}

	index = 0;
	UINT* indices = new UINT[indexCount];

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

	mesh = new Mesh(vertices, sizeof(VertexUV), vertexCount,
		indices, indexCount);

	delete[] vertices;
	delete[] indices;
}
