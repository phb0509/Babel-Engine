#include "Framework.h"
#include "Model.h"

Model::Model(string file) : ModelReader(file)
{
	boneBuffer = new BoneBuffer();

	MakeBoneTransform();

	if (!bones.empty())
		typeBuffer->data.values[0] = 1;
}

Model::~Model()
{
	delete boneBuffer;
	delete[]  nodeTransforms;
}

void Model::Render()
{
	SetBoneTransforms();
	boneBuffer->SetVSBuffer(3);	

	MeshRender();
}

void Model::MakeBoneTransform()
{
	nodeTransforms = new Matrix[nodes.size()];
	UINT nodeIndex = 0;

	for (NodeData* node : nodes)
	{
		Matrix parent;
		int parentIndex = node->parent;

		if (parentIndex < 0)
			parent = XMMatrixIdentity();
		else
			parent = nodeTransforms[parentIndex];

		nodeTransforms[nodeIndex] = XMLoadFloat4x4(&node->transform) * parent;

		if (boneMap.count(node->name) > 0)
		{
			int boneIndex = boneMap[node->name];

			Matrix offset = XMLoadFloat4x4(&bones[boneIndex]->offset);

			//boneBuffer->Add(offset * nodeTransforms[nodeIndex], boneIndex);
			boneTransforms[boneIndex] = offset * nodeTransforms[nodeIndex];
		}

		nodeIndex++;
	}	
}

void Model::SetBoneTransforms()
{
	for (auto bone : boneTransforms)
		boneBuffer->Add(bone.second, bone.first);
}

int Model::GetNodeByName(string name)
{
	for (NodeData* node : nodes)
	{
		if (node->name == name)
			return node->index;
	}
	return 0;
}
