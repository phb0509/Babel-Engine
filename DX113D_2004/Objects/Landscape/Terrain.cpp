#include "Framework.h"

Terrain::Terrain()
	: mWidth(10), 
	mHeight(10)
{
	mMaterial = new Material(L"Lighting");
	mMaterial->SetDiffuseMap(L"Textures/Stones.png");
	mMaterial->SetNormalMap(L"Textures/Stones_normal.png");

	mMaterial->GetBuffer()->data.emissive = { 0, 0, 0, 0 };

	mFillMode[0] = new RasterizerState();
	mFillMode[1] = new RasterizerState();
	mFillMode[0]->FillMode(D3D11_FILL_SOLID);
	mFillMode[1]->FillMode(D3D11_FILL_WIREFRAME);
	//D3D11_FILL_SOLID
//	heightMap = Texture::Add(L"Textures/HeightMaps/MyHeightMap.png");
	//heightMap = Texture::Add(L"Textures/HeightMaps/testtest.png"); // 700 * 500
	mHeightMap = Texture::Add(L"Textures/HeightMap.png"); // 100 & 100

	CreateMesh();

	mComputeShader = Shader::AddCS(L"ComputePicking");	

	mStructuredBuffer = new ComputeStructuredBuffer(mInput, sizeof(InputDesc), mPolygonCount,
		sizeof(OutputDesc), mPolygonCount); // ����ü, ����üũ��, ����ü����(������ ����), 

	mRayBuffer = new RayBuffer();
	mOutput = new OutputDesc[mPolygonCount];

	mTypeBuffer = new TypeBuffer();
}

Terrain::~Terrain()
{
	delete mMaterial;
	delete mMesh;	

	delete mFillMode[0];
	delete mFillMode[1];

	delete mRayBuffer;
	delete mStructuredBuffer;

	delete[] mInput;
	delete[] mOutput;

	delete mTypeBuffer;
}

void Terrain::Update()
{
	UpdateWorld();
}

void Terrain::Render()
{
	mMesh->IASet();

	mWorldBuffer->SetVSBuffer(0);	
	mTypeBuffer->SetVSBuffer(5);

	mMaterial->Set();
	mFillMode[0]->SetState();
	DEVICECONTEXT->DrawIndexed((UINT)mIndices.size(), 0, 0);
	//fillMode[0]->SetState();
}

bool Terrain::Picking(OUT Vector3* position)
{
	Ray ray = TARGETCAMERA->ScreenPointToRay(MOUSEPOS);

	for (UINT z = 0; z < mHeight; z++) // 0~255
	{
		for (UINT x = 0; x < mWidth; x++) // 0~255
		{
			UINT index[4];
			index[0] = (mWidth + 1) * z + x; // 0 ~
			index[1] = (mWidth + 1) * z + x + 1; // 1~
			index[2] = (mWidth + 1) * (z + 1) + x; // 1 ~
			index[3] = (mWidth + 1) * (z + 1) + x + 1; // 2 ~

			Vector3 p[4];
			for (UINT i = 0; i < 4; i++)
			{
				p[i] = mVertices[index[i]].position;
			}
				

			float distance;
			if (Intersects(ray.position.data, ray.direction.data, // Intersects�� �������̶� ������ �浹�˻��ϴ� �Լ�. // DirectCollision.h
				p[0].data, p[1].data, p[2].data, distance))
			{
				*position = ray.position + ray.direction * distance;
				return true;
			}

			if (Intersects(ray.position.data, ray.direction.data,
				p[3].data, p[1].data, p[2].data, distance))
			{
				*position = ray.position + ray.direction * distance;
				return true;
			}
		}
	}

	return false;
}

bool Terrain::ComputePicking(OUT Vector3* position)
{	
	Ray ray = TARGETCAMERA->ScreenPointToRay(MOUSEPOS);

	mRayBuffer->data.position = ray.position;
	mRayBuffer->data.direction = ray.direction;
	mRayBuffer->data.size = mPolygonCount; // ������ ����. indices[i] / 3;
	mComputeShader->Set();

	// inputDesc�� CreateMesh�Ҷ� �Ҵ�޾Ƽ� ��ǻƮ���̴��� �ѱ�.
	// size = �����ﰳ��. �� ���Ҹ��� ���ؽ�3�� ������,index(���������)

	mRayBuffer->SetCSBuffer(0);

	DEVICECONTEXT->CSSetShaderResources(0, 1, &mStructuredBuffer->GetSRV()); // SRV ����. �б������ڿ��� �ѱ������ ����� ����.
	DEVICECONTEXT->CSSetUnorderedAccessViews(0, 1, &mStructuredBuffer->GetUAV(), nullptr); // UAV����. �а� �����ִ� �ڿ��� �ѱ������ ����� ����.

	UINT x = ceil((float)mPolygonCount / 1024.0f); // �����ﰳ���� 1024�� ������? ��?

	DEVICECONTEXT->Dispatch(x, 1, 1); // ComputeShader ����.

	mStructuredBuffer->Copy(mOutput, sizeof(OutputDesc) * mPolygonCount);
	
	float minDistance = FLT_MAX;
	int minIndex = -1;

	for (UINT i = 0; i < mPolygonCount; i++)
	{
		OutputDesc temp = mOutput[i];
		if (temp.picked)
		{
			if (minDistance > temp.distance)
			{
				minDistance = temp.distance;
				minIndex = i;
			}
		}
	}

	if (minIndex >= 0)
	{
		*position = ray.position + ray.direction * minDistance;
		return true;
	}

	return false;
}

