#include "Framework.h"

TerrainLOD::TerrainLOD(wstring heightFile): 
    mHeightMapFileName(heightFile), 
    mCellsPerPatch(32) // 한 셀에 들어가는 정점개수? 듬성듬성 나누는 기준. 이 값이 높을수록 처음에 버텍스를 조금만 찍는다.
{
    mMaterial = new Material(L"TerrainLOD");
    mHullShader = Shader::AddHS(L"TerrainLOD");
    mDomainShader = Shader::AddDS(L"TerrainLOD");

    mMaterial->SetDiffuseMap(L"Textures/Dirt2.png");

    readHeightData();
    createTempVertices();

    mTerrainBuffer = new LODTerrainBuffer();
    mTerrainBuffer->data.cellSpacingU = 1.0f / mTextureDefaultWidth; // 256
    mTerrainBuffer->data.cellSpacingV = 1.0f / mTextureDefaultHeight; // 256

    mPatchWidth = ((mTextureDefaultWidth - 1) / mCellsPerPatch) + 1; // 듬성듬성 몇개 찍을것인가. 원랜 256개 찍어야하는데 테셀레이션에서 쪼갤꺼니까 여기선 조금만 찍는다. UINT형이라 8나옴.
    mPatchHeight = ((mTextureDefaultHeight - 1) / mCellsPerPatch) + 1;

    mVertices.resize((size_t)mPatchWidth * mPatchHeight); // 버텍스 개수 64개 256,256 기준
    mIndices.resize(((size_t)mPatchWidth - 1) * (mPatchHeight - 1) * 4); 

    int a = 0;
    // (patchWidth - 1) * (patchHeight - 1) = 네모개수. 49개
    // 그냥 터레인이면 *6 이다. 2개의 폴리곤을 네모처럼 이으려면 6개를 사용했으니까 (0,1,2, 2,1,3)
    // 근데 왜 4개? IA에 넘길 때 기존의 프리미티브 토폴로지인 트라이앵글리스트로 넘기는게 아니라 4 Control Pointer로 넘기고있다.

    createPatchVertex();
    createPatchIndex();
}

TerrainLOD::~TerrainLOD()
{
    delete mMaterial;
    delete mMesh;
    delete mTerrainBuffer;
}

void TerrainLOD::Update()
{   
    mCamera->Update();
    mCamera->SetViewToFrustum(mCamera->GetViewMatrix());
    //mCamera->GetFrustum()->GetPlanesForTerrainFrustumCulling(mTerrainBuffer->data.cullings); // 현재 업데이트중인 카메라의 절두체의 6면을 mTerrainBuffer에 넣어준다.

    UpdateWorld();
}

