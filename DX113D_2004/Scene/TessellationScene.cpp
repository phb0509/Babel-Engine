#include "Framework.h"
#include "TessellationScene.h"

TessellationScene::TessellationScene()
{
	material = new Material(L"TSRect");
	hullShader = Shader::AddHS(L"TSRect");
	domainShader = Shader::AddDS(L"TSRect");

	Vertex vertices[4];
	vertices[0].position = { -0.9f, -0.9f, 0.0f };
	vertices[1].position = { -0.9f, +0.9f, 0.0f };
	vertices[2].position = { +0.9f, -0.9f, 0.0f };
	vertices[3].position = { +0.9f, +0.9f, 0.0f };

	vertexBuffer = new VertexBuffer(vertices, sizeof(Vertex), 4);

	rsState = new RasterizerState();
	rsState->FillMode(D3D11_FILL_WIREFRAME);

	edgeBuffer = new TypeBuffer();
	edgeBuffer->data.values[0] = 2;
	edgeBuffer->data.values[1] = 2;
	edgeBuffer->data.values[2] = 2;
	edgeBuffer->data.values[3] = 2;

	insideBuffer = new TypeBuffer();
	insideBuffer->data.values[0] = 1;
	insideBuffer->data.values[1] = 1;
}

TessellationScene::~TessellationScene()
{
	delete material;
	delete vertexBuffer;
	delete rsState;

	delete edgeBuffer;
	delete insideBuffer;
}

void TessellationScene::Update()
{
}

void TessellationScene::PreRender()
{
}

void TessellationScene::Render()
{
	rsState->SetState();

	vertexBuffer->IASet();
	DEVICECONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	edgeBuffer->SetHSBuffer(10);
	insideBuffer->SetHSBuffer(11);

	material->Set();
	hullShader->Set();
	domainShader->Set();

	DEVICECONTEXT->Draw(4, 0);
}

void TessellationScene::PostRender()
{
	ImGui::SliderInt("Edge0", &edgeBuffer->data.values[0], 0, 100);
	ImGui::SliderInt("Edge1", &edgeBuffer->data.values[1], 0, 100);
	ImGui::SliderInt("Edge2", &edgeBuffer->data.values[2], 0, 100);
	ImGui::SliderInt("Edge3", &edgeBuffer->data.values[3], 0, 100);

	ImGui::SliderInt("Inside0", &insideBuffer->data.values[0], 0, 100);
	ImGui::SliderInt("Inside1", &insideBuffer->data.values[1], 0, 100);
}
