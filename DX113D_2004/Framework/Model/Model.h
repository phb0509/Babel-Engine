#pragma once

class Model : public ModelReader
{

public:
	Model();
	virtual ~Model();

	virtual void Render();

	void MakeBoneTransform();
	void SetBoneTransforms();	

	int GetNodeByName(string name);
	UINT GetBoneSize() { return mBones.size(); }



protected:
	BoneBuffer* mBoneBuffer;
	map<int, Matrix> mBoneTransforms;
	Matrix* mNodeTransforms;
};