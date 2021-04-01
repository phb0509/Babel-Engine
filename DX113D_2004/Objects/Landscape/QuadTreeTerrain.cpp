#include "Framework.h"

QuadTreeTerrain::QuadTreeTerrain(TerrainData* terrainData)
    : drawCount(0)
{
    material = new Material(L"Lighting");
    material->SetDiffuseMap(L"Textures/Landscape/Dirt2.png");

    frustum = new Frustum();

    UINT vertexCount = terrainData->vertices.size();
    triangleCount = vertexCount / 3;

    vertices = new VertexType[vertexCount];
    memcpy(vertices, terrainData->vertices.data(), sizeof(VertexType) * vertexCount);

    float centerX = 0.0f;
    float centerZ = 0.0f;
    float width = 0.0f;

    CalcMeshDimensions(vertexCount, centerX, centerZ, width);

    root = new Node();
    CreateTreeNode(root, centerX, centerZ, width);

    this->width = terrainData->width;
    this->height = terrainData->height;
}

QuadTreeTerrain::~QuadTreeTerrain()
{
    DeleteNode(root);
    delete root;

    delete material;

    delete frustum;
    delete[] vertices;
}

void QuadTreeTerrain::Update()
{
    frustum->Update();
    UpdateWorld();
}

void QuadTreeTerrain::Render()
{
    drawCount = 0;
    RenderNode(root);
}

void QuadTreeTerrain::PostRender()
{
    ImGui::Text("DrawCount : %d", drawCount);
}

void QuadTreeTerrain::RenderNode(Node* node)
{
    Vector3 center(node->x, 0.0f, node->z);
    float radius = node->width * 0.5f;

    if (!frustum->ContainSphere(center, radius))
        return;

    UINT count = 0;
    for (UINT i = 0; i < 4; i++)
    {
        if (node->children[i] != nullptr)
        {
            count++;
            RenderNode(node->children[i]);
        }
    }

    if (count != 0)
        return;

    node->mesh->IASet();
    
    worldBuffer->SetVSBuffer(0);
    material->Set();

    UINT indexCount = node->triangleCount * 3;
    DC->DrawIndexed(indexCount, 0, 0);

    drawCount += node->triangleCount;
}

void QuadTreeTerrain::DeleteNode(Node* node)
{
    for (UINT i = 0; i < 4; i++)
    {
        if (node->children[i] != nullptr)
        {
            DeleteNode(node->children[i]);
            delete node->children[i];
        }
    }

    delete node->mesh;
}

void QuadTreeTerrain::CalcMeshDimensions(UINT vertexCount, float& centerX, float& centerZ, float& width)
{
    for (UINT i = 0; i < vertexCount; i++)
    {
        centerX += vertices[i].position.x;
        centerZ += vertices[i].position.z;
    }

    centerX /= (float)vertexCount;
    centerZ /= (float)vertexCount;

    float maxX = 0.0f;
    float maxZ = 0.0f;

    for (UINT i = 0; i < vertexCount; i++)
    {
        float width = abs(vertices[i].position.x - centerX);
        float depth = abs(vertices[i].position.z - centerZ);

        if (width > maxX) maxX = width;
        if (depth > maxZ) maxZ = depth;
    }

    width = max(maxX, maxZ) * 2.0f;
}

void QuadTreeTerrain::CreateTreeNode(Node* node, float x, float z, float width)
{
    node->x = x;
    node->z = z;
    node->width = width;

    node->triangleCount = 0;
    node->mesh = nullptr;

    for (Node* child : node->children)
        child = nullptr;

    UINT triangles = ContainTriangleCount(x, z, width);

    if (triangles == 0)
        return;

    if (triangles > MIN_TRIANGLE)//쪼개야 하는 상황(나눌수 있는 노드)
    {
        for (UINT i = 0; i < 4; i++)
        {
            float offsetX = (((i % 2) == 0) ? -1.0f : 1.0f) * (width / 4.0f);
            float offsetZ = (((i % 4) < 2) ? -1.0f : 1.0f) * (width / 4.0f);

            UINT count = ContainTriangleCount(x + offsetX, z + offsetZ, width * 0.5f);

            if (count > 0)
            {
                node->children[i] = new Node();
                CreateTreeNode(node->children[i], x + offsetX, z + offsetZ, width * 0.5f);
            }
        }

        return;
    }

    //더이상 나줘지지 않는 노드(최하위 노드)
    node->triangleCount = triangles;

    UINT vertexCount = triangles * 3;
    VertexType* vertices = new VertexType[vertexCount];
    UINT* indices = new UINT[vertexCount];

    UINT index = 0, vertexIndex = 0;
    for (UINT i = 0; i < triangleCount; i++)
    {
        if (IsTriangleContained(i, x, z, width))
        {
            vertexIndex = i * 3;
            vertices[index] = this->vertices[vertexIndex];
            indices[index] = index;
            index++;

            vertexIndex++;
            vertices[index] = this->vertices[vertexIndex];
            indices[index] = index;
            index++;

            vertexIndex++;
            vertices[index] = this->vertices[vertexIndex];
            indices[index] = index;
            index++;
        }
    }

    node->mesh = new Mesh(vertices, sizeof(VertexType), vertexCount,
        indices, vertexCount);

    delete[] vertices;
    delete[] indices;
}

UINT QuadTreeTerrain::ContainTriangleCount(float x, float z, float width)
{
    UINT count = 0;

    for (UINT i = 0; i < triangleCount; i++)
    {
        if (IsTriangleContained(i, x, z, width))
            count++;
    }

    return count;
}

bool QuadTreeTerrain::IsTriangleContained(UINT index, float x, float z, float width)
{
    UINT vertexIndex = index * 3;
    float radius = width * 0.5f;

    float x1 = vertices[vertexIndex].position.x;
    float z1 = vertices[vertexIndex].position.z;
    vertexIndex++;

    float x2 = vertices[vertexIndex].position.x;
    float z2 = vertices[vertexIndex].position.z;
    vertexIndex++;

    float x3 = vertices[vertexIndex].position.x;
    float z3 = vertices[vertexIndex].position.z;   

    float minX = min(x1, min(x2, x3));
    if (minX > (x + radius))
        return false;

    float minZ = min(z1, min(z2, z3));
    if (minZ > (z + radius))
        return false;

    float maxX = max(x1, max(x2, x3));
    if (maxX < (x - radius))
        return false;

    float maxZ = max(z1, max(z2, z3));
    if (maxZ < (z - radius))
        return false;

    return true;
}
