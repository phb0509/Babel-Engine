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
	mProjectPath = igfd::ImGuiFileDialog::Instance()->GetCurrentPath(); // ������Ʈ���������� ��ü���. ex) DX113D_2004����.
	igfd::ImGuiFileDialog::Instance()->CloseDialog("ChooseFileDlgKey");

	mMaterial = new Material(L"TerrainSplatting");
	mMaterial->SetDiffuseMap(L"Textures/White.png");
	mTerrainDiffuseMap = Texture::Add(L"Textures/White.png");
	mLayerNames = { "Layer1", "Layer2", "Layer3", "Layer4" };
	//mLayerNames = { "Brush" };

	for (int i = 0; i < mLayerNames.size(); i++) // Layers�� �ʱ�ȭ��
	{
		mLayers.push_back(Texture::Add(L"Textures/defaultImageButton.png"));
	}

	// DepthShader Setting.
	mDepthMaterial = new Material(L"DepthShader");
	mDepthStencil = new DepthStencil(WIN_WIDTH, WIN_HEIGHT, true); // ���̰�
	mDepthRenderTarget = new RenderTarget(WIN_WIDTH, WIN_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM);
	mRenderTargets[0] = mDepthRenderTarget;

	//mRenderTargetSRVs[0] = mDepthRenderTarget->GetSRV();
	mRenderTargetSRVs[0] = mRenderTargets[0]->GetSRV(); // mDepthRenderTarget->GetSRV()
	mRenderTargetSRVs[1] = mDepthStencil->GetSRV();

	mTempTexture = Texture::AddUsingSRV(mRenderTargetSRVs[0]);

	createMesh(); // ������ HeightMap �������� mTerrainWidth�� mTerrainHeight ����.

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
		if (checkMouseMove()) // Ŀ���� �������ٸ�
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
		//createNormal(); // �̰� �ΰ��� ������ ���� �Դ´�.
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
	mMesh->IASet(); // ���ؽ�,�ε�������,������Ƽ���������� Set.
	mWorldBuffer->SetVSBuffer(0);

	// ����� DepthShader ����.
	RenderTarget::ClearAndSetWithDSV(mRenderTargets, 1, mDepthStencil); // ���̰��� mDepthStencil�� ����..
	mDepthMaterial->Set(); 

	DEVICECONTEXT->DrawIndexed((UINT)mIndices.size(), 0, 0);
}

