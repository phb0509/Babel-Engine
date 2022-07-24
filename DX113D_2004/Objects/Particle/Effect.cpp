#include "Framework.h"

Effect::Effect(string file) : mesh(nullptr)
{
	material = new Material(L"Effect");
	buffer = new EffectBuffer();
	ReadFile(file);	
	
	blendState[0] = new BlendState();
	blendState[1] = new BlendState();

	depthState[0] = new DepthStencilState();
	depthState[1] = new DepthStencilState();
	depthState[1]->DepthWriteMask(D3D11_DEPTH_WRITE_MASK_ZERO);

	Reset();
}

Effect::~Effect()
{
	delete material;
	delete mesh;

	delete buffer;

	delete[] vertices;
	delete[] indices;

	delete blendState[0];
	delete blendState[1];
}

void Effect::Reset()
{
	curTime = 0.0f;
	lastAddTime = Timer::Get()->GetRunTime();
	leadCount = gpuCount = activeCount = deactiveCount = 0;	
	vertices = new VertexParticle[data.maxParticles * 4];
	for (UINT i = 0; i < data.maxParticles; i++)
	{
		vertices[i * 4 + 0].uv = Float2(-1, -1);
		vertices[i * 4 + 1].uv = Float2(-1, +1);
		vertices[i * 4 + 2].uv = Float2(+1, -1);
		vertices[i * 4 + 3].uv = Float2(+1, +1);
	}

	indices = new UINT[data.maxParticles * 6];
	for (UINT i = 0; i < data.maxParticles; i++)
	{
		indices[i * 6 + 0] = i * 4 + 0;
		indices[i * 6 + 1] = i * 4 + 1;
		indices[i * 6 + 2] = i * 4 + 2;
		indices[i * 6 + 3] = i * 4 + 2;
		indices[i * 6 + 4] = i * 4 + 1;
		indices[i * 6 + 5] = i * 4 + 3;
	}

	if (mesh != nullptr)
		delete mesh;

	mesh = new Mesh(vertices, sizeof(VertexParticle), data.maxParticles * 4,
		indices, data.maxParticles * 6, true);

	if (data.type == EffectData::BlendType::ADDITIVE)
		blendState[1]->Additive();
	else
		blendState[1]->Alpha(true);
}

void Effect::Add(Vector3& position)
{
	if (Timer::Get()->GetRunTime() - lastAddTime < 60 / 1000.0f)
		return;

	lastAddTime = Timer::Get()->GetRunTime();

	UINT next = leadCount + 1;

	if (next >= data.maxParticles)
	{
		if (data.isLoop == true)
		{
			next = 0;
		}
		else
		{
			next = data.maxParticles;
			return;
		}
	}

	if (next == deactiveCount)
		return;

	Vector3 velocity = Vector3(1, 1, 1);
	velocity *= data.startVelocity;

	float horizontalVelocity = LERP(data.minHorizontalVelocity, data.maxHorizontalVelocity,
		Random(0.0f, 1.0f));
	float horizontalAngle = Random(0.0f, XM_2PI);

	velocity.x += horizontalVelocity * cos(horizontalAngle);
	velocity.y += horizontalVelocity * sin(horizontalAngle);
	velocity.z += LERP(data.minVerticalVelocity, data.maxVerticalVelocity, Random(0.0f, 1.0f));

	Float4 random;
	random.x = Random(0.0f, 1.0f);
	random.y = Random(0.0f, 1.0f);
	random.z = Random(0.0f, 1.0f);
	random.w = Random(0.0f, 1.0f);

	for (UINT i = 0; i < 4; i++)
	{
		vertices[leadCount * 4 + i].position = position;
		vertices[leadCount * 4 + i].velocity = velocity;
		vertices[leadCount * 4 + i].random = random;
		vertices[leadCount * 4 + i].time = curTime;
	}

	leadCount = next;
}

void Effect::Update()
{
	curTime += DELTA;

	MapVertices();
	Activation();
	Deactivation();

	if (activeCount == leadCount)
		curTime = 0.0f;

	buffer->data.minColor = data.minColor;
	buffer->data.maxColor = data.maxColor;

	buffer->data.gravity = data.gravity;
	buffer->data.endVelocity = data.endVelocity;

	buffer->data.rotateSpeed = Float2(data.minRotateSpeed, data.maxRotateSpeed);
	buffer->data.startSize = Float2(data.minStartSize, data.maxStartSize);
	buffer->data.endSize = Float2(data.minEndSize, data.maxEndSize);

	buffer->data.readyTime = data.readyTime;
	buffer->data.readyRandomTime = data.readyRandomTime;

	buffer->data.curTime = curTime;
}

void Effect::Render()
{
	mesh->SetIA();

	buffer->SetVSBuffer(10);

	material->Set();

	blendState[1]->SetState();
	depthState[1]->SetState();

	if (activeCount != gpuCount)
	{
		if(gpuCount > activeCount)
			DEVICECONTEXT->DrawIndexed((gpuCount - activeCount) * 6, activeCount * 6, 0);
		else
		{
			DEVICECONTEXT->DrawIndexed((data.maxParticles - activeCount) * 6, activeCount * 6, 0);

			if (gpuCount > 0)
				DEVICECONTEXT->DrawIndexed(gpuCount * 6, 0, 0);
		}
	}

	blendState[0]->SetState();
	depthState[0]->SetState();
}

