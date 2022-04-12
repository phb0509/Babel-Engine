#include "Framework.h"


TerrainEditor::TerrainEditor(UINT width, UINT height) :
	mTerrainWidth(width),
	mTerrainHeight(height),
	mbIsPainting(false),
	mAdjustValue(500),
	mHeightMap(nullptr),
	mInputFileName{},
	mComputePickingStructuredBuffer(nullptr),
	mOutput(nullptr),
	mRayBuffer(nullptr),
	mPaintValue(5),
	mCurrentMousePosition(0.0f, 0.0f, 0.0f),
	mLastPickingMousePosition(0.0f, 0.0f, 0.0f)
{
	igfd::ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", 0, ".");
	mProjectPath = igfd::ImGuiFileDialog::Instance()->GetCurrentPath(); // 프로젝트폴더까지의 전체경로. ex) DX113D_2004까지.
	igfd::ImGuiFileDialog::Instance()->CloseDialog("ChooseFileDlgKey");

	mMaterial = new Material(L"TerrainSplatting");
	mMaterial->SetDiffuseMap(L"Textures/White.png");
	mTerrainDiffuseMap = Texture::Add(L"Textures/White.png");
	mLayerNames = { "Layer1", "Layer2", "Layer3", "Layer4" };
	//mLayerNames = { "Brush" };

	for (int i = 0; i < mLayerNames.size(); i++) // Layers들 초기화들
	{
		mLayers.push_back(Texture::Add(L"Textures/defaultImageButton.png"));
	}

	// DepthShader Setting.
	mDepthMaterial = new Material(L"DepthShader");
	mDepthStencil = new DepthStencil(WIN_WIDTH, WIN_HEIGHT, true); // 깊이값
	mDepthRenderTarget = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM);
	mRenderTargets[0] = mDepthRenderTarget;

	//mRenderTargetSRVs[0] = mDepthRenderTarget->GetSRV();
	mRenderTargetSRVs[0] = mRenderTargets[0]->GetSRV(); // mDepthRenderTarget->GetSRV()
	mRenderTargetSRVs[1] = mDepthStencil->GetSRV();

	mTempTexture = Texture::AddUsingSRV(mRenderTargetSRVs[0]);

	createMesh(); // 적용할 HeightMap 기준으로 mTerrainWidth와 mTerrainHeight 결정.

	mbIsUVPicking = true;

	//createCompute();
	createPixelPickingCompute();
	
	mBrushBuffer = new BrushBuffer();

	mExtensionPreviewImages["png"] = Texture::Add(L"Textures/tempPreviewImage.jpg");
	mExtensionPreviewImages["jpg"] = Texture::Add(L"Textures/tempPreviewImage.jpg");
	mExtensionPreviewImages["dds"] = Texture::Add(L"Textures/DDS_PreviewImage.png");
	mExtensionPreviewImages["tga"] = Texture::Add(L"Textures/TGA_PreviewImage.png");
	mExtensionPreviewImages["default"] = Texture::Add(L"Textures/DefaultImage.png");

	boxLeft = 0;
	boxRight = 249999;
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

	delete mDepthStencil;
	delete mDepthRenderTarget;

	for (UIImage* texture : mRenderTextures)
		delete texture;
}

