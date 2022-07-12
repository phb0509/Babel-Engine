#include "Framework.h"

MutantPatrolState::MutantPatrolState() :
	mPatrolRandomNum(0.0f),
	mbIsPatrolMove(false),
	mbIsSetPatrolDest(true),
	mCurrentTime(0.0f),
	mStandTime(0.0f),
	mPatrolDestPos(0.0f, 0.0f, 0.0f)
{
}

MutantPatrolState::~MutantPatrolState()
{
}

void MutantPatrolState::Initialize()
{
	mPatrolRandomNum = 0.0f;
	mbIsPatrolMove = false;
	mbIsSetPatrolDest = true;
	mCurrentTime = 0.0f;
	mStandTime = 0.0f;
	mPatrolDestPos = { 0.0f,0.0f,0.0f };
}

void MutantPatrolState::SetPatrolTargetPoint(Vector3& patrolTargetPoint)
{

}

void MutantPatrolState::Enter(Monster* monster)
{
	monster->SetFSMState(static_cast<int>(eMutantFSMStates::Patrol));
	Initialize();
}

void MutantPatrolState::Execute(Monster* monster)
{
	mPlayer = GM->Get()->GetPlayer();
	mMaxPatrolWidth = monster->GetTerrain()->GetSize().x - 1;
	mMaxPatrolHeight = monster->GetTerrain()->GetSize().y - 1;

	if (!mbIsPatrolMove) // ���� ���� ���� üũ. 
	{
		if (Timer::Get()->GetRunTime() >= mStandTime)
		{
			mbIsSetPatrolDest = true; // ������ǥ��ǥ������ �����ؾ��Ѵٴ� ǥ��.
		}
	}

	if (mbIsSetPatrolDest && !mbIsPatrolMove) // IDLE���� ���� �ð� ������, �������. ��ǥ���� �����ϰ� ���ʹ������� ȸ��. PatrolState���� �� ó�� ����.
	{
		// ������ǥ��ǥ ����.
		float patrolDestPosCorrectionValue = 50.0f; // ����������
		mPatrolDestPos.x = Random(monster->mPosition.x - patrolDestPosCorrectionValue, monster->mPosition.x + patrolDestPosCorrectionValue);
		mPatrolDestPos.z = Random(monster->mPosition.z - patrolDestPosCorrectionValue, monster->mPosition.z + patrolDestPosCorrectionValue);

		mbIsSetPatrolDest = false; // ������ǥ��ǥ���� �������� false.
		mbIsPatrolMove = true; // ���� �̵��ؾ��ϴ� true.

		// ������ǥ������ ��ũ�⸦ ��� ��� ����ó��.
		float patrolRangeCorrectionValue = 30.0f;

		if (mPatrolDestPos.x >= mMaxPatrolWidth)
		{
			mPatrolDestPos.x = mMaxPatrolWidth - patrolRangeCorrectionValue;
		}
		else if (mPatrolDestPos.x <= 0.0f)
		{
			mPatrolDestPos.x = patrolRangeCorrectionValue;
		}

		if (mPatrolDestPos.z >= mMaxPatrolHeight)
		{
			mPatrolDestPos.z = mMaxPatrolHeight - patrolRangeCorrectionValue;
		}
		else if (mPatrolDestPos.z <= 0.0f)
		{
			mPatrolDestPos.z = patrolRangeCorrectionValue;
		}

		//mbIsSetPatrolDest = false;
		//mbIsPatrolMove = true;

		mPatrolDestPos = monster->GetAStar()->FindCloseNodePosition(mPatrolDestPos); // ��ǥ��ǥ���� ���� ����� �����ġ ����.�׷��� ����??

		//monster->GetPath().clear(); // SetAStarPath���� ��ó���� ��������. 
		monster->SetAStarPath(mPatrolDestPos);
		monster->RotateToDestinationForModel(monster, monster->GetPath().back());
	}

	else if (!mbIsSetPatrolDest && mbIsPatrolMove) // �����̵�.
	{
		if (Timer::Get()->GetFPS() > 10) // �ʱ����� FPS���� ���������� 1�� �Ǵ°Ϳ� ���� ���׷� ���� ����ó��. 
		{
			//mMonster->MoveToDestUsingAStar(mPatrolDestPos);

			float length = (monster->GetPath().back() - monster->mPosition).Length();

			if (length < 1.0f) // ��忡 �����ϸ�
			{
				monster->GetPath().pop_back();

				if (monster->GetPath().size() > 0)
				{
					monster->RotateToDestinationForModel(monster, monster->GetPath().back());
				}
			}

			if (monster->GetPath().size() > 0)
			{
				monster->MoveToDestination(monster, monster->GetPath().back(), monster->GetMoveSpeed());
			}

			monster->SetAnimation(static_cast<int>(eMutantAnimationStates::Run)); // Run.
		}

		if (fabs(mPatrolDestPos.x - monster->mPosition.x) < 1.0f && // ��ǥ���� �����ϸ�
			fabs(mPatrolDestPos.z - monster->mPosition.z) < 1.0f)
		{
			mbIsPatrolMove = false;
			mPatrolRandomNum = GameMath::Random(5.0f, 10.0f); // ���� ����.
			mCurrentTime = TIME;
			mStandTime = mCurrentTime + mPatrolRandomNum;

			monster->SetAnimation(static_cast<int>(eMutantAnimationStates::Idle));
		}
	}

	//���� �� �÷��̾� �ִ��� üũ.
	if (monster->GetDistanceToPlayer() <= monster->GetPlayerDetectRange()) // �÷��̾ �Ÿ� �ȿ� ������.
	{
		monster->ChangeState(monster->GetFSMState(static_cast<int>(eMutantFSMStates::Stalk)));
	}
}

void MutantPatrolState::Exit(Monster* mMonster)
{
}
