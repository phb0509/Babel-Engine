#pragma once

struct Vertex
{
	Float3 position;

	Vertex() : position(0.0f, 0.0f, 0.0f)
	{}

	Vertex(float x, float y, float z) : position(x, y, z)
	{}
};

struct VertexSize
{
	Float3 position;
	Float2 size;

	VertexSize() : position(0.0f, 0.0f, 0.0f), size(0.0f, 0.0f)
	{}
};

struct VertexColor
{
	Float3 position;
	Float4 colorForPicking;

	VertexColor() : position(0.0f, 0.0f, 0.0f), colorForPicking(1.0f, 1.0f, 1.0f, 1.0f)
	{}

	VertexColor(Float3 _position, Float4 _colorForPicking): position(_position), colorForPicking(_colorForPicking)
	{}

};

struct VertexUV
{
	Float3 position;
	Float2 uv;

	VertexUV() : position(0.0f, 0.0f, 0.0f), uv(0.0f, 0.0f)
	{}
};

struct VertexUVNormal
{
	Float3 position;
	Float2 uv;
	Float3 normal;

	VertexUVNormal() : position(0.0f, 0.0f, 0.0f), uv(0.0f, 0.0f), normal(0.0f, 0.0f, 0.0f)
	{}
};

struct VertexUVNormalTangent
{
	Float3 position;
	Float2 uv;
	Float3 normal;
	Float3 tangent;

	VertexUVNormalTangent() : position(0.0f, 0.0f, 0.0f), uv(0.0f, 0.0f), normal(0.0f, 0.0f, 0.0f), tangent(0.0f, 0.0f, 0.0f)
	{}
};

struct VertexUVNormalTangentAlpha
{
	Float3 position;
	Float2 uv;
	Float3 normal;
	Float3 tangent;
	float alpha[4];

	VertexUVNormalTangentAlpha():
		position(0.0f, 0.0f, 0.0f), uv(0.0f, 0.0f), normal(0.0f, 0.0f, 0.0f), tangent(0.0f, 0.0f, 0.0f), alpha{ 0.0f,0.0f,0.0f,0.0f }
	{}
};

struct VertexUVNormalTangentBlend
{
	Float3 position;
	Float2 uv;
	Float3 normal;
	Float3 tangent;
	Float4 indices;//본 인덱스 값
	Float4 weights;//본에 대한 보간 값

	VertexUVNormalTangentBlend() :
		position(0.0f, 0.0f, 0.0f), uv(0.0f, 0.0f), normal(0.0f, 0.0f, 0.0f),
		tangent(0.0f, 0.0f, 0.0f), indices(0.0f, 0.0f, 0.0f, 0.0f), weights(0.0f, 0.0f, 0.0f, 0.0f)
	{}
};

