#pragma once

class Camera;

namespace GameMath
{
	float Saturate(const float& value);
	int Random(int min, int max);
	float Random(float min, int max);
	float Distance(const Vector3& v1, const Vector3& v2);
	bool CompareFloat(float v1, float v2);
	bool CompareVector3XZ(Vector3 v1, Vector3 v2);

	Vector3 ClosestPointOnLineSegment(const Vector3& v1, const Vector3& v2, const Vector3& point);

	Vector3 WorldToScreen(const Vector3& worldPos, Camera* camera);
	
	void MatrixToFloatArray(Matrix matrix, float* floatArray);
	void FloatArrayToMatrix(float* floatArray, Matrix& matrix);
}