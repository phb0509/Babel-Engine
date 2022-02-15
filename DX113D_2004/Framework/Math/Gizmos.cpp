#include "Framework.h"

Gizmos::Gizmos()
{
	createMesh();
	createGizmosHashColor();
	//createMeshForColorPicking();

	mMaterial = new Material(L"Gizmos");
	mRSState = new RasterizerState();
}

Gizmos::~Gizmos()
{
	delete mMaterial;
}

void Gizmos::Update()
{
	UpdateWorld();
}

void Gizmos::PreRender()
{
	mMaterial->SetShader(L"GizmosColorPicking");

	Transform::SetWorldBuffer();

	mMesh->IASet();
	mMaterial->Set();

	Environment::Get()->SetOrthographicProjectionBuffer(); // Set ProjectionBuffer
	DEVICECONTEXT->DrawIndexed(mIndices.size(), 0, 0); // Draw Gizmos
	Environment::Get()->SetPerspectiveProjectionBuffer(); // Perspective로 다시 돌려놓기.
}

void Gizmos::Render()
{
	mMaterial->SetShader(L"Gizmos");

	Transform::SetWorldBuffer();

	mMesh->IASet();
	mMaterial->Set();

	//mRSState->FillMode(D3D11_FILL_WIREFRAME);
	mRSState->FillMode(D3D11_FILL_SOLID);
	mRSState->SetState();

	Environment::Get()->SetOrthographicProjectionBuffer();
	DEVICECONTEXT->DrawIndexed(mIndices.size(), 0, 0);
	Environment::Get()->SetPerspectiveProjectionBuffer();
}

void Gizmos::createMesh()
{
	float length = 3.0f;
	float thickness = 0.5f;

	//Axis X		

	Float4 color = { 0.2f, 0, 0, 1 };
	mVertices.emplace_back(Float3(0, 0, 0), color, mHashColor.x);
	mVertices.emplace_back(Float3(length, 0, 0), color, mHashColor.x);
	mVertices.emplace_back(Float3(length, thickness, 0), color, mHashColor.x);
	mVertices.emplace_back(Float3(length, 0, thickness), color, mHashColor.x);

	mIndices.emplace_back(0);
	mIndices.emplace_back(2);
	mIndices.emplace_back(1);

	mIndices.emplace_back(0);
	mIndices.emplace_back(1);
	mIndices.emplace_back(3);

	mIndices.emplace_back(0);
	mIndices.emplace_back(3);
	mIndices.emplace_back(2);

	mIndices.emplace_back(1);
	mIndices.emplace_back(2);
	mIndices.emplace_back(3);

	//Axis Y
	color = { 0, 0.2f, 0, 1 };
	mVertices.emplace_back(Float3(0, 0, 0), color, mHashColor.y);
	mVertices.emplace_back(Float3(0, length, 0), color, mHashColor.y);
	mVertices.emplace_back(Float3(0, length, thickness), color, mHashColor.y);
	mVertices.emplace_back(Float3(thickness, length, 0), color, mHashColor.y);

	mIndices.emplace_back(4);
	mIndices.emplace_back(6);
	mIndices.emplace_back(5);

	mIndices.emplace_back(4);
	mIndices.emplace_back(5);
	mIndices.emplace_back(7);

	mIndices.emplace_back(4);
	mIndices.emplace_back(7);
	mIndices.emplace_back(6);

	mIndices.emplace_back(5);
	mIndices.emplace_back(6);
	mIndices.emplace_back(7);

	//Axis Z
	color = { 0, 0, 0.2f, 1 };
	mVertices.emplace_back(Float3(0, 0, 0), color, mHashColor.z);
	mVertices.emplace_back(Float3(0, 0, length), color, mHashColor.z);
	mVertices.emplace_back(Float3(thickness, 0, length), color, mHashColor.z);
	mVertices.emplace_back(Float3(0, thickness, length), color, mHashColor.z);

	mIndices.emplace_back(8);
	mIndices.emplace_back(10);
	mIndices.emplace_back(9);

	mIndices.emplace_back(8);
	mIndices.emplace_back(9);
	mIndices.emplace_back(11);

	mIndices.emplace_back(8);
	mIndices.emplace_back(11);
	mIndices.emplace_back(10);

	mIndices.emplace_back(9);
	mIndices.emplace_back(10);
	mIndices.emplace_back(11);

	mMesh = new Mesh(mVertices.data(), sizeof(VertexColor), mVertices.size(),
		mIndices.data(), mIndices.size());
}

void Gizmos::createGizmosHashColor()
{
	int hashValueX = rand() % 1000000;
	int a = (hashValueX >> 24) & 0xff;
	int b = (hashValueX >> 16) & 0xff;
	int g = (hashValueX >> 8) & 0xff;
	int r = hashValueX & 0xff;

	float fr = static_cast<float>(r);
	float fg = static_cast<float>(g);
	float fb = static_cast<float>(b);

	Float4 tempColor = { fr / 255.0f,fg / 255.0f,fb / 255.0f,1.0f };

	mHashColor.x = tempColor;

	int hashValueY = rand() % 1000000;
	a = (hashValueY >> 24) & 0xff;
	b = (hashValueY >> 16) & 0xff;
	g = (hashValueY >> 8) & 0xff;
	r = hashValueY & 0xff;

	fr = static_cast<float>(r);
	fg = static_cast<float>(g);
	fb = static_cast<float>(b);

	tempColor = { fr / 255.0f,fg / 255.0f,fb / 255.0f,1.0f };

	mHashColor.y = tempColor;

	int hashValueZ = rand() % 1000000;
	a = (hashValueZ >> 24) & 0xff;
	b = (hashValueZ >> 16) & 0xff;
	g = (hashValueZ >> 8) & 0xff;
	r = hashValueZ & 0xff;

	fr = static_cast<float>(r);
	fg = static_cast<float>(g);
	fb = static_cast<float>(b);

	tempColor = { fr / 255.0f,fg / 255.0f,fb / 255.0f,1.0f };

	mHashColor.z = tempColor;
}

void Gizmos::createMeshForColorPicking()
{
}
