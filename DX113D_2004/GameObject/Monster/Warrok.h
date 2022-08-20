#pragma once

class Warrok : public Monster
{
public:
	friend class Player;

	struct colliderData
	{
		string colliderName;
		string nodeName;
		Vector3 position;
		Vector3 rotation;
		Vector3 scale;
	};

	struct temp_colliderData
	{
		Vector3 position;
		Vector3 rotation;
		Vector3 scale;
	};

	Warrok();
	~Warrok();

	virtual void Update() override;
	virtual void InstanceUpdate() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual Collider* GetHitCollider() override;
	virtual void SetAttackInformation(AttackInformation attackInformation) override;
	virtual bool CheckIsCollision(Collider* collider) override;
	virtual void CheckOnHit() override;
	virtual Collider* GetColliderForAStar() override;
	virtual void ReActivation() override;

	void PostRender();
	void SetIdle();
	void SetAnimation(int animationState, float speed = 1.0f, float takeTime = 0.2f, bool isForcingPlay = false) override;


private:

	void setOnDamageEnd();
	void setColliders();
	void setAttackEnd();
	void loadBinaryFile();
	void findCollider(string name, Collider* collider);



private:

	Collider* mBodyCollider;
	Matrix mBodyMatrix;
	Collider* mSmashAttackCollider;
	Matrix mSmashAttackMatrix;

	vector<colliderData> mColliderDatas;
	vector<temp_colliderData> mTempColliderDatas;

	bool mbOnHit;

	eMutantAnimationStates mAnimation;
	eMutantFSMStates mFSM;
};