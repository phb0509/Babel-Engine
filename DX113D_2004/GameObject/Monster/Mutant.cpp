#include "Framework.h"

Mutant::Mutant()
	: ModelAnimator("Mutant/Mutant"), state(IDLE)
{
	scale = { 0.05f, 0.05f, 0.05f };

	SetShader(L"ModelAnimation");

	ReadClip("Mutant/Idle0.clip");
	ReadClip("Mutant/Run0.clip");
	ReadClip("Mutant/SmashAttack0.clip");
	ReadClip("Mutant/OnDamage0.clip");
	ReadClip("Mutant/Die0.clip");

	SetEndEvent(RUN, bind(&Mutant::SetIdle, this));
	SetEndEvent(ONDAMAGE, bind(&Mutant::SetOnDamageEnd, this));
	PlayClip(0);

	bodyCollider = new BoxCollider();
	smashAttackCollider = new BoxCollider();

	LoadCollider();
}

Mutant::~Mutant()
{
	delete bodyCollider;
	delete smashAttackCollider;
}

void Mutant::Update()
{
	SetColliders();

	player = GM->GetPlayer();
	Move();

	bodyCollider->Update();

	bodyCollider->position;
	bodyCollider->scale;
	bodyCollider->rotation;


	smashAttackCollider->Update();

	smashAttackCollider->position;
	smashAttackCollider->scale;
	smashAttackCollider->rotation;

	CheckOnHit();

	UpdateWorld();
	ModelAnimator::Update();
}

void Mutant::Render()
{
	bodyCollider->Render();
	smashAttackCollider->Render();

	SetWorldBuffer();
	ModelAnimator::Render();
}

void Mutant::PostRender()
{
	//ImGui::SliderFloat3("Pos", (float*)&weapon->position, -30, 30);
	//ImGui::SliderFloat3("Rot", (float*)&weapon->rotation, 0, XM_2PI);
	//ImGui::SliderFloat3("Scale", (float*)&weapon->scale, -30, 30);
}









void Mutant::OnDamage(float damage)
{
	isOnHit = true;
	GM->SetHitCheckMap(this, true);
	currentHP -= 10.0f;

}

void Mutant::CheckOnHit()
{
	if (!isOnHit) return;
	
	SetAnimation(ONDAMAGE);
	

}



void Mutant::Move()
{
	position.y = terrain->GetHeight(position);

	
}

void Mutant::SetIdle()
{
	SetAnimation(IDLE);
}

void Mutant::SetAnimation(State value)
{
	if (state != value)
	{
		state = value;
		PlayClip(state);
	}
}

void Mutant::SetOnDamageEnd()
{
	SetAnimation(IDLE);
	GM->SetHitCheckMap(this, false);
	isOnHit = false;
}


void Mutant::SetColliders()
{
	int bodyIndex = GetNodeByName("Mutant:Spine");
	bodyMatrix = GetTransformByNode(bodyIndex) * world;
	bodyCollider->SetParent(&bodyMatrix);


	int smashAttackIndex = GetNodeByName("Mutant:LeftHand");
	smashAttackMatrix = GetTransformByNode(smashAttackIndex) * world;
	smashAttackCollider->SetParent(&smashAttackMatrix);
}

void Mutant::LoadCollider()
{
	BinaryReader colliderReader(L"TextData/Mutant.map");
	UINT colliderSize = colliderReader.UInt();

	temp_colliderDatas.resize(colliderSize);
	colliderDatas.resize(colliderSize);


	void* ptr1 = (void*)temp_colliderDatas.data();

	for (int i = 0; i < colliderSize; i++)
	{
		colliderDatas[i].name = colliderReader.String();
	}


	colliderReader.Byte(&ptr1, sizeof(temp_colliderData) * colliderSize);

	for (int i = 0; i < colliderSize; i++)
	{
		colliderDatas[i].position = temp_colliderDatas[i].position;
		colliderDatas[i].rotation = temp_colliderDatas[i].rotation;
		colliderDatas[i].scale = temp_colliderDatas[i].scale;
	}

	FindCollider("smashAttackCollider", smashAttackCollider);
	FindCollider("BodyCollider", bodyCollider);
}

void Mutant::FindCollider(string name, Collider* collider)
{
	for (int i = 0; i < colliderDatas.size(); i++)
	{
		if (colliderDatas[i].name == name)
		{
			collider->position = colliderDatas[i].position;
			collider->rotation = colliderDatas[i].rotation;
			collider->scale = colliderDatas[i].scale;
		}
	}
}



Collider* Mutant::GetHitCollider()
{
	return bodyCollider;
}