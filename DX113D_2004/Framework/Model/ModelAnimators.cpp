#include "Framework.h"

ModelAnimators::ModelAnimators() : 
	ModelAnimator(), 
	mCameraForFrustumCulling(nullptr),
	mDrawCount(0),
	mbIsFrustumCullingMode(false)
{
	for (int i = 0; i < MAX_INSTANCE; i++) // 인스턴싱할 월드행렬과 인덱스번호 셋팅.
	{
		InstanceData temp;
		temp.worldMatrix = XMMatrixIdentity();
		temp.instanceIndex = i;
		mInstanceData.push_back(temp);
	}

    mInstanceBuffer = new VertexBuffer(mInstanceData.data(), sizeof(InstanceData), MAX_INSTANCE);
}

ModelAnimators::~ModelAnimators()
{
	GM->SafeDelete(mInstanceBuffer);
}

void ModelAnimators::Update()
{
	if (mCameraForFrustumCulling != nullptr)
	{
		mCameraForFrustumCulling->Update();
	}

	mCompletedAnimInstanceIndices.clear();
	mStartedAnimInstanceIndices.clear();

	for (UINT i = 0; i < mTransforms.size(); i++)
	{
		if (!mTransforms[i]->GetIsActive()) continue;

		FrameBuffer::TweenDesc& tweenDesc = mFrameBuffer->data.tweenDesc[i];

		{//CurAnimation
			FrameBuffer::KeyFrameDesc& desc = tweenDesc.cur;
			ModelClip* clip = mClips[desc.clip];

			float time = 1.0f / clip->mFramePerSecond / desc.speed;
			desc.runningTime += DELTA;

			if (desc.curFrame == 1)
			{
				mStartedAnimInstanceIndices.push_back(i);
			}

			if (desc.time >= 1.0f)
			{
				if (desc.curFrame + desc.time  >= clip->mFrameCount - 1) // 현재 클립재생이 끝나면 
				{
					if (mEndEvents[i].count(desc.clip) > 0)
					{
						mEndEvents[i][desc.clip](mParams[i][desc.clip]);
					}

					if (mEndParamEvent.count(desc.clip) > 0)
					{
						mEndParamEvent[desc.clip](mParam[desc.clip]);
					}

					mCompletedAnimInstanceIndices.push_back(i); // EndEvent진행할 인스턴스들 따로 모아놓기.
				}

				desc.curFrame = (desc.curFrame + 1) % clip->mFrameCount;
				desc.nextFrame = (desc.curFrame + 1) % clip->mFrameCount;
				desc.runningTime = 0.0f;
			}

			desc.time = desc.runningTime / time;
		}

		{//NextAnimation
			FrameBuffer::KeyFrameDesc& desc = tweenDesc.next;

			if (desc.clip > -1)
			{
				ModelClip* clip = mClips[desc.clip];

				tweenDesc.runningTime += DELTA;
				tweenDesc.tweenTime = tweenDesc.runningTime / tweenDesc.takeTime;

				if (tweenDesc.tweenTime >= 1.0f)
				{
					tweenDesc.cur = desc;
					tweenDesc.runningTime = 0.0f;
					tweenDesc.tweenTime = 0.0f;

					desc.runningTime = 0.0f;
					desc.curFrame = 0;
					desc.nextFrame = 0;
					desc.time = 0.0f;
					desc.clip = -1;
				}
				else
				{
					float time = 1.0f / clip->mFramePerSecond / desc.speed;
					desc.runningTime += DELTA;

					if (desc.time >= 1.0f)
					{
						desc.curFrame = (desc.curFrame + 1) % clip->mFrameCount;
						desc.nextFrame = (desc.curFrame + 1) % clip->mFrameCount;
						desc.runningTime = 0.0f;
					}

					desc.time = desc.runningTime / time;
				}
			}
		}
	}	

	UpdateTransforms(); // FrustumCulling and Set InstanceBuffer
}

void ModelAnimators::Render()
{
	if (mTexture == nullptr)
	{
		ModelAnimator::CreateTexture();
	}
		
	mFrameBuffer->SetVSBuffer(4);
	DEVICECONTEXT->VSSetShaderResources(0, 1, &mSRV); // Set TransformTexture

	mInstanceBuffer->SetIA(1);

	MeshRender(mDrawCount);
}

