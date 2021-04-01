#pragma once

namespace GameMath
{
	float Saturate(const float& value);

	int Random(int min, int max);
	float Random(float min, int max);
	float Distance(const Vector3& v1, const Vector3& v2);
	bool FloatComparison(float v1, float v2);

	Vector3 ClosestPointOnLineSegment(const Vector3& v1, const Vector3& v2, const Vector3& point);

	Vector3 WorldToScreen(const Vector3& worldPos);
}