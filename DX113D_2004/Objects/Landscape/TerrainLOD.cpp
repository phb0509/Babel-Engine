#include "Framework.h"

TerrainLOD::TerrainLOD(wstring heightFile)
    : heightFile(heightFile), 
    cellsPerPatch(32) // �� ���� ���� ��������? �뼺�뼺 ������ ����. �� ���� �������� ó���� ���ؽ��� ���ݸ� ��´�.
{
    material = new Material(L"TerrainLOD");
    hullShader = Shader::AddHS(L"TerrainLOD");
    domainShader = Shader::AddDS(L"TerrainLOD");

    material->SetDiffuseMap(L"Textures/Landscape/Dirt2.png");

    ReadHeightData();

    mTerrainBuffer = new TerrainBuffer();
    mTerrainBuffer->data.cellSpacingU = 1.0f / width;
    mTerrainBuffer->data.cellSpacingV = 1.0f / height;

    patchWidth = ((width - 1) / cellsPerPatch) + 1; // �뼺�뼺 � �������ΰ�. ���� 256�� �����ϴµ� �׼����̼ǿ��� �ɰ����ϱ� ���⼱ ���ݸ� ��´�. UINT���̶� 8����.
    patchHeight = ((height - 1) / cellsPerPatch) + 1;

    vertices.resize(patchWidth * patchHeight); // ���ؽ� ���� 64��
    indices.resize((patchWidth - 1) * (patchHeight - 1) * 4); 
    // (patchWidth - 1) * (patchHeight - 1) = �׸𰳼�. 49��
    // �׳� �ͷ����̸� *6 �̴�. 2���� �������� �׸�ó�� �������� 6���� ��������ϱ� (0,1,2, 2,1,3)
    // �ٵ� �� 4��? IA�� �ѱ� �� ������ ������Ƽ�� ���������� Ʈ���̾ޱ۸���Ʈ�� �ѱ�°� �ƴ϶� 4 Control Pointer�� �ѱ���ִ�.

    CreatePatchVertex();
    CreatePatchIndex();

    frustum = new Frustum();
}

TerrainLOD::~TerrainLOD()
{
    delete material;
    delete mesh;

    delete mTerrainBuffer;
}

void TerrainLOD::Update()
{    
    frustum->Update();
    frustum->GetPlanes(mTerrainBuffer->data.cullings); // ���� ������Ʈ���� ī�޶��� ����ü�� 6���� mTerrainBuffer�� �־��ش�.

    UpdateWorld();
}

void TerrainLOD::Render()
{
    mesh->IASet(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

    mWorldBuffer->SetHSBuffer(0);
    mWorldBuffer->SetDSBuffer(0);

    TARGETCAMERA->GetViewBuffer()->SetHSBuffer(1);
    TARGETCAMERA->GetViewBuffer()->SetDSBuffer(1);

    Environment::Get()->GetProjectionBuffer()->SetDSBuffer(2);

    mTerrainBuffer->SetHSBuffer(10);
    mTerrainBuffer->SetDSBuffer(10);

    heightTexture->DSSet(0);

    material->Set();
    hullShader->Set();
    domainShader->Set();

    DEVICECONTEXT->DrawIndexed(indices.size(), 0, 0);

    DEVICECONTEXT->HSSetShader(nullptr, nullptr, 0); // �������س����� �ٸ��������� �״�� ����Ǽ� ���׳�. �ѹ��������ְ� �ٽ� ������������Ѵ�.
    DEVICECONTEXT->DSSetShader(nullptr, nullptr, 0);
}

void TerrainLOD::PostRender()
{
    ImGui::SliderFloat2("Distance", (float*)&mTerrainBuffer->data.distance, 1, 1000);
    ImGui::SliderFloat2("Factor", (float*)&mTerrainBuffer->data.factor, 1, 64.0f);
    ImGui::SliderFloat("HeightScale", &mTerrainBuffer->data.heightScale, 0.0f, 100.0f);
}

float TerrainLOD::GetWidth()
{
    return width * mTerrainBuffer->data.cellSpacing; // cellSpacing�� default�� 5�� �Ǿ�����.
}

float TerrainLOD::GetHeight()
{
    return height * mTerrainBuffer->data.cellSpacing;
}

void TerrainLOD::ReadHeightData()
{
    heightTexture = Texture::Add(heightFile);

    width = heightTexture->GetWidth();
    height = heightTexture->GetHeight();
}

void TerrainLOD::CreatePatchVertex() // ������ ������ �������� ���ؽ� ����ֱ�.
{
    float halfWidth = GetWidth() * 0.5f; // 256 * 5 = 1280 * 0.5 = 640   // ������.  GetWidth����  ���� width * cellSpaceing(default 5.0f) �����ִµ� �����ǰ�. Ŭ���� �׳� �ͷ��� Ŀ���� �� �� �����ɰ��� �ҵ�
    float halfHeight = GetHeight() * 0.5f;
    // half�� �̿��ϴ°� ���͸� 0,0���� �������� �ϴ°Ͱ���. ��·�� cellSpacing���� ���� �ͷ���ũ�Ⱑ �����ȴ�.


    float tempWidth = GetWidth() / patchWidth; // patchWidth�� �뼺�뼺 ��������ΰ��� ��. 8 ����. ���������� 1280 / 8 = 160
    float tempHeight = GetHeight() / patchHeight;

    float du = 1.0f / patchWidth; // �뼺�뼺���ؽ� uv�� ����.  1/8
    float dv = 1.0f / patchHeight;

    for (UINT z = 0; z < patchHeight; z++) // �뼺���ؽ� ���� 8��     // ���⼭ ���ؽ������ǰ��� ũ�� ��Ƽ� ���ؽ� ������ ������ �о����� �ͷ����� ������ ���� �ſ� Ŀ��.
    {
        float tempZ = halfHeight - z * tempHeight; // 640 - z * 160

        for (UINT x = 0; x < patchWidth; x++)
        {
            float tempX = -halfWidth + x * tempWidth; // ������? -640 + x * 160

            UINT index = patchWidth * z + x;
            vertices[index].position = Vector3(tempX, 0.0f, tempZ); // position.y���� �����μ��̴����� ������.
            vertices[index].uv = { x * du, z * dv };            
        }
    }
}

void TerrainLOD::CreatePatchIndex()
{
    UINT index = 0;

    for (UINT z = 0; z < patchHeight - 1; z++) // �鰳����ŭ �ݺ�.
    {
        for (UINT x = 0; x < patchWidth - 1; x++)
        {
            indices[index++] = patchWidth * z + x;
            indices[index++] = patchWidth * z + x + 1;
            indices[index++] = patchWidth * (z + 1) + x;
            indices[index++] = patchWidth * (z + 1) + x + 1;
        } // �Ѹ��� ��Ʈ������Ʈ 4��
    }

    

    mesh = new Mesh(vertices.data(), sizeof(VertexType), vertices.size(),
        indices.data(), indices.size());
}
