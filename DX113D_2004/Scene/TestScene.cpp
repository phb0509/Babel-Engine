#include "Framework.h"
#include "TestScene.h"

TestScene::TestScene()
{
	D3D11_TEXTURE2D_DESC TexDesc;
	TexDesc.Width = Width;
	TexDesc.Height = Height;
	TexDesc.MipLevels = 0;
	TexDesc.ArraySize = 1;
	TexDesc.Format = DXGI_FORMAT_R32_FLOAT;
	TexDesc.SampleDesc.Count = 1;
	TexDesc.SampleDesc.Quality = 0;
	TexDesc.BindFlags = 0;
	TexDesc.MiscFlags = 0;
	TexDesc.Usage = D3D11_USAGE_DEFAULT;
	TexDesc.CPUAccessFlags = 0;

	DEVICE->CreateTexture2D(&TexDesc, NULL, &Texture);

	TexDesc.Usage = D3D11_USAGE_STAGING;
	TexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	DEVICE->CreateTexture2D(&TexDesc, NULL, &Staging);


	/////////////////
	// Upload Data //
	/////////////////

	Data = new float[Width * Height]();

	for (int i = 0; i < Width; ++i)
	{
		for (int j = 0; j < Height; ++j)
		{
			int temp = i + j * Width;
			Data[temp] = (float)i + (float)j / 1000.f;
			//Data[i + j * Width] = (float)i + (float)j * Width;

		}
	}

	UINT const DataSize = sizeof(float);
	UINT const RowPitch = DataSize * Width;
	UINT const DepthPitch = DataSize * Width * Height;

	D3D11_BOX Box;
	Box.left = 0;
	Box.right = Width;
	Box.top = 0;
	Box.bottom = Height;
	Box.front = 0;
	Box.back = 1;

	if (Texture != nullptr)
	{
		DEVICECONTEXT->UpdateSubresource(Texture, 0, &Box, Data, RowPitch, DepthPitch);
	}


	Read = new float[Width * Height]();

	///////////////////
	// Download Data //
	///////////////////

	/*DEVICECONTEXT->CopyResource(Staging, Texture);

	Read = new float[Width * Height]();

	for (int i = 0; i < Width; ++i)
	{
		for (int j = 0; j < Height; ++j)
		{
			Read[i + j * Width] = -1.f;
		}
	}


	D3D11_MAPPED_SUBRESOURCE ResourceDesc = {};
	DEVICECONTEXT->Map(Staging, 0, D3D11_MAP_READ, 0, &ResourceDesc);

	if (ResourceDesc.pData)
	{
		int const BytesPerPixel = sizeof(FLOAT);
		for (int i = 0; i < Height; ++i)
		{
			std::memcpy(
				(::byte*)Read + Width * BytesPerPixel * i,
				(::byte*)ResourceDesc.pData + ResourceDesc.RowPitch * i,
				Width * BytesPerPixel);
		}
	}

	DEVICECONTEXT->Unmap(Staging, 0);*/

	//for (int i = 0; i < Width; ++i)
	//{
	//	for (int j = 0; j < Height; ++j)
	//	{
	//		char buff[100];
	//		sprintf_s(buff, "[%d, %d] = { %.6f }\n", i, j, Read[i + j * Width]);
	//		OutputDebugStringA(buff);

	//		//printf("[%d, %d] = { %.6f }\n", i, j, Read[i + j * Width]);
	//	}
	//}

	//delete[] Read;

	
	
}

TestScene::~TestScene()
{

}

void TestScene::Update()
{
	DEVICECONTEXT->CopyResource(Staging, Texture);

	D3D11_MAPPED_SUBRESOURCE ResourceDesc = {};
	DEVICECONTEXT->Map(Staging, 0, D3D11_MAP_READ, 0, &ResourceDesc);

	if (ResourceDesc.pData)
	{
		int const BytesPerPixel = sizeof(FLOAT);
		for (int i = 0; i < Height; ++i)
		{
			std::memcpy(
				(::byte*)Read + Width * BytesPerPixel * i,
				(::byte*)ResourceDesc.pData + ResourceDesc.RowPitch * i,
				Width * BytesPerPixel);
		}
	}

	DEVICECONTEXT->Unmap(Staging, 0);


	if (KEY_DOWN(VK_LBUTTON))
	{
		for (int i = 0; i < Width; ++i)
		{
			for (int j = 0; j < Height; ++j)
			{
				int temp = i + j * Width;
				Data[temp] = (float)i + (float)j / 1000.f + 1.0f;
			}
		}

		UINT const DataSize = sizeof(float);
		UINT const RowPitch = DataSize * Width;
		UINT const DepthPitch = DataSize * Width * Height;

		D3D11_BOX Box;
		Box.left = 0;
		Box.right = Width;
		Box.top = 0;
		Box.bottom = Height;
		Box.front = 0;
		Box.back = 1;

		if (Texture != nullptr)
		{
			DEVICECONTEXT->UpdateSubresource(Texture, 0, &Box, Data, RowPitch, DepthPitch);
		}
	}
}

void TestScene::PreRender()
{
	Environment::Get()->SetPerspectiveProjectionBuffer();
}

void TestScene::Render()
{
	Environment::Get()->SetPerspectiveProjectionBuffer();
}

void TestScene::PostRender()
{
	Environment::Get()->SetPerspectiveProjectionBuffer();

	Vector3 temp = { Read[0],Read[1],Read[2] };
	ImGui::InputFloat3("Read 0 1 2 Value", (float*)&temp, "%.3f");
}
