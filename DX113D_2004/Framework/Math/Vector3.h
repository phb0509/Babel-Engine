#pragma once

class Vector3
{
public:
	Vector4 data;

	Vector3();
	Vector3(Float3 value);
	Vector3(float x, float y, float z);
	Vector3(Vector4 value);

	operator Float3();

	void SetX(float value);
	void SetY(float value);
	void SetZ(float value);
	void SetW(float value);
	float GetX();
	float GetY();
	float GetZ();
	float GetW();

	_declspec(property(get = GetX, put = SetX)) float x;
	_declspec(property(get = GetY, put = SetY)) float y;
	_declspec(property(get = GetZ, put = SetZ)) float z;
	_declspec(property(get = GetW, put = SetW)) float w;

	Vector3 operator+(const Vector3& value) const;
	Vector3 operator-(const Vector3& value) const;
	Vector3 operator*(const Vector3& value) const;
	Vector3 operator/(const Vector3& value) const;

	void operator+=(const Vector3& value);
	void operator-=(const Vector3& value);
	void operator*=(const Vector3& value);
	void operator/=(const Vector3& value);

	Vector3 operator+(const float& value) const;
	Vector3 operator-(const float& value) const;
	Vector3 operator*(const float& value) const;
	Vector3 operator/(const float& value) const;

	void operator+=(const float& value);
	void operator-=(const float& value);
	void operator*=(const float& value);
	void operator/=(const float& value);

	friend Vector3 operator+(const float value1, const Vector3& value2);
	friend Vector3 operator-(const float value1, const Vector3& value2);
	friend Vector3 operator*(const float value1, const Vector3& value2);
	friend Vector3 operator/(const float value1, const Vector3& value2);

	friend Vector3 operator+(const Float3& value1, const Vector3& value2);
	friend Vector3 operator-(const Float3& value1, const Vector3& value2);
	friend Vector3 operator*(const Float3& value1, const Vector3& value2);
	friend Vector3 operator/(const Float3& value1, const Vector3& value2);

	bool operator== (const Vector3& value) const;

	float operator[](const UINT& index) const;

	float Length() const;

	Vector3 Normal() const;
	void Normalize();
	
	static Vector3 Cross(const Vector3& vec1, const Vector3& vec2);
	static float Dot(const Vector3& vec1, const Vector3& vec2);
};