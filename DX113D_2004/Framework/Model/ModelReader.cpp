#include "Framework.h"

ModelReader::ModelReader():
	mbIsSetColorPickingShader(false)
{
	mDefaultMaterial = new Material();
	mDefaultMaterial->SetDiffuseMap(L"ModelData/DefaultDiffuseMap.png");
	mTypeBuffer = new TypeBuffer();
}

ModelReader::~ModelReader()
{
	for (auto material : mMaterials)
	{
		GM->SafeDelete(material.second);
	}

	GM->SafeDelete(mTypeBuffer);
	GM->SafeDelete(mDefaultMaterial);

	deleteDatas();
}

void ModelReader::MeshRender()
{
	mTypeBuffer->SetVSBuffer(5);

	for (ModelMesh* mesh : mMeshes)
		mesh->Render();
}

void ModelReader::MeshRender(UINT drawCount) // 인스턴싱용.
{
	mTypeBuffer->SetVSBuffer(5);

	for (ModelMesh* mesh : mMeshes)
	{
		mesh->Render(drawCount);
	}
}

void ModelReader::ReadMaterial(string folderName,string fileName)
{
	//file = "ModelData/Materials/" + file + ".mat"; // ModelExporter로 만든 MaterialFile.
	string filePath = "ModelData/" + folderName + "/" + fileName;

	wstring folderPath = ToWString(GetDirectoryName(filePath)); // 상위폴더까지경로.

	XmlDocument* document = new XmlDocument();
	document->LoadFile(filePath.c_str());
	
	XmlElement* root = document->FirstChildElement();

	XmlElement* matNode = root->FirstChildElement();

	do
	{
		XmlElement* node = matNode->FirstChildElement();

		Material* material = new Material();
		material->mName = node->GetText();

		node = node->NextSiblingElement();

		// 바꿀 디퓨즈맵파일스트링값 가져왔다  치고.
		if (node->GetText()) // DiffuseMap
		{
			//node->SetText("sibar");
			//document->SaveFile(file.c_str());
			
			wstring fileName = ToWString(node->GetText());
			
			if (ExistFile(ToString(folderPath + fileName)))
			{
				material->SetDiffuseMap(folderPath + fileName); // 여기서 그냥 해버려도 상관은 없는데, mat파일안이라
			}
		}

		node = node->NextSiblingElement();

		if (node->GetText()) // SpecularMap
		{
			wstring fileName = ToWString(node->GetText());

			if (ExistFile(ToString(folderPath + fileName)))
			{
				material->SetSpecularMap(folderPath + fileName);
			}
		}

		node = node->NextSiblingElement();

		if (node->GetText()) // NormalMap
		{
			wstring fileName = ToWString(node->GetText());
			if (ExistFile(ToString(folderPath + fileName)))
			{
				material->SetNormalMap(folderPath + fileName);
			}
		}			

		Float4 color;

		// .mat파일이 가지고 있는 Ambient R값 가져와서 세팅.
		node = node->NextSiblingElement();
		color.x = node->FloatAttribute("R");
		color.y = node->FloatAttribute("G");
		color.z = node->FloatAttribute("B");
		color.w = node->FloatAttribute("A");
		material->GetBuffer()->data.ambient = color;

		node = node->NextSiblingElement(); // Diffuse R값. 비율을 말하는거같긴한데...
		color.x = node->FloatAttribute("R");
		color.y = node->FloatAttribute("G");
		color.z = node->FloatAttribute("B");
		color.w = node->FloatAttribute("A");
		material->GetBuffer()->data.diffuse = color;

		node = node->NextSiblingElement();
		color.x = node->FloatAttribute("R");
		color.y = node->FloatAttribute("G");
		color.z = node->FloatAttribute("B");
		color.w = 1.0f;
		material->GetBuffer()->data.specular = color; // RGBA중에서 RGB까지만 specular값.
		material->GetBuffer()->data.shininess = node->FloatAttribute("A");	// A는 shininess값.

		node = node->NextSiblingElement();
		color.x = node->FloatAttribute("R");
		color.y = node->FloatAttribute("G");
		color.z = node->FloatAttribute("B");
		color.w = node->FloatAttribute("A");
		material->GetBuffer()->data.emissive = color;

		mMaterials[material->mName] = material;

		matNode = matNode->NextSiblingElement();
	} while (matNode != nullptr);

	delete document;
}

