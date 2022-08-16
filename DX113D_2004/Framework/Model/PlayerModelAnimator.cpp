#include "Framework.h"

PlayerModelAnimator::PlayerModelAnimator() :
	ModelReader(),
	mTexture(nullptr),
	mSRV(nullptr),
	mClipTransform(nullptr),
	mNodeTransform(nullptr),
	mNodeTransformMatrices(nullptr),
	mbIsPlayedAnimation(true),
	mbIsEndCurrentAnimation(false),
	mCurrentClipIndex(-1)
{
	mFrameBuffer = new PlayerFrameBuffer();
	mTypeBuffer->data.values[0] = 2;
}

PlayerModelAnimator::~PlayerModelAnimator()
{
	GM->SafeDelete(mFrameBuffer);
	GM->SafeDeleteArray(mClipTransform);
	GM->SafeDeleteArray(mNodeTransform);
	GM->SafeDeleteVector(mClips);
	GM->SafeDelete(mBoneBuffer);
	GM->SafeDeleteArray(mNodeTransformMatrices);

	if (mTexture != nullptr)
	{
		mTexture->Release();
	}

	if (mSRV != nullptr)
	{
		mSRV->Release();
	}
}


bool PlayerModelAnimator::ReadClip(string modelName, string clipFileName) // Ȯ���� ������ ���ϸ�.
{
	mClipNames.push_back(GetFileNameWithoutExtension(clipFileName));

	string filePath = "ModelData/" + modelName + "/" + clipFileName;

	BinaryReader binaryReader(filePath); // 

	ModelClip* clip = new ModelClip();

	clip->mName = binaryReader.ReadString();
	clip->mDuration = binaryReader.ReadFloat(); // �� ������ ����. �ͻ��� ��ġ.
	clip->mFramePerSecond = binaryReader.ReadFloat(); // FramesPerSecond // �ʴ� ����Ǵ� ������ ����.
	clip->mFrameCount = binaryReader.ReadUInt(); // ����⿡�� �о���� �� �׳� �������Ӱ���+1�� �����ع���.

	if (clip->mFrameCount >= 1000)
	{
		delete clip;
		mClipNames.pop_back();
		binaryReader.CloseReader();
		return false;
	}

	UINT keyFrameCount = binaryReader.ReadUInt(); // �� ��尳��. ���ʿ� �ͻ�𿡼� �ٿ���� �� �𵨳ְ� �ٿ�����ϱ�.

	for (UINT i = 0; i < keyFrameCount; i++)
	{
		KeyFrame* keyFrame = new KeyFrame();
		keyFrame->boneName = binaryReader.ReadString();

		UINT size = binaryReader.ReadUInt(); // transforsm.size, 

		if (size > 0)
		{
			keyFrame->transforms.resize(size);

			void* ptr = (void*)keyFrame->transforms.data();
			binaryReader.Byte(&ptr, sizeof(KeyTransform) * size);
		}
		clip->mKeyFrames[keyFrame->boneName] = keyFrame;
	}

	mClips.emplace_back(clip);

	binaryReader.CloseReader();

	return true;
}

