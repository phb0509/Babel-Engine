#pragma once


class MatrixBuffer : public ConstBuffer
{
private:
	struct Data
	{
		Matrix matrix;
	}data;

public:
	MatrixBuffer() : ConstBuffer(&data, sizeof(Data))
	{
		data.matrix = XMMatrixIdentity();
	}

	void SetMatrix(Matrix value)
	{
		data.matrix = XMMatrixTranspose(value);
	}
};

class ViewBuffer : public ConstBuffer
{
private:
	struct Data
	{
		Matrix matrix;
		Matrix invMatrix;
	}data;

public:
	ViewBuffer() : ConstBuffer(&data, sizeof(Data))
	{
		data.matrix = XMMatrixIdentity();
		data.invMatrix = XMMatrixIdentity();
	}

	void SetMatrix(Matrix value)
	{
		data.matrix = XMMatrixTranspose(value); // ��ġ��ķ� ��ȯ. HLSL������ ���켱�̶� ��ġ��ķ� �ٲ������.
		Matrix temp = XMMatrixInverse(nullptr, value);
		data.invMatrix = XMMatrixTranspose(temp);
	}

	Matrix GetInvMatrix() { return data.invMatrix; }
	Matrix GetMatrix() { return data.matrix; }
};

class ProjectionBuffer : public ConstBuffer
{
private:
	struct Data
	{
		Matrix matrix;
		Matrix invMatrix;
	}data;

public:
	ProjectionBuffer() : ConstBuffer(&data, sizeof(Data))
	{
		data.matrix = XMMatrixIdentity();
		data.invMatrix = XMMatrixIdentity();
	}

	void SetMatrix(Matrix value)
	{
		data.matrix = XMMatrixTranspose(value); // ��ġ��ķ� ��ȯ. HLSL������ ���켱�̶� ��ġ��ķ� �ٲ������.
		Matrix temp = XMMatrixInverse(nullptr, value);
		data.invMatrix = XMMatrixTranspose(temp);
	}

	Matrix GetProjectionMatrix() { return data.matrix; }
	Matrix GetInvProjectionMatrix() { return data.invMatrix; }
};


enum class LightType
{
	DIRECTIONAL,
	POINT,
	SPOT,
	CAPSULE
};

struct LightInfo
{
	Float4 color;

	Float3 direction;
	LightType type;

	Float3 position;
	float range;

	float inner;
	float outer;
	float length;
	int active;

	LightInfo()
	{
		color = Float4(1, 1, 1, 1);

		direction = Float3(0, -1, 1);
		type = LightType::DIRECTIONAL;

		position = Float3(0, 0, 0);
		range = 80.0f;

		inner = 55.0f;
		outer = 65.0f;

		length = 50;
		active = 1;
	}
};

class ShadowMappingLightBuffer : public ConstBuffer
{
private:
	struct Data
	{
		Matrix viewMatrix;
		Matrix projectionMatrix;
		Float3 position;
		float padding;
		
	}data;

public:
	ShadowMappingLightBuffer() : ConstBuffer(&data, sizeof(Data))
	{
		data.viewMatrix = XMMatrixIdentity();
		data.projectionMatrix = XMMatrixIdentity();
		data.position = { 0.0f,0.0f,0.0f };
		data.padding = 0.0f;
	}

	void SetData(Matrix viewMatrix, Matrix projectionMatrix, Vector3 position)
	{
		data.viewMatrix = XMMatrixTranspose(viewMatrix); // ��ġ��ķ� ��ȯ. HLSL������ ���켱�̶� ��ġ��ķ� �ٲ������.
		data.projectionMatrix = XMMatrixTranspose(projectionMatrix); // ��ġ��ķ� ��ȯ. HLSL������ ���켱�̶� ��ġ��ķ� �ٲ������.
		data.position = position;
	}
};

class RayBuffer : public ConstBuffer
{
public:
	struct Data
	{
		Float3 position;
		float size;

		Float3 direction;
		float padding;
	}data;

	RayBuffer() : ConstBuffer(&data, sizeof(Data))
	{
		data.position = Float3(0, 0, 0);
		data.size = 0.0f;
		data.direction = Float3(0, 0, 0);
	}
};

class MouseUVBuffer : public ConstBuffer
{
public:
	struct Data
	{
		Float2 mouseScreenPosition;
		Float2 mouseNDCPosition;

		Matrix invViewMatrix;
		Matrix invProjectionMatrix;
	}data;

	MouseUVBuffer() : ConstBuffer(&data, sizeof(Data))
	{
		data.mouseScreenPosition = { 0.0f,0.0f };
		data.mouseNDCPosition = { 0.0f,0.0f };
	}
};


class ColorPickingInputBuffer : public ConstBuffer
{
public:
	struct Data
	{
		Float2 mouseScreenUVPosition;
		Int2 mouseScreenPosition;
	}data;

	ColorPickingInputBuffer() : ConstBuffer(&data, sizeof(Data))
	{
		data.mouseScreenUVPosition = { 0.0f,0.0f };
		data.mouseScreenPosition = { 0,0 };
	}
};

struct ColorPickingOutputBuffer
{
	Float4 color;
};

class BoneBuffer : public ConstBuffer
{
public:
	struct Data
	{
		Matrix bones[MAX_BONE];		
	}data;

	BoneBuffer() : ConstBuffer(&data, sizeof(Data))
	{
		for (UINT i = 0; i < MAX_BONE; i++)
			data.bones[i] = XMMatrixIdentity();		
	}

