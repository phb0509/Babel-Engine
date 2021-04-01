#pragma once

class Model : public ModelReader
{
protected:
	BoneBuffer* boneBuffer;

	map<int, Matrix> boneTransforms;
	Matrix* nodeTransforms;
public:
	Model(string file);
	virtual ~Model();

	virtual void Render();

	void MakeBoneTransform();
	void SetBoneTransforms();	

	int GetNodeByName(string name);
	UINT GetBoneSize() { return bones.size(); }
};