#include "Framework.h"

UIImage::UIImage(wstring shaderFile)
{
	material = new Material(shaderFile);
	CreateMesh();	
	CreateVP();

	blendState[0] = new BlendState();
	blendState[1] = new BlendState();
	blendState[1]->Alpha(false);

	depthMode[0] = new DepthStencilState();
	depthMode[1] = new DepthStencilState();
	depthMode[1]->DepthEnable(false);
}

UIImage::~UIImage()
{
	delete material;
	delete mesh;

	delete viewBuffer;
	delete projectionBuffer;

	delete blendState[0];
	delete blendState[1];

	delete depthMode[0];
	delete depthMode[1];
}

void UIImage::Update()
{
}

void UIImage::Render()
{
	
	mesh->IASet();

	SetWorldBuffer(); // 0번에
	viewBuffer->SetVSBuffer(1);
	projectionBuffer->SetVSBuffer(2);

	DEVICECONTEXT->PSSetShaderResources(0, 1, &srv);

	material->Set();

	blendState[1]->SetState();
	depthMode[1]->SetState();
	DEVICECONTEXT->DrawIndexed(6, 0, 0);
	blendState[0]->SetState(); // 다시 0번으로 SetState하는 이유 => 1번 SetState하고 다시 안바꿔놓으면 다른거에 영향을 미치기 떄문.
	depthMode[0]->SetState();
}

void UIImage::CreateMesh()
{
	VertexUV vertices[4];
	vertices[0].position = { -0.5f, -0.5f, 0.0f };
	vertices[1].position = { -0.5f, +0.5f, 0.0f };
	vertices[2].position = { +0.5f, -0.5f, 0.0f };
	vertices[3].position = { +0.5f, +0.5f, 0.0f };

	vertices[0].uv = { 0, 1 };
	vertices[1].uv = { 0, 0 };
	vertices[2].uv = { 1, 1 };
	vertices[3].uv = { 1, 0 };

	UINT indices[] = {
		0, 1, 2,
		2, 1, 3
	};

	mesh = new Mesh(vertices, sizeof(VertexUV), 4, indices, 6);
}

void UIImage::CreateVP()
{
	view = XMMatrixIdentity();
	orthographic = XMMatrixOrthographicOffCenterLH(0, WIN_WIDTH, 0, WIN_HEIGHT, -1, 1);

	viewBuffer = new MatrixBuffer();
	viewBuffer->Set(view);

	projectionBuffer = new MatrixBuffer();
	projectionBuffer->Set(orthographic);
}
