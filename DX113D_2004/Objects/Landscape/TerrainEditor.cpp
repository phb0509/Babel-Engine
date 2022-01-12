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
	mCurrentMousePosition(0.0f, 0.0f, 0.0f),
	mLastPickingMousePosition(0.0f, 0.0f, 0.0f)
{
	mMaterial = new Material(L"TerrainSplatting");

	mMaterial->SetDiffuseMap(L"Textures/Landscape/White.png");
	mTerrainDiffuseMap = Texture::Add(L"Textures/Landscape/White.png");
	mLayerNames = { "Layer1", "Layer2", "Layer3", "Layer4" };
	//mLayerNames = { "Brush" };

	for (int i = 0; i < mLayerNames.size(); i++) // Layers�� �ʱ�ȭ��
	{
		mLayers.push_back(Texture::Add(L"Textures/Landscape/defaultImageButton.png"));
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

	createMesh();

	mbIsUVPicking = true;

	//createCompute();
	createPixelPickingCompute();
	
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

	delete mDepthStencil;
	delete mDepthRenderTarget;

	for (UIImage* texture : mRenderTextures)
		delete texture;
}

void TerrainEditor::Update()
{
	mCurrentMousePosition = MOUSEPOS;
	computePixelPicking(&mPickedPosition);
	//computePicking(&mPickedPosition);
	mBrushBuffer->data.location = mPickedPosition;

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
		//mMesh->UpdateVertexUsingMap(mVertices.data(), mVertices.size());
	}

	UpdateWorld();
}

void TerrainEditor::PreRender()
{
	mMesh->IASet(); // ���ؽ�,�ε�������,������Ƽ���������� Set.
	mWorldBuffer->SetVSBuffer(0);

	// ����� DepthShader ����.
	RenderTarget::Sets(mRenderTargets, 1, mDepthStencil); // ���̰��� mDepthStencil�� ����..
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

	addTexture(); // ImGuiFileDialog �̿��ؼ� �ؽ��ĵ� �߰�.
	showAddedTextures(); // �߰��� �ؽ��ĵ� ����.

	SpacingRepeatedly(3);

	ImGui::Separator();

	UnIndentRepeatedly(20);

	mMouseScreenPosition = { MOUSEPOS.x / WIN_WIDTH, MOUSEPOS.y / WIN_HEIGHT ,0.0f };
	mMouseNDCPosition = { ((2 * MOUSEPOS.x) / WIN_WIDTH) - 1.0f,
		(((2 * MOUSEPOS.y) / WIN_HEIGHT) - 1.0f) * -1.0f,
		0.0f }; // ���콺��ġ���� -1~1�� ����ȭ. NDC��ǥ�� ��ȯ.

	ImGui::End();
}

bool TerrainEditor::computePicking(OUT Vector3* position) // �ͷ����� ��ŷ�Ѱ��� ���������ǰ� ���ؼ� �Ѱ���.
{
	Ray ray = WORLDCAMERA->ScreenPointToRay(MOUSEPOS);

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
	mMouseUVBuffer->data.invViewMatrix = WORLDCAMERA->GetViewBuffer()->GetInvView();
	mMouseUVBuffer->data.invProjectionMatrix = Environment::Get()->GetProjectionBuffer()->GetInvProjectionMatrix();

	mComputeShader->Set(); // ����̽��� Set..
	mMouseUVBuffer->SetCSBuffer(0);

	DEVICECONTEXT->CSSetShaderResources(0, 1, &mDepthStencil->GetSRV());
	DEVICECONTEXT->CSSetUnorderedAccessViews(0, 1, &mPixelPickingStructuredBuffer->GetUAV(), nullptr);

	DEVICECONTEXT->Dispatch(1, 1, 1);

	mPixelPickingStructuredBuffer->Copy(mOutputUVDesc, sizeof(OutputUVDesc)); // GPU���� ����Ѱ� �޾ƿ�. 
																			 
	mTestOutputDesc.worldPosition = mOutputUVDesc->worldPosition;

	*position = mOutputUVDesc->worldPosition;
}