void TerrainEditor::Update()
{
	mCurrentMousePosition = MOUSEPOS;

	UpdateWorld();
	computePixelPicking(&mPickedPosition);
	mBrushBuffer->data.location = mPickedPosition;

	//computePicking(&mPickedPosition);

	if (KEY_PRESS(VK_LBUTTON) && !ImGui::GetIO().WantCaptureMouse)
	{
		if (checkMouseMove()) // 커서가 움직였다면
		{
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
		//mMesh->UpdateVertexUsingBox(mVertices.data(), mVertices.size());
		//mMesh->UpdateVertexUsingMap(mVertices.data(), mVertices.size());
	}

	if (KEY_DOWN('P'))
	{
		//std::reverse(mVertices.begin(), mVertices.end());
		mVertices;
		int a = 0;
	}
	
}

void TerrainEditor::PreRender()
{
	mMesh->IASet(); // 버텍스,인덱스버퍼,프리미티브토폴로지 Set.
	mWorldBuffer->SetVSBuffer(0);

	// 여기다 DepthShader 관련.
	RenderTarget::ClearAndSetWithDSV(mRenderTargets, 1, mDepthStencil); // 깊이값이 mDepthStencil에 저장..
	mDepthMaterial->Set(); 

	DEVICECONTEXT->DrawIndexed((UINT)mIndices.size(), 0, 0);
}

void TerrainEditor::Render()
{
	mMesh->IASet(); // 버텍스,인덱스버퍼,프리미티브토폴로지 Set.

	mWorldBuffer->SetVSBuffer(0);
	mBrushBuffer->SetPSBuffer(10);

	DEVICECONTEXT->PSSetShaderResources(11, 4, mLayerArray);
	//mLayers[0]->PSSet(11); // 텍스쳐 SRV로 PS에 바인딩.
	//mLayers[1]->PSSet(12);
	//mLayers[2]->PSSet(13);
	//mLayers[3]->PSSet(14);

	mMaterial->Set(); // 버퍼,srv,TerrainEditor 셰이더 Set.

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
	ImGui::RadioButton("Rect", &mBrushBuffer->data.type, 1);

	ImGui::RadioButton("Raise", &mbIsPainting, 0); ImGui::SameLine();
	ImGui::RadioButton("Brush", &mbIsPainting, 1);

	ImGui::SliderFloat("Range", &mBrushBuffer->data.range, 1, 50);
	ImGui::ColorEdit3("Color", (float*)&mBrushBuffer->data.color);

	ImGui::Spacing();

	ImGui::Separator();

	SpacingRepeatedly(3);

	showAssetsWindow(); // ImGuiFileDialog 이용해서 텍스쳐들 추가.
	showTerrainEditor(); // 추가한 텍스쳐들 렌더.

	SpacingRepeatedly(3);

	ImGui::Separator();

	UnIndentRepeatedly(20);

	mMouseScreenPosition = { MOUSEPOS.x / WIN_WIDTH, MOUSEPOS.y / WIN_HEIGHT ,0.0f };
	mMouseNDCPosition = { ((2 * MOUSEPOS.x) / WIN_WIDTH) - 1.0f,
		(((2 * MOUSEPOS.y) / WIN_HEIGHT) - 1.0f) * -1.0f,
		0.0f }; // 마우스위치값을 -1~1로 정규화. NDC좌표로 변환.

	ImGui::End();

	ImGui::Begin("TerrainInformation");

	SpacingRepeatedly(2);
	ImGui::InputFloat3("PickedPosition", (float*)&mPickedPosition);
	SpacingRepeatedly(2);

	ImGui::InputInt("Box.Left", &boxLeft);
	ImGui::InputInt("Box.Right", &boxRight);
	SpacingRepeatedly(2);

	ImGui::End();
}

bool TerrainEditor::computePicking(OUT Vector3* position) // 터레인의 피킹한곳의 월드포지션값 구해서 넘겨줌.
{
	Ray ray = mCamera->ScreenPointToRay(MOUSEPOS);

	mRayBuffer->data.position = ray.position; // 카메라 포지션(원점)
	mRayBuffer->data.direction = ray.direction; // 원점에서 마우스피킹한곳까지(near평면) 방향벡터.
	mRayBuffer->data.size = mPolygonCount; // 터레인 폴리곤 개수.
	mComputeShader->Set(); // 디바이스에 Set..

	mRayBuffer->SetCSBuffer(0);

	DEVICECONTEXT->CSSetShaderResources(0, 1, &mComputePickingStructuredBuffer->GetSRV());
	DEVICECONTEXT->CSSetUnorderedAccessViews(0, 1, &mComputePickingStructuredBuffer->GetUAV(), nullptr);

	UINT x = ceil((float)mPolygonCount / 1024.0f); // 폴리곤개수 / 1024.0f 반올림.

	DEVICECONTEXT->Dispatch(x, 1, 1);

	mComputePickingStructuredBuffer->Copy(mOutput, sizeof(OutputDesc) * mPolygonCount); // GPU에서 계산한거 받아옴. // 여기서 프레임 많이먹음.
																		  // 구조체, 받아와야할 전체크기 (구조체크기 * 폴리곤개수), 메모리사이즈만큼 복사해줌.
	float minDistance = FLT_MAX;
	int minIndex = -1;

	for (UINT i = 0; i < mPolygonCount; i++) // 직선위의 피킹된 메쉬들중, 가장 가까운것 선별.
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

void TerrainEditor::computePixelPicking(OUT Vector3* position)
{
	mMouseUVBuffer->data.mouseScreenPosition = { mMouseScreenPosition.x,mMouseScreenPosition.y }; // 마우스좌표 uv값
	mMouseUVBuffer->data.mouseNDCPosition = { mMouseNDCPosition.x,mMouseNDCPosition.y };
	mMouseUVBuffer->data.invViewMatrix = mCamera->GetViewBuffer()->GetInvViewMatrix();
	mMouseUVBuffer->data.invProjectionMatrix = mCamera->GetProjectionBufferInUse()->GetInvProjectionMatrix();

	mComputeShader->Set(); // 디바이스에 Set..
	mMouseUVBuffer->SetCSBuffer(0);

	DEVICECONTEXT->CSSetShaderResources(0, 1, &mDepthStencil->GetSRV());
	DEVICECONTEXT->CSSetUnorderedAccessViews(0, 1, &mPixelPickingStructuredBuffer->GetUAV(), nullptr);

	DEVICECONTEXT->Dispatch(1, 1, 1);

	ID3D11ShaderResourceView* pSRV = NULL;
	DEVICECONTEXT->CSSetShaderResources(0, 1, &pSRV); // CS 0번 레지스터에 NULL값 세팅. 안하면 경고 뜬다

	mPixelPickingStructuredBuffer->Copy(mOutputUVDesc, sizeof(OutputUVDesc)); // GPU에서 계산한거 받아옴.  왜 여기서 에러나지? VertexBuffer 건드렸는데
																			 
	mTestOutputDesc.worldPosition = mOutputUVDesc->worldPosition;

	*position = mOutputUVDesc->worldPosition;
}

void TerrainEditor::createCompute()
{
	mComputeShader = Shader::AddCS(L"ComputePicking");

	if (mComputePickingStructuredBuffer != nullptr)
		delete mComputePickingStructuredBuffer;

	mComputePickingStructuredBuffer = new ComputeStructuredBuffer(mInput, sizeof(InputDesc), mPolygonCount, //mInput에 터레인버텍스정보 들어있다.
		sizeof(OutputDesc), mPolygonCount);

	if (mRayBuffer == nullptr)
		mRayBuffer = new RayBuffer();

	if (mOutput != nullptr)
		delete[] mOutput;

	mOutput = new OutputDesc[mPolygonCount];
}

void TerrainEditor::createPixelPickingCompute()
{
	mComputeShader = Shader::AddCS(L"PixelPicking");
	mPixelPickingStructuredBuffer = new ComputeStructuredBuffer(sizeof(OutputUVDesc), 1);

	if (mMouseUVBuffer == nullptr)
		mMouseUVBuffer = new MouseUVBuffer();

	mOutputUVDesc = new OutputUVDesc[1];
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
	if (rect.top >= (LONG)mTerrainHeight) rect.top = (LONG)mTerrainHeight;
	if (rect.right >= (LONG)mTerrainWidth) rect.right = (LONG)mTerrainWidth;
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
				UINT index = mTerrainWidth * z + x;

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
			for (LONG x = rect.left; x < rect.right; x++)
			{
				UINT index = mTerrainWidth * z + x;

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

	//mMesh->UpdateVertexUsingMap(mVertices.data(), mVertices.size() * sizeof(VertexType)); // 버텍스 전체를 업데이트. 부분업데이트하게 바꿔야됨.
	//mMesh->UpdateVertex(mVertices.data(), mVertices.size()); // // 벡터주소,원소개수
	mMesh->UpdateVertexUsingBox(mVertices.data(), mVertices.size(), mPickedPosition, range, (UINT)boxLeft, (UINT)boxRight); // // 벡터주소,원소개수
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
	if (rect.top >= (LONG)mTerrainHeight) rect.top = (LONG)mTerrainHeight;
	if (rect.right >= (LONG)mTerrainWidth) rect.right = (LONG)mTerrainWidth;
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
				UINT index = mTerrainWidth * z + x;

				float dx = x - position.x;
				float dz = z - position.z;

				distance = sqrt(dx * dx + dz * dz); // 원의 중심과 버텍스 사이의 거리

				if (fabsf(distance) > range) continue;// 범위내의 버텍스가 아닐경우

				float paintValue = mPaintValue * max(0, cos(XM_PIDIV2 * distance / range));

				if (KEY_PRESS(VK_CONTROL))
				{
					mVertices[index].alpha[mPickedLayerIndex] -= paintValue * DELTA; // TerrainTexture로 섞기.
				}
				else
				{
					mVertices[index].alpha[mPickedLayerIndex] += paintValue * DELTA; // BrushTexture로 섞기.
				}

				mVertices[index].alpha[mPickedLayerIndex] = Saturate(mVertices[index].alpha[mPickedLayerIndex]);
			}
		}
	}
	break;

	default:
		break;
	}

	//mMesh->UpdateVertexUsingMap(mVertices.data(), mVertices.size() * sizeof(VertexType));
	mMesh->UpdateVertex(mVertices.data(), mVertices.size());
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
	UINT size = mTerrainWidth * mTerrainHeight * 4;
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
	image.width = mTerrainWidth;
	image.height = mTerrainHeight;
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