void PlayerModelAnimator::Update()
{
	if (mClips.size() != 0) // ModelAnimation
	{
		if (mbIsPlayedAnimation)
		{
			PlayerFrameBuffer::TweenDesc& tweenDesc = mFrameBuffer->data.tweenDesc[0]; //�ν��Ͻ��� �ƴϴϱ� ���̴��� 1��ü���� �ѱ�� �Ǵϱ� 0��°.

			{ // ���� Ŭ��.
				PlayerFrameBuffer::KeyFrameDesc& curDesc = tweenDesc.cur; // ���� Clip�� ���� KeyFrameDesc
				ModelClip* clip = mClips[curDesc.clip]; // desc.clip == clipIndex

				float time = 1.0f / clip->mFramePerSecond / curDesc.speed; // speed�� 1.0f�� 1/30��. �ͻ�𿡼� �ٿ������ �׳� 30���� �ٿ����.
				curDesc.runningTime += DELTA;

				if (curDesc.time >= 1.0f)
				{
					if (curDesc.curFrame + curDesc.time >= clip->mFrameCount) // ���� Ŭ������� ������ 
					{
						if (mEndEvent.count(curDesc.clip) > 0) // �����̺�Ʈ�� ������
						{
							mEndEvent[curDesc.clip]();
						}

						if (mEndParamEvent.count(curDesc.clip) > 0)
						{
							mEndParamEvent[curDesc.clip](mParam[curDesc.clip]);
						}
					}

					curDesc.curFrame = (curDesc.curFrame + 1) % clip->mFrameCount; // ���� ������
					curDesc.nextFrame = (curDesc.curFrame + 1) % clip->mFrameCount; // ���� ������.
					curDesc.runningTime = 0.0f;

					mCurrentClipFrame = curDesc.curFrame; // ������ ImGui�� �����Ϸ��� ���� �����س��°�.
					mCurrentClipFrameCount = clip->mFrameCount;
				}

				curDesc.time = curDesc.runningTime / time;
			}

			{ // ���� Ŭ��
				PlayerFrameBuffer::KeyFrameDesc& nextDesc = tweenDesc.next;

				if (nextDesc.clip > -1) // ����Ŭ���� ������?
				{
					ModelClip* clip = mClips[nextDesc.clip];

					tweenDesc.runningTime += DELTA;
					tweenDesc.tweenTime = tweenDesc.runningTime / tweenDesc.takeTime;

					if (tweenDesc.tweenTime >= 1.0f)
					{
						tweenDesc.cur = nextDesc;
						tweenDesc.runningTime = 0.0f;
						tweenDesc.tweenTime = 0.0f;

						nextDesc.runningTime = 0.0f;
						nextDesc.curFrame = 0;
						nextDesc.nextFrame = 0;
						nextDesc.time = 0.0f;
						nextDesc.clip = -1;
					}

					else
					{
						float time = 1.0f / clip->mFramePerSecond / nextDesc.speed;
						nextDesc.runningTime += DELTA;

						if (nextDesc.time >= 1.0f)
						{
							nextDesc.curFrame = (nextDesc.curFrame + 1) % clip->mFrameCount;
							nextDesc.nextFrame = (nextDesc.curFrame + 1) % clip->mFrameCount;
							nextDesc.runningTime = 0.0f;
						}

						nextDesc.time = nextDesc.runningTime / time;
					}
				}
			}
		} // end if isPlayedAnimation
	}
}


void PlayerModelAnimator::Render()
{
	if (mClips.size() != 0) // ModelAnimation
	{
		SetShader(L"PlayerModelAnimation");

		if (mTexture == nullptr)
		{
			CreateTexture(); // TransformMapTexture.
		}

		mFrameBuffer->SetVSBuffer(4);
		DEVICECONTEXT->VSSetShaderResources(0, 1, &mSRV); // TransformMapSRV
	}

	else // Model
	{
		SetShader(L"Lighting");
		//SetBoneTransforms();
		if (mBoneBuffer == nullptr)
		{
			ExecuteSetMeshEvent(); // create BoneBuffer
		}
		mBoneBuffer->SetVSBuffer(3);
	}

	ModelReader::MeshRender(); 
}

void PlayerModelAnimator::DeferredRender()
{
	if (mClips.size() != 0) // ModelAnimation
	{
		if (mTexture == nullptr)
		{
			CreateTexture(); // TransformMapTexture.
		}

		mFrameBuffer->SetVSBuffer(4);
		DEVICECONTEXT->VSSetShaderResources(0, 1, &mSRV); // TransformMapSRV
	}

	else // Model
	{
		if (mBoneBuffer == nullptr)
		{
			ExecuteSetMeshEvent(); // create BoneBuffer
		}
		mBoneBuffer->SetVSBuffer(3);
	}

	MeshRender(); // �θ�Ŭ����(ModelReader)�Լ�.
}

void PlayerModelAnimator::PostRender()
{

}

void PlayerModelAnimator::PlayClip(UINT clip, float speed, float takeTime)
{
	mFrameBuffer->data.tweenDesc[0].next.clip = clip;
	mFrameBuffer->data.tweenDesc[0].next.speed = speed;
	mFrameBuffer->data.tweenDesc[0].takeTime = takeTime;
}

Matrix PlayerModelAnimator::GetTransformByNode(int nodeIndex)
{
	if (mNodeTransform == nullptr)
		return XMMatrixIdentity();

	PlayerFrameBuffer::KeyFrameDesc& curDesc = mFrameBuffer->data.tweenDesc[0].cur;

	Matrix cur = mNodeTransform[curDesc.clip].transform[curDesc.curFrame][nodeIndex];
	Matrix next = mNodeTransform[curDesc.clip].transform[curDesc.nextFrame][nodeIndex];

	Matrix curClip = LERP(cur, next, curDesc.time);

	PlayerFrameBuffer::KeyFrameDesc& nextDesc = mFrameBuffer->data.tweenDesc[0].next;

	if (nextDesc.clip == -1)
	{
		return curClip;
	}

	cur = mNodeTransform[nextDesc.clip].transform[nextDesc.curFrame][nodeIndex];
	next = mNodeTransform[nextDesc.clip].transform[nextDesc.nextFrame][nodeIndex];

	Matrix nextClip = LERP(cur, next, nextDesc.time);

	return LERP(curClip, nextClip, mFrameBuffer->data.tweenDesc[0].tweenTime);
}

