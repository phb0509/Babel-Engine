#pragma once


class Player;
class MonsterState;

class Monster : public Transform , public ModelAnimator
{
public:
	Monster();
	~Monster();

	virtual void Update() = 0;
	virtual void InstanceUpdate() = 0;
	virtual void PreRender() = 0;
	virtual void Render() = 0;
	virtual Collider* GetHitCollider() = 0;
	virtual bool CheckIsCollision(Collider* collider) = 0;
	virtual void OnDamage(AttackInformation attackInformation) = 0;
	virtual void CheckOnHit() = 0;
	virtual Collider* GetColliderForAStar() = 0;
	virtual MonsterState* GetFSMState(int num) = 0;
	virtual int GetEnumFSMState() = 0;
	virtual void SetFSMState(int state) = 0;
	virtual int GetAnimationState() = 0;
	virtual void SetAnimation(int animationState, float speed = 1.0f, float takeTime = 0.2f, bool isForcingPlay = false) = 0;
	virtual void ReActivation() = 0;
	
	virtual void UIUpdate();
	virtual void UIRender();

	void MoveToDestUsingAStar(Vector3 dest);
	void ChangeState(MonsterState* nextState);

	Terrain* GetTerrain() const { return mTerrain; }
	vector<Vector3>& GetPath() { return mPath; }
	float GetMoveSpeed() const { return mMoveSpeed; }
	float GetDistanceToPlayer();
	float GetDistanceToPlayerForAttack() const { return mDistanceToPlayerForAttack; }
	float GetPlayerDetectRange() const { return mPlayerDetectRange; }
	AStar* GetAStar() { return mAStar; }
	MonsterState* GetCurrentState() { return mCurrentFSMState; }
	int GetCurrentClip() { return mFrameBuffer->data.tweenDesc[0].cur.clip; }
	int GetNextClip() { return mFrameBuffer->data.tweenDesc[0].next.clip; }
	InstanceColliderData GetInstanceColliderData() { return mInstanceColliderData; }
	bool GetIsCompletedAnim() { return mbIsCompletedAnim; }
	bool GetIsStartedAnim() { return mbIsStartedAnim; }
	float GetMaxHP() { return mMaxHP; }
	float GetCurHP() { return mCurHP; }
	bool GetIsDie() { return mbIsDie; }
	Texture* GetPortraitTexture() { return mPortraitTexture; }
	
	void SetTerrain(Terrain* value, bool hasTerrainObstacles);
	void SetAStar(AStar* value) { mAStar = value; }
	void SetDistanceToPlayerForAttack(float value) { mDistanceToPlayerForAttack = value; }
	void SetRealtimeAStarPath(Vector3 destPos);
	void SetAStarPath(Vector3 destPos);
	void SetInstanceIndex(int index) { mInstanceIndex = index; }
	void SetUpperFrameBuffer(FrameBuffer* frameBuffer) { mUpperFrameBuffer = frameBuffer; }
	void SetInstanceCollider(InstanceColliderData instanceColliderData) { mInstanceColliderData = instanceColliderData; }
	void SetIsCompletedAnim(bool value) { mbIsCompletedAnim = value; }
	void SetIsStartedAnim(bool value) { mbIsStartedAnim = value; }
	void SetPortraitTexture(Texture* texture);
	void SetCurMainCamera(Camera* mainCamera);
	void SetUIOffset(Vector3& offset) { mUIOffset = offset; }
	
private:
	void setObstaclesTerrain(Vector3 destPos);
	void setNoneObstaclesTerrain(Vector3 destPos);

protected:
	float mDamage;
	float mMaxHP;
	float mCurHP;

	Camera* mCurMainCamera;
	Terrain* mTerrain;
	Player* mPlayer;
	AStar* mAStar;
	Vector3 mDestPos;
	vector<Vector3> mPath;

	Vector3 mDistanceVector3ToPlayer; // 플레이어까지 거리 x,z만 반영.
	float mDistanceToPlayer;
	float mPlayerDetectRange;
	float mDistanceToPlayerForAttack;
	float mAStarPathUpdatePeriodTime;

	MonsterState* mCurrentFSMState;
	int mCurrentEnumFSMState;
	ModelAnimator* mModelAnimator;

	FrameBuffer* mUpperFrameBuffer;
	int mInstanceIndex;
	InstanceColliderData mInstanceColliderData;
	bool mbIsCompletedAnim;
	bool mbIsStartedAnim;
	bool mbIsDie;

	MonsterStatusBar* mStatusBar;

private:
	function<void(Vector3)> mPathUpdatePeriodFuncPointer;
	function<void(Transform*, Vector3)> mRotationPeriodFuncPointer;

	Vector3 mTargetNodeDirVector3;
	Vector3 mTargetNode;

	Vector3 mBeforeDirVector3;
	Vector3 mBeforeNode;

	vector<bool> mPathNodesCheck;
	int mPathNodesCheckSize;

	Vector3 mBeforeTargetPosition;
	Vector3 mCurrentTargetPosition;

	bool mbPathSizeCheck;
	bool mbHasTerrainObstacles;

	Texture* mPortraitTexture;
	Vector3 mUIOffset;
};