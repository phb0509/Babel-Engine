#include "Framework.h"

ModelReader::ModelReader(string file)
{
	ReadMaterial(file);
	ReadMesh(file);
	typeBuffer = new TypeBuffer();
}

ModelReader::~ModelReader()
{
	for (auto material : materials)
		delete material.second;

	for (ModelMesh* mesh : meshes)
		delete mesh;

	delete typeBuffer;
}

void ModelReader::MeshRender()
{
	typeBuffer->SetVSBuffer(5);

	for (ModelMesh* mesh : meshes)
		mesh->Render();
}

void ModelReader::MeshRender(UINT drawCount)
{
	typeBuffer->SetVSBuffer(5);

	for (ModelMesh* mesh : meshes)
		mesh->Render(drawCount);
}

void ModelReader::ReadMaterial(string file)
{
	file = "ModelData/Materials/" + file + ".mat";

	wstring folder = ToWString(GetDirectoryName(file));

	XmlDocument* document = new XmlDocument();
	document->LoadFile(file.c_str());

	XmlElement* root = document->FirstChildElement();

	XmlElement* matNode = root->FirstChildElement();

	do
	{
		XmlElement* node = matNode->FirstChildElement();

		Material* material = new Material();
		material->mName = node->GetText();

		node = node->NextSiblingElement();

		if (node->GetText())
		{
			wstring file = ToWString(node->GetText());
			if(ExistFile(ToString(folder + file)))
				material->SetDiffuseMap(folder + file);
		}

		node = node->NextSiblingElement();

		if (node->GetText())
		{
			wstring file = ToWString(node->GetText());
			if (ExistFile(ToString(folder + file)))
				material->SetSpecularMap(folder + file);
		}

		node = node->NextSiblingElement();

		if (node->GetText())
		{
			wstring file = ToWString(node->GetText());
			if (ExistFile(ToString(folder + file)))
				material->SetNormalMap(folder + file);
		}			

		Float4 color;

		node = node->NextSiblingElement();
		color.x = node->FloatAttribute("R");
		color.y = node->FloatAttribute("G");
		color.z = node->FloatAttribute("B");
		color.w = node->FloatAttribute("A");
		material->GetBuffer()->data.ambient = color;

		node = node->NextSiblingElement();
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
		material->GetBuffer()->data.specular = color;
		material->GetBuffer()->data.shininess = node->FloatAttribute("A");	

		node = node->NextSiblingElement();
		color.x = node->FloatAttribute("R");
		color.y = node->FloatAttribute("G");
		color.z = node->FloatAttribute("B");
		color.w = node->FloatAttribute("A");
		material->GetBuffer()->data.emissive = color;

		materials[material->mName] = material;

		matNode = matNode->NextSiblingElement();
	} while (matNode != nullptr);

	delete document;
}

void ModelReader::ReadMesh(string file)
{
	file = "ModelData/Meshes/" + file + ".mesh";
	BinaryReader* r = new BinaryReader(file);

	UINT count = r->UInt();

	for (UINT i = 0; i < count; i++)
	{
		ModelMesh* mesh = new ModelMesh();
		mesh->name = r->String();
		mesh->materialName = r->String();

		mesh->material = materials[mesh->materialName];

		{//Vertices
			UINT count = r->UInt();

			mesh->vertexCount = count;
			mesh->vertices = new ModelVertex[count];

			void* ptr = (void*)mesh->vertices;
			r->Byte(&ptr, sizeof(ModelVertex) * count);
		}

		{//Indices
			UINT count = r->UInt();

			mesh->indexCount = count;
			mesh->indices = new UINT[count];

			void* ptr = (void*)mesh->indices;
			r->Byte(&ptr, sizeof(UINT) * count);
		}

		mesh->CreateMesh();
		meshes.emplace_back(mesh);
	}

	count = r->UInt();

	for (UINT i = 0; i < count; i++)
	{
		NodeData* node = new NodeData();
		node->index = r->Int();
		node->name = r->String();
		node->parent = r->Int();
		node->transform = r->Float4x4();

		nodes.emplace_back(node);
	}

	count = r->UInt();

	for (UINT i = 0; i < count; i++)
	{
		BoneData* bone = new BoneData();
		bone->name = r->String();
		bone->index = r->Int();
		bone->offset = r->Float4x4();

		boneMap[bone->name] = bone->index;

		bones.emplace_back(bone);
	}
}

void ModelReader::SetShader(wstring file)
{
	for (auto material : materials)
		material.second->SetShader(file);
}

void ModelReader::SetDiffuseMap(wstring file)
{
	for (auto material : materials)
		material.second->SetDiffuseMap(file);
}

void ModelReader::SetSpecularMap(wstring file)
{
	for (auto material : materials)
		material.second->SetSpecularMap(file);
}

void ModelReader::SetNormalMap(wstring file)
{
	for (auto material : materials)
		material.second->SetNormalMap(file);
}

int ModelReader::GetNodeByName(string name)
{
	for (NodeData* node : nodes)
	{
		if (node->name == name)
			return node->index;
	}

	return 0;
}

void ModelReader::SetBox(Vector3* minBox, Vector3* maxBox)
{	
	meshes[0]->SetBox(minBox, maxBox);

	for (UINT i = 1; i < meshes.size(); i++)
	{
		Vector3 minPos, maxPos;
		meshes[i]->SetBox(&minPos, &maxPos);

		minBox->x = min(minBox->x, minPos.x);
		minBox->y = min(minBox->y, minPos.y);
		minBox->z = min(minBox->z, minPos.z);

		maxBox->x = max(maxBox->x, maxPos.x);
		maxBox->y = max(maxBox->y, maxPos.y);
		maxBox->z = max(maxBox->z, maxPos.z);
	}
}
