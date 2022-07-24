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

void ModelReader::MeshRender(UINT drawCount) // �ν��Ͻ̿�.
{
	mTypeBuffer->SetVSBuffer(5);

	for (ModelMesh* mesh : mMeshes)
	{
		mesh->Render(drawCount);
	}
}

void ModelReader::ReadMaterial(string folderName,string fileName)
{
	//file = "ModelData/Materials/" + file + ".mat"; // ModelExporter�� ���� MaterialFile.
	string filePath = "ModelData/" + folderName + "/" + fileName;

	wstring folderPath = ToWString(GetDirectoryName(filePath)); // ���������������.

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

		// �ٲ� ��ǻ������Ͻ�Ʈ���� �����Դ�  ġ��.
		if (node->GetText()) // DiffuseMap
		{
			//node->SetText("sibar");
			//document->SaveFile(file.c_str());
			
			wstring fileName = ToWString(node->GetText());
			
			if (ExistFile(ToString(folderPath + fileName)))
			{
				material->SetDiffuseMap(folderPath + fileName); // ���⼭ �׳� �ع����� ����� ���µ�, mat���Ͼ��̶�
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

		// .mat������ ������ �ִ� Ambient R�� �����ͼ� ����.
		node = node->NextSiblingElement();
		color.x = node->FloatAttribute("R");
		color.y = node->FloatAttribute("G");
		color.z = node->FloatAttribute("B");
		color.w = node->FloatAttribute("A");
		material->GetBuffer()->data.ambient = color;

		node = node->NextSiblingElement(); // Diffuse R��. ������ ���ϴ°Ű����ѵ�...
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
		material->GetBuffer()->data.specular = color; // RGBA�߿��� RGB������ specular��.
		material->GetBuffer()->data.shininess = node->FloatAttribute("A");	// A�� shininess��.

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
	ReadMesh(folderName,fileName); // 191ms����.
}

void ModelReader::SetMaterial(string folderName, string fileName)
{
	{ // �ѹ� �� �ʱ�ȭ���ֱ�
		for (auto material : mMaterials)
		{
			delete material.second;
		}
			
		mMaterials.clear();
	}

	ReadMaterial(folderName,fileName); // mMaterials Update.

	for (int i = 0; i < mMeshes.size(); i++) 
	{
		if (mMaterials[mMeshes[i]->mMaterialName] == nullptr) // �޽ð� �䱸�ϴ� material�� mMaterials�� ���� ��� (��, �޽ö� �ٸ� ���͸��������� �����Ѱ��)
		{
			mMeshes[i]->mMaterial = mDefaultMaterial; // �׳� ������� �ٸ���.
		}
		else
		{
			mMeshes[i]->mMaterial = mMaterials[mMeshes[i]->mMaterialName];
		}
	}
}