void TerrainEditor::Render()
{
	mMesh->IASet(); // ���ؽ�,�ε�������,������Ƽ���������� Set.

	mWorldBuffer->SetVSBuffer(0);
	mBrushBuffer->SetPSBuffer(10);

	DEVICECONTEXT->PSSetShaderResources(11, 4, mLayerArray);
	//mLayers[0]->PSSet(11); // �ؽ��� SRV�� PS�� ���ε�.
	//mLayers[1]->PSSet(12);
	//mLayers[2]->PSSet(13);
	//mLayers[3]->PSSet(14);

	mMaterial->Set(); // ����,srv,TerrainEditor ���̴� Set.

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

	showAssetsWindow(); // ImGuiFileDialog �̿��ؼ� �ؽ��ĵ� �߰�.
	showTerrainEditor(); // �߰��� �ؽ��ĵ� ����.

	SpacingRepeatedly(3);

	ImGui::Separator();

	UnIndentRepeatedly(20);

	mMouseScreenPosition = { MOUSEPOS.x / WIN_WIDTH, MOUSEPOS.y / WIN_HEIGHT ,0.0f };
	mMouseNDCPosition = { ((2 * MOUSEPOS.x) / WIN_WIDTH) - 1.0f,
		(((2 * MOUSEPOS.y) / WIN_HEIGHT) - 1.0f) * -1.0f,
		0.0f }; // ���콺��ġ���� -1~1�� ����ȭ. NDC��ǥ�� ��ȯ.

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

bool TerrainEditor::computePicking(OUT Vector3* position) // �ͷ����� ��ŷ�Ѱ��� ���������ǰ� ���ؼ� �Ѱ���.
{
	Ray ray = mCamera->ScreenPointToRay(MOUSEPOS);

	mRayBuffer->data.position = ray.position; // ī�޶� ������(����)
	mRayBuffer->data.direction = ray.direction; // �������� ���콺��ŷ�Ѱ�����(near���) ���⺤��.
	mRayBuffer->data.size = mPolygonCount; // �ͷ��� ������ ����.
	mComputeShader->Set(); // ����̽��� Set..

	mRayBuffer->SetCSBuffer(0);

	DEVICECONTEXT->CSSetShaderResources(0, 1, &mComputePickingStructuredBuffer->GetSRV());
	DEVICECONTEXT->CSSetUnorderedAccessViews(0, 1, &mComputePickingStructuredBuffer->GetUAV(), nullptr);

	UINT x = ceil((float)mPolygonCount / 1024.0f); // �����ﰳ�� / 1024.0f �ݿø�.

	DEVICECONTEXT->Dispatch(x, 1, 1);

	mComputePickingStructuredBuffer->Copy(mOutput, sizeof(OutputDesc) * mPolygonCount); // GPU���� ����Ѱ� �޾ƿ�. // ���⼭ ������ ���̸���.
																		  // ����ü, �޾ƿ;��� ��üũ�� (����üũ�� * �����ﰳ��), �޸𸮻����ŭ ��������.
	float minDistance = FLT_MAX;
	int minIndex = -1;

	for (UINT i = 0; i < mPolygonCount; i++) // �������� ��ŷ�� �޽�����, ���� ������ ����.
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
	mMouseUVBuffer->data.mouseScreenPosition = { mMouseScreenPosition.x,mMouseScreenPosition.y }; // ���콺��ǥ uv��
	mMouseUVBuffer->data.mouseNDCPosition = { mMouseNDCPosition.x,mMouseNDCPosition.y };
	mMouseUVBuffer->data.invViewMatrix = mCamera->GetViewBuffer()->GetInvViewMatrix();
	mMouseUVBuffer->data.invProjectionMatrix = mCamera->GetProjectionBufferInUse()->GetInvProjectionMatrix();

	mComputeShader->Set(); // ����̽��� Set..
	mMouseUVBuffer->SetCSBuffer(0);

	DEVICECONTEXT->CSSetShaderResources(0, 1, &mDepthStencil->GetSRV());
	DEVICECONTEXT->CSSetUnorderedAccessViews(0, 1, &mPixelPickingStructuredBuffer->GetUAV(), nullptr);

	DEVICECONTEXT->Dispatch(1, 1, 1);

	ID3D11ShaderResourceView* pSRV = NULL;
	DEVICECONTEXT->CSSetShaderResources(0, 1, &pSRV); // CS 0�� �������Ϳ� NULL�� ����. ���ϸ� ��� ���

	mPixelPickingStructuredBuffer->Copy(mOutputUVDesc, sizeof(OutputUVDesc)); // GPU���� ����Ѱ� �޾ƿ�.  �� ���⼭ ��������? VertexBuffer �ǵ�ȴµ�
																			 
	mTestOutputDesc.worldPosition = mOutputUVDesc->worldPosition;

	*position = mOutputUVDesc->worldPosition;
}

void TerrainEditor::createCompute()
{
	mComputeShader = Shader::AddCS(L"ComputePicking");

	if (mComputePickingStructuredBuffer != nullptr)
		delete mComputePickingStructuredBuffer;

	mComputePickingStructuredBuffer = new ComputeStructuredBuffer(mInput, sizeof(InputDesc), mPolygonCount, //mInput�� �ͷ��ι��ؽ����� ����ִ�.
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

void TerrainEditor::adjustY(Vector3 position) // ��ŷ������..
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
	case 0: // ��.
	{
		float distance;
		for (LONG z = rect.bottom; z < rect.top; z++)
		{
			for (LONG x = rect.left; x < rect.right; x++)
			{
				UINT index = mTerrainWidth * z + x;

				float dx = x - position.x;
				float dz = z - position.z;

				distance = sqrt(dx * dx + dz * dz); // ���� �߽ɰ� ���ؽ� ������ �Ÿ�

				if (fabsf(distance) > range) continue;// �������� ���ؽ��� �ƴҰ��

				float heightValue = mAdjustValue * max(0, cos(XM_PIDIV2 * distance / range));

				if (KEY_PRESS(VK_CONTROL)) // ���̰� -
				{
					mVertices[index].position.y -= heightValue * DELTA;

					if (mVertices[index].position.y <= 0.0f)
					{
						mVertices[index].position.y = 0.0f;
					}
				}
				else // ���̰� +
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

	case 1: // �簢��.
	{
		for (LONG z = rect.bottom; z < rect.top; z++)
		{
			for (LONG x = rect.left; x < rect.right; x++)
			{
				UINT index = mTerrainWidth * z + x;

				// ���� ���� ���̱�
				if (KEY_PRESS(VK_CONTROL)) // ���̰� -
				{
					mVertices[index].position.y -= mAdjustValue * DELTA;

					if (mVertices[index].position.y <= 0.0f)
					{
						mVertices[index].position.y = 0.0f;
					}
				}
				else // ���̰� +
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

	//mMesh->UpdateVertexUsingMap(mVertices.data(), mVertices.size() * sizeof(VertexType)); // ���ؽ� ��ü�� ������Ʈ. �κо�����Ʈ�ϰ� �ٲ�ߵ�.
	//mMesh->UpdateVertex(mVertices.data(), mVertices.size()); // // �����ּ�,���Ұ���
	mMesh->UpdateVertexUsingBox(mVertices.data(), mVertices.size(), mPickedPosition, range, (UINT)boxLeft, (UINT)boxRight); // // �����ּ�,���Ұ���
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

				distance = sqrt(dx * dx + dz * dz); // ���� �߽ɰ� ���ؽ� ������ �Ÿ�

				if (fabsf(distance) > range) continue;// �������� ���ؽ��� �ƴҰ��

				float paintValue = mPaintValue * max(0, cos(XM_PIDIV2 * distance / range));

				if (KEY_PRESS(VK_CONTROL))
				{
					mVertices[index].alpha[mPickedLayerIndex] -= paintValue * DELTA; // TerrainTexture�� ����.
				}
				else
				{
					mVertices[index].alpha[mPickedLayerIndex] += paintValue * DELTA; // BrushTexture�� ����.
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

	if (x == 0 && // ���콺Ŀ���� �ȿ���������
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

void TerrainEditor::updateVertexNormal() // �������ͱ��ϴ� �Լ�.
{
	for (UINT i = 0; i < mIndices.size() / 3; i++) // �����ﰹ����ŭ �ݺ�. (�������￡ �ε���3���ϱ�)
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

	for (UINT i = 0; i < mPolygonCount; i++) // �����ﰳ��
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
	loadFileList("\\Textures\\",fileList); // Ȯ���ڱ��� ������ ���ϸ�� ����.
	//mModelAssetsFileList = fileList;

	mStandardCursorPos = ImGui::GetCursorPos(); // 8, 50
	ImVec2 windowSize = ImGui::GetWindowSize();

	//ImageButton Setting

	int frame_padding = 0;
	ImVec2 imageButtonSize = ImVec2(64.0f, 64.0f); // �̹�����ư ũ�⼳��.                     
	ImVec2 imageButtonUV0 = ImVec2(0.0f, 0.0f); // ������̹��� uv��ǥ����.
	ImVec2 imageButtonUV1 = ImVec2(1.0f, 1.0f); // ��ü�� ����ҰŴϱ� 1.
	ImVec4 imageButtonBackGroundColor = ImVec4(0.06f, 0.06f, 0.06f, 0.94f); // ImGuiWindowBackGroundColor.
	ImVec4 imageButtonTintColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	float widthPadding = 26.0f;
	float heightPadding = 56.0f;
	float distanceHorizontalGap = widthPadding + imageButtonSize.x; // ����������ġ�������� �� �̹�����ư������ ���ΰŸ�.
	float distanceVerticalGap = heightPadding + imageButtonSize.y; // ����������ġ�������� �� �̹�����ư������ ���ΰŸ�. 
	float distanceTextToImage = 6.0f;
	int currentLineIndex = 0;
	int fileCountPerRow = windowSize.x / distanceHorizontalGap; // ��� ǥ���� ���ϰ���. �������� �����ҰͿ���.

	if (fileCountPerRow < 1)
	{
		fileCountPerRow = 1;
	}

	for (int i = 0; i < fileList.size(); i++)
	{
		if ((i % fileCountPerRow) == 0) // �������� 0 �ƴϿ��� ����.
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

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) // ���� �巡�� �̺�Ʈ. 
		{
			if (i == checkIndex)
			{
				ImGui::SetDragDropPayload("TextureAssets", &i, sizeof(int));
				ImGui::ImageButton(mExtensionPreviewImages[fileExtension]->GetSRV(), imageButtonSize, imageButtonUV0, imageButtonUV1, frame_padding, imageButtonBackGroundColor, imageButtonTintColor); // �巡�� �� ���, ���콺Ŀ����ġ�� ������ �̸������̹���.
				mDraggedFileName = fileList[i];
				mbIsDropped = false;
			}

			ImGui::EndDragDropSource();
		}

		// fileName TextRender.

		static float wrap_width = 64.0f; // �ؽ�Ʈ�ٹٲ����� ����ũ��.

		ImGui::SetCursorPos(textPosition); // Set TextPosition.
		ImGui::PushTextWrapPos(textPosition.x + wrap_width);
		ImGui::Text(fileList[i].c_str(), wrap_width); // Text Render.
		ImGui::PopTextWrapPos();

		currentLineIndex++;
	}

	ImGui::End();
}

void TerrainEditor::showTerrainEditor() // ����â..
{
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Separator();
	ImGui::Spacing();

	// ImageButton ������.
	int frame_padding = 2;
	ImVec2 bigSize = ImVec2(128.0f, 128.0f);
	ImVec2 size = ImVec2(64.0f, 64.0f); // �̹�����ư ũ�⼳��.                     
	ImVec2 uv0 = ImVec2(0.0f, 0.0f); // ������̹��� uv��ǥ����.
	ImVec2 uv1 = ImVec2(1.0f, 1.0f); // ��ü�� ����ҰŴϱ� 1.
	ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // ������.(Background Color)        
	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);


	{ // DiffuseMapButton
		ImGui::Text("DiffuseMap");
		ImGui::ImageButton(mTerrainDiffuseMap->GetSRV(), size, uv0, uv1, frame_padding, bg_col, tint_col);
		
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TextureAssets"))
			{
				//IM_ASSERT(payload->DataSize == sizeof(int));
				//int payload_n = *(const int*)payload->Data; // source���� �巡���� �̹����� index.
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

			if (ImGui::BeginDragDropTarget()) // Ÿ�ٿ� ������ �̺�Ʈ�ε�?
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

				for (int i = 0; i < mLayers.size(); i++) // PS�� �ѱ� �ؽ��Ĺ迭 ������Ʈ.
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
		// ���� ������ �߻���ų �̺�Ʈ.
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