void ModelReader::ReadMesh(string folderName,string fileName)
{
	deleteDatas();
	mBoneMap.clear();

	string filePath = "ModelData/" + folderName + "/" + fileName;

	BinaryReader* r = new BinaryReader(filePath);

	UINT count = r->UInt();

	for (UINT i = 0; i < count; i++)
	{
		ModelMesh* mesh = new ModelMesh();
		mesh->mName = r->String();
		mesh->mMaterialName = r->String();
		mesh->mMaterial = mDefaultMaterial; // 
		
		{//Vertices
			UINT count = r->UInt();

			mesh->mVertexCount = count;
			mesh->mVertices = new ModelVertex[count];

			void* ptr = (void*)mesh->mVertices;
			r->Byte(&ptr, sizeof(ModelVertex) * count);
		}

		{//Indices
			UINT count = r->UInt();

			mesh->mIndexCount = count;
			mesh->mIndices = new UINT[count];

			void* ptr = (void*)mesh->mIndices;
			r->Byte(&ptr, sizeof(UINT) * count);
		}

		mesh->createMesh(); 
		mMeshes.emplace_back(mesh);
	}

	count = r->UInt();

	// Nodes Update
	for (UINT i = 0; i < count; i++)
	{
		NodeData* node = new NodeData();
		node->index = r->Int();
		node->name = r->String();
		node->parent = r->Int();
		node->transform = r->Float4x4();

		mNodes.emplace_back(node);
		mNodeLookupTable[node->name] = node->index;
	}

	count = r->UInt();

	// BoneMap Update
	for (UINT i = 0; i < count; i++) 
	{
		BoneData* bone = new BoneData();
		bone->name = r->String();
		bone->index = r->Int();
		bone->offset = r->Float4x4();

		mBoneMap[bone->name] = bone->index;

		mBones.emplace_back(bone);
	}



	delete r;
}

void ModelReader::SetShader(wstring fileName)
{
	for (int i = 0; i < mMeshes.size(); i++)
	{
		mMeshes[i]->mMaterial->SetShader(fileName);
	}
}

void ModelReader::SetDiffuseMap(wstring file)
{
	for (auto material : mMaterials)
	{
		material.second->SetDiffuseMap(file);
	}
}

void ModelReader::SetSpecularMap(wstring file)
{
	for (auto material : mMaterials)
	{
		material.second->SetSpecularMap(file);
	}
}

void ModelReader::SetNormalMap(wstring file)
{
	for (auto material : mMaterials)
	{
		material.second->SetNormalMap(file);
	}
}

void ModelReader::IsRenderSpecularMap(bool value)
{
	for (auto material : mMaterials)
	{
		material.second->SetHasSpecularMap(value);
		int a = 0;
	}
}

void ModelReader::IASet()
{
	mTypeBuffer->SetVSBuffer(5);

	for (ModelMesh* mesh : mMeshes)
	{
		mesh->IASet();
	}
}

int ModelReader::GetNodeIndex(string name)
{
	for (NodeData* node : mNodes)
	{
		if (node->name == name)
		{
			return node->index;
		}	
	}

	return 0;
}

bool ModelReader::GetHasMeshes()
{
	if (mMeshes.size() != 0)
	{
		return true;
	}

	return false;
}

void ModelReader::SetBox(Vector3* minBox, Vector3* maxBox)
{	
	mMeshes[0]->SetBox(minBox, maxBox);

	for (UINT i = 1; i < mMeshes.size(); i++)
	{
		Vector3 minPos, maxPos;
		mMeshes[i]->SetBox(&minPos, &maxPos);

		minBox->x = min(minBox->x, minPos.x);
		minBox->y = min(minBox->y, minPos.y);
		minBox->z = min(minBox->z, minPos.z);

		maxBox->x = max(maxBox->x, maxPos.x);
		maxBox->y = max(maxBox->y, maxPos.y);
		maxBox->z = max(maxBox->z, maxPos.z);
	}
}

void ModelReader::deleteDatas()
{
	for (int i = 0; i < mMeshes.size(); i++)
	{
		delete mMeshes[i];
	}
	mMeshes.clear();

	for (int i = 0; i < mNodes.size(); i++)
	{
		delete mNodes[i];
	}
	mNodes.clear();

	for (int i = 0; i < mBones.size(); i++)
	{
		delete mBones[i];
	}
	mBones.clear();
}

void ModelReader::SetMesh(string folderName, string fileName)
{
	ReadMesh(folderName,fileName); // 191ms이하.
}

void ModelReader::SetMaterial(string folderName, string fileName)
{
	{ // 한번 싹 초기화해주기
		for (auto material : mMaterials)
		{
			delete material.second;
		}
			
		mMaterials.clear();
	}

	ReadMaterial(folderName,fileName); // mMaterials Update.

	for (int i = 0; i < mMeshes.size(); i++) 
	{
		if (mMaterials[mMeshes[i]->mMaterialName] == nullptr) // 메시가 요구하는 material이 mMaterials에 없는 경우 (즉, 메시랑 다른 머터리얼파일을 세팅한경우)
		{
			mMeshes[i]->mMaterial = mDefaultMaterial; // 그냥 흰색으로 바르기.
		}
		else
		{
			mMeshes[i]->mMaterial = mMaterials[mMeshes[i]->mMaterialName];
		}
	}
}