Matrix PlayerModelAnimator::GetTransformByNode(UINT instance, int nodeIndex)
{
	if (mNodeTransform == nullptr)
		return XMMatrixIdentity();

	PlayerFrameBuffer::KeyFrameDesc& curDesc = mFrameBuffer->data.tweenDesc[instance].cur;

	Matrix cur = mNodeTransform[curDesc.clip].transform[curDesc.curFrame][nodeIndex];
	Matrix next = mNodeTransform[curDesc.clip].transform[curDesc.nextFrame][nodeIndex];

	Matrix curClip = LERP(cur, next, curDesc.time);

	PlayerFrameBuffer::KeyFrameDesc& nextDesc = mFrameBuffer->data.tweenDesc[instance].next;

	if (nextDesc.clip == -1)
	{
		return curClip;
	}

	cur = mNodeTransform[nextDesc.clip].transform[nextDesc.curFrame][nodeIndex];
	next = mNodeTransform[nextDesc.clip].transform[nextDesc.nextFrame][nodeIndex];

	Matrix nextClip = LERP(cur, next, nextDesc.time);

	return LERP(curClip, nextClip, mFrameBuffer->data.tweenDesc[instance].tweenTime);
}

void PlayerModelAnimator::MakeBoneTransform()
{
	mNodeTransformMatrices = new Matrix[mNodes.size()];
	UINT nodeIndex = 0;

	for (NodeData* node : mNodes)
	{
		Matrix parent;
		int parentIndex = node->parent;

		if (parentIndex < 0)
			parent = XMMatrixIdentity();
		else
			parent = mNodeTransformMatrices[parentIndex];

		mNodeTransformMatrices[nodeIndex] = XMLoadFloat4x4(&node->transform) * parent;

		if (mBoneMap.count(node->name) > 0)
		{
			int boneIndex = mBoneMap[node->name];

			Matrix offset = XMLoadFloat4x4(&mBones[boneIndex]->offset);

			//boneBuffer->Add(offset * nodeTransforms[nodeIndex], boneIndex);
			mBoneTransforms[boneIndex] = offset * mNodeTransformMatrices[nodeIndex];
		}

		nodeIndex++;
	}

	map<int, Matrix>::iterator iter;

	for (iter = mBoneTransforms.begin(); iter != mBoneTransforms.end(); iter++)
	{
		mBoneBuffer->Add(iter->second, iter->first);
	}
}

void PlayerModelAnimator::SetBoneTransforms() // 
{
	//for (auto bone : mBoneTransforms)
	//{
	//	mBoneBuffer->Add(bone.second, bone.first);
	//}

	/*map<int, Matrix>::iterator iter;

	for (iter = mBoneTransforms.begin(); iter != mBoneTransforms.end(); iter++)
	{
		mBoneBuffer->Add(iter->second, iter->first);
	}*/
}

