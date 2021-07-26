#include "Framework.h"


TerrainEditor::TerrainEditor(UINT width, UINT height) :
	mWidth(width),
	mHeight(height),
	mbIsPainting(false),
	mAdjustValue(50),
	mHeightMap(nullptr),
	mInputFileName{},
	mComputePickingStructuredBuffer(nullptr),
	mOutput(nullptr),
	mRayBuffer(nullptr),
	mPaintValue(5),
	mBrushMap(0),
	mCurrentMousePosition(0.0f, 0.0f, 0.0f),
	mLastPickingMousePosition(0.0f, 0.0f, 0.0f)

{
	mMaterial = new Material(L"TerrainSplatting");
	mMaterial->SetDiffuseMap(L"Textures/Landscape/White.png");

	mBrushMap = Texture::Add(L"Textures/Landscape/Floor.png");

	mTerrainTextureImageButton = Texture::Add(L"Textures/Landscape/defaultImageButton.png");
	mBrushTextureImageButton = Texture::Add(L"Textures/Landscape/defaultImageButton.png");

	createMesh();
	createCompute();

	mBrushBuffer = new BrushBuffer();
}

TerrainEditor::~TerrainEditor()
{
	delete mMaterial;
	delete mMesh;

	delete mRayBuffer;
	delete mComputePickingStructuredBuffer;

	delete[] mInput;
	delete[] mOutput;

	delete mBrushBuffer;
}

void TerrainEditor::Update()
{
	mCurrentMousePosition = MOUSEPOS;

	if (KEY_PRESS(VK_LBUTTON) && !ImGui::GetIO().WantCaptureMouse)
	{
		if (checkMouseMove()) // 커서가 움직였다면
		{
			computePicking(&mPickedPosition);
			mBrushBuffer->data.location = mPickedPosition;
			mLastPickingMousePosition = mCurrentMousePosition;
		}

		if (mbIsPainting)
		{
			paintBrush(mPickedPosition);
		}
		else
		{
			adjustY(mPickedPosition);
		}
	}

	if (KEY_UP(VK_LBUTTON))
	{
		//createNormal(); // 이거 두개가 프레임 은근 먹는다.
		//createTangent();
		//mMesh->UpdateVertex(mVertices.data(), mVertices.size());
		//mMesh->UpdateVertexUsingMap(mVertices.data(), mVertices.size());
	}

	UpdateWorld();
}

void TerrainEditor::Render()
{
	mMesh->IASet();

	mWorldBuffer->SetVSBuffer(0);
	mBrushBuffer->SetPSBuffer(10);

	mBrushMap->PSSet(11);

	mMaterial->Set(); // 버퍼,srv,셰이더 Set.

	DEVICECONTEXT->DrawIndexed((UINT)mIndices.size(), 0, 0);
}

void TerrainEditor::PostRender()
{
	igfd::ImGuiFileDialog::Instance()->SetExtentionInfos(".png", ImVec4(0, 1, 1, 0.9), "[PNG]");

	ImGui::Begin("TerrainEditor");

	ImGui::Text("BrushEditor");

	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::RadioButton("Circle", &mBrushBuffer->data.type, 0); ImGui::SameLine();
	ImGui::RadioButton("Recr", &mBrushBuffer->data.type, 1);

	ImGui::RadioButton("Raise", &mbIsPainting, 0); ImGui::SameLine();
	ImGui::RadioButton("Brush", &mbIsPainting, 1);


	ImGui::SliderFloat("Range", &mBrushBuffer->data.range, 1, 50);
	ImGui::ColorEdit3("Color", (float*)&mBrushBuffer->data.color);

	ImGui::Separator();

	//ImGui::Checkbox("Raise", &mbIsRaise);
	//ImGui::Checkbox("Painting", &mbIsPainting);

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	//ImGui::InputInt("SelectMap", &mSelectedMap);

	addTexture(); // ImGuiFileDialog 이용해서 텍스쳐들 추가.
	showAddedTextures(); // 추가한 텍스쳐들 렌더.

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Separator();

	ImGui::Unindent();
	ImGui::Unindent();
	ImGui::Unindent();
	ImGui::Unindent();
	ImGui::Unindent();
	ImGui::Unindent();
	ImGui::Unindent();
	ImGui::Unindent();
	ImGui::Unindent();



	ImGui::Separator();



	/*ImGui::InputText("FileName", mInputFileName, 100);
	wstring heightFile = L"Textures/HeightMaps/" + ToWString(mInputFileName) + L".png";

	if (ImGui::Button("HeightMapSave"))
	{
		saveHeightMap(heightFile);
	}*/



	//ImGui::ShowDemoWindow();
	ImGui::End();
}

