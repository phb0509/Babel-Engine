#include "Framework.h"

ModelAnimator::ModelAnimator(string file)
	: ModelReader(file), texture(nullptr), srv(nullptr)
{
	frameBuffer = new FrameBuffer();
	typeBuffer->data.values[0] = 2;
}

ModelAnimator::~ModelAnimator()
{
	delete frameBuffer;
	delete[] clipTransform;
	delete[] nodeTransform;

	if (texture != nullptr)
	{
		texture->Release();
	}
	if (srv != nullptr)
	{
		srv->Release();
	}
	

	for (ModelClip* clip : clips)
		delete clip;
}

void ModelAnimator::ReadClip(string file)
{
	file = "ModelData/Clips/" + file;

	BinaryReader* r = new BinaryReader(file);

	ModelClip* clip = new ModelClip();

	clip->name = r->String();
	clip->duration = r->Float();
	clip->tickPerSecond = r->Float();
	clip->frameCount = r->UInt();

	UINT keyFrameCount = r->UInt();
	for (UINT i = 0; i < keyFrameCount; i++)
	{
		KeyFrame* keyFrame = new KeyFrame();
		keyFrame->boneName = r->String();

		UINT size = r->UInt();
		if (size > 0)
		{
			keyFrame->transforms.resize(size);

			void* ptr = (void*)keyFrame->transforms.data();
			r->Byte(&ptr, sizeof(KeyTransform) * size);
		}
		clip->keyFrames[keyFrame->boneName] = keyFrame;
	}
	clips.emplace_back(clip);

	delete r;
}

