#include "Framework.h"

ModelAnimator::ModelAnimator(string file): 
	ModelReader(file), 
	mTexture(nullptr), 
	mSRV(nullptr),
	mClipTransform(nullptr),
	mNodeTransform(nullptr)
{
	mFrameBuffer = new FrameBuffer();
	typeBuffer->data.values[0] = 2;
}

ModelAnimator::~ModelAnimator()
{
	delete mFrameBuffer;
	delete[] mClipTransform;
	delete[] mNodeTransform;

	if (mTexture != nullptr)
	{
		mTexture->Release();
	}
	if (mSRV != nullptr)
	{
		mSRV->Release();
	}
	

	for (ModelClip* clip : mClips)
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

	mClips.emplace_back(clip);

	delete r;
}

void ModelAnimator::ReadTPoseClip()
{
	string file = "ModelData/TPose0.clip";

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
	mClips.emplace_back(clip);

	delete r;
}

void ModelAnimator::Update()
{
	FrameBuffer::TweenDesc& tweenDesc = mFrameBuffer->data.tweenDesc[0];

	{ // ���� �ִϸ��̼�.
		FrameBuffer::KeyFrameDesc& desc = tweenDesc.cur;
		ModelClip* clip = mClips[desc.clip];
		
		float time = 1.0f / clip->tickPerSecond / desc.speed;
		desc.runningTime += DELTA;

		if (desc.time >= 1.0f)
		{
			if (desc.curFrame + desc.time >= clip->frameCount)
			{
				if (mEndEvent.count(desc.clip) > 0)
					mEndEvent[desc.clip]();
				if (mEndParamEvent.count(desc.clip) > 0)
					mEndParamEvent[desc.clip](mParam[desc.clip]);
			}

			desc.curFrame = (desc.curFrame + 1) % clip->frameCount; // ���� ������
			desc.nextFrame = (desc.curFrame + 1) % clip->frameCount; // ���� ������.
			desc.runningTime = 0.0f;
		}

		desc.time = desc.runningTime / time;
	}

	{ // ���� �ִϸ��̼�.
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
	if (mTexture == nullptr)
		CreateTexture(); // TransformMapTexture.

	mFrameBuffer->SetVSBuffer(4);
	DEVICECONTEXT->VSSetShaderResources(0, 1, &mSRV); // TransformMapSRV

	MeshRender(); // �θ�Ŭ����(ModelReader)�Լ�.
}

void ModelAnimator::PostRender()
{	
}

void ModelAnimator::PlayClip(UINT clip, float speed, float takeTime)
{
	mFrameBuffer->data.tweenDesc[0].takeTime = takeTime;
	mFrameBuffer->data.tweenDesc[0].next.clip = clip;
	mFrameBuffer->data.tweenDesc[0].next.speed = speed;
}

Matrix ModelAnimator::GetTransformByNode(int nodeIndex)
{
	if (mNodeTransform == nullptr)
		return XMMatrixIdentity();

	FrameBuffer::KeyFrameDesc& curDesc = mFrameBuffer->data.tweenDesc[0].cur;

	Matrix cur = mNodeTransform[curDesc.clip].transform[curDesc.curFrame][nodeIndex];
	Matrix next = mNodeTransform[curDesc.clip].transform[curDesc.nextFrame][nodeIndex];

	Matrix curClip = LERP(cur, next, curDesc.time);

	FrameBuffer::KeyFrameDesc& nextDesc = mFrameBuffer->data.tweenDesc[0].next;

	if (nextDesc.clip == -1)
		return curClip;

	cur = mNodeTransform[nextDesc.clip].transform[nextDesc.curFrame][nodeIndex];
	next = mNodeTransform[nextDesc.clip].transform[nextDesc.nextFrame][nodeIndex];

	Matrix nextClip = LERP(cur, next, nextDesc.time);

	return LERP(curClip, nextClip, mFrameBuffer->data.tweenDesc[0].tweenTime);
}

Matrix ModelAnimator::GetTransformByNode(UINT instance, int nodeIndex)
{
	if (mNodeTransform == nullptr)
		return XMMatrixIdentity();

	FrameBuffer::KeyFrameDesc& curDesc = mFrameBuffer->data.tweenDesc[instance].cur;

	Matrix cur = mNodeTransform[curDesc.clip].transform[curDesc.curFrame][nodeIndex];
	Matrix next = mNodeTransform[curDesc.clip].transform[curDesc.nextFrame][nodeIndex];

	Matrix curClip = LERP(cur, next, curDesc.time);

	FrameBuffer::KeyFrameDesc& nextDesc = mFrameBuffer->data.tweenDesc[instance].next;

	if (nextDesc.clip == -1)
		return curClip;

	cur = mNodeTransform[nextDesc.clip].transform[nextDesc.curFrame][nodeIndex];
	next = mNodeTransform[nextDesc.clip].transform[nextDesc.nextFrame][nodeIndex];

	Matrix nextClip = LERP(cur, next, nextDesc.time);

	return LERP(curClip, nextClip, mFrameBuffer->data.tweenDesc[instance].tweenTime);
}

void ModelAnimator::CreateTexture() //��Ʈ������ �ѱ���.
{
	UINT clipCount = mClips.size();

	mClipTransform = new ClipTransform[clipCount];
	mNodeTransform = new ClipTransform[clipCount];

	for (UINT i = 0; i < clipCount; i++)
		CreateClipTransform(i);

	{//CreatTexture
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = MAX_BONE * 4; // 4�ȼ��� ��� 1��
		desc.Height = MAX_FRAME_KEY;
		desc.ArraySize = clipCount; // Ŭ�� ������ŭ
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // �ȼ��� 16����Ʈ.
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;

		UINT pageSize = MAX_BONE * sizeof(Matrix) * MAX_FRAME_KEY; // �� Ŭ���� ��ü ������ ũ��.

		void* p = VirtualAlloc(nullptr, pageSize * clipCount, MEM_RESERVE, PAGE_READWRITE); // �뷮�� �ʹ� ũ�� �����ͺ��簡 �ȵǱ� ������ �������� '������ ���Ŷ�'�� ��Ƴ��°�
																							// ��üŬ�������� ũ�� ��Ƴ�����.

		for (UINT c = 0; c < clipCount; c++) // �� Ŭ�� ������ŭ, �޸𸮿뷮�� ��üŬ��ũ��� ��Ƴ����� �ʹ� ũ�⋚���� ��Ŭ���� �޸𸮺���.
		{
			UINT start = c * pageSize; // �������� ����

			for (UINT y = 0; y < MAX_FRAME_KEY; y++) // ��ü Ű�����Ӹ�ŭ
			{
				void* temp = (BYTE*)p + MAX_BONE * y * sizeof(Matrix) + start; // ������ ����

				VirtualAlloc(temp, MAX_BONE * sizeof(Matrix), MEM_COMMIT, PAGE_READWRITE); // MEM_COMMIT : temp�� ���ڴٰ� Ŀ����(���� ���� �ƴ�) PAGE_READWRITE�뵵�� ���.
				memcpy(temp, mClipTransform[c].transform[y], MAX_BONE * sizeof(Matrix)); // ������ ���ִ� �κ�.
			}
		}


		// SRV ����.
		D3D11_SUBRESOURCE_DATA* subResource = new D3D11_SUBRESOURCE_DATA[clipCount];
		for (UINT c = 0; c < clipCount; c++)
		{
			void* temp = (BYTE*)p + c * pageSize;

			subResource[c].pSysMem = temp;
			subResource[c].SysMemPitch = MAX_BONE * sizeof(Matrix);
			subResource[c].SysMemSlicePitch = pageSize;
		}
		V(DEVICE->CreateTexture2D(&desc, subResource, &mTexture));

		delete[] subResource;
		VirtualFree(p, 0, MEM_RELEASE);
	}

	{//Create SRV
		D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // ���� ���� �������� ����.
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipLevels = 1;
		desc.Texture2DArray.ArraySize = clipCount;

		V(DEVICE->CreateShaderResourceView(mTexture, &desc, &mSRV));
	}
}

void ModelAnimator::CreateClipTransform(UINT index)
{
	ModelClip* clip = mClips[index]; // Ŭ�� ��������

	for (UINT f = 0; f < clip->frameCount; f++) // Ŭ�� �����Ӽ���ŭ.
	{
		UINT nodeIndex = 0;

		for (NodeData* node : nodes) // �� �� ������ŭ.
		{
			Matrix parent;
			int parentIndex = node->parent; // �θ� �ε��� �ޱ�.

			if (parentIndex < 0)
				parent = XMMatrixIdentity(); // �θ� ���ٸ�.
			else
				parent = mNodeTransform[index].transform[f][parentIndex]; // �θ� �ִٸ�.

			Matrix animation;
			KeyFrame* frame = clip->GetKeyFrame(node->name); 

			if (frame != nullptr) // �������� �ִٸ� animation�� SRT ��������.
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

			mNodeTransform[index].transform[f][nodeIndex] = animation * parent; // animation SRT�� �������� parent��ŭ �����δ�.

			if (boneMap.count(node->name) > 0) // ������� ���.
			{
				int boneIndex = boneMap[node->name];
				Matrix transform = XMLoadFloat4x4(&bones[boneIndex]->offset);
				transform *= mNodeTransform[index].transform[f][nodeIndex];
				mClipTransform[index].transform[f][boneIndex] = transform;
				// ���� �۷ι���ǥ(invGlobal) * �ִϸ��̼� SRT, ������ǥ(animation) * �θ��� �۷ι���ǥ(parent)
				// ��, invGlobal�� �������� �ִϸ��̼� local ��ǥ��ŭ �̵��ϰ�, �θ��� Global��ŭ �̵�.
			}

			nodeIndex++;
		}
	}
}
