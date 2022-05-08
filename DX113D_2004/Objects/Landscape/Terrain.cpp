#include "Framework.h"

Terrain::Terrain(): 
	mTerrainWidth(10), 
	mTerrainHeight(10),
	mDistanceBetweenNodes(0.0f,0.0f),
    mNodeCount(30,30)
{
	mMaterial = new Material(L"Lighting");
	mMaterial->SetDiffuseMap(L"Textures/dirt.png");
	//mMaterial->SetNormalMap(L"Textures/Stones_normal.png");

	mMaterial->GetBuffer()->data.emissive = { 0, 0, 0, 0 };

	mFillMode[0] = new RasterizerState();
	mFillMode[1] = new RasterizerState();
	mFillMode[0]->FillMode(D3D11_FILL_SOLID);
	mFillMode[1]->FillMode(D3D11_FILL_WIREFRAME);
	//D3D11_FILL_SOLID
	//heightMap = Texture::Add(L"Textures/HeightMaps/MyHeightMap.png");
	//heightMap = Texture::Add(L"Textures/HeightMaps/testtest.png"); // 700 * 500
	//mHeightMap = Texture::Add(L"Textures/500x500.png"); // 256 * 256
	mHeightMap = Texture::Add(L"Textures/HeightMap256.png"); // 256 * 256

	createMesh();

	mComputeShader = Shader::AddCS(L"ComputePicking");	

	mStructuredBuffer = new ComputeStructuredBuffer(mInput, sizeof(InputDesc), mPolygonCount,
		sizeof(OutputDesc), mPolygonCount); // 구조체, 구조체크기, 구조체개수(폴리곤 개수), 

	mRayBuffer = new RayBuffer();
	mOutput = new OutputDesc[mPolygonCount];

	mTypeBuffer = new TypeBuffer();

	createNodeMap();
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
	Ray ray = mCamera->ScreenPointToRay(MOUSEPOS);

	for (UINT z = 0; z < mTerrainHeight; z++) // 0~255
	{
		for (UINT x = 0; x < mTerrainWidth; x++) // 0~255
		{
			UINT index[4];
			index[0] = (mTerrainWidth + 1) * z + x; // 0 ~
			index[1] = (mTerrainWidth + 1) * z + x + 1; // 1~
			index[2] = (mTerrainWidth + 1) * (z + 1) + x; // 1 ~
			index[3] = (mTerrainWidth + 1) * (z + 1) + x + 1; // 2 ~

			Vector3 p[4];
			for (UINT i = 0; i < 4; i++)
			{
				p[i] = mVertices[index[i]].position;
			}
				
			float distance;
			if (Intersects(ray.position.data, ray.direction.data, // Intersects는 폴리곤이랑 반직선 충돌검사하는 함수. // DirectCollision.h
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
	Ray ray = mCamera->ScreenPointToRay(MOUSEPOS);

	mRayBuffer->data.position = ray.position;
	mRayBuffer->data.direction = ray.direction;
	mRayBuffer->data.size = mPolygonCount; // 폴리곤 개수. indices[i] / 3;
	mComputeShader->Set();

	// inputDesc는 CreateMesh할때 할당받아서 컴퓨트셰이더에 넘김.
	// size = 폴리곤개수. 각 원소마다 버텍스3개 포지션,index(폴리곤단위)

	mRayBuffer->SetCSBuffer(0);

	DEVICECONTEXT->CSSetShaderResources(0, 1, &mStructuredBuffer->GetSRV()); // SRV 셋팅. 읽기전용자원을 넘기기위한 어댑터 설정.
	DEVICECONTEXT->CSSetUnorderedAccessViews(0, 1, &mStructuredBuffer->GetUAV(), nullptr); // UAV셋팅. 읽고 쓸수있는 자원을 넘기기위한 어댑터 설정.

	UINT x = ceil((float)mPolygonCount / 1024.0f); // 폴리곤개수를 1024로 나눴네?

	DEVICECONTEXT->Dispatch(x, 1, 1); // ComputeShader 실행.

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

float Terrain::GetTargetPositionY(Vector3 target)
{
	UINT x = (UINT)target.x;
	UINT z = (UINT)target.z;

	if (x < 0 || x >= mTerrainWidth) return 0.0f;
	if (z < 0 || z >= mTerrainHeight) return 0.0f;

	UINT index[4];
	index[0] = (mTerrainWidth + 1) * z + x;
	index[1] = (mTerrainWidth + 1) * (z + 1) + x;
	index[2] = (mTerrainWidth + 1) * z + x + 1;
	index[3] = (mTerrainWidth + 1) * (z + 1) + x + 1;

	Vector3 p[4];

	for (int i = 0; i < 4; i++)
	{
		p[i] = mVertices[index[i]].position;
	}
		
	float u = target.x - p[0].x;
	float v = target.z - p[0].z;

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

void Terrain::createMesh()
{
	mTerrainWidth = mHeightMap->GetWidth()-1;
	mTerrainHeight = mHeightMap->GetHeight()-1;
	vector<Float4> pixels = mHeightMap->ReadPixels();
	 
	//Vertices
	for (UINT z = 0; z <= mTerrainHeight; z++) // 0~255
	{
		for (UINT x = 0; x <= mTerrainWidth; x++)
		{
			VertexType vertex;
			vertex.position = Float3((float)x, 0.0f, (float)z);
			vertex.uv = Float2(x / (float)mTerrainWidth, 1.0f - z / (float)mTerrainHeight);

			UINT index = (mTerrainWidth + 1) * z + x;
			vertex.position.y += pixels[index].x * 20.0f; // 걍 값이 정규화되서 너무 작으니까 임의의 값 20.0f를 곱해준것.

			mVertices.emplace_back(vertex);
		}
	}

	//Indices
	for (UINT z = 0; z < mTerrainHeight; z++)
	{
		for (UINT x = 0; x < mTerrainWidth; x++)
		{
			mIndices.emplace_back((mTerrainWidth + 1) * z + x);//0
			mIndices.emplace_back((mTerrainWidth + 1) * (z + 1) + x);//1
			mIndices.emplace_back((mTerrainWidth + 1) * (z + 1) + x + 1);//2

			mIndices.emplace_back((mTerrainWidth + 1) * z + x);//0
			mIndices.emplace_back((mTerrainWidth + 1) * (z + 1) + x + 1);//2
			mIndices.emplace_back((mTerrainWidth + 1) * z + x + 1);//3
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

	createNormal();
	createTangent();

	mMesh = new Mesh(mVertices.data(), sizeof(VertexType), (UINT)mVertices.size(),
		mIndices.data(), (UINT)mIndices.size());
}

void Terrain::createNormal()
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

void Terrain::createTangent()
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

void Terrain::createNodeMap()
{
	Float2 size = GetSize();

	mDistanceBetweenNodes.x = size.x / mNodeCount.x; // mWidth,mHeight 의 default값 20.
	mDistanceBetweenNodes.y = size.y / mNodeCount.y;

	for (UINT z = 0; z < mNodeCount.y; z++)
	{
		for (UINT x = 0; x < mNodeCount.x; x++)
		{
			Vector3 pos = Vector3(x * mDistanceBetweenNodes.x, 0, z * mDistanceBetweenNodes.y);
			pos.y = GetTargetPositionY(pos);

			int index = z * mNodeCount.x + x;
			mNodeMap.emplace_back(new Node(pos, index, mDistanceBetweenNodes));
		}
	}

	for (UINT i = 0; i < mNodeMap.size(); i++)
	{
		if (i % mNodeCount.x != mNodeCount.x - 1)
		{
			mNodeMap[i + 0]->AddEdge(mNodeMap[i + 1]);
			mNodeMap[i + 1]->AddEdge(mNodeMap[i + 0]);
		}

		if (i < mNodeMap.size() - mNodeCount.x)
		{
			mNodeMap[i + 0]->AddEdge(mNodeMap[i + mNodeCount.x]);
			mNodeMap[i + mNodeCount.x]->AddEdge(mNodeMap[i + 0]);
		}

		if (i < mNodeMap.size() - mNodeCount.x && i % mNodeCount.x != mNodeCount.x - 1)
		{
			mNodeMap[i + 0]->AddEdge(mNodeMap[i + mNodeCount.x + 1]);
			mNodeMap[i + mNodeCount.x + 1]->AddEdge(mNodeMap[i + 0]);
		}

		if (i < mNodeMap.size() - mNodeCount.x && i % mNodeCount.x != 0)
		{
			mNodeMap[i + 0]->AddEdge(mNodeMap[i + mNodeCount.x - 1]);
			mNodeMap[i + mNodeCount.x - 1]->AddEdge(mNodeMap[i + 0]);
		}
	}
}
