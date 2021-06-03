#include "Framework.h"

TerrainEditor::TerrainEditor(UINT width, UINT height)
	: width(width), height(height), isRaise(true), adjustValue(50),
	heightMap(nullptr), inputFileName{},
	structuredBuffer(nullptr), output(nullptr), rayBuffer(nullptr),
	isPainting(true), paintValue(5), selectMap(0)
{
	material = new Material(L"TerrainSplatting");
	material->SetDiffuseMap(L"Textures/Landscape/Dirt2.png");	

	alphaMap = Texture::Add(L"Textures/HeightMaps/AlphaMap.png");
	secondMap = Texture::Add(L"Textures/Landscape/Floor.png");
	thirdMap = Texture::Add(L"Textures/Landscape/Stones.png");

	CreateMesh();
	CreateCompute();	

	brushBuffer = new BrushBuffer();
}

TerrainEditor::~TerrainEditor()
{
	delete material;
	delete mesh;

	delete rayBuffer;
	delete structuredBuffer;

	delete[] input;
	delete[] output;

	delete brushBuffer;
}

void TerrainEditor::Update()
{		
	if (KEY_PRESS(VK_LBUTTON) && !ImGui::GetIO().WantCaptureMouse)
	{
		if (isPainting)
			PaintBrush(brushBuffer->data.location);
		else
			AdjustY(brushBuffer->data.location);
	}

	if (KEY_UP(VK_LBUTTON))
	{
		CreateNormal(); // 법선벡터 구하는함수.
		CreateTangent();
		mesh->UpdateVertex(vertices.data(), vertices.size());
	}

	UpdateWorld();
}

void TerrainEditor::Render()
{
	mesh->IASet();

	mWorldBuffer->SetVSBuffer(0);
	brushBuffer->SetPSBuffer(10);

	alphaMap->PSSet(10);
	secondMap->PSSet(11);
	thirdMap->PSSet(12);

	material->Set();
		
	DC->DrawIndexed((UINT)indices.size(), 0, 0);	
}

void TerrainEditor::PostRender()
{
	Vector3 temp;
	ComputePicking(&temp);
	brushBuffer->data.location = temp;

	ImGui::Text("TerainEditor");
	ImGui::SliderInt("Type", &brushBuffer->data.type, 0, 1);
	ImGui::SliderFloat("Range", &brushBuffer->data.range, 1, 50);
	ImGui::ColorEdit3("Color", (float*)&brushBuffer->data.color);
	ImGui::Checkbox("Raise", &isRaise);
	ImGui::Checkbox("Painting", &isPainting);
	ImGui::InputInt("SelectMap", &selectMap);
	
	ImGui::InputText("FileName", inputFileName, 100);
	wstring heightFile = L"Textures/HeightMaps/" + ToWString(inputFileName) + L".png";
	if (ImGui::Button("Save"))
	{		
		Save(heightFile);
	}

	if (ImGui::Button("Open File Dialog"))
		igfd::ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".png,.jpg,.dds", ".");
		
	// display
	if (igfd::ImGuiFileDialog::Instance()->FileDialog("ChooseFileDlgKey"))
	{
		if (igfd::ImGuiFileDialog::Instance()->IsOk == true)
		{			
			string fileName = igfd::ImGuiFileDialog::Instance()->GetCurrentFileName();
			Load(L"Textures/HeightMaps/" + ToWString(fileName));
		}
		// close
		igfd::ImGuiFileDialog::Instance()->CloseDialog("ChooseFileDlgKey");
	}
}

