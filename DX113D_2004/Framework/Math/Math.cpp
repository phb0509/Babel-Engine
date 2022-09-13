#include "Framework.h"
#include "Math.h"

float GameMath::Saturate(const float& value)
{
    return max(0.0f, min(1.0f, value));
}

int GameMath::Random(int min, int max)
{
    return rand() % (max - min) + min;
}

float GameMath::Random(float min, int max)
{
    float normal = rand() / (float)RAND_MAX;
    return min + (max - min) * normal;
}

float GameMath::Distance(const Vector3& v1, const Vector3& v2)
{
    return (v1 - v2).Length();
}

bool GameMath::CompareFloat(float v1, float v2) // v1과 v2가 같은지.
{
    if (fabsf(v1 - v2) <= 0.04f)
    {
        return true;
    }

    return false;
}

bool GameMath::CompareVector3XZ(Vector3 v1, Vector3 v2)
{
    if (CompareFloat(v1.x, v2.x) && CompareFloat(v1.z, v2.z))
    {
        return true;
    }
    return false;
}

Vector3 GameMath::ClosestPointOnLineSegment(const Vector3& v1, const Vector3& v2, const Vector3& point)
{
    Vector3 line = v2 - v1;
    float t = Vector3::Dot(line, point - v1) / Vector3::Dot(line, line);
    t = Saturate(t);

    return v1 + t * line;
}

Vector3 GameMath::WorldToScreen(const Vector3& worldPos, Camera* camera)
{
    Vector3 screenPos;   

 /* Matrix cameraViewMatrix = XMMatrixInverse(nullptr, camera->GetWorldMatrixValue());
    Matrix cameraProjctionMatirx = camera->GetProjectionMatrixInUse();*/

    //Matrix cameraViewMatrix = XMMatrixInverse(nullptr, camera->GetWorldMatrixValue());
    Matrix cameraViewMatrix = camera->GetViewMatrix();
    Matrix cameraProjctionMatirx = camera->GetProjectionMatrixInUse(); 
    //Matrix cameraProjctionMatirx = XMMatrixPerspectiveFovLH(XM_PIDIV4, WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 1000.0f);


    screenPos = XMVector3TransformCoord(worldPos.data, cameraViewMatrix);
    screenPos = XMVector3TransformCoord(screenPos.data, cameraProjctionMatirx);

    //NDC공간 좌표(-1 ~ 1) -> 화면좌표(0 ~ WIN_WIDTH)

    screenPos = (screenPos + 1.0f) * 0.5f;
    screenPos.x *= WIN_WIDTH;
    screenPos.y *= WIN_HEIGHT;

    return screenPos;
}

void GameMath::MatrixToFloatArray(Matrix matrix, float* floatArray)
{
    floatArray[0] = matrix.r[0].m128_f32[0];
    floatArray[1] = matrix.r[0].m128_f32[1];
    floatArray[2] = matrix.r[0].m128_f32[2];
    floatArray[3] = matrix.r[0].m128_f32[3];
    floatArray[4] = matrix.r[1].m128_f32[0];
    floatArray[5] = matrix.r[1].m128_f32[1];
    floatArray[6] = matrix.r[1].m128_f32[2];
    floatArray[7] = matrix.r[1].m128_f32[3];
    floatArray[8] = matrix.r[2].m128_f32[0];
    floatArray[9] = matrix.r[2].m128_f32[1];
    floatArray[10] = matrix.r[2].m128_f32[2];
    floatArray[11] = matrix.r[2].m128_f32[3];
    floatArray[12] = matrix.r[3].m128_f32[0];
    floatArray[13] = matrix.r[3].m128_f32[1];
    floatArray[14] = matrix.r[3].m128_f32[2];
    floatArray[15] = matrix.r[3].m128_f32[3];
}

void GameMath::FloatArrayToMatrix(float* floatArray, Matrix& matrix)
{
    matrix.r[0].m128_f32[0] = floatArray[0];
    matrix.r[0].m128_f32[1] = floatArray[1];
    matrix.r[0].m128_f32[2] = floatArray[2];
    matrix.r[0].m128_f32[3] = floatArray[3];
    matrix.r[1].m128_f32[0] = floatArray[4];
    matrix.r[1].m128_f32[1] = floatArray[5];
    matrix.r[1].m128_f32[2] = floatArray[6];
    matrix.r[1].m128_f32[3] = floatArray[7];
    matrix.r[2].m128_f32[0] = floatArray[8];
    matrix.r[2].m128_f32[1] = floatArray[9];
    matrix.r[2].m128_f32[2] = floatArray[10];
    matrix.r[2].m128_f32[3] = floatArray[11];
    matrix.r[3].m128_f32[0] = floatArray[12];
    matrix.r[3].m128_f32[1] = floatArray[13];
    matrix.r[3].m128_f32[2] = floatArray[14];
    matrix.r[3].m128_f32[3] = floatArray[15];
}