bool TerrainEditor::computePicking(OUT Vector3* position)
{
	Ray ray = CAMERA->ScreenPointToRay(MOUSEPOS);

	mRayBuffer->data.position = ray.position;
	mRayBuffer->data.direction = ray.direction;
	mRayBuffer->data.size = mPolygonCount;
	mComputeShader->Set(); // 디바이스에 Set..

	mRayBuffer->SetCSBuffer(0);

	DEVICECONTEXT->CSSetShaderResources(0, 1, &mComputePickingStructuredBuffer->GetSRV());
	DEVICECONTEXT->CSSetUnorderedAccessViews(0, 1, &mComputePickingStructuredBuffer->GetUAV(), nullptr);

	UINT x = ceil((float)mPolygonCount / 1024.0f); // 폴리곤개수 / 1024.0f 반올림.

	DEVICECONTEXT->Dispatch(x, 1, 1);

	mComputePickingStructuredBuffer->Copy(mOutput, sizeof(OutputDesc) * mPolygonCount); // GPU에서 계산한거 받아옴. // 여기서 프레임 많이먹음.
																		  // 구조체, 받아와야할 전체크기 (구조체크기 * 폴리곤개수)
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

void TerrainEditor::adjustY(Vector3 position) // 피킹포지션..
{
	D3D11_RECT rect;
	float range = mBrushBuffer->data.range;
	rect.left = (LONG)position.x - range;
	rect.top = (LONG)position.z + range;
	rect.right = (LONG)position.x + range;
	rect.bottom = (LONG)position.z - range;


	if (rect.left < 0) rect.left = 0;
	if (rect.top >= (LONG)mHeight) rect.top = (LONG)mHeight;
	if (rect.right >= (LONG)mWidth) rect.right = (LONG)mWidth;
	if (rect.bottom < 0) rect.bottom = 0;

	switch (mBrushBuffer->data.type)
	{
	case 0: // 원.
	{
		float distance;
		for (LONG z = rect.bottom; z < rect.top; z++)
		{
			for (LONG x = rect.left; x < rect.right; x++)
			{
				UINT index = mWidth * z + x;

				float dx = x - position.x;
				float dz = z - position.z;

				distance = sqrt(dx * dx + dz * dz); // 원의 중심과 버텍스 사이의 거리

				if (fabsf(distance) > range) continue;// 범위내의 버텍스가 아닐경우

				float heightValue = mAdjustValue * max(0, cos(XM_PIDIV2 * distance / range));

				if (KEY_PRESS(VK_CONTROL)) // 높이값 -
				{
					mVertices[index].position.y -= heightValue * DELTA;

					if (mVertices[index].position.y <= 0.0f)
					{
						mVertices[index].position.y = 0.0f;
					}
				}
				else // 높이값 +
				{
					mVertices[index].position.y += heightValue * DELTA;

					if (mVertices[index].position.y >= MAX_HEIGHT)
					{
						mVertices[index].position.y = MAX_HEIGHT;
					}
				}
			}
		}
	}
	break;
	case 1: // 사각형.
	{
		for (LONG z = rect.bottom; z < rect.top; z++)
		{
			for (LONG x = rect.left; x <= rect.right; x++)
			{
				UINT index = mWidth * (UINT)z + (UINT)x;

				// 정점 높이 높이기
				if (KEY_PRESS(VK_CONTROL)) // 높이값 -
				{
					mVertices[index].position.y -= mAdjustValue * DELTA;

					if (mVertices[index].position.y <= 0.0f)
					{
						mVertices[index].position.y = 0.0f;
					}
				}
				else // 높이값 +
				{
					mVertices[index].position.y += mAdjustValue * DELTA;

					if (mVertices[index].position.y >= MAX_HEIGHT)
					{
						mVertices[index].position.y = MAX_HEIGHT;
					}
				}
			}
		}
	}
	break;
	default:
		break;
	}

	mMesh->UpdateVertexUsingMap(mVertices.data(), mVertices.size() * sizeof(VertexType));
}

void TerrainEditor::paintBrush(Vector3 position)
{
	D3D11_RECT rect;
	float range = mBrushBuffer->data.range;
	rect.left = (LONG)position.x - range;
	rect.top = (LONG)position.z + range;
	rect.right = (LONG)position.x + range;
	rect.bottom = (LONG)position.z - range;


	if (rect.left < 0) rect.left = 0;
	if (rect.top >= (LONG)mHeight) rect.top = (LONG)mHeight;
	if (rect.right >= (LONG)mWidth) rect.right = (LONG)mWidth;
	if (rect.bottom < 0) rect.bottom = 0;

	switch (mBrushBuffer->data.type)
	{
	case 0:
	{
		float distance;
		for (LONG z = rect.bottom; z < rect.top; z++)
		{
			for (LONG x = rect.left; x < rect.right; x++)
			{
				UINT index = mWidth * z + x;

				float dx = x - position.x;
				float dz = z - position.z;

				distance = sqrt(dx * dx + dz * dz); // 원의 중심과 버텍스 사이의 거리

				if (fabsf(distance) > range) continue;// 범위내의 버텍스가 아닐경우

				float paintValue = mPaintValue * max(0, cos(XM_PIDIV2 * distance / range));

				if (KEY_PRESS(VK_CONTROL))
				{
					mVertices[index].alpha[mBrushMapIndex] -= paintValue * DELTA; // TerrainTexture로 섞기.
				}
				else
				{
					mVertices[index].alpha[mBrushMapIndex] += paintValue * DELTA; // BrushTexture로 섞기.
				}

				mVertices[index].alpha[mBrushMapIndex] = Saturate(mVertices[index].alpha[mBrushMapIndex]);
			}
		}
	}
	break;
	default:
		break;
	}

	mMesh->UpdateVertexUsingMap(mVertices.data(), mVertices.size() * sizeof(VertexType));
}

bool TerrainEditor::checkMouseMove()
{
	int x = static_cast<int>(mCurrentMousePosition.x - mLastPickingMousePosition.x);
	int y = static_cast<int>(mCurrentMousePosition.y - mLastPickingMousePosition.y);

	if (x == 0 && // 마우스커서가 안움직였으면
		y == 0)
	{
		return false;
	}

	return true;
}

void TerrainEditor::saveHeightMap(wstring heightFile)
{
	UINT size = mWidth * mHeight * 4;
	uint8_t* pixels = new uint8_t[size];

	for (UINT i = 0; i < size / 4; i++)
	{
		float y = mVertices[i].position.y;

		uint8_t height = y * 255 / MAX_HEIGHT;

		pixels[i * 4 + 0] = height;
		pixels[i * 4 + 1] = height;
		pixels[i * 4 + 2] = height;
		pixels[i * 4 + 3] = 255;
	}

	Image image;
	image.width = mWidth;
	image.height = mHeight;
	image.pixels = pixels;
	image.format = DXGI_FORMAT_R8G8B8A8_UNORM;
	image.rowPitch = image.width * 4;

	image.slicePitch = image.width * image.height * 4;

	SaveToWICFile(image, WIC_FLAGS_FORCE_RGB, GetWICCodec(WIC_CODEC_PNG),
		heightFile.c_str());
}

void TerrainEditor::saveTextureMap(wstring textureFile)
{
}

void TerrainEditor::loadHeightMap(wstring heightFile)
{
	mHeightMap = Texture::Load(heightFile);

	delete mMesh;

	createMesh();
	createCompute();
}

void TerrainEditor::changeHeightMap(wstring heightFileName)
{
	//mMaterial->SetHeightMap(heightFileName);
}

void TerrainEditor::changeTextureMap(wstring textureFileName)
{
	mMaterial->SetDiffuseMap(textureFileName);
}

void TerrainEditor::addTexture()
{
	if (ImGui::Button("Add Texture"))
	{
		igfd::ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose TextureFile", ".png,.jpg,.dds", ".", 0);
	}

	if (igfd::ImGuiFileDialog::Instance()->FileDialog("ChooseFileDlgKey")) // OpenDialog 했으면..
	{
		if (igfd::ImGuiFileDialog::Instance()->IsOk == true)
		{
			//확인 누른 후 이벤트 처리.

			map<string, string> tMap = igfd::ImGuiFileDialog::Instance()->GetSelection();

			for (auto it = tMap.begin(); it != tMap.end(); it++)
			{
				wstring tName = L"Textures/LandScape/" + ToWString(it->first);
				mAddedTextures.push_back(AddedTextureInfo(Texture::Add(tName), tName));
			}
		}

		igfd::ImGuiFileDialog::Instance()->CloseDialog("ChooseFileDlgKey");
	}
}

void TerrainEditor::showAddedTextures()
{
	for (int i = 0; i < mAddedTextures.size(); i++)
	{
		if ((i % 5) != 0)
			ImGui::SameLine();

		int frame_padding = 2;
		ImVec2 size = ImVec2(64.0f, 64.0f); // 이미지버튼 크기설정.                     
		ImVec2 uv0 = ImVec2(0.0f, 0.0f); // 출력할이미지 uv좌표설정.
		ImVec2 uv1 = ImVec2(1.0f, 1.0f); // 전체다 출력할거니까 1.
		ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // 바탕색.(Background Color)        
		ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

		ImGui::ImageButton(mAddedTextures[i].texture->GetSRV(), size, uv0, uv1, frame_padding, bg_col, tint_col);

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) // 원본 드래그 이벤트.
		{
			ImGui::SetDragDropPayload("DND_DEMO_CELL", &i, sizeof(int)); // 드래그할 때 인덱스(int값) 정보 가지고있음.
			ImGui::EndDragDropSource();
		}
	}

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();

	// TerrainTextureImageButton
	{
		int frame_padding = 2;
		ImVec2 size = ImVec2(64.0f, 64.0f); // 이미지버튼 크기설정.                     
		ImVec2 uv0 = ImVec2(0.0f, 0.0f); // 출력할이미지 uv좌표설정.
		ImVec2 uv1 = ImVec2(1.0f, 1.0f); // 전체다 출력할거니까 1.
		ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // 바탕색.(Background Color)        
		ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

		ImGui::Indent();
		ImGui::Text("TerrainTexture");
		ImGui::SameLine();
		ImGui::Indent();
		ImGui::Indent();
		ImGui::Indent();
		ImGui::Indent();
		ImGui::Indent();
		ImGui::Indent();
		ImGui::Indent();
		ImGui::Text(" BrushTexture");
		ImGui::NewLine();

		ImGui::Spacing();

		//ImGui::Indent();
		ImGui::Unindent();
		ImGui::Unindent();
		ImGui::Unindent();
		ImGui::Unindent();
		ImGui::Unindent();
		ImGui::Unindent();
		ImGui::ImageButton(mTerrainTextureImageButton->GetSRV(), size, uv0, uv1, frame_padding, bg_col, tint_col);

		if (ImGui::BeginDragDropTarget()) // 타겟에 놓을때 이벤트인듯?
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL"))
			{
				IM_ASSERT(payload->DataSize == sizeof(int));
				int payload_n = *(const int*)payload->Data;
				mTerrainTextureImageButton = mAddedTextures[payload_n].texture;
				mMaterial->SetDiffuseMap(mAddedTextures[payload_n].textureName);
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::SameLine();
		ImGui::Indent();
		ImGui::Indent();
		ImGui::Indent();
		ImGui::Indent();
		ImGui::Indent();
		ImGui::Indent();
		ImGui::Indent();
		ImGui::ImageButton(mBrushTextureImageButton->GetSRV(), size, uv0, uv1, frame_padding, bg_col, tint_col);

		if (ImGui::BeginDragDropTarget()) // 타겟에 놓을때 이벤트
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL"))
			{
				IM_ASSERT(payload->DataSize == sizeof(int));
				int payload_n = *(const int*)payload->Data;
				mBrushTextureImageButton = mAddedTextures[payload_n].texture;
				mBrushMap = mAddedTextures[payload_n].texture;
			}
			ImGui::EndDragDropTarget();
		}
	}

}

