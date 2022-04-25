#include "Framework.h"

map<wstring, Texture*> Texture::totalTexture;
map<ID3D11ShaderResourceView*, Texture*> Texture::totalSRVTexture;

Texture::Texture(ID3D11ShaderResourceView* srv, ScratchImage& image) : 
    srv(srv), 
    image(move(image))
{
    mWidth = image.GetMetadata().width;
    mHeight = image.GetMetadata().height;

    //D3D11_TEXTURE2D_DESC textureDesc;
    //textureDesc.Height = mHeight;
    //textureDesc.Width = mWidth;
    //textureDesc.MipLevels = 0;
    //textureDesc.ArraySize = 1;
    //textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    //textureDesc.SampleDesc.Count = 1;
    //textureDesc.SampleDesc.Quality = 0;
    //textureDesc.Usage = D3D11_USAGE_DEFAULT;
    //textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    //textureDesc.CPUAccessFlags = 0;
    //textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
    //
    //V(DEVICE->CreateTexture2D(&textureDesc, nullptr, &mTexture));

    ////mPixels = ReadPixels();
    //getImages();

    //UINT rowPitch = (mWidth * 4) * sizeof(unsigned char);
    //UINT depthPitch = mWidth * mHeight * 4 * sizeof(unsigned char);

    //DEVICECONTEXT->UpdateSubresource(mTexture, 0, nullptr, mCharArray, rowPitch, 0);  // 벡터주소,원소크기,원소개수 sizeof(mPixels[0])

    // mTexture는 
    // mCharArray의 크기는 텍스쳐의 픽셀 수 (width*height) * 4의 크기다. 한 픽셀당 rgba값을 갖고있기 때문.
    // 1차원배열이니 결국 rgbargbargbargba.......형태로 저장되어있을것.
    // rowPitch는 딱 width만큼의 크기.
}

Texture::Texture(ID3D11ShaderResourceView* srv)
    : srv(srv)
{
   
}

Texture::~Texture()
{
    srv->Release();
}

Texture* Texture::Add(wstring file)
{
    if (totalTexture.count(file) > 0) // 이미있으면
        return totalTexture[file]; // 있는거리턴.

    ScratchImage image;

    wstring extension = GetExtension(file);

    if (extension == L"tga")
    {
        LoadFromTGAFile(file.c_str(), nullptr, image);
    }
    else if (extension == L"dds")
    {
        LoadFromDDSFile(file.c_str(), DDS_FLAGS_NONE, nullptr, image);
    }
    else
    {
        LoadFromWICFile(file.c_str(), WIC_FLAGS_FORCE_RGB, nullptr, image);
    }
       
    //int a = image.GetImageCount(); // 1나옴.
    
    ID3D11ShaderResourceView* srv;

    V(CreateShaderResourceView(DEVICE, image.GetImages(), image.GetImageCount(),
        image.GetMetadata(), &srv));
    
    totalTexture[file] = new Texture(srv, image);
    
    return totalTexture[file];
}

Texture* Texture::AddUsingSRV(ID3D11ShaderResourceView* _srv)
{
    if (totalSRVTexture.count(_srv) > 0)
        return totalSRVTexture[_srv];

    totalSRVTexture[_srv] = new Texture(_srv);

    return totalSRVTexture[_srv];
}

Texture* Texture::Load(wstring file)
{
    ScratchImage image;

    wstring extension = GetExtension(file);

    if (extension == L"tga")
        LoadFromTGAFile(file.c_str(), nullptr, image);
    else if (extension == L"dds")
        LoadFromDDSFile(file.c_str(), DDS_FLAGS_NONE, nullptr, image);
    else
        LoadFromWICFile(file.c_str(), WIC_FLAGS_FORCE_RGB, nullptr, image);

    ID3D11ShaderResourceView* srv;

    V(CreateShaderResourceView(DEVICE, image.GetImages(), image.GetImageCount(),
        image.GetMetadata(), &srv));


    if (totalTexture[file] != 0)
        delete totalTexture[file];

    totalTexture[file] = new Texture(srv, image);

    return totalTexture[file];
}

void Texture::Delete()
{
    for (auto texture : totalTexture)
        delete texture.second;
}

void Texture::PSSet(UINT slot)
{
    DEVICECONTEXT->PSSetShaderResources(slot, 1, &srv);
}

void Texture::DSSet(UINT slot)
{
    DEVICECONTEXT->DSSetShaderResources(slot, 1, &srv);
}

void Texture::getImages()
{
    uint8_t* colors = image.GetPixels();
    UINT size = image.GetPixelsSize(); // 각 픽셀은 rgba값을 가지고있으니 픽셀수 * 4한 결과값 나옴.

    vector<Float4> result(size / 4); // 픽셀수만큼..

    mCharArray = new unsigned char[size];

    //for (UINT i = 0; i < result.size(); i++)
    //{
    //    result[i].x = colors[i * 4 + 0] * scale; // rgba값 차례대로 넣어주기.
    //    result[i].y = colors[i * 4 + 1] * scale;
    //    result[i].z = colors[i * 4 + 2] * scale;
    //    result[i].w = colors[i * 4 + 3] * scale;
    //}

    for (UINT i = 0; i < result.size(); i++) // 픽셀수만큼.
    {
        mCharArray[i * 4 + 0] = colors[i * 4 + 0];
        mCharArray[i * 4 + 1] = colors[i * 4 + 1];
        mCharArray[i * 4 + 2] = colors[i * 4 + 2];
        mCharArray[i * 4 + 3] = colors[i * 4 + 3];
    }
}

vector<Float4> Texture::ReadPixels()
{
    uint8_t* colors = image.GetPixels();
    UINT size = image.GetPixelsSize(); // 각 픽셀은 rgba값을 가지고있으니 픽셀수 * 4한 결과값 나옴.
    float scale = 1.0f / 255.0f;
    vector<Float4> result(size / 4); // 픽셀수만큼..

    for (UINT i = 0; i < result.size(); i++)
    {
        result[i].x = colors[i * 4 + 0] * scale; // rgba값 차례대로 넣어주기.
        result[i].y = colors[i * 4 + 1] * scale;
        result[i].z = colors[i * 4 + 2] * scale;
        result[i].w = colors[i * 4 + 3] * scale;
    }

    return result;
}