float Terrain::GetHeight(Vector3 position)
{
	UINT x = (UINT)position.x;
	UINT z = (UINT)position.z;

	if (x < 0 || x > mWidth) return 0.0f;
	if (z < 0 || z > mHeight) return 0.0f;

	UINT index[4];
	index[0] = (mWidth + 1) * z + x;
	index[1] = (mWidth + 1) * (z + 1) + x;
	index[2] = (mWidth + 1) * z + x + 1;
	index[3] = (mWidth + 1) * (z + 1) + x + 1;

	Vector3 p[4];
	for (int i = 0; i < 4; i++)
		p[i] = mVertices[index[i]].position;

	float u = position.x - p[0].x;
	float v = position.z - p[0].z;

	Vector3 result;
	if (u + v <= 1.0f)
	{
		result = p[0] + (p[2] - p[0]) * u + (p[1] - p[0]) * v;
	}
	else
	{
		u = 1.0f - u;
		v = 1.0f - v;

		result = p[3] + (p[1] - p[3]) * u + (p[2] - p[3]) * v;	
	}

	return result.y;
}

void Terrain::CreateMesh()
{
	mWidth = mHeightMap->GetWidth() - 1;
	mHeight = mHeightMap->GetHeight() - 1;

	vector<Float4> pixels = mHeightMap->ReadPixels();// HeightMap�� �ȼ��̴�. position.y�� �������� �ȼ�.

	//Vertices
	for (UINT z = 0; z <= mHeight; z++)
	{
		for (UINT x = 0; x <= mWidth; x++)
		{
			VertexType vertex;
			vertex.position = Float3((float)x, 0.0f, (float)z);
			vertex.uv = Float2(x / (float)mWidth, 1.0f - z / (float)mHeight);

			UINT index = (mWidth + 1) * z + x;
			vertex.position.y += pixels[index].x * 20.0f; // �� ���� ����ȭ�Ǽ� �ʹ� �����ϱ� ������ �� 20.0f�� �����ذ�.

			mVertices.emplace_back(vertex);
		}
	}

	//Indices
	for (UINT z = 0; z < mHeight; z++)
	{
		for (UINT x = 0; x < mWidth; x++)
		{
			mIndices.emplace_back((mWidth + 1) * z + x);//0
			mIndices.emplace_back((mWidth + 1) * (z + 1) + x);//1
			mIndices.emplace_back((mWidth + 1) * (z + 1) + x + 1);//2

			mIndices.emplace_back((mWidth + 1) * z + x);//0
			mIndices.emplace_back((mWidth + 1) * (z + 1) + x + 1);//2
			mIndices.emplace_back((mWidth + 1) * z + x + 1);//3
		}
	}
	 
	mPolygonCount = mIndices.size() / 3;

	mInput = new InputDesc[mPolygonCount];

	for (UINT i = 0; i < mPolygonCount; i++)
	{
		UINT index0 = mIndices[i * 3 + 0];
		UINT index1 = mIndices[i * 3 + 1];
		UINT index2 = mIndices[i * 3 + 2];

		mInput[i].v0 = mVertices[index0].position;
		mInput[i].v1 = mVertices[index1].position;
		mInput[i].v2 = mVertices[index2].position;

		mInput[i].index = i;
	}

	CreateNormal();
	CreateTangent();

	mMesh = new Mesh(mVertices.data(), sizeof(VertexType), (UINT)mVertices.size(),
		mIndices.data(), (UINT)mIndices.size());
}

void Terrain::CreateNormal()
{
	for (UINT i = 0; i < mIndices.size() / 3; i++)
	{
		UINT index0 = mIndices[i * 3 + 0];
		UINT index1 = mIndices[i * 3 + 1];
		UINT index2 = mIndices[i * 3 + 2];

		Vector3 v0 = mVertices[index0].position;
		Vector3 v1 = mVertices[index1].position;
		Vector3 v2 = mVertices[index2].position;

		Vector3 A = v1 - v0;
		Vector3 B = v2 - v0;

		Vector3 normal = Vector3::Cross(A, B).Normal();

		mVertices[index0].normal = normal + mVertices[index0].normal;
		mVertices[index1].normal = normal + mVertices[index1].normal;
		mVertices[index2].normal = normal + mVertices[index2].normal;
	}

	for (VertexType& vertex : mVertices)
		vertex.normal = Vector3(vertex.normal).Normal();

	int a = 0;
	mVertices;
}

void Terrain::CreateTangent()
{
	for (UINT i = 0; i < mIndices.size() / 3; i++)
	{
		UINT index0 = mIndices[i * 3 + 0];
		UINT index1 = mIndices[i * 3 + 1];
		UINT index2 = mIndices[i * 3 + 2];

		VertexType vertex0 = mVertices[index0];
		VertexType vertex1 = mVertices[index1];
		VertexType vertex2 = mVertices[index2];

		Vector3 p0 = vertex0.position;
		Vector3 p1 = vertex1.position;
		Vector3 p2 = vertex2.position;

		Float2 uv0 = vertex0.uv;
		Float2 uv1 = vertex1.uv;
		Float2 uv2 = vertex2.uv;

		Vector3 e0 = p1 - p0;
		Vector3 e1 = p2 - p0;

		float u0 = uv1.x - uv0.x;
		float u1 = uv2.x - uv0.x;
		float v0 = uv1.y - uv0.y;
		float v1 = uv2.y - uv0.y;

		Vector3 tangent = (v1 * e0 - v0 * e1);

		mVertices[index0].tangent = tangent + mVertices[index0].tangent;
		mVertices[index1].tangent = tangent + mVertices[index1].tangent;
		mVertices[index2].tangent = tangent + mVertices[index2].tangent;
	}

	for (VertexType& vertex : mVertices)
	{
		Vector3 t = vertex.tangent;
		Vector3 n = vertex.normal;

		Vector3 temp = (t - n * Vector3::Dot(n, t)).Normal();

		vertex.tangent = temp;
	}
}