void ModelAnimator::Update()
{
	FrameBuffer::TweenDesc& tweenDesc = frameBuffer->data.tweenDesc[0];

	{//CurAnimation
		FrameBuffer::KeyFrameDesc& desc = tweenDesc.cur;
		ModelClip* clip = clips[desc.clip];
		
		float time = 1.0f / clip->tickPerSecond / desc.speed;
		desc.runningTime += DELTA;

		if (desc.time >= 1.0f)
		{
			if (desc.curFrame + desc.time >= clip->frameCount)
			{
				if (EndEvent.count(desc.clip) > 0)
					EndEvent[desc.clip]();
				if (EndParamEvent.count(desc.clip) > 0)
					EndParamEvent[desc.clip](param[desc.clip]);
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


void ModelAnimator::Render()
{
	if (texture == nullptr)
		CreateTexture();

	frameBuffer->SetVSBuffer(4);
	DEVICECONTEXT->VSSetShaderResources(0, 1, &srv);

	MeshRender();
}

void ModelAnimator::PostRender()
{	
}

void ModelAnimator::PlayClip(UINT clip, float speed, float takeTime)
{
	frameBuffer->data.tweenDesc[0].takeTime = takeTime;
	frameBuffer->data.tweenDesc[0].next.clip = clip;
	frameBuffer->data.tweenDesc[0].next.speed = speed;
}

Matrix ModelAnimator::GetTransformByNode(int nodeIndex)
{
	if (nodeTransform == nullptr)
		return XMMatrixIdentity();

	FrameBuffer::KeyFrameDesc& curDesc = frameBuffer->data.tweenDesc[0].cur;

	Matrix cur = nodeTransform[curDesc.clip].transform[curDesc.curFrame][nodeIndex];
	Matrix next = nodeTransform[curDesc.clip].transform[curDesc.nextFrame][nodeIndex];

	Matrix curClip = LERP(cur, next, curDesc.time);

	FrameBuffer::KeyFrameDesc& nextDesc = frameBuffer->data.tweenDesc[0].next;

	if (nextDesc.clip == -1)
		return curClip;

	cur = nodeTransform[nextDesc.clip].transform[nextDesc.curFrame][nodeIndex];
	next = nodeTransform[nextDesc.clip].transform[nextDesc.nextFrame][nodeIndex];

	Matrix nextClip = LERP(cur, next, nextDesc.time);

	return LERP(curClip, nextClip, frameBuffer->data.tweenDesc[0].tweenTime);
}

Matrix ModelAnimator::GetTransformByNode(UINT instance, int nodeIndex)
{
	if (nodeTransform == nullptr)
		return XMMatrixIdentity();

	FrameBuffer::KeyFrameDesc& curDesc = frameBuffer->data.tweenDesc[instance].cur;

	Matrix cur = nodeTransform[curDesc.clip].transform[curDesc.curFrame][nodeIndex];
	Matrix next = nodeTransform[curDesc.clip].transform[curDesc.nextFrame][nodeIndex];

	Matrix curClip = LERP(cur, next, curDesc.time);

	FrameBuffer::KeyFrameDesc& nextDesc = frameBuffer->data.tweenDesc[instance].next;

	if (nextDesc.clip == -1)
		return curClip;

	cur = nodeTransform[nextDesc.clip].transform[nextDesc.curFrame][nodeIndex];
	next = nodeTransform[nextDesc.clip].transform[nextDesc.nextFrame][nodeIndex];

	Matrix nextClip = LERP(cur, next, nextDesc.time);

	return LERP(curClip, nextClip, frameBuffer->data.tweenDesc[instance].tweenTime);
}

void ModelAnimator::CreateTexture()
{
	UINT clipCount = clips.size();

	clipTransform = new ClipTransform[clipCount];
	nodeTransform = new ClipTransform[clipCount];
	for (UINT i = 0; i < clipCount; i++)
		CreateClipTransform(i);

	{//CreatTexture
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = MAX_BONE * 4;
		desc.Height = MAX_FRAME_KEY;
		desc.ArraySize = clipCount;
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;

		UINT pageSize = MAX_BONE * sizeof(Matrix) * MAX_FRAME_KEY;

		void* p = VirtualAlloc(nullptr, pageSize * clipCount, MEM_RESERVE, PAGE_READWRITE);

		for (UINT c = 0; c < clipCount; c++)
		{
			UINT start = c * pageSize;

			for (UINT y = 0; y < MAX_FRAME_KEY; y++)
			{
				void* temp = (BYTE*)p + MAX_BONE * y * sizeof(Matrix) + start;

				VirtualAlloc(temp, MAX_BONE * sizeof(Matrix), MEM_COMMIT, PAGE_READWRITE);
				memcpy(temp, clipTransform[c].transform[y], MAX_BONE * sizeof(Matrix));
			}
		}

		D3D11_SUBRESOURCE_DATA* subResource = new D3D11_SUBRESOURCE_DATA[clipCount];
		for (UINT c = 0; c < clipCount; c++)
		{
			void* temp = (BYTE*)p + c * pageSize;

			subResource[c].pSysMem = temp;
			subResource[c].SysMemPitch = MAX_BONE * sizeof(Matrix);
			subResource[c].SysMemSlicePitch = pageSize;
		}
		V(DEVICE->CreateTexture2D(&desc, subResource, &texture));

		delete[] subResource;
		VirtualFree(p, 0, MEM_RELEASE);
	}

	{//Create SRV
		D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipLevels = 1;
		desc.Texture2DArray.ArraySize = clipCount;

		V(DEVICE->CreateShaderResourceView(texture, &desc, &srv));
	}
}

void ModelAnimator::CreateClipTransform(UINT index)
{
	ModelClip* clip = clips[index];
	for (UINT f = 0; f < clip->frameCount; f++)
	{
		UINT nodeIndex = 0;
		for (NodeData* node : nodes)
		{
			Matrix animation;
			KeyFrame* frame = clip->GetKeyFrame(node->name);
			if (frame != nullptr)
			{
				KeyTransform& transform = frame->transforms[f];

				Matrix S = XMMatrixScaling(transform.scale.x, transform.scale.y, transform.scale.z);
				Matrix R = XMMatrixRotationQuaternion(XMLoadFloat4(&transform.rotation));
				Matrix T = XMMatrixTranslation(transform.position.x, transform.position.y, transform.position.z);

				animation = S * R * T;
			}
			else
			{
				animation = XMMatrixIdentity();
			}

			Matrix parent;
			int parentIndex = node->parent;

			if (parentIndex < 0)
				parent = XMMatrixIdentity();
			else
				parent = nodeTransform[index].transform[f][parentIndex];

			nodeTransform[index].transform[f][nodeIndex] = animation * parent;

			if (boneMap.count(node->name) > 0)
			{
				int boneIndex = boneMap[node->name];
				Matrix transform = XMLoadFloat4x4(&bones[boneIndex]->offset);
				transform *= nodeTransform[index].transform[f][nodeIndex];
				clipTransform[index].transform[f][boneIndex] = transform;
			}

			nodeIndex++;
		}
	}
}
