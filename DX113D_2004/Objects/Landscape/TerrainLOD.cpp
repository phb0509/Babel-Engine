#include "Framework.h"

TerrainLOD::TerrainLOD(wstring heightFile)
    : heightFile(heightFile), cellsPerPatch(32)
{
    material = new Material(L"TerrainLOD");
    hullShader = Shader::AddHS(L"TerrainLOD");
    domainShader = Shader::AddDS(L"TerrainLOD");

    material->SetDiffuseMap(L"Textures/Landscape/Dirt2.png");

    ReadHeightData();

    buffer = new TerrainBuffer();
    buffer->data.cellSpacingU = 1.0f / width;
    buffer->data.cellSpacingV = 1.0f / height;

    patchWidth = ((width - 1) / cellsPerPatch) + 1;
    patchHeight = ((height - 1) / cellsPerPatch) + 1;

    vertices.resize(patchWidth * patchHeight);
    indices.resize((patchWidth - 1) * (patchHeight - 1) * 4);

    CreatePatchVertex();
    CreatePatchIndex();

    frustum = new Frustum();
}

TerrainLOD::~TerrainLOD()
{
    delete material;
    delete mesh;

    delete buffer;
}

void TerrainLOD::Update()
{    
    frustum->Update();
    frustum->GetPlanes(buffer->data.cullings);

    UpdateWorld();
}

void TerrainLOD::Render()
{
    mesh->IASet(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

    worldBuffer->SetHSBuffer(0);
    worldBuffer->SetDSBuffer(0);

    CAMERA->GetViewBuffer()->SetHSBuffer(1);
    CAMERA->GetViewBuffer()->SetDSBuffer(1);

    Environment::Get()->GetProjectionBuffer()->SetDSBuffer(2);

    buffer->SetHSBuffer(10);
    buffer->SetDSBuffer(10);

    heightTexture->DSSet(0);

    material->Set();
    hullShader->Set();
    domainShader->Set();

    DC->DrawIndexed(indices.size(), 0, 0);

    DC->HSSetShader(nullptr, nullptr, 0);
    DC->DSSetShader(nullptr, nullptr, 0);
}

void TerrainLOD::PostRender()
{
    ImGui::SliderFloat2("Distance", (float*)&buffer->data.distance, 1, 1000);
    ImGui::SliderFloat2("Factor", (float*)&buffer->data.factor, 1, 64.0f);
    ImGui::SliderFloat("HeightScale", &buffer->data.heightScale, 0.0f, 100.0f);
}

float TerrainLOD::GetWidth()
{
    return width * buffer->data.cellSpacing;
}

float TerrainLOD::GetHeight()
{
    return height * buffer->data.cellSpacing;
}

void TerrainLOD::ReadHeightData()
{
    heightTexture = Texture::Add(heightFile);

    width = heightTexture->Width();
    height = heightTexture->Height();
}

void TerrainLOD::CreatePatchVertex()
{
    float halfWidth = GetWidth() * 0.5f;
    float halfHeight = GetHeight() * 0.5f;

    float tempWidth = GetWidth() / patchWidth;
    float tempHeight = GetHeight() / patchHeight;

    float du = 1.0f / patchWidth;
    float dv = 1.0f / patchHeight;

    for (UINT z = 0; z < patchHeight; z++)
    {
        float tempZ = halfHeight - z * tempHeight;

        for (UINT x = 0; x < patchWidth; x++)
        {
            float tempX = -halfWidth + x * tempWidth;

            UINT index = patchWidth * z + x;
            vertices[index].position = Vector3(tempX, 0.0f, tempZ);
            vertices[index].uv = { x * du, z * dv };            
        }
    }
}

void TerrainLOD::CreatePatchIndex()
{
    UINT index = 0;
    for (UINT z = 0; z < patchHeight - 1; z++)
    {
        for (UINT x = 0; x < patchWidth - 1; x++)
        {
            indices[index++] = patchWidth * z + x;
            indices[index++] = patchWidth * z + x + 1;
            indices[index++] = patchWidth * (z + 1) + x;
            indices[index++] = patchWidth * (z + 1) + x + 1;
        }
    }

    mesh = new Mesh(vertices.data(), sizeof(VertexType), vertices.size(),
        indices.data(), indices.size());
}
