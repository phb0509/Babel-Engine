#include "Framework.h"

TerrainLOD::TerrainLOD(wstring heightFile): 
    mHeightMapFileName(heightFile), 
    mCellsPerPatch(32) // �� ���� ���� ��������? �뼺�뼺 ������ ����. �� ���� �������� ó���� ���ؽ��� ���ݸ� ��´�.
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

    mPatchWidth = ((mTextureDefaultWidth - 1) / mCellsPerPatch) + 1; // �뼺�뼺 � �������ΰ�. ���� 256�� �����ϴµ� �׼����̼ǿ��� �ɰ����ϱ� ���⼱ ���ݸ� ��´�. UINT���̶� 8����.
    mPatchHeight = ((mTextureDefaultHeight - 1) / mCellsPerPatch) + 1;

    mVertices.resize((size_t)mPatchWidth * mPatchHeight); // ���ؽ� ���� 64�� 256,256 ����
    mIndices.resize(((size_t)mPatchWidth - 1) * (mPatchHeight - 1) * 4); 

    int a = 0;
    // (patchWidth - 1) * (patchHeight - 1) = �׸𰳼�. 49��
    // �׳� �ͷ����̸� *6 �̴�. 2���� �������� �׸�ó�� �������� 6���� ��������ϱ� (0,1,2, 2,1,3)
    // �ٵ� �� 4��? IA�� �ѱ� �� ������ ������Ƽ�� ���������� Ʈ���̾ޱ۸���Ʈ�� �ѱ�°� �ƴ϶� 4 Control Pointer�� �ѱ���ִ�.

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
    //mCamera->GetFrustum()->GetPlanesForTerrainFrustumCulling(mTerrainBuffer->data.cullings); // ���� ������Ʈ���� ī�޶��� ����ü�� 6���� mTerrainBuffer�� �־��ش�.

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
    DEVICECONTEXT->HSSetShader(nullptr, nullptr, 0); // �������س����� �ٸ��������� �״�� ����Ǽ� ���׳�. �ѹ��������ְ� �ٽ� ������������Ѵ�.
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

    if (x < 0 || x > mFinalWidth) return 0.0f; // �̰� �״�� �ִ��.
    if (z < 0 || z > mFinalHeight) return 0.0f;

    float tempX = x/offsetX;   //  target.x / 4.��¼����¼��
    float tempZ = z/offsetZ;   //  target.z / 4.��¼����¼��    -> 256,256 �ʿ����� �������̳���.

    UINT index[4];
    index[0] = (mTextureDefaultWidth) *tempZ + tempX; //  �̰� 256,256 �������� �ؾ��ҵ���. �� �������� 256,256 �������� ��ȯ�����ϱ�.
    index[1] = (mTextureDefaultWidth) * (tempZ + 1) + tempX; //
    index[2] = (mTextureDefaultWidth) *tempZ + tempX + 1; //
    index[3] = (mTextureDefaultWidth) * (tempZ + 1) + tempX + 1; // 

    Vector3 p[4];

    for (int i = 0; i < 4; i++)
    {
        p[i] = mTempVertices[index[i]].position;        // ���⼭ ��Ƽ��� 256,256 ¥����.
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

void TerrainLOD::createPatchVertex() // ������ ������ �������� ���ؽ� ����ֱ�.
{
    //float halfWidth = mTextureDefaultWidth * mTerrainBuffer->data.cellSpacing * 0.5f; // 256 * 5 = 1280 * 0.5 = 640   // ������.  GetWidth����  ���� width * cellSpaceing(default 5.0f) �����ִµ� �����ǰ�. Ŭ���� �׳� �ͷ��� Ŀ���� �� �� �����ɰ��� �ҵ�
    //float halfHeight = mTextureDefaultHeight * mTerrainBuffer->data.cellSpacing * 0.5f;
    //// half�� �̿��ϴ°� ���͸� 0,0���� �������� �ϴ°Ͱ���. ��·�� cellSpacing���� ���� �ͷ���ũ�Ⱑ �����ȴ�.

    //float tempWidth = mTextureDefaultWidth * mTerrainBuffer->data.cellSpacing / mPatchWidth; // patchWidth�� �뼺�뼺 ��������ΰ��� ��. 8 ����. ���������� 1280 / 8 = 160
    //float tempHeight = mTextureDefaultHeight * mTerrainBuffer->data.cellSpacing / mPatchHeight;

    //float du = 1.0f / mPatchWidth; // �뼺�뼺���ؽ� uv�� ����.  1/8
    //float dv = 1.0f / mPatchHeight;

    //float offsetZ = -(halfHeight - (mPatchHeight - 1) * tempHeight);
    //float offsetX = halfWidth;
    //mFinalWidth = -halfWidth + (mPatchWidth - 1) * tempWidth + offsetX;
    //mFinalHeight = halfHeight + offsetZ;

    //for (UINT z = 0; z < mPatchHeight; z++) // �뼺���ؽ� ���� 8��     // ���⼭ ���ؽ������ǰ��� ũ�� ��Ƽ� ���ؽ� ������ ������ �о����� �ͷ����� ������ ���� �ſ� Ŀ��.
    //{
    //    float tempZ = halfHeight - z * tempHeight + offsetZ;
 
    //    for (UINT x = 0; x < mPatchWidth; x++)
    //    {
    //        float tempX = -halfWidth + x * tempWidth + offsetX; 

    //        UINT index = mPatchWidth * z + x;
    //        mVertices[index].position = Vector3(tempX, 0.0f, tempZ); // position.y���� �����μ��̴����� ������.
    //        mVertices[index].uv = { x * du, z * dv };
    //    }
    //}

    float halfWidth = mTextureDefaultWidth * mTerrainBuffer->data.cellSpacing * 0.5f; // 256 * 5 = 1280 * 0.5 = 640   // ������.  GetWidth����  ���� width * cellSpaceing(default 5.0f) �����ִµ� �����ǰ�. Ŭ���� �׳� �ͷ��� Ŀ���� �� �� �����ɰ��� �ҵ�
    float halfHeight = mTextureDefaultHeight * mTerrainBuffer->data.cellSpacing * 0.5f;
    // half�� �̿��ϴ°� ���͸� 0,0���� �������� �ϴ°Ͱ���. ��·�� cellSpacing���� ���� �ͷ���ũ�Ⱑ �����ȴ�.

    float tempWidth = mTextureDefaultWidth * mTerrainBuffer->data.cellSpacing / mPatchWidth; // patchWidth�� �뼺�뼺 ��������ΰ��� ��. 8 ����. ���������� 1280 / 8 = 160
    float tempHeight = mTextureDefaultHeight * mTerrainBuffer->data.cellSpacing / mPatchHeight;

    float du = 1.0f / mPatchWidth; // �뼺�뼺���ؽ� uv�� ����.  1/8
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

    for (UINT z = 0; z < mPatchHeight - 1; z++) // �鰳����ŭ �ݺ�.
    {
        for (UINT x = 0; x < mPatchWidth - 1; x++)
        {
            mIndices[index++] = mPatchWidth * z + x;
            mIndices[index++] = mPatchWidth * z + x + 1;
            mIndices[index++] = mPatchWidth * (z + 1) + x;
            mIndices[index++] = mPatchWidth * (z + 1) + x + 1;
        } // �Ѹ��� ��Ʈ������Ʈ 4��
    }

    mMesh = new Mesh(mVertices.data(), sizeof(VertexType), mVertices.size(),
        mIndices.data(), mIndices.size());
}

void TerrainLOD::createTempVertices()
{
    UINT mTerrainWidth = mHeightTexture->GetWidth() - 1;
    UINT mTerrainHeight = mHeightTexture->GetHeight() - 1;

    vector<Float4> pixels = mHeightTexture->ReadPixels();// HeightMap�� �ȼ��̴�. position.y�� �������� �ȼ�.

    //Vertices
    for (UINT z = 0; z <= mTerrainHeight; z++)
    {
        for (UINT x = 0; x <= mTerrainWidth; x++)
        {
            VertexType vertex;
            vertex.position = Float3((float)x, 0.0f, (float)z);
            vertex.uv = Float2(x / (float)mTerrainWidth, 1.0f - z / (float)mTerrainHeight);

            UINT index = (mTerrainWidth + 1) * z + x;
            vertex.position.y += pixels[index].x * 20.0f; // �� ���� ����ȭ�Ǽ� �ʹ� �����ϱ� ������ �� 20.0f�� �����ذ�.

            mTempVertices.emplace_back(vertex);
        }
    }
}
