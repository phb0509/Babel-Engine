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
	SetShader(L"PlayerModelAnimation");
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


bool PlayerModelAnimator::ReadClip(string modelName, string clipFileName) // 확장자 포함한 파일명.
{
	mClipNames.push_back(GetFileNameWithoutExtension(clipFileName));

	string filePath = "ModelData/" + modelName + "/" + clipFileName;

	BinaryReader binaryReader(filePath); // 

	ModelClip* clip = new ModelClip();

	clip->mName = binaryReader.ReadString();
	clip->mDuration = binaryReader.ReadFloat(); // 총 프레임 개수. 믹사모랑 일치.
	clip->mFramePerSecond = binaryReader.ReadFloat(); // FramesPerSecond // 초당 재생되는 프레임 개수.
	clip->mFrameCount = binaryReader.ReadUInt(); // 추출기에서 읽어들일 때 그냥 총프레임개수+1로 설정해버림.

	if (clip->mFrameCount >= 1000)
	{
		delete clip;
		mClipNames.pop_back();
		binaryReader.CloseReader();
		return false;
	}

	UINT keyFrameCount = binaryReader.ReadUInt(); // 모델 노드개수. 애초에 믹사모에서 다운받을 때 모델넣고 다운받으니까.

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
			PlayerFrameBuffer::TweenDesc& tweenDesc = mFrameBuffer->data.tweenDesc[0]; //인스턴스용 아니니까 셰이더에 1개체씩만 넘기면 되니까 0번째.

			{ // 현재 클립.
				PlayerFrameBuffer::KeyFrameDesc& curDesc = tweenDesc.cur; // 현재 Clip에 대한 KeyFrameDesc
				ModelClip* clip = mClips[curDesc.clip]; // desc.clip == clipIndex

				float time = 1.0f / clip->mFramePerSecond / curDesc.speed; // speed가 1.0f면 1/30초. 믹사모에서 다운받을때 그냥 30으로 다운받음.
				curDesc.runningTime += DELTA;

				if (curDesc.time >= 1.0f)
				{
					if (curDesc.curFrame + curDesc.time >= clip->mFrameCount) // 현재 클립재생이 끝나면 
					{
						if (mEndEvent.count(curDesc.clip) > 0) // 엔드이벤트가 있으면
						{
							mEndEvent[curDesc.clip]();
						}

						if (mEndParamEvent.count(curDesc.clip) > 0)
						{
							mEndParamEvent[curDesc.clip](mParam[curDesc.clip]);
						}
					}

					curDesc.curFrame = (curDesc.curFrame + 1) % clip->mFrameCount; // 현재 프레임
					curDesc.nextFrame = (curDesc.curFrame + 1) % clip->mFrameCount; // 다음 프레임.
					curDesc.runningTime = 0.0f;

					mCurrentClipFrame = curDesc.curFrame; // 씬에서 ImGui로 렌더하려고 따로 저장해놓는거.
					mCurrentClipFrameCount = clip->mFrameCount;
				}

				curDesc.time = curDesc.runningTime / time;
			}

			{ // 다음 클립
				PlayerFrameBuffer::KeyFrameDesc& nextDesc = tweenDesc.next;

				if (nextDesc.clip > -1) // 다음클립이 있으면?
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
	//if (mClips.size() != 0) // ModelAnimation
	//{
	//	SetShader(L"PlayerModelAnimation");

	//	if (mTexture == nullptr)
	//	{
	//		CreateTexture(); // TransformMapTexture.
	//	}

	//	mFrameBuffer->SetVSBuffer(4);
	//	DEVICECONTEXT->VSSetShaderResources(0, 1, &mSRV); // TransformMapSRV
	//}

	//else // Model
	//{
	//	SetShader(L"Lighting");
	//	//SetBoneTransforms();
	//	if (mBoneBuffer == nullptr)
	//	{
	//		ExecuteSetMeshEvent(); // create BoneBuffer
	//	}
	//	mBoneBuffer->SetVSBuffer(3);
	//}

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

	MeshRender(); // 부모클래스(ModelReader)함수.
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

void PlayerModelAnimator::CreateTexture() //본트랜스폼 넘기기용.
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

	mClipTransform = new ClipTransform[clipCount]; // 애니메이션클립 갯수.
	mNodeTransform = new ClipTransform[clipCount];

	for (UINT i = 0; i < clipCount; i++)
	{
		CreateClipTransform(i); // mClipTransform, mNodeTransform에 채워넣기.
	}

	{//Create Texture
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = PLAYER_MAX_BONE * 4; // 4픽셀당 행렬 1개 (행렬 1개 = 64바이트, 픽셀당 16바이트씩 해서 64바이트)
		desc.Height = PLAYER_MAX_FRAME_KEY;
		desc.ArraySize = clipCount; // 클립 개수만큼
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // 픽셀당 16바이트.
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;

		UINT pageSize = sizeof(Matrix) * PLAYER_MAX_BONE * PLAYER_MAX_FRAME_KEY; // 한 클립의 전체 데이터 크기.
		//UINT pageSize =  MAX_BONE * sizeof(Matrix) * MAX_FRAME_KEY; // 한 클립의 전체 데이터 크기.

		void* p = VirtualAlloc(nullptr, (SIZE_T)pageSize * clipCount, MEM_RESERVE, PAGE_READWRITE); // 용량이 너무 크면 데이터복사가 안되기 때문에 가상으로 '이정도 쓸거라'고 잡아놓는것
																							// 전체클립데이터(n개의 클립데이터) 크기 잡아놓은것.

		for (UINT c = 0; c < clipCount; c++) // 모델 클립 개수만큼, 메모리용량은 전체클립크기로 잡아놨지만 너무 크기떄문에 한클립씩 메모리복사.
		{
			UINT start = c * pageSize; // 페이지의 시작

			for (UINT y = 0; y < PLAYER_MAX_FRAME_KEY; y++) // 전체 키프레임만큼
			{
				void* temp = (BYTE*)p + PLAYER_MAX_BONE * y * sizeof(Matrix) + start; // 한줄의 시작

				VirtualAlloc(temp, PLAYER_MAX_BONE * sizeof(Matrix), MEM_COMMIT, PAGE_READWRITE); // MEM_COMMIT : temp에 쓰겠다고 커밋함(아직 쓴건 아님) PAGE_READWRITE용도로 사용.
				memcpy(temp, mClipTransform[c].transform[y], PLAYER_MAX_BONE * sizeof(Matrix)); // 실제로 써주는 부분.
			}
		}

		// SRV 세팅.
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
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // 위와 동일 포맷으로 설정.
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipLevels = 1;
		desc.Texture2DArray.ArraySize = clipCount;

		V(DEVICE->CreateShaderResourceView(mTexture, &desc, &mSRV));
	}
}

void PlayerModelAnimator::CreateClipTransform(UINT index) // index of ModelClips
{
	ModelClip* clip = mClips[index]; // 클립 가져오기

	for (UINT i = 0; i < clip->mFrameCount; i++) // 클립 프레임수만큼.
	{
		UINT nodeIndex = 0;

		for (NodeData* node : mNodes) // 모델 본 개수만큼.
		{
			Matrix parent;
			int parentIndex = node->parent; // 부모 인덱스 받기.

			if (parentIndex < 0)
			{
				parent = XMMatrixIdentity(); // 부모가 없다면.(최상단노드)
			}
			else
			{
				parent = mNodeTransform[index].transform[i][parentIndex]; // 부모가 있다면.
					// = 0번째 클립의 i번째 프레임의 노드(parentIndex니 부모노드)
			}

			Matrix animationMatrix;
			KeyFrame* frame = clip->GetKeyFrame(node->name); // node에 해당하는 키프레임. ex) 왼발에 해당하는 키프레임 144개

			if (frame != nullptr) // 프레임이 있다면 animation의 SRT 가져오기.
			{
				KeyTransform& transform = frame->transforms[i]; // i번째 프레임의 SRT값

				Matrix S = XMMatrixScaling(transform.scale.x, transform.scale.y, transform.scale.z);
				Matrix R = XMMatrixRotationQuaternion(XMLoadFloat4(&transform.rotation));
				Matrix T = XMMatrixTranslation(transform.position.x, transform.position.y, transform.position.z);

				animationMatrix = S * R * T;
			}
			else
			{
				animationMatrix = XMMatrixIdentity();
			}

			mNodeTransform[index].transform[i][nodeIndex] = animationMatrix * parent; // animation SRT를 기준으로 parent만큼 움직인다.
			// 추출한 클립정보에 있는 i번째 프레임의 SRT값에 부모행렬 곱해주기. 부모에 영향을 받는식이기 때문

			if (mBoneMap.count(node->name) > 0) // 최종행렬 계산.
			{
				int boneIndex = mBoneMap[node->name];
				Matrix transform = XMLoadFloat4x4(&mBones[boneIndex]->offset);
				transform *= mNodeTransform[index].transform[i][nodeIndex]; // 해당본의 offset값도 곱해줌.
				mClipTransform[index].transform[i][boneIndex] = transform;

				// 본의 글로벌좌표(invGlobal) * 애니메이션 SRT, 로컬좌표(animation) * 부모의 글로벌좌표(parent)
				// 즉, invGlobal을 기준으로 애니메이션 local 좌표만큼 이동하고, 부모의 Global만큼 이동.
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

	if (!mBones.empty()) // 본없으면, 즉 그냥 정적인 메시면 셰이더에서 BoneWolrd로 계산..
		mTypeBuffer->data.values[0] = 1;
}