void TerrainEditor::createMesh()
{
	vector<Float4> pixels;

	if (mHeightMap != nullptr)
	{
		mWidth = mHeightMap->Width();
		mHeight = mHeightMap->Height();
		pixels = mHeightMap->ReadPixels();
	}

	mVertices.clear();
	mIndices.clear();

	//Vertices
	for (UINT z = 0; z < mHeight; z++)
	{
		for (UINT x = 0; x < mWidth; x++)
		{
			VertexType vertex;
			vertex.position = Float3((float)x, 0.0f, (float)z);
			vertex.uv = Float2(x / (float)mWidth, 1.0f - z / (float)mHeight);

			UINT index = mWidth * z + x;
			if (pixels.size() > index)
				vertex.position.y += pixels[index].x * MAX_HEIGHT;

			mVertices.emplace_back(vertex);
		}
	}

	//Indices
	for (UINT z = 0; z < mHeight - 1; z++)
	{
		for (UINT x = 0; x < mWidth - 1; x++)
		{
			mIndices.emplace_back(mWidth * z + x);//0
			mIndices.emplace_back(mWidth * (z + 1) + x);//1
			mIndices.emplace_back(mWidth * (z + 1) + x + 1);//2

			mIndices.emplace_back(mWidth * z + x);//0
			mIndices.emplace_back(mWidth * (z + 1) + x + 1);//2
			mIndices.emplace_back(mWidth * z + x + 1);//3
		}
	}

	mPolygonCount = mIndices.size() / 3;

	mInput = new InputDesc[mPolygonCount];
	for (UINT i = 0; i < mPolygonCount; i++) // 폴리곤개수
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
		mIndices.data(), (UINT)mIndices.size(), true);
}

void TerrainEditor::createNormal() // 법선벡터구하는 함수.
{
	for (UINT i = 0; i < mIndices.size() / 3; i++) // 폴리곤갯수만큼 반복. (한폴리곤에 인덱스3개니까)
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
}

void TerrainEditor::createTangent()
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

void TerrainEditor::createCompute()
{
	mComputeShader = Shader::AddCS(L"ComputePicking");

	if (mComputePickingStructuredBuffer != nullptr)
		delete mComputePickingStructuredBuffer;

	mComputePickingStructuredBuffer = new ComputeStructuredBuffer(mInput, sizeof(InputDesc), mPolygonCount,
		sizeof(OutputDesc), mPolygonCount);

	if (mRayBuffer == nullptr)
		mRayBuffer = new RayBuffer();

	if (mOutput != nullptr)
		delete[] mOutput;

	mOutput = new OutputDesc[mPolygonCount];
}