void TerrainEditor::updateVertexNormal() // 법선벡터구하는 함수.
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

void TerrainEditor::updateVertexTangent()
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

void TerrainEditor::createMesh()
{
	vector<Float4> pixels;

	if (mHeightMap != nullptr)
	{
		mTerrainWidth = mHeightMap->GetWidth();
		mTerrainHeight = mHeightMap->GetHeight();
		pixels = mHeightMap->ReadPixels();
	}

	mVertices.clear();
	mIndices.clear();

	//Vertices
	for (UINT z = 0; z < mTerrainHeight; z++)
	{
		for (UINT x = 0; x < mTerrainWidth; x++)
		{
			VertexType vertex;
			vertex.position = Float3((float)x, 0.0f, (float)z);
			vertex.uv = Float2(x / (float)mTerrainWidth, 1.0f - z / (float)mTerrainHeight);

			UINT index = mTerrainWidth * z + x;
			if (pixels.size() > index)
				vertex.position.y += pixels[index].x * MAX_HEIGHT;

			mVertices.emplace_back(vertex);
		}
	}

	//Indices
	for (UINT z = 0; z < mTerrainHeight - 1; z++)
	{
		for (UINT x = 0; x < mTerrainWidth - 1; x++)
		{
			mIndices.emplace_back(mTerrainWidth * z + x);//0
			mIndices.emplace_back(mTerrainWidth * (z + 1) + x);//1
			mIndices.emplace_back(mTerrainWidth * (z + 1) + x + 1);//2

			mIndices.emplace_back(mTerrainWidth * z + x);//0
			mIndices.emplace_back(mTerrainWidth * (z + 1) + x + 1);//2
			mIndices.emplace_back(mTerrainWidth * z + x + 1);//3
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

	updateVertexNormal();
	updateVertexTangent();

	//mMesh = new Mesh(mVertices.data(), sizeof(VertexType), (UINT)mVertices.size(),
	//	mIndices.data(), (UINT)mIndices.size(), true);

	mMesh = new Mesh(mVertices.data(), sizeof(VertexType), (UINT)mVertices.size(),
		mIndices.data(), (UINT)mIndices.size(), false);
}

void TerrainEditor::showAssetsWindow() 
{
	ImGui::Begin("TextureAssets");

	vector<string> fileList;
	loadFileList("\\Textures\\",fileList); // 확장자까지 포함한 파일명들 리턴.
	//mModelAssetsFileList = fileList;

	mStandardCursorPos = ImGui::GetCursorPos(); // 8, 50
	ImVec2 windowSize = ImGui::GetWindowSize();

	//ImageButton Setting

	int frame_padding = 0;
	ImVec2 imageButtonSize = ImVec2(64.0f, 64.0f); // 이미지버튼 크기설정.                     
	ImVec2 imageButtonUV0 = ImVec2(0.0f, 0.0f); // 출력할이미지 uv좌표설정.
	ImVec2 imageButtonUV1 = ImVec2(1.0f, 1.0f); // 전체다 출력할거니까 1.
	ImVec4 imageButtonBackGroundColor = ImVec4(0.06f, 0.06f, 0.06f, 0.94f); // ImGuiWindowBackGroundColor.
	ImVec4 imageButtonTintColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	float widthPadding = 26.0f;
	float heightPadding = 56.0f;
	float distanceHorizontalGap = widthPadding + imageButtonSize.x; // 렌더시작위치기준으로 각 이미지버튼사이의 가로거리.
	float distanceVerticalGap = heightPadding + imageButtonSize.y; // 렌더시작위치기준으로 각 이미지버튼사이의 세로거리. 
	float distanceTextToImage = 6.0f;
	int currentLineIndex = 0;
	int fileCountPerRow = windowSize.x / distanceHorizontalGap; // 행당 표시할 파일개수. 동적으로 변경할것예정.

	if (fileCountPerRow < 1)
	{
		fileCountPerRow = 1;
	}

	for (int i = 0; i < fileList.size(); i++)
	{
		if ((i % fileCountPerRow) == 0) // 나머지가 0 아니여야 실행.
		{
			if (i != 0)
			{
				mStandardCursorPos.y += distanceVerticalGap;
				currentLineIndex = 0;
			}
		}

		string fileExtension = GetExtension(fileList[i]);
		int checkIndex = -1;

		if (fileExtension == "png")
		{
			string temp = "Textures/" + fileList[i];
			mExtensionPreviewImages[fileExtension] = Texture::Add(ToWString(temp));
		}

		if (fileExtension == "jpg")
		{
			string temp = "Textures/" + fileList[i];
			mExtensionPreviewImages[fileExtension] = Texture::Add(ToWString(temp));
		}

		ImVec2 textPosition = { mStandardCursorPos.x + currentLineIndex * distanceHorizontalGap , mStandardCursorPos.y + (imageButtonSize.y + distanceTextToImage) };

		ImGui::SetCursorPosY(mStandardCursorPos.y);
		ImGui::SetCursorPosX(mStandardCursorPos.x + currentLineIndex * distanceHorizontalGap);

		ImGui::ImageButton(mExtensionPreviewImages[fileExtension]->GetSRV(), imageButtonSize, imageButtonUV0, imageButtonUV1, frame_padding, imageButtonBackGroundColor, imageButtonTintColor);

		if (KEY_UP(VK_LBUTTON))
		{
			mbIsDropped = true;
		}

		if (ImGui::IsItemHovered() && mbIsDropped)
		{
			checkIndex = i;
		}

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) // 원본 드래그 이벤트. 
		{
			if (i == checkIndex)
			{
				ImGui::SetDragDropPayload("TextureAssets", &i, sizeof(int));
				ImGui::ImageButton(mExtensionPreviewImages[fileExtension]->GetSRV(), imageButtonSize, imageButtonUV0, imageButtonUV1, frame_padding, imageButtonBackGroundColor, imageButtonTintColor); // 드래그 할 경우, 마우스커서위치에 렌더될 미리보기이미지.
				mDraggedFileName = fileList[i];
				mbIsDropped = false;
			}

			ImGui::EndDragDropSource();
		}

		// fileName TextRender.

		static float wrap_width = 64.0f; // 텍스트줄바꿈해줄 기준크기.

		ImGui::SetCursorPos(textPosition); // Set TextPosition.
		ImGui::PushTextWrapPos(textPosition.x + wrap_width);
		ImGui::Text(fileList[i].c_str(), wrap_width); // Text Render.
		ImGui::PopTextWrapPos();

		currentLineIndex++;
	}

	ImGui::End();
}

void TerrainEditor::showTerrainEditor() // 우측창..
{
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Separator();
	ImGui::Spacing();

	// ImageButton 설정값.
	int frame_padding = 2;
	ImVec2 bigSize = ImVec2(128.0f, 128.0f);
	ImVec2 size = ImVec2(64.0f, 64.0f); // 이미지버튼 크기설정.                     
	ImVec2 uv0 = ImVec2(0.0f, 0.0f); // 출력할이미지 uv좌표설정.
	ImVec2 uv1 = ImVec2(1.0f, 1.0f); // 전체다 출력할거니까 1.
	ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // 바탕색.(Background Color)        
	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);


	{ // DiffuseMapButton
		ImGui::Text("DiffuseMap");
		ImGui::ImageButton(mTerrainDiffuseMap->GetSRV(), size, uv0, uv1, frame_padding, bg_col, tint_col);
		
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TextureAssets"))
			{
				//IM_ASSERT(payload->DataSize == sizeof(int));
				//int payload_n = *(const int*)payload->Data; // source에서 드래그한 이미지의 index.
				string temp = "Textures/" + mDraggedFileName;
				mTerrainDiffuseMap = Texture::Add(ToWString(temp));
				mMaterial->SetDiffuseMap(mTerrainDiffuseMap);
			}
			ImGui::EndDragDropTarget();
		}
	}

	ImGui::Spacing();
	ImGui::Spacing();

	//mRenderTargetSRVs
	{
		{ // LayerButtons 
			for (int i = 0; i < mLayerNames.size(); i++)
			{
				//ImGui::Text(mLayerNames[i]);
				ImGui::RadioButton(mLayerNames[i], &mPickedLayerIndex, i);
				ImGui::SameLine();
				ImGui::Indent();
				ImGui::Indent();
				ImGui::Indent();
				ImGui::Indent();
				ImGui::Indent();
			}

			for (int i = 0; i < mLayerNames.size(); i++)
			{
				ImGui::Unindent();
				ImGui::Unindent();
				ImGui::Unindent();
				ImGui::Unindent();
				ImGui::Unindent();
			}
		}

		ImGui::NewLine();
		ImGui::Spacing();

		for (int i = 0; i < mLayers.size(); i++)
		{
			ImGui::ImageButton(mLayers[i]->GetSRV(), size, uv0, uv1, frame_padding, bg_col, tint_col);

			if (ImGui::BeginDragDropTarget()) // 타겟에 놓을때 이벤트인듯?
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TextureAssets"))
				{
					IM_ASSERT(payload->DataSize == sizeof(int));
					int payload_n = *(const int*)payload->Data;

					//mLayers[i] = mAddedTextures[payload_n].texture;
					//mLayerSRVs[i] = mLayers[i]->GetSRV();

					string temp = "Textures/" + mDraggedFileName;
					mLayers[i] = Texture::Add(ToWString(temp));
					mLayerSRVs[i] = mLayers[i]->GetSRV();
				}

				for (int i = 0; i < mLayers.size(); i++) // PS에 넘길 텍스쳐배열 업데이트.
				{
					mLayerArray[i] = mLayers[i]->GetSRV();
				}

				ImGui::EndDragDropTarget();
			}

			ImGui::SameLine();

			ImGui::Indent();
			ImGui::Indent();
			ImGui::Indent();
			ImGui::Indent();
			ImGui::Indent();
		}
	}
}

void TerrainEditor::getFileNames(string path)
{

	DIR* dirp = opendir(path.c_str());
	struct dirent* dp;

	while ((dp = readdir(dirp)) != NULL)
	{
		mTextureAssetFileNames.push_back(dp->d_name);
	}

	closedir(dirp);
}

void TerrainEditor::showTextureAsset()
{
	ImGui::Begin("Asset");
	getFileNames("Textures/LandScape");

	ImGui::End();
}

void TerrainEditor::loadFileList(string folderName, vector<string>& fileList)
{
	string path = mProjectPath;

	path += folderName + "\\";
	path += "*.*";

	struct _finddata_t fd;
	intptr_t handle;

	int i = 0;

	if ((handle = _findfirst(path.c_str(), &fd)) == -1L)
	{
		// 파일 없을때 발생시킬 이벤트.
	}
	do
	{
		if (i >= 2)
		{
			fd.name;
			string temp(fd.name);
			fileList.push_back(temp);
		}

		i++;
	} while (_findnext(handle, &fd) == 0);

	_findclose(handle);
}