bool TerrainEditor::ComputePicking(OUT Vector3* position)
{
	Ray ray = CAMERA->ScreenPointToRay(MOUSEPOS);

	rayBuffer->data.position = ray.position;
	rayBuffer->data.direction = ray.direction;
	rayBuffer->data.size = size;
	computeShader->Set();

	rayBuffer->SetCSBuffer(0);

	DC->CSSetShaderResources(0, 1, &structuredBuffer->GetSRV());
	DC->CSSetUnorderedAccessViews(0, 1, &structuredBuffer->GetUAV(), nullptr);

	UINT x = ceil((float)size / 1024.0f);

	DC->Dispatch(x, 1, 1);

	structuredBuffer->Copy(output, sizeof(OutputDesc) * size);

	float minDistance = FLT_MAX;
	int minIndex = -1;

	for (UINT i = 0; i < size; i++)
	{
		OutputDesc temp = output[i];
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

void TerrainEditor::AdjustY(Vector3 position)
{
	switch (brushBuffer->data.type)
	{
	case 0:
	{
		for (VertexType& vertex : vertices)
		{
			Vector3 p1 = Vector3(vertex.position.x, 0, vertex.position.z);
			Vector3 p2 = Vector3(position.x, 0, position.z);

			float distance = (p2 - p1).Length();

			float temp = adjustValue * max(0, cos(XM_PIDIV2 * distance / brushBuffer->data.range));

			if (distance <= brushBuffer->data.range)
			{
				if(isRaise)
					vertex.position.y += temp * DELTA;
				else
					vertex.position.y -= temp * DELTA;

				if (vertex.position.y < 0)
					vertex.position.y = 0;
				else if (vertex.position.y > MAX_HEIGHT)
					vertex.position.y = MAX_HEIGHT;
			}
		}
	}
		break;
	default:
		break;
	}	

	mesh->UpdateVertex(vertices.data(), vertices.size());
}

void TerrainEditor::PaintBrush(Vector3 position)
{
	switch (brushBuffer->data.type)
	{
	case 0:
	{
		for (VertexType& vertex : vertices)
		{
			Vector3 p1 = Vector3(vertex.position.x, 0, vertex.position.z);
			Vector3 p2 = Vector3(position.x, 0, position.z);

			float distance = (p2 - p1).Length();

			float temp = paintValue * max(0, cos(XM_PIDIV2 * distance / brushBuffer->data.range));

			if (distance <= brushBuffer->data.range)
			{
				if (isRaise)
					vertex.alpha[selectMap] += temp * DELTA;
				else
					vertex.alpha[selectMap] -= temp * DELTA;

				vertex.alpha[selectMap] = Saturate(vertex.alpha[selectMap]);
			}
		}
	}
	break;
	default:
		break;
	}

	mesh->UpdateVertex(vertices.data(), vertices.size());
}

void TerrainEditor::Save(wstring heightFile)
{
	UINT size = width * height * 4;
	uint8_t* pixels = new uint8_t[size];

	for (UINT i = 0; i < size / 4; i++)
	{
		float y = vertices[i].position.y;

		uint8_t height = y * 255 / MAX_HEIGHT;

		pixels[i * 4 + 0] = height;
		pixels[i * 4 + 1] = height;
		pixels[i * 4 + 2] = height;
		pixels[i * 4 + 3] = 255;
	}

	Image image;
	image.width = width;
	image.height = height;
	image.pixels = pixels;
	image.format = DXGI_FORMAT_R8G8B8A8_UNORM;
	image.rowPitch = image.width * 4;

	image.slicePitch = image.width * image.height * 4;

	SaveToWICFile(image, WIC_FLAGS_FORCE_RGB, GetWICCodec(WIC_CODEC_PNG),
		heightFile.c_str());
}

void TerrainEditor::Load(wstring heightFile)
{
	heightMap = Texture::Load(heightFile);

	delete mesh;

	CreateMesh();
	CreateCompute();
}

void TerrainEditor::CreateMesh()
{
	vector<Float4> pixels;

	if (heightMap != nullptr)
	{
		width = heightMap->Width();
		height = heightMap->Height();
		pixels = heightMap->ReadPixels();
	}

	vertices.clear();
	indices.clear();

	//Vertices
	for (UINT z = 0; z < height; z++)
	{
		for (UINT x = 0; x < width; x++)
		{
			VertexType vertex;
			vertex.position = Float3((float)x, 0.0f, (float)z);
			vertex.uv = Float2(x / (float)width, 1.0f - z / (float)height);

			UINT index = width * z + x;
			if(pixels.size() > index)
				vertex.position.y += pixels[index].x * MAX_HEIGHT;

			vertices.emplace_back(vertex);
		}
	}

	//Indices
	for (UINT z = 0; z < height - 1; z++)
	{
		for (UINT x = 0; x < width - 1; x++)
		{
			indices.emplace_back(width * z + x);//0
			indices.emplace_back(width * (z + 1) + x);//1
			indices.emplace_back(width * (z + 1) + x + 1);//2

			indices.emplace_back(width * z + x);//0
			indices.emplace_back(width * (z + 1) + x + 1);//2
			indices.emplace_back(width * z + x + 1);//3
		}
	}

	size = indices.size() / 3;

	input = new InputDesc[size];
	for (UINT i = 0; i < size; i++)
	{
		UINT index0 = indices[i * 3 + 0];
		UINT index1 = indices[i * 3 + 1];
		UINT index2 = indices[i * 3 + 2];

		input[i].v0 = vertices[index0].position;
		input[i].v1 = vertices[index1].position;
		input[i].v2 = vertices[index2].position;

		input[i].index = i;
	}

	CreateNormal();
	CreateTangent();

	mesh = new Mesh(vertices.data(), sizeof(VertexType), (UINT)vertices.size(),
		indices.data(), (UINT)indices.size());
}

void TerrainEditor::CreateNormal() // 법선벡터구하는 함수.
{
	for (UINT i = 0; i < indices.size() / 3; i++) // 폴리곤갯수만큼 반복. (한폴리곤에 인덱스3개니까)
	{
		UINT index0 = indices[i * 3 + 0];
		UINT index1 = indices[i * 3 + 1];
		UINT index2 = indices[i * 3 + 2];

		Vector3 v0 = vertices[index0].position;
		Vector3 v1 = vertices[index1].position;
		Vector3 v2 = vertices[index2].position;

		Vector3 A = v1 - v0;
		Vector3 B = v2 - v0;

		Vector3 normal = Vector3::Cross(A, B).Normal();

		vertices[index0].normal = normal + vertices[index0].normal;
		vertices[index1].normal = normal + vertices[index1].normal;
		vertices[index2].normal = normal + vertices[index2].normal;
	}

	for (VertexType& vertex : vertices)
		vertex.normal = Vector3(vertex.normal).Normal();
}

void TerrainEditor::CreateTangent()
{
	for (UINT i = 0; i < indices.size() / 3; i++)
	{
		UINT index0 = indices[i * 3 + 0];
		UINT index1 = indices[i * 3 + 1];
		UINT index2 = indices[i * 3 + 2];

		VertexType vertex0 = vertices[index0];
		VertexType vertex1 = vertices[index1];
		VertexType vertex2 = vertices[index2];

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

		vertices[index0].tangent = tangent + vertices[index0].tangent;
		vertices[index1].tangent = tangent + vertices[index1].tangent;
		vertices[index2].tangent = tangent + vertices[index2].tangent;
	}

	for (VertexType& vertex : vertices)
	{
		Vector3 t = vertex.tangent;
		Vector3 n = vertex.normal;

		Vector3 temp = (t - n * Vector3::Dot(n, t)).Normal();

		vertex.tangent = temp;
	}
}

void TerrainEditor::CreateCompute()
{
	computeShader = Shader::AddCS(L"ComputePicking");

	if (structuredBuffer != nullptr)
		delete structuredBuffer;

	structuredBuffer = new StructuredBuffer(input, sizeof(InputDesc), size,
		sizeof(OutputDesc), size);

	if(rayBuffer == nullptr)
		rayBuffer = new RayBuffer();

	if (output != nullptr)
		delete[] output;

	output = new OutputDesc[size];
}