void Effect::SetTexture(wstring file)
{
	material->SetDiffuseMap(file);
}

void Effect::MapVertices()
{
	if (leadCount == gpuCount) return;
	
	D3D11_MAPPED_SUBRESOURCE subResource;

	if (leadCount > gpuCount)
	{
		DEVICECONTEXT->Map(mesh->GetVertexBuffer(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

		UINT start = gpuCount * 4;
		UINT size = (leadCount - gpuCount) * sizeof(VertexParticle) * 4;
		UINT offset = gpuCount * sizeof(VertexParticle) * 4;

		BYTE* p = (BYTE*)subResource.pData + offset;
		memcpy(p, vertices + start, size);

		DEVICECONTEXT->Unmap(mesh->GetVertexBuffer(), 0);
	}
	else
	{
		DEVICECONTEXT->Map(mesh->GetVertexBuffer(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

		UINT start = gpuCount * 4;
		UINT size = (data.maxParticles - gpuCount) * sizeof(VertexParticle) * 4;
		UINT offset = gpuCount * sizeof(VertexParticle) * 4;		

		BYTE* p = (BYTE*)subResource.pData + offset;
		memcpy(p, vertices + start, size);

		if (leadCount > 0)
		{
			UINT size = leadCount * sizeof(VertexParticle) * 4;

			memcpy(subResource.pData, vertices, size);
		}

		DEVICECONTEXT->Unmap(mesh->GetVertexBuffer(), 0);
	}

	gpuCount = leadCount;
}

void Effect::Activation()
{
	while (activeCount != gpuCount)
	{
		float age = curTime - vertices[activeCount * 4].time;
		if (age < data.readyTime)
			return;

		vertices[activeCount * 4].time = curTime;
		activeCount++;

		if (activeCount >= data.maxParticles)
			activeCount = (data.isLoop) ? 0 : data.maxParticles;

	}
}

void Effect::Deactivation()
{
	while (deactiveCount != activeCount)
	{
		float age = curTime - vertices[activeCount * 4].time;
		if (age > data.readyTime)
			return;

		deactiveCount++;

		if (deactiveCount >= data.maxParticles)
			deactiveCount = (data.isLoop) ? 0 : data.maxParticles;
	}
}

void Effect::ReadFile(string file)
{
	XmlDocument* document = new XmlDocument();
	document->LoadFile(file.c_str());

	XmlElement* root = document->FirstChildElement();

	XmlElement* node = root->FirstChildElement();
	data.type = (EffectData::BlendType)node->IntText();

	node = node->NextSiblingElement();
	data.isLoop = node->BoolText();

	node = node->NextSiblingElement();	
	data.textureFile = ToWString(node->GetText());
	material->SetDiffuseMap(data.textureFile);

	node = node->NextSiblingElement();
	data.maxParticles = node->IntText();

	node = node->NextSiblingElement();
	data.readyTime = node->FloatText();

	node = node->NextSiblingElement();
	data.readyRandomTime = node->FloatText();

	node = node->NextSiblingElement();
	data.startVelocity = node->FloatText();

	node = node->NextSiblingElement();
	data.endVelocity = node->FloatText();

	node = node->NextSiblingElement();
	data.minHorizontalVelocity = node->FloatText();

	node = node->NextSiblingElement();
	data.maxHorizontalVelocity = node->FloatText();

	node = node->NextSiblingElement();
	data.minVerticalVelocity = node->FloatText();

	node = node->NextSiblingElement();
	data.maxVerticalVelocity = node->FloatText();

	node = node->NextSiblingElement();
	data.gravity.x = node->FloatAttribute("X");
	data.gravity.y = node->FloatAttribute("Y");
	data.gravity.z = node->FloatAttribute("Z");

	node = node->NextSiblingElement();
	data.minColor.x = node->FloatAttribute("R");
	data.minColor.y = node->FloatAttribute("G");
	data.minColor.z = node->FloatAttribute("B");
	data.minColor.w = node->FloatAttribute("A");

	node = node->NextSiblingElement();
	data.maxColor.x = node->FloatAttribute("R");
	data.maxColor.y = node->FloatAttribute("G");
	data.maxColor.z = node->FloatAttribute("B");
	data.maxColor.w = node->FloatAttribute("A");

	node = node->NextSiblingElement();
	data.minRotateSpeed = node->FloatText();

	node = node->NextSiblingElement();
	data.maxRotateSpeed = node->FloatText();

	node = node->NextSiblingElement();
	data.minStartSize = node->FloatText();

	node = node->NextSiblingElement();
	data.maxStartSize = node->FloatText();

	node = node->NextSiblingElement();
	data.minEndSize = node->FloatText();

	node = node->NextSiblingElement();
	data.maxEndSize = node->FloatText();
	
	delete document;
}
