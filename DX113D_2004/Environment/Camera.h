#pragma once

class Camera : public Transform
{
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

		void Set(Matrix value)
		{
			data.matrix = XMMatrixTranspose(value);
			Matrix temp = XMMatrixInverse(nullptr, value); // 다른 모든 오브젝트의 worldMatrix에 카메라의 viewMatrix의 역행렬을 곱해줘야한다. 뷰공간으로 전환하기위한 과정.
			data.invMatrix = XMMatrixTranspose(temp);
		}
	};

public:
	Camera();
	~Camera();

	void Update();
	void TargetMove();
	void FollowControl();
	void FreeMode();
	void FreeMove();
	void Rotation();
	void View();

	void PostRender();

	void SetVS(UINT slot = 1);
	Vector3 GetOriginForward() { return originForward; }

	Ray ScreenPointToRay(Vector3 pos);

	void SetTarget(Transform* value) { target = value; }


	Matrix GetView() { return view; }
	ViewBuffer* GetViewBuffer() { return viewBuffer; }


public:
	float moveSpeed;
	float wheelSpeed;
	Vector3 cameraForward;
	POINT pt;

private:
	float rotSpeed;

	ViewBuffer* viewBuffer;
	Matrix view;

	Vector3 oldPos;

	float distance;
	float height;

	Vector3 offset;

	Vector3 destPos;
	float destRot;

	float moveDamping;
	float rotDamping;
	Vector3 originForward;
	float rotY;
	float rotX;

	Matrix mRotMatrixY;
	Matrix mRotMatrixX;

	Transform* target;
	bool mbIsOnFrustumCollider;



};