	void Add(Matrix matrix, UINT index)
	{
		data.bones[index] = XMMatrixTranspose(matrix);		
	}
};


class PlayerFrameBuffer : public ConstBuffer
{
public:
	struct KeyFrameDesc
	{
		int clip = 0;
		//UINT curFrame = 0;
		//UINT nextFrame = 0;
		int curFrame = 0;
		int nextFrame = 0;
		float time = 0.0f;

		float runningTime = 0.0f;
		float speed = 1.0f;
		float padding[2];
	};

	struct TweenDesc // ���絿��->������������ lerp�ؼ� �ε巴�� �Ѱ��ֱ� ���� ����ü.
	{
		float takeTime; // �� �ɸ��� �ð�
		float tweenTime;
		float runningTime;
		float padding;

		KeyFrameDesc cur; // ���� ����
		KeyFrameDesc next; // ���� ����

		TweenDesc() : takeTime(0.5f), tweenTime(0.5f), runningTime(0.0f)
		{
			cur.clip = 0;
			next.clip = -1;
		}
	};

	struct Data
	{
		TweenDesc tweenDesc[1];
	}data;

	PlayerFrameBuffer() : ConstBuffer(&data, sizeof(Data))
	{
	}
};


class FrameBuffer : public ConstBuffer
{
public:
	struct KeyFrameDesc
	{
		int clip = 0;
		int curFrame = 0;
		int nextFrame = 0;
		float time = 0.0f;

		float runningTime = 0.0f;
		float speed = 1.0f;
		float padding[2];
	};

	struct TweenDesc // ���絿��->������������ lerp�ؼ� �ε巴�� �Ѱ��ֱ� ���� ����ü.
	{
		float takeTime; // �� �ɸ��� �ð�
		float tweenTime;
		float runningTime;
		float padding;

		KeyFrameDesc cur; // ���� ����
		KeyFrameDesc next; // ���� ����

		TweenDesc() : takeTime(0.5f), tweenTime(0.5f), runningTime(0.0f)
		{
			cur.clip = 0;
			next.clip = -1;
		}
	};

	struct Data
	{		
		//TweenDesc tweenDesc[MAX_INSTANCE];
		//TweenDesc tweenDesc[100];
		vector<TweenDesc> tweenDesc;
	}data;

	FrameBuffer(int instanceCount) : ConstBuffer(&data, sizeof(Data))
	{
	/*	data.tweenDesc.resize(instanceCount);
		ConstBuffer::ReAllocData(data.tweenDesc.data(), sizeof(TweenDesc) * instanceCount);*/

		data.tweenDesc.resize(MAX_INSTANCE); // �ϴ� �ִ밪���� �س��°�. ���߿� ���̴����� �����ؼ� �������̰� �Ϸ��� �˾Ƽ�..
		ConstBuffer::ReAllocData(data.tweenDesc.data(), sizeof(TweenDesc) * MAX_INSTANCE);
	}
};

class TypeBuffer : public ConstBuffer
{
public:
	struct Data
	{
		int values[4];

		Data() : values{}
		{}
	}data;

	TypeBuffer() : ConstBuffer(&data, sizeof(Data))
	{		
	}
};

class LODTerrainBuffer : public ConstBuffer
{
public:
	struct Data
	{
		Float2 distance = Float2(1, 1000); // �ִ밪 = �����
		Float2 factor = Float2(1, 64); // �ɰ��� ������ �߽�����(in��¼��) 64�̻����ķ� ���ɰ���.

		float cellSpacing = 5.0f; // �� ����
		float cellSpacingU; // �ʸ���鼭 ������.
		float cellSpacingV;
		float heightScale = 20.0f;

		Float4 cullings[6];
	}data;

	LODTerrainBuffer() : ConstBuffer(&data, sizeof(Data))
	{
	}
};

class ColorBuffer : public ConstBuffer
{
public:
	struct Data
	{
		Float4 color = { 1.0f,1.0f,1.0f,1.0f };
	}data;

	ColorBuffer() : ConstBuffer(&data, sizeof(Data))
	{
	}

	void SetMatrix(Float4 color)
	{
		data.color = color;
	}
};

class SizeBuffer : public ConstBuffer
{
public:
	struct Data
	{
		Float2 size = Float2(0, 0);

		float padding[2];
	}data;

	SizeBuffer() : ConstBuffer(&data, sizeof(Data))
	{
	}
};

class TimeBuffer : public ConstBuffer
{
public:
	struct Data
	{
		float time = 0.0f;
		float padding[3];
	}data;

	TimeBuffer() : ConstBuffer(&data, sizeof(Data))
	{
	}
};

class EffectBuffer : public ConstBuffer
{
public:
	struct Data
	{
		Float4 minColor;
		Float4 maxColor;

		Float3 gravity;
		float endVelocity;

		Float2 startSize;
		Float2 endSize;

		Float2 rotateSpeed;
		float readyTime;
		float readyRandomTime;

		float curTime;
		float padding[3];
	}data;

	EffectBuffer() : ConstBuffer(&data, sizeof(Data))
	{
	}
};

class TextureBuffer : public ConstBuffer
{
public:
	struct Data
	{
		float widthRatio;
		float heightRatio;
		Float2 padding;
	}data;

	TextureBuffer() : ConstBuffer(&data, sizeof(Data))
	{
		data.widthRatio = 1.0f;
		data.heightRatio = 1.0f;
		float padding[2] = { 0.0f,0.0f };
	}
};