void PlayerModelAnimator::CreateTexture() //��Ʈ������ �ѱ���.
{
	UINT clipCount = mClips.size();

	if (mTexture != nullptr)
	{
		mTexture->Release();
	}

	if (mClipTransform != nullptr)
	{
		delete[] mClipTransform;
	}

	if (mNodeTransform != nullptr)
	{
		delete[] mNodeTransform;
	}

	mClipTransform = new ClipTransform[clipCount]; // �ִϸ��̼�Ŭ�� ����.
	mNodeTransform = new ClipTransform[clipCount];

	for (UINT i = 0; i < clipCount; i++)
	{
		CreateClipTransform(i); // mClipTransform, mNodeTransform�� ä���ֱ�.
	}

	{//Create Texture
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = PLAYER_MAX_BONE * 4; // 4�ȼ��� ��� 1�� (��� 1�� = 64����Ʈ, �ȼ��� 16����Ʈ�� �ؼ� 64����Ʈ)
		desc.Height = PLAYER_MAX_FRAME_KEY;
		desc.ArraySize = clipCount; // Ŭ�� ������ŭ
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // �ȼ��� 16����Ʈ.
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;

		UINT pageSize = sizeof(Matrix) * PLAYER_MAX_BONE * PLAYER_MAX_FRAME_KEY; // �� Ŭ���� ��ü ������ ũ��.
		//UINT pageSize =  MAX_BONE * sizeof(Matrix) * MAX_FRAME_KEY; // �� Ŭ���� ��ü ������ ũ��.

		void* p = VirtualAlloc(nullptr, (SIZE_T)pageSize * clipCount, MEM_RESERVE, PAGE_READWRITE); // �뷮�� �ʹ� ũ�� �����ͺ��簡 �ȵǱ� ������ �������� '������ ���Ŷ�'�� ��Ƴ��°�
																							// ��üŬ��������(n���� Ŭ��������) ũ�� ��Ƴ�����.

		for (UINT c = 0; c < clipCount; c++) // �� Ŭ�� ������ŭ, �޸𸮿뷮�� ��üŬ��ũ��� ��Ƴ����� �ʹ� ũ�⋚���� ��Ŭ���� �޸𸮺���.
		{
			UINT start = c * pageSize; // �������� ����

			for (UINT y = 0; y < PLAYER_MAX_FRAME_KEY; y++) // ��ü Ű�����Ӹ�ŭ
			{
				void* temp = (BYTE*)p + PLAYER_MAX_BONE * y * sizeof(Matrix) + start; // ������ ����

				VirtualAlloc(temp, PLAYER_MAX_BONE * sizeof(Matrix), MEM_COMMIT, PAGE_READWRITE); // MEM_COMMIT : temp�� ���ڴٰ� Ŀ����(���� ���� �ƴ�) PAGE_READWRITE�뵵�� ���.
				memcpy(temp, mClipTransform[c].transform[y], PLAYER_MAX_BONE * sizeof(Matrix)); // ������ ���ִ� �κ�.
			}
		}

		// SRV ����.
		D3D11_SUBRESOURCE_DATA* subResource = new D3D11_SUBRESOURCE_DATA[clipCount];

		for (UINT c = 0; c < clipCount; c++)
		{
			void* temp = (BYTE*)p + c * pageSize;

			subResource[c].pSysMem = temp;
			subResource[c].SysMemPitch = PLAYER_MAX_BONE * sizeof(Matrix);
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

void PlayerModelAnimator::CreateClipTransform(UINT index) // index of ModelClips
{
	ModelClip* clip = mClips[index]; // Ŭ�� ��������

	for (UINT i = 0; i < clip->mFrameCount; i++) // Ŭ�� �����Ӽ���ŭ.
	{
		UINT nodeIndex = 0;

		for (NodeData* node : mNodes) // �� �� ������ŭ.
		{
			Matrix parent;
			int parentIndex = node->parent; // �θ� �ε��� �ޱ�.

			if (parentIndex < 0)
			{
				parent = XMMatrixIdentity(); // �θ� ���ٸ�.(�ֻ�ܳ��)
			}
			else
			{
				parent = mNodeTransform[index].transform[i][parentIndex]; // �θ� �ִٸ�.
					// = 0��° Ŭ���� i��° �������� ���(parentIndex�� �θ���)
			}

			Matrix animationMatrix;
			KeyFrame* frame = clip->GetKeyFrame(node->name); // node�� �ش��ϴ� Ű������. ex) �޹߿� �ش��ϴ� Ű������ 144��

			if (frame != nullptr) // �������� �ִٸ� animation�� SRT ��������.
			{
				KeyTransform& transform = frame->transforms[i]; // i��° �������� SRT��

				Matrix S = XMMatrixScaling(transform.scale.x, transform.scale.y, transform.scale.z);
				Matrix R = XMMatrixRotationQuaternion(XMLoadFloat4(&transform.rotation));
				Matrix T = XMMatrixTranslation(transform.position.x, transform.position.y, transform.position.z);

				animationMatrix = S * R * T;
			}
			else
			{
				animationMatrix = XMMatrixIdentity();
			}

			mNodeTransform[index].transform[i][nodeIndex] = animationMatrix * parent; // animation SRT�� �������� parent��ŭ �����δ�.
			// ������ Ŭ�������� �ִ� i��° �������� SRT���� �θ���� �����ֱ�. �θ� ������ �޴½��̱� ����

			if (mBoneMap.count(node->name) > 0) // ������� ���.
			{
				int boneIndex = mBoneMap[node->name];
				Matrix transform = XMLoadFloat4x4(&mBones[boneIndex]->offset);
				transform *= mNodeTransform[index].transform[i][nodeIndex]; // �ش纻�� offset���� ������.
				mClipTransform[index].transform[i][boneIndex] = transform;

				// ���� �۷ι���ǥ(invGlobal) * �ִϸ��̼� SRT, ������ǥ(animation) * �θ��� �۷ι���ǥ(parent)
				// ��, invGlobal�� �������� �ִϸ��̼� local ��ǥ��ŭ �̵��ϰ�, �θ��� Global��ŭ �̵�.
			}

			nodeIndex++;
		}
	}
}

void PlayerModelAnimator::ExecuteSetMeshEvent()
{
	if (mBoneBuffer != nullptr)
	{
		delete mBoneBuffer;
	}

	mBoneBuffer = new BoneBuffer();
	MakeBoneTransform();

	if (!mBones.empty()) // ��������, �� �׳� ������ �޽ø� ���̴����� BoneWolrd�� ���..
		mTypeBuffer->data.values[0] = 1;
}
