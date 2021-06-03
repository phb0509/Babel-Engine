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
			Matrix temp = XMMatrixInverse(nullptr, value); 
			data.invMatrix = XMMatrixTranspose(temp);
		}
	};

public:
	Camera();
	~Camera();

	void Update();
	void Render();
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
	Frustum* GetFrustum() { return mFrustum; }

private:
	void setViewToFrustum(Matrix view);




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
	Frustum* mFrustum;



};