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

Vector3 GameMath::WorldToScreen(const Vector3& worldPos)
{
    Vector3 screenPos;

    screenPos = XMVector3TransformCoord(worldPos.data, TARGETCAMERA->GetViewMatrix());
    screenPos = XMVector3TransformCoord(screenPos.data, Environment::Get()->GetProjection());
    //NDC공간 좌표(-1 ~ 1) -> 화면좌표(0 ~ WIN_WIDTH)

    screenPos.y *= -1;

    screenPos = (screenPos + 1.0f) * 0.5f;

    screenPos.x *= WIN_WIDTH;
    screenPos.y *= WIN_HEIGHT;

    return screenPos;
}
