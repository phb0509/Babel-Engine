#include "Framework.h"

ModelAnimators::ModelAnimators(string file)
    : ModelAnimator(file), drawCount(0)
{
	for (int i = 0; i < MAX_INSTANCE; i++)
	{
		instanceData[i].world = XMMatrixIdentity();
		instanceData[i].index = i;
	}

    instanceBuffer = new VertexBuffer(instanceData, sizeof(InstanceData), MAX_INSTANCE);
	frustum = new Frustum();

	SetBox(&minBox, &maxBox);
}

ModelAnimators::~ModelAnimators()
{
    delete instanceBuffer;
	delete frustum;
}

void ModelAnimators::Update()
{
	for (UINT i = 0; i < transforms.size(); i++)
	{
		FrameBuffer::TweenDesc& tweenDesc = frameBuffer->data.tweenDesc[i];

		{//CurAnimation
			FrameBuffer::KeyFrameDesc& desc = tweenDesc.cur;
			ModelClip* clip = clips[desc.clip];

			float time = 1.0f / clip->tickPerSecond / desc.speed;
			desc.runningTime += DELTA;

			if (desc.time >= 1.0f)
			{
				if (desc.curFrame + desc.time >= clip->frameCount)
				{
					if (EndEvents[i].count(desc.clip) > 0)
						EndEvents[i][desc.clip](params[i][desc.clip]);
				}

				desc.curFrame = (desc.curFrame + 1) % clip->frameCount;
				desc.nextFrame = (desc.curFrame + 1) % clip->frameCount;
				desc.runningTime = 0.0f;
			}

			desc.time = desc.runningTime / time;
		}

		{//NextAnimation
			FrameBuffer::KeyFrameDesc& desc = tweenDesc.next;

			if (desc.clip > -1)
			{
				ModelClip* clip = clips[desc.clip];

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
					float time = 1.0f / clip->tickPerSecond / desc.speed;
					desc.runningTime += DELTA;

					if (desc.time >= 1.0f)
					{
						desc.curFrame = (desc.curFrame + 1) % clip->frameCount;
						desc.nextFrame = (desc.curFrame + 1) % clip->frameCount;
						desc.runningTime = 0.0f;
					}

					desc.time = desc.runningTime / time;
				}
			}
		}
	}	

	UpdateTransforms();
}

void ModelAnimators::Render()
{
	if (texture == nullptr)
		CreateTexture();

	frameBuffer->SetVSBuffer(4);
	DC->VSSetShaderResources(0, 1, &srv);

	instanceBuffer->IASet(1);

	MeshRender(drawCount);
}

void ModelAnimators::PostRender()
{
	for (Transform* transform : transforms)
	{
		Vector3 screenPos = WorldToScreen(transform->GlobalPos());

		POINT size = { 200, 100 };
		RECT rect;
		rect.left = screenPos.x;
		rect.top = screenPos.y;
		rect.right = rect.left + size.x;
		rect.bottom = rect.top + size.y;

		DirectWrite::Get()->RenderText(ToWString(transform->tag), rect);
	}
}

Transform* ModelAnimators::Add()
{
	Transform* transform = new Transform();
	transforms.emplace_back(transform);

	EndEvents.emplace_back();
	params.emplace_back();

	return transform;
}

void ModelAnimators::PlayClip(UINT instance, UINT clip, float speed, float takeTime)
{
	frameBuffer->data.tweenDesc[instance].takeTime = takeTime;
	frameBuffer->data.tweenDesc[instance].next.clip = clip;
	frameBuffer->data.tweenDesc[instance].next.speed = speed;
}

void ModelAnimators::UpdateTransforms()
{
	frustum->Update();
	drawCount = 0;

	for (UINT i = 0; i < transforms.size(); i++)
	{
		//if (frustum->ContainPoint(transforms[i]->GlobalPos()))
		Vector3 worldMin = XMVector3TransformCoord(minBox.data, *transforms[i]->GetWorld());
		Vector3 worldMax = XMVector3TransformCoord(maxBox.data, *transforms[i]->GetWorld());
		if (frustum->ContainBox(worldMin, worldMax))
		{			
			transforms[i]->UpdateWorld();
			instanceData[drawCount].world = XMMatrixTranspose(*transforms[i]->GetWorld());
			instanceData[drawCount].index = i;
			drawCount++;
		}
	}

	instanceBuffer->Update(instanceData, drawCount);
}