void ModelAnimators::DeferredRender()
{
}

void ModelAnimators::PostRender()
{
	/*for (Transform* transform : mTransforms)
	{
		Vector3 screenPos = WorldToScreen(transform->GetGlobalPosition(),mCameraForFrustumCulling);

		POINT size = { 200, 100 };
		RECT rect;
		rect.left = screenPos.x;
		rect.top = screenPos.y;
		rect.right = rect.left + size.x;
		rect.bottom = rect.top + size.y;

		DirectWrite::Get()->RenderText(ToWString(transform->mTag), rect);
	}*/
}

Transform* ModelAnimators::Add()
{
	Transform* transform = new Transform();
	mTransforms.emplace_back(transform);
	mEndEvents.emplace_back();
	mParams.emplace_back();

	return transform;
}

void ModelAnimators::AddTransform(Transform* transform)
{
	mTransforms.emplace_back(transform);
	mEndEvents.emplace_back();
	mParams.emplace_back();
}

void ModelAnimators::PlayClip(UINT instanceIndex, UINT clip, float speed, float takeTime)
{
	mFrameBuffer->data.tweenDesc[instanceIndex].next.clip = clip;
	mFrameBuffer->data.tweenDesc[instanceIndex].next.speed = speed;
	mFrameBuffer->data.tweenDesc[instanceIndex].takeTime = takeTime;
}

void ModelAnimators::SetInstanceCount(int instanceCount)
{
	//for (int i = 0; i < MAX_INSTANCE; i++) // 인스턴싱할 월드행렬과 인덱스번호 셋팅.
	//{
	//	InstanceData temp;
	//	temp.worldMatrix = XMMatrixIdentity();
	//	temp.instanceIndex = i;
	//	mInstanceData.push_back(temp);
	//}

	//mInstanceBuffer = new VertexBuffer(mInstanceData.data(), sizeof(InstanceData), MAX_INSTANCE);
}

void ModelAnimators::UpdateTransforms() // 컬링 및 인스턴스버퍼 세팅.
{
	mDrawCount = 0;
	mRenderedInstanceIndices.clear();

	if (mbIsFrustumCullingMode) // 프러스텀컬링 기능 활성화일시
	{
		for (UINT i = 0; i < mTransforms.size(); i++)
		{
			if (!mTransforms[i]->GetIsActive()) continue;

			mTransforms[i]->SetIsInFrustum(false);
			Vector3 worldMin = XMVector3TransformCoord(mMinBox.data, *mTransforms[i]->GetWorldMatrix()); // 임의의 컬링용 컬라이더박스 생성 후 세팅.
			Vector3 worldMax = XMVector3TransformCoord(mMaxBox.data, *mTransforms[i]->GetWorldMatrix());

			if (mCameraForFrustumCulling->GetFrustum()->ContainBox(worldMin, worldMax)) // 프러스텀 컬링.
			{
				// 프러스텀범위 안의 인스턴스들.
				mTransforms[i]->SetIsInFrustum(true);
				mTransforms[i]->UpdateWorld();
				mInstanceData[mDrawCount].worldMatrix = XMMatrixTranspose(*mTransforms[i]->GetWorldMatrix());
				mInstanceData[mDrawCount].instanceIndex = i;
				mDrawCount++;

				mRenderedInstanceIndices.push_back(i); // 실제 렌더될 인스턴스의 인덱스값 저장.
			}
		}
	}

	else // 프러스텀컬링 기능 비활성화일시
	{
		for (UINT i = 0; i < mTransforms.size(); i++)
		{
			if (!mTransforms[i]->GetIsActive()) continue;

			mTransforms[i]->SetIsInFrustum(false);
			mTransforms[i]->UpdateWorld();
			mInstanceData[mDrawCount].worldMatrix = XMMatrixTranspose(*mTransforms[i]->GetWorldMatrix());
			mInstanceData[mDrawCount].instanceIndex = i;
			mDrawCount++;

			mRenderedInstanceIndices.push_back(i); // 실제 렌더될 인스턴스의 인덱스값 저장.
		}
	}

	mInstanceBuffer->Update(mInstanceData.data(), mDrawCount);
}