void TerrainEditor::computeTestPicking()
{
	mComputeShader->Set();
	DEVICECONTEXT->CSSetShaderResources(0, 1, &mTexture2DBuffer->GetSRV());
	DEVICECONTEXT->CSSetUnorderedAccessViews(0, 1, &mTexture2DBuffer->GetUAV(), nullptr);

	UINT width = mTexture2DBuffer->GetWidth();
	UINT height = mTexture2DBuffer->GetHeight();
	UINT page = mTexture2DBuffer->GetPage();

	UINT x = ((float)width / 32.0f) < 1.0f ? 1.0f : ((float)width / 32.0f);
	UINT y = ((float)height / 32.0f) < 1.0f ? 1.0f : ((float)height / 32.0f);

	DEVICECONTEXT->Dispatch((UINT)ceilf(x), (UINT)ceilf(y), page);
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

void TerrainEditor::createTestCompute()
{
	Texture* t = Texture::Add(L"Textures/Landscape/TestBlueImage.png");

	mComputeShader = Shader::AddCS(L"TestCS");
	mTexture2DBuffer = new Texture2DBuffer(t->GetTexture());
	mComputeShader->Set();

	/*DEVICECONTEXT->CSSetShaderResources(0,1,&mTexture2DBuffer->GetSRV());
	DEVICECONTEXT->CSSetUnorderedAccessViews(0, 1, &mTexture2DBuffer->GetUAV(), nullptr);

	UINT width = mTexture2DBuffer->GetWidth();
	UINT height = mTexture2DBuffer->GetHeight();
	UINT page = mTexture2DBuffer->GetPage();

	float x = ((float)width / 32.0f) < 1.0f ? 1.0f : ((float)width / 32.0f);
	float y = ((float)height / 32.0f) < 1.0f ? 1.0f : ((float)height / 32.0f);


	DEVICECONTEXT->Dispatch((UINT)ceilf(x), (UINT)ceilf(y), page);*/

	//Texture* tTexture = Texture::AddUsingSRV(mTexture2DBuffer->OutputSRV());

	//mTempTexture = tTexture;

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
	if (rect.top >= (LONG)mHeight) rect.top = (LONG)mHeight;
	if (rect.right >= (LONG)mWidth) rect.right = (LONG)mWidth;
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
				UINT index = mWidth * z + x;

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
			for (LONG x = rect.left; x <= rect.right; x++)
			{
				UINT index = mWidth * (UINT)z + (UINT)x;

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

	mMesh->UpdateVertexUsingMap(mVertices.data(), mVertices.size() * sizeof(VertexType));
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

void TerrainEditor::createNormal() // �������ͱ��ϴ� �Լ�.
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

void TerrainEditor::createMesh()
{
	vector<Float4> pixels;

	if (mHeightMap != nullptr)
	{
		mWidth = mHeightMap->GetWidth();
		mHeight = mHeightMap->GetHeight();
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

	createNormal();
	createTangent();

	mMesh = new Mesh(mVertices.data(), sizeof(VertexType), (UINT)mVertices.size(),
		mIndices.data(), (UINT)mIndices.size(), true);
}

void TerrainEditor::addTexture() // �����ؽ��Ŀ���â.
{
	ImGui::Begin("TerrainTextures");

	if (ImGui::Button("Add Texture"))
	{
		igfd::ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose TextureFile", ".png,.jpg,.dds", ".", 0);
	}

	if (igfd::ImGuiFileDialog::Instance()->FileDialog("ChooseFileDlgKey")) // OpenDialog ������..
	{
		if (igfd::ImGuiFileDialog::Instance()->IsOk == true)
		{
			//Ȯ�� ���� �� �̺�Ʈ ó��.

			map<string, string> tMap = igfd::ImGuiFileDialog::Instance()->GetSelection();

			for (auto it = tMap.begin(); it != tMap.end(); it++) // ImGuiFileDialog���� ������ �����̸��� Map.
			{
				wstring tName = L"Textures/LandScape/" + ToWString(it->first); 
				mAddedTextures.push_back(AddedTextureInfo(Texture::Add(tName), tName));
			}
		}

		igfd::ImGuiFileDialog::Instance()->CloseDialog("ChooseFileDlgKey");
	}

	for (int i = 0; i < mAddedTextures.size(); i++)
	{
		if ((i % 3) != 0)
			ImGui::SameLine();

		int frame_padding = 2;
		ImVec2 size = ImVec2(64.0f, 64.0f); // �̹�����ư ũ�⼳��.                     
		ImVec2 uv0 = ImVec2(0.0f, 0.0f); // ������̹��� uv��ǥ����.
		ImVec2 uv1 = ImVec2(1.0f, 1.0f); // ��ü�� ����ҰŴϱ� 1.
		ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // ������.(Background Color)        
		ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

		ImGui::ImageButton(mAddedTextures[i].texture->GetSRV(), size, uv0, uv1, frame_padding, bg_col, tint_col);

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) // ���� �巡�� �̺�Ʈ.
		{
			ImGui::SetDragDropPayload("DND_DEMO_CELL", &i, sizeof(int)); // �巡���� �� �ε���(int��) ���� ����������.
			ImGui::EndDragDropSource();
		}
	}

	ImGui::End();
}

void TerrainEditor::showAddedTextures() // ����â..
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
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL"))
			{
				IM_ASSERT(payload->DataSize == sizeof(int));
				int payload_n = *(const int*)payload->Data; // source���� �巡���� �̹����� index.
				mTerrainDiffuseMap = mAddedTextures[payload_n].texture;
				mMaterial->SetDiffuseMap(mTerrainDiffuseMap);
			}
			ImGui::EndDragDropTarget();
		}
	}

	ImGui::Spacing();
	ImGui::Spacing();
	//ImGui::ImageButton(mTempTexture->GetSRV(), bigSize, uv0, uv1, frame_padding, bg_col, tint_col);
	//ImGui::ImageButton(mRenderTargetSRVs[0], bigSize, uv0, uv1, frame_padding, bg_col, tint_col); // �������� ��Ӱ�... 
	//ImGui::ImageButton(mDepthStencil->GetSRV(), bigSize, uv0, uv1, frame_padding, bg_col, tint_col);
	//ImGui::ImageButton(mTexture2DBuffer->OutputSRV(), bigSize, uv0, uv1, frame_padding, bg_col, tint_col);
	//ImGui::ImageButton(mTempTexture->GetSRV(), bigSize, uv0, uv1, frame_padding, bg_col, tint_col);


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
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL"))
				{
					IM_ASSERT(payload->DataSize == sizeof(int));
					int payload_n = *(const int*)payload->Data;
					mLayers[i] = mAddedTextures[payload_n].texture;
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
