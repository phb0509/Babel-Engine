#include "Framework.h"

ModelAnimators::ModelAnimators() : 
	ModelAnimator(), 
	mCameraForFrustumCulling(nullptr),
	mDrawCount(0),
	mbIsFrustumCullingMode(false)
{
	for (int i = 0; i < MAX_INSTANCE; i++) // 인스턴싱할 월드행렬과 인덱스번호 셋팅.
	{
		mInstanceData[i].worldMatrix = XMMatrixIdentity();
		mInstanceData[i].instanceIndex = i;
	}

    mInstanceBuffer = new VertexBuffer(mInstanceData, sizeof(InstanceData), MAX_INSTANCE);
}

ModelAnimators::~ModelAnimators()
{
    delete mInstanceBuffer;
}

void ModelAnimators::Update()
{
	if (mCameraForFrustumCulling != nullptr)
	{
		mCameraForFrustumCulling->Update();
	}

	for (UINT i = 0; i < mTransforms.size(); i++)
	{
		FrameBuffer::TweenDesc& tweenDesc = mFrameBuffer->data.tweenDesc[i];

		{//CurAnimation
			FrameBuffer::KeyFrameDesc& desc = tweenDesc.cur;
			ModelClip* clip = mClips[desc.clip];

			float time = 1.0f / clip->mFramePerSecond / desc.speed;
			desc.runningTime += DELTA;

			if (desc.time >= 1.0f)
			{
				if (desc.curFrame + desc.time >= clip->mFrameCount)
				{
					if (mEndEvents[i].count(desc.clip) > 0)
					{
						mEndEvents[i][desc.clip](mParams[i][desc.clip]);
					}
						
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

	UpdateTransforms(); // 컬링 및 인스턴스버퍼 셋팅.
}

void ModelAnimators::Render()
{
	if (mTexture == nullptr)
		CreateTexture();

	mFrameBuffer->SetVSBuffer(4);
	DEVICECONTEXT->VSSetShaderResources(0, 1, &mSRV);

	mInstanceBuffer->IASet(1);

	MeshRender(mDrawCount);
}

void ModelAnimators::PostRender()
{
	for (Transform* transform : mTransforms)
	{
		Vector3 screenPos = WorldToScreen(transform->GetGlobalPosition(),mCameraForFrustumCulling);

		POINT size = { 200, 100 };
		RECT rect;
		rect.left = screenPos.x;
		rect.top = screenPos.y;
		rect.right = rect.left + size.x;
		rect.bottom = rect.top + size.y;

		DirectWrite::Get()->RenderText(ToWString(transform->mTag), rect);
	}
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

void ModelAnimators::PlayClip(UINT instance, UINT clip, float speed, float takeTime)
{
	mFrameBuffer->data.tweenDesc[instance].next.clip = clip;
	mFrameBuffer->data.tweenDesc[instance].next.speed = speed;
	mFrameBuffer->data.tweenDesc[instance].takeTime = takeTime;
}

void ModelAnimators::UpdateTransforms() // 컬링 및 인스턴스버퍼 세팅.
{
	mDrawCount = 0;

	if (mbIsFrustumCullingMode)
	{
		for (UINT i = 0; i < mTransforms.size(); i++)
		{
			Vector3 worldMin = XMVector3TransformCoord(mMinBox.data, *mTransforms[i]->GetWorldMatrix()); // 임의의 컬링용 컬라이더박스 생성 후 세팅.
			Vector3 worldMax = XMVector3TransformCoord(mMaxBox.data, *mTransforms[i]->GetWorldMatrix());

			if (mCameraForFrustumCulling->GetFrustum()->ContainBox(worldMin, worldMax)) // 프러스텀 컬링.
			{
				mTransforms[i]->UpdateWorld();
				mInstanceData[mDrawCount].worldMatrix = XMMatrixTranspose(*mTransforms[i]->GetWorldMatrix());
				mInstanceData[mDrawCount].instanceIndex = i;
				mDrawCount++;
			}
		}
	}
	else
	{
		for (UINT i = 0; i < mTransforms.size(); i++)
		{
			mTransforms[i]->UpdateWorld();
			mInstanceData[mDrawCount].worldMatrix = XMMatrixTranspose(*mTransforms[i]->GetWorldMatrix());
			mInstanceData[mDrawCount].instanceIndex = i;
			mDrawCount++;
		}
	}

	mInstanceBuffer->Update(mInstanceData, mDrawCount);
}


