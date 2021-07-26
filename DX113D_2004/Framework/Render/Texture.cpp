#include "Framework.h"

map<wstring, Texture*> Texture::totalTexture;
map<ID3D11ShaderResourceView*, Texture*> Texture::totalSRVTexture;

Texture::Texture(ID3D11ShaderResourceView* srv, ScratchImage& image)
    : srv(srv), image(move(image))
{
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
        LoadFromTGAFile(file.c_str(), nullptr, image);
    else if(extension == L"dds")
        LoadFromDDSFile(file.c_str(), DDS_FLAGS_NONE, nullptr, image);
    else
        LoadFromWICFile(file.c_str(), WIC_FLAGS_FORCE_RGB, nullptr, image);

    ID3D11ShaderResourceView* srv;
    


    V(CreateShaderResourceView(DEVICE, image.GetImages(), image.GetImageCount(),
        image.GetMetadata(), &srv));

    totalTexture[file] = new Texture(srv, image);

    return totalTexture[file];
}

Texture* Texture::Add(ID3D11ShaderResourceView* _srv)
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

vector<Float4> Texture::ReadPixels()
{
    uint8_t* colors = image.GetPixels();
    UINT size = image.GetPixelsSize(); // 각 픽셀은 rgba값을 가지고있으니 픽셀수 * 4한 결과값 나옴.

    float scale = 1.0f / 255.0f;

    vector<Float4> result(size / 4); // 픽셀수만큼..

    for (UINT i = 0; i < result.size() ; i ++)
    {
        result[i].x = colors[i*4 + 0] * scale; // rgba값 차례대로 넣어주기.
        result[i].y = colors[i*4 + 1] * scale;
        result[i].z = colors[i*4 + 2] * scale;
        result[i].w = colors[i*4 + 3] * scale;
    }

    return result;
}
