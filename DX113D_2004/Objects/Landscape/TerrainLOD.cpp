#include "Framework.h"

TerrainLOD::TerrainLOD(wstring heightFile)
    : heightFile(heightFile), 
    cellsPerPatch(32) // 한 셀에 들어가는 정점개수? 듬성듬성 나누는 기준. 이 값이 높을수록 처음에 버텍스를 조금만 찍는다.
{
    material = new Material(L"TerrainLOD");
    hullShader = Shader::AddHS(L"TerrainLOD");
    domainShader = Shader::AddDS(L"TerrainLOD");

    material->SetDiffuseMap(L"Textures/Landscape/Dirt2.png");

    ReadHeightData();

    mTerrainBuffer = new TerrainBuffer();
    mTerrainBuffer->data.cellSpacingU = 1.0f / width;
    mTerrainBuffer->data.cellSpacingV = 1.0f / height;

    patchWidth = ((width - 1) / cellsPerPatch) + 1; // 듬성듬성 몇개 찍을것인가. 원랜 256개 찍어야하는데 테셀레이션에서 쪼갤꺼니까 여기선 조금만 찍는다. UINT형이라 8나옴.
    patchHeight = ((height - 1) / cellsPerPatch) + 1;

    vertices.resize(patchWidth * patchHeight); // 버텍스 개수 64개
    indices.resize((patchWidth - 1) * (patchHeight - 1) * 4); 
    // (patchWidth - 1) * (patchHeight - 1) = 네모개수. 49개
    // 그냥 터레인이면 *6 이다. 2개의 폴리곤을 네모처럼 이으려면 6개를 사용했으니까 (0,1,2, 2,1,3)
    // 근데 왜 4개? IA에 넘길 때 기존의 프리미티브 토폴로지인 트라이앵글리스트로 넘기는게 아니라 4 Control Pointer로 넘기고있다.

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
    frustum->GetPlanes(mTerrainBuffer->data.cullings); // 현재 업데이트중인 카메라의 절두체의 6면을 mTerrainBuffer에 넣어준다.

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

    DEVICECONTEXT->HSSetShader(nullptr, nullptr, 0); // 해제안해놓으면 다른곳에서도 그대로 적용되서 버그남. 한번렌더해주고 다시 해제시켜줘야한다.
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
    return width * mTerrainBuffer->data.cellSpacing; // cellSpacing은 default로 5로 되어있음.
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

void TerrainLOD::CreatePatchVertex() // 원점이 가운데라는 기준으로 버텍스 찍어주기.
{
    float halfWidth = GetWidth() * 0.5f; // 256 * 5 = 1280 * 0.5 = 640   // 반지름.  GetWidth에서  실제 width * cellSpaceing(default 5.0f) 곱해주는데 임의의값. 클수록 그냥 터레인 커지고 뭐 더 많이쪼개고 할듯
    float halfHeight = GetHeight() * 0.5f;
    // half값 이용하는건 센터를 0,0으로 잡으려고 하는것같다. 어쨌든 cellSpacing값에 의해 터레인크기가 결정된다.


    float tempWidth = GetWidth() / patchWidth; // patchWidth는 듬성듬성 몇개찍을것인가의 값. 8 나옴. 최종적으로 1280 / 8 = 160
    float tempHeight = GetHeight() / patchHeight;

    float du = 1.0f / patchWidth; // 듬성듬성버텍스 uv값 단위.  1/8
    float dv = 1.0f / patchHeight;

    for (UINT z = 0; z < patchHeight; z++) // 듬성버텍스 개수 8개     // 여기서 버텍스포지션값을 크게 잡아서 버텍스 사이의 간격이 넓어져서 터레인이 기존에 비해 매우 커짐.
    {
        float tempZ = halfHeight - z * tempHeight; // 640 - z * 160

        for (UINT x = 0; x < patchWidth; x++)
        {
            float tempX = -halfWidth + x * tempWidth; // 시작점? -640 + x * 160

            UINT index = patchWidth * z + x;
            vertices[index].position = Vector3(tempX, 0.0f, tempZ); // position.y값은 도메인셰이더에서 적용함.
            vertices[index].uv = { x * du, z * dv };            
        }
    }
}

void TerrainLOD::CreatePatchIndex()
{
    UINT index = 0;

    for (UINT z = 0; z < patchHeight - 1; z++) // 면개수만큼 반복.
    {
        for (UINT x = 0; x < patchWidth - 1; x++)
        {
            indices[index++] = patchWidth * z + x;
            indices[index++] = patchWidth * z + x + 1;
            indices[index++] = patchWidth * (z + 1) + x;
            indices[index++] = patchWidth * (z + 1) + x + 1;
        } // 한면의 컨트롤포인트 4개
    }

    

    mesh = new Mesh(vertices.data(), sizeof(VertexType), vertices.size(),
        indices.data(), indices.size());
}
