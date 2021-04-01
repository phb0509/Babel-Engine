#pragma once

class Camera : public Transform
{
private:
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

	Matrix rotMatrix;

	Transform* target;
	Transform* player;

	

public:
	Camera();
	~Camera();

	void Update();

	void FreeMode();
	void FollowMode();

	void FreeMove();
	void FollowMove();

	void PlayerMove();
	void FollowControl();

	void Rotation();
	void View();

	void PostRender();

	void SetVS(UINT slot = 1);
	Vector3 GetOriginForward() { return originForward; }


	Ray ScreenPointToRay(Vector3 pos);

	void SetTarget(Transform* value) { target = value; }
	void SetPlayer(Transform* value) { player = value; }

	Matrix GetView() { return view; }
	ViewBuffer* GetViewBuffer() { return viewBuffer; }

	float moveSpeed;
	float wheelSpeed;
	Vector3 cameraForward;
	POINT pt;


};