void TerrainLOD::Render()
{
    mMesh->SetIA(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

    mWorldBuffer->SetHSBuffer(0);
    mWorldBuffer->SetDSBuffer(0);

    mCamera->GetViewBuffer()->SetHSBuffer(1);
    mCamera->GetViewBuffer()->SetDSBuffer(1);

    mCamera->GetPerspectiveProjectionBuffer()->SetDSBuffer(2);

    mTerrainBuffer->SetHSBuffer(10);
    mTerrainBuffer->SetDSBuffer(10);

    mHeightTexture->DSSet(0);

    mMaterial->Set();
    mHullShader->Set();
    mDomainShader->Set();

    DEVICECONTEXT->DrawIndexed(mIndices.size(), 0, 0);
    DEVICECONTEXT->HSSetShader(nullptr, nullptr, 0); // 해제안해놓으면 다른곳에서도 그대로 적용되서 버그남. 한번렌더해주고 다시 해제시켜줘야한다.
    DEVICECONTEXT->DSSetShader(nullptr, nullptr, 0);
}

void TerrainLOD::PostRender()
{
    ImGui::SliderFloat2("Distance", (float*)&mTerrainBuffer->data.distance, 1, 1000);
    ImGui::SliderFloat2("Factor", (float*)&mTerrainBuffer->data.factor, 1, 64.0f);
    ImGui::SliderFloat("HeightScale", &mTerrainBuffer->data.heightScale, 0.0f, 100.0f);
}

float TerrainLOD::GetTargetPositionY(Vector3 target)
{
    //return 0;
    float offsetX = mFinalWidth / (mTextureDefaultWidth-1);   //   1120 / 255
    float offsetZ = mFinalHeight / (mTextureDefaultHeight-1);

    //float offsetX = mFinalWidth / mTextureDefaultWidth;   //   1120 / 255
    //float offsetZ = mFinalHeight / mTextureDefaultHeight;

    //return 0;
    UINT x = (UINT)target.x; 
    UINT z = (UINT)target.z; 

    if (x < 0 || x > mFinalWidth) return 0.0f; // 이건 그대로 최대로.
    if (z < 0 || z > mFinalHeight) return 0.0f;

    float tempX = x/offsetX;   //  target.x / 4.어쩌고저쩌고
    float tempZ = z/offsetZ;   //  target.z / 4.어쩌고저쩌고    -> 256,256 맵에서의 포지션이나옴.

    UINT index[4];
    index[0] = (mTextureDefaultWidth) *tempZ + tempX; //  이건 256,256 기준으로 해야할듯함. 위 포지션을 256,256 기준으로 변환했으니까.
    index[1] = (mTextureDefaultWidth) * (tempZ + 1) + tempX; //
    index[2] = (mTextureDefaultWidth) *tempZ + tempX + 1; //
    index[3] = (mTextureDefaultWidth) * (tempZ + 1) + tempX + 1; // 

    Vector3 p[4];

    for (int i = 0; i < 4; i++)
    {
        p[i] = mTempVertices[index[i]].position;        // 여기서 버티시즈도 256,256 짜리로.
    }

    //float u = target.x - p[0].x;
    //float v = target.z - p[0].z;
    float u = tempX - p[0].x;
    float v = tempZ - p[0].z;

    Vector3 result;
    if (u + v <= 1.0f)
    {
        result = p[0] + (p[2] - p[0]) * u + (p[1] - p[0]) * v;
    }
    else
    {
        u = 1.0f - u;
        v = 1.0f - v;

        result = p[3] + (p[1] - p[3]) * u + (p[2] - p[3]) * v;
    }

    return result.y;
}

void TerrainLOD::readHeightData()
{
    mHeightTexture = Texture::Add(mHeightMapFileName);
    //mHeightTexture = Texture::Add(L"Textures/100x100.png");
   
    mTextureDefaultWidth = mHeightTexture->GetWidth();
    mTextureDefaultHeight = mHeightTexture->GetHeight();
}

void TerrainLOD::createPatchVertex() // 원점이 가운데라는 기준으로 버텍스 찍어주기.
{
    //float halfWidth = mTextureDefaultWidth * mTerrainBuffer->data.cellSpacing * 0.5f; // 256 * 5 = 1280 * 0.5 = 640   // 반지름.  GetWidth에서  실제 width * cellSpaceing(default 5.0f) 곱해주는데 임의의값. 클수록 그냥 터레인 커지고 뭐 더 많이쪼개고 할듯
    //float halfHeight = mTextureDefaultHeight * mTerrainBuffer->data.cellSpacing * 0.5f;
    //// half값 이용하는건 센터를 0,0으로 잡으려고 하는것같다. 어쨌든 cellSpacing값에 의해 터레인크기가 결정된다.

    //float tempWidth = mTextureDefaultWidth * mTerrainBuffer->data.cellSpacing / mPatchWidth; // patchWidth는 듬성듬성 몇개찍을것인가의 값. 8 나옴. 최종적으로 1280 / 8 = 160
    //float tempHeight = mTextureDefaultHeight * mTerrainBuffer->data.cellSpacing / mPatchHeight;

    //float du = 1.0f / mPatchWidth; // 듬성듬성버텍스 uv값 단위.  1/8
    //float dv = 1.0f / mPatchHeight;

    //float offsetZ = -(halfHeight - (mPatchHeight - 1) * tempHeight);
    //float offsetX = halfWidth;
    //mFinalWidth = -halfWidth + (mPatchWidth - 1) * tempWidth + offsetX;
    //mFinalHeight = halfHeight + offsetZ;

    //for (UINT z = 0; z < mPatchHeight; z++) // 듬성버텍스 개수 8개     // 여기서 버텍스포지션값을 크게 잡아서 버텍스 사이의 간격이 넓어져서 터레인이 기존에 비해 매우 커짐.
    //{
    //    float tempZ = halfHeight - z * tempHeight + offsetZ;
 
    //    for (UINT x = 0; x < mPatchWidth; x++)
    //    {
    //        float tempX = -halfWidth + x * tempWidth + offsetX; 

    //        UINT index = mPatchWidth * z + x;
    //        mVertices[index].position = Vector3(tempX, 0.0f, tempZ); // position.y값은 도메인셰이더에서 적용함.
    //        mVertices[index].uv = { x * du, z * dv };
    //    }
    //}

    float halfWidth = mTextureDefaultWidth * mTerrainBuffer->data.cellSpacing * 0.5f; // 256 * 5 = 1280 * 0.5 = 640   // 반지름.  GetWidth에서  실제 width * cellSpaceing(default 5.0f) 곱해주는데 임의의값. 클수록 그냥 터레인 커지고 뭐 더 많이쪼개고 할듯
    float halfHeight = mTextureDefaultHeight * mTerrainBuffer->data.cellSpacing * 0.5f;
    // half값 이용하는건 센터를 0,0으로 잡으려고 하는것같다. 어쨌든 cellSpacing값에 의해 터레인크기가 결정된다.

    float tempWidth = mTextureDefaultWidth * mTerrainBuffer->data.cellSpacing / mPatchWidth; // patchWidth는 듬성듬성 몇개찍을것인가의 값. 8 나옴. 최종적으로 1280 / 8 = 160
    float tempHeight = mTextureDefaultHeight * mTerrainBuffer->data.cellSpacing / mPatchHeight;

    float du = 1.0f / mPatchWidth; // 듬성듬성버텍스 uv값 단위.  1/8
    float dv = 1.0f / mPatchHeight;

    for (UINT z = 0; z < mPatchHeight; z++)
    {
        float tempZ = halfHeight - z * tempHeight;

        for (UINT x = 0; x < mPatchWidth; x++)
        {
            float tempX = -halfWidth + x * tempWidth;

            UINT index = mPatchWidth * z + x;
            mVertices[index].position = Vector3(tempX, 0.0f, tempZ);
            mVertices[index].uv = { x * du, z * dv };
        }
    }
}

void TerrainLOD::createPatchIndex()
{
    UINT index = 0;

    for (UINT z = 0; z < mPatchHeight - 1; z++) // 면개수만큼 반복.
    {
        for (UINT x = 0; x < mPatchWidth - 1; x++)
        {
            mIndices[index++] = mPatchWidth * z + x;
            mIndices[index++] = mPatchWidth * z + x + 1;
            mIndices[index++] = mPatchWidth * (z + 1) + x;
            mIndices[index++] = mPatchWidth * (z + 1) + x + 1;
        } // 한면의 컨트롤포인트 4개
    }

    mMesh = new Mesh(mVertices.data(), sizeof(VertexType), mVertices.size(),
        mIndices.data(), mIndices.size());
}

void TerrainLOD::createTempVertices()
{
    UINT mTerrainWidth = mHeightTexture->GetWidth() - 1;
    UINT mTerrainHeight = mHeightTexture->GetHeight() - 1;

    vector<Float4> pixels = mHeightTexture->ReadPixels();// HeightMap의 픽셀이다. position.y값 셋팅전용 픽셀.

    //Vertices
    for (UINT z = 0; z <= mTerrainHeight; z++)
    {
        for (UINT x = 0; x <= mTerrainWidth; x++)
        {
            VertexType vertex;
            vertex.position = Float3((float)x, 0.0f, (float)z);
            vertex.uv = Float2(x / (float)mTerrainWidth, 1.0f - z / (float)mTerrainHeight);

            UINT index = (mTerrainWidth + 1) * z + x;
            vertex.position.y += pixels[index].x * 20.0f; // 걍 값이 정규화되서 너무 작으니까 임의의 값 20.0f를 곱해준것.

            mTempVertices.emplace_back(vertex);
        }
    }
}
