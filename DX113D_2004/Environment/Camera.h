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

	
	void PostRender();

	Ray ScreenPointToRay(Vector3 pos);

	void SetVS(UINT slot = 1);
	void SetTarget(Transform* value) { target = value; }
	void SetIsMouseInputing(bool cameraInput) { mbIsMouseInputing = cameraInput; }
	void SetViewMatrixToBuffer();

	Vector3 GetOriginForward() { return originForward; }
	Matrix GetView() { return mViewMatrix; }
	ViewBuffer* GetViewBuffer() { return mViewBuffer; }
	Frustum* GetFrustum() { return mFrustum; }

private:
	void targetMove();
	void targetMoveInWorldCamera();
	void followControl();
	void freeMode();
	void freeMove();
	void rotation();
	void initialize();

	void setViewToFrustum(Matrix view);
	



public:
	float moveSpeed;
	float wheelSpeed;
	Vector3 cameraForward;
	POINT pt;

private:
	float mRotationSpeed;

	ViewBuffer* mViewBuffer;
	Matrix mViewMatrix;

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
	bool mbIsMouseInputing;
	bool mbHasInitalized;



};