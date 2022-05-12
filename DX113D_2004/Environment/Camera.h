#pragma once



class Camera : public Transform
{
public:
	Camera();
	~Camera();

	void Update();
	void Render();
	void RenderFrustumCollider();
	void PostRender();

	Ray ScreenPointToRay(Vector3 pos);
	void Move();

	Matrix GetViewMatrix() { return mViewMatrix; }
	Matrix GetProjectionMatrixInUse();
	ProjectionBuffer* GetProjectionBufferInUse();
	ViewBuffer* GetViewBuffer() { return mViewBuffer; }
	Frustum* GetFrustum() { return mFrustum; }
	bool GetIsUsingFrustumCulling() { return mbIsUsingFrustumCulling; }
	bool GetIsRenderFrustumCollider() { return mbIsRenderFrustumCollider; }
	ProjectionBuffer* GetPerspectiveProjectionBuffer() { return mPerspectiveProjectionBuffer; }
	ProjectionBuffer* GetOrthographicProjectionBuffer() { return mOrthographicProjectionBuffer; }
	float GetFoV() { return mFoV; }
	float GetAspectRatio() { return mAspectRatio; }
	float GetDistanceToNearZ() { return mDistanceToNearZ; }
	float GetDistanceToFarZ() { return mDistanceToFarZ; }

	void SetViewBufferToVS(UINT slot = 1);
	void SetViewBufferToPS(UINT slot = 1);
	void SetViewMatrix(Matrix matrix) { mViewMatrix = matrix; }
	void SetViewToFrustum(Matrix view);
	void SetViewMatrixToBuffer();
	void SetProjectionBufferToVS(UINT slot = 2);
	void SetProjectionBufferToPS(UINT slot = 2);
	void SetIsUsingFrustumCulling(bool value) { mbIsUsingFrustumCulling = value; }
	void SetIsRenderFrustumCollider(bool value) { mbIsRenderFrustumCollider = value; }
	void SetProjectionOption(float FoV, float aspectRatio, float distanceToNearZ, float distanceToFarZ);
	void SetFoV(float fov);
	void SetAspectRatio(float aspectRatio);
	void SetDistanceToNearZ(float distanceToNearZ);
	void SetDistanceToFarZ(float distanceToFarZ);

private:
	void initialize();
	void createPerspectiveProjectionBuffer(float fov, float aspectRatio, float distanceToNearZ, float distanceToFarZ);
	void createOrthographicProjectionBuffer();

public:
	float mMoveSpeed;
	float mWheelSpeed;
	float mRotationSpeed;
	Vector3 mCameraForward;
	POINT mPoint;

private:
	ViewBuffer* mViewBuffer;
	Matrix mViewMatrix;

	Matrix mPerspectiveProjectionMatrix;
	Matrix mOrthographicProjectionMatrix;

	ProjectionBuffer* mPerspectiveProjectionBuffer;
	ProjectionBuffer* mOrthographicProjectionBuffer;

	float mDistanceToFarZ;
	float mDistanceToNearZ;
	float mAspectRatio;
	float mFoV;

	Frustum* mFrustum;
	bool mbIsInitialized;
	bool mbIsPerspectiveProjection;
	bool mbIsUsingFrustumCulling;
	bool mbIsRenderFrustumCollider;

	Vector3 mPreFrameMousePosition;
};