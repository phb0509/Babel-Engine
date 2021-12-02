#include "Framework.h"
#include "Model.h"

Model::Model() : 
	ModelReader()
{
	mBoneBuffer = new BoneBuffer();

	MakeBoneTransform();

	if (!mBones.empty()) // 본없으면, 그냥 정적인 메시면 셰이더에서 BoneWolrd로 계산..
		mTypeBuffer->data.values[0] = 1;
}

Model::~Model()
{
	delete mBoneBuffer;
	delete[]  mNodeTransforms;
}

void Model::Render()
{
	SetBoneTransforms();
	mBoneBuffer->SetVSBuffer(3);	

	MeshRender();
}

void Model::MakeBoneTransform()
{
	mNodeTransforms = new Matrix[mNodes.size()];
	UINT nodeIndex = 0;

	for (NodeData* node : mNodes)
	{
		Matrix parent;
		int parentIndex = node->parent;

		if (parentIndex < 0)
			parent = XMMatrixIdentity();
		else
			parent = mNodeTransforms[parentIndex];

		mNodeTransforms[nodeIndex] = XMLoadFloat4x4(&node->transform) * parent;

		if (mBoneMap.count(node->name) > 0)
		{
			int boneIndex = mBoneMap[node->name];

			Matrix offset = XMLoadFloat4x4(&mBones[boneIndex]->offset);

			//boneBuffer->Add(offset * nodeTransforms[nodeIndex], boneIndex);
			mBoneTransforms[boneIndex] = offset * mNodeTransforms[nodeIndex];
		}

		nodeIndex++;
	}	
}

void Model::SetBoneTransforms()
{
	for (auto bone : mBoneTransforms)
		mBoneBuffer->Add(bone.second, bone.first);
}

int Model::GetNodeByName(string name)
{
	for (NodeData* node : mNodes)
	{
		if (node->name == name)
			return node->index;
	}
	return 0;
}
