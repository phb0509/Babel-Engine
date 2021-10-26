#include "Framework.h"

ModelExporter::ModelExporter(string file) : boneCount(0)
{
	importer = new Assimp::Importer();

	scene = importer->ReadFile(file,
		aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_MaxQuality); //FBX���� ����.
	assert(scene != nullptr);
}

ModelExporter::~ModelExporter()
{
	delete importer;
}

void ModelExporter::ExportMaterial(string savePath)
{
	ReadMaterial();
	savePath = "ModelData/Materials/" + savePath + ".mat";
	WriteMaterial(savePath);
}

void ModelExporter::ExportMesh(string savePath)
{
	ReadNode(scene->mRootNode, -1, -1);
	ReadMesh(scene->mRootNode);
	savePath = "ModelData/Meshes/" + savePath + ".mesh";
	WriteMesh(savePath);
}

void ModelExporter::ExportClip(string savePath)
{
	for (UINT i = 0; i < scene->mNumAnimations; i++) // ���� �ִϸ��̼� �ϳ��� �����ϴϱ� ���� 1�̱���.
	{
		Clip* clip = ReadClip(scene->mAnimations[i]);
		string path = "ModelData/Clips/" + savePath + to_string(i) + ".clip"; // ����Ʈ������ OnDamage0.clip ����.
		WriteClip(clip, path);
	}
}

void ModelExporter::ReadMaterial()
{
	for (UINT i = 0; i < scene->mNumMaterials; i++)
	{
		aiMaterial* srcMaterial = scene->mMaterials[i];
		MaterialData* material = new MaterialData();

		material->name = srcMaterial->GetName().C_Str();

		aiColor3D color;

		srcMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color);
		material->ambient = Float4(color.r, color.g, color.b, 1.0f);

		srcMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		material->diffuse = Float4(color.r, color.g, color.b, 1.0f);

		srcMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color);
		material->specular = Float4(color.r, color.g, color.b, 1.0f);

		srcMaterial->Get(AI_MATKEY_SHININESS, material->specular.w);

		srcMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, color);
		material->emissive = Float4(color.r, color.g, color.b, 1.0f);

		aiString file;

		srcMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &file);
		material->diffuseMap = file.C_Str();

		file.Clear();
		srcMaterial->GetTexture(aiTextureType_SPECULAR, 0, &file);
		material->specularMap = file.C_Str();

		file.Clear();
		srcMaterial->GetTexture(aiTextureType_NORMALS, 0, &file);
		material->normalMap = file.C_Str();

		materials.emplace_back(material);
	}
}

void ModelExporter::WriteMaterial(string savePath)
{
	CreateFolders(savePath);

	XmlDocument* document = new XmlDocument();

	XmlElement* root = document->NewElement("Materials");
	document->LinkEndChild(root);

	string folder = GetDirectoryName(savePath);

	for (MaterialData* material : materials)
	{
		XmlElement* node = document->NewElement("Material");
		root->LinkEndChild(node);

		XmlElement* element = document->NewElement("Name");
		element->SetText(material->name.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("DiffuseMap");
		element->SetText(CreateTexture(folder, material->diffuseMap).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("SpecularMap");
		element->SetText(CreateTexture(folder, material->specularMap).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("NormalMap");
		element->SetText(CreateTexture(folder, material->normalMap).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("Ambient");
		element->SetAttribute("R", material->ambient.x);
		element->SetAttribute("G", material->ambient.y);
		element->SetAttribute("B", material->ambient.z);
		element->SetAttribute("A", material->ambient.w);
		node->LinkEndChild(element);

		element = document->NewElement("Diffuse");
		element->SetAttribute("R", material->diffuse.x);
		element->SetAttribute("G", material->diffuse.y);
		element->SetAttribute("B", material->diffuse.z);
		element->SetAttribute("A", material->diffuse.w);
		node->LinkEndChild(element);

		element = document->NewElement("Specular");
		element->SetAttribute("R", material->specular.x);
		element->SetAttribute("G", material->specular.y);
		element->SetAttribute("B", material->specular.z);
		element->SetAttribute("A", material->specular.w);
		node->LinkEndChild(element);

		element = document->NewElement("Emissive");
		element->SetAttribute("R", material->emissive.x);
		element->SetAttribute("G", material->emissive.y);
		element->SetAttribute("B", material->emissive.z);
		element->SetAttribute("A", material->emissive.w);
		node->LinkEndChild(element);

		delete material;
	}
	materials.clear();

	document->SaveFile(savePath.c_str());

	delete document;
}

string ModelExporter::CreateTexture(string savePath, string file)
{
	if (file.length() == 0)
		return "";

	string fileName = GetFileNameWithoutExtension(file) + ".png";
	const aiTexture* texture = scene->GetEmbeddedTexture(file.c_str());

	string path = savePath + fileName;

	if (ExistFile(path))
		return fileName;

	if (texture)
	{
		if (texture->mHeight < 1)
		{
			BinaryWriter w(path);
			w.Byte(texture->pcData, texture->mWidth);
		}
		else
		{
			Image image;

			image.width = texture->mWidth;
			image.height = texture->mHeight;
			image.pixels = reinterpret_cast<uint8_t*>(texture->pcData);
			image.format = DXGI_FORMAT_R8G8B8A8_UNORM;
			image.rowPitch = image.width * 4;
			image.slicePitch = image.width * image.height * 4;

			SaveToWICFile(image, WIC_FLAGS_NONE, GetWICCodec(WIC_CODEC_PNG),
				ToWString(path).c_str());
		}
	}

	return fileName;
}

void ModelExporter::ReadMesh(aiNode* node)
{
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		MeshData* mesh = new MeshData();
		mesh->name = node->mName.C_Str();

		UINT index = node->mMeshes[i];
		aiMesh* srcMesh = scene->mMeshes[index];

		aiMaterial* material = scene->mMaterials[srcMesh->mMaterialIndex];
		mesh->materialName = material->GetName().C_Str();

		UINT startVertex = mesh->vertices.size();

		vector<VertexWeights> vertexWeights;
		vertexWeights.resize(srcMesh->mNumVertices);

		ReadBone(srcMesh, vertexWeights);

		mesh->vertices.resize(srcMesh->mNumVertices);
		for (UINT v = 0; v < srcMesh->mNumVertices; v++)
		{
			ModelVertex vertex;
			memcpy(&vertex.position, &srcMesh->mVertices[v], sizeof(Float3));

			if (srcMesh->HasTextureCoords(0))
				memcpy(&vertex.uv, &srcMesh->mTextureCoords[0][v], sizeof(Float2));

			if (srcMesh->HasNormals())
				memcpy(&vertex.normal, &srcMesh->mNormals[v], sizeof(Float3));

			if (srcMesh->HasTangentsAndBitangents())
				memcpy(&vertex.tangent, &srcMesh->mTangents[v], sizeof(Float3));

			if (!vertexWeights.empty())
			{
				vertexWeights[v].Normalize();

				vertex.indices.x = (float)vertexWeights[v].indices[0];
				vertex.indices.y = (float)vertexWeights[v].indices[1];
				vertex.indices.z = (float)vertexWeights[v].indices[2];
				vertex.indices.w = (float)vertexWeights[v].indices[3];

				vertex.weights.x = vertexWeights[v].weights[0];
				vertex.weights.y = vertexWeights[v].weights[1];
				vertex.weights.z = vertexWeights[v].weights[2];
				vertex.weights.w = vertexWeights[v].weights[3];
			}
					
			mesh->vertices[v] = vertex;
		}

		for (UINT f = 0; f < srcMesh->mNumFaces; f++)
		{
			aiFace& face = srcMesh->mFaces[f];

			for (UINT k = 0; k < face.mNumIndices; k++)
			{
				mesh->indices.emplace_back(face.mIndices[k]);
				mesh->indices.back() += startVertex;
			}
		}

		meshes.emplace_back(mesh);
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
		ReadMesh(node->mChildren[i]);
}

void ModelExporter::ReadNode(aiNode* node, int index, int parent)
{
	NodeData* nodeData = new NodeData();
	nodeData->index = index;
	nodeData->parent = parent;
	nodeData->name = node->mName.C_Str();

	Matrix matrix(node->mTransformation[0]);
	matrix = XMMatrixTranspose(matrix);
	XMStoreFloat4x4(&nodeData->transform, matrix);

	nodes.emplace_back(nodeData);

	for (UINT i = 0; i < node->mNumChildren; i++)
		ReadNode(node->mChildren[i], nodes.size(), index);
}

void ModelExporter::ReadBone(aiMesh* mesh, vector<VertexWeights>& vertexWeights)
{
	for (UINT i = 0; i < mesh->mNumBones; i++)
	{
		UINT boneIndex = 0;
		string name = mesh->mBones[i]->mName.C_Str();

		if (boneMap.count(name) == 0)
		{
			boneIndex = boneCount++;

			boneMap[name] = boneIndex;

			BoneData* boneData = new BoneData();
			boneData->name = name;
			boneData->index = boneIndex;
			Matrix matrix(mesh->mBones[i]->mOffsetMatrix[0]);
			matrix = XMMatrixTranspose(matrix);
			XMStoreFloat4x4(&boneData->offset, matrix);

			bones.emplace_back(boneData);
		}
		else
		{
			boneIndex = boneMap[name];
		}

		for (UINT j = 0; j < mesh->mBones[i]->mNumWeights; j++)
		{
			UINT index = mesh->mBones[i]->mWeights[j].mVertexId;
			vertexWeights[index].Add(boneIndex, mesh->mBones[i]->mWeights[j].mWeight);
		}
	}
}

void ModelExporter::WriteMesh(string savePath)
{
	CreateFolders(savePath);

	BinaryWriter* w = new BinaryWriter(savePath);

	w->UInt(meshes.size());
	for (MeshData* mesh : meshes)
	{
		w->String(mesh->name);
		w->String(mesh->materialName);

		w->UInt(mesh->vertices.size());
		w->Byte(mesh->vertices.data(), sizeof(ModelVertex) * mesh->vertices.size());

		w->UInt(mesh->indices.size());
		w->Byte(mesh->indices.data(), sizeof(UINT) * mesh->indices.size());

		delete mesh;
	}
	meshes.clear();

	w->UInt(nodes.size());
	for (NodeData* node : nodes)
	{
		w->Int(node->index);
		w->String(node->name);
		w->Int(node->parent);
		w->Float4x4(node->transform);

		delete node;
	}
	nodes.clear();

	w->UInt(bones.size());
	for (BoneData* bone : bones)
	{
		w->String(bone->name);
		w->Int(bone->index);
		w->Float4x4(bone->offset);

		delete bone;
	}
	bones.clear();

	delete w;
}

Clip* ModelExporter::ReadClip(aiAnimation* animation)
{
	Clip* clip = new Clip();
	clip->name = animation->mName.C_Str();
	clip->tickPerSecond = (float)animation->mTicksPerSecond; // FramePerSecond.
	clip->frameCount = (UINT)animation->mDuration + 1; // +1 �ϴ� ���� : 
	clip->duration = 0.0f;

	vector<ClipNode> clipNodes;
	for (UINT i = 0; i < animation->mNumChannels; i++) // �ִϸ��̼��� Node����. ��, Bone �������� ����ִ�.
	{
		aiNodeAnim* srcNode = animation->mChannels[i];

		ClipNode node; // ����� ��屸��ü.
		node.name = srcNode->mNodeName;

		UINT keyCount = max(srcNode->mNumPositionKeys, srcNode->mNumRotationKeys);
		keyCount = max(keyCount, srcNode->mNumScalingKeys); // �� Ű�鸶�� Ű���� �ϴ� �޶� ���� ������ �������� ��������.

		KeyTransform transform;
		for (UINT k = 0; k < keyCount; k++)
		{
			bool isFound = false;
			UINT t = node.keyFrame.size(); // ���� ������ ����.

			if (k < srcNode->mNumPositionKeys && // ���� ������(t)�� Position�� �������� ���ٸ�.
				abs((float)srcNode->mPositionKeys[k].mTime - (float)t) <= FLT_EPSILON) // ���Ƿа� �񱳷� üũ.
			{
				aiVectorKey key = srcNode->mPositionKeys[k];
				memcpy_s(&transform.position, sizeof(Float3),
					&key.mValue, sizeof(aiVector3D));
				transform.time = (float)srcNode->mPositionKeys[k].mTime;

				isFound = true;
			}

			if (k < srcNode->mNumRotationKeys &&
				abs((float)srcNode->mRotationKeys[k].mTime - (float)t) <= FLT_EPSILON)
			{
				aiQuatKey key = srcNode->mRotationKeys[k];
				
				transform.rotation.x = (float)key.mValue.x;
				transform.rotation.y = (float)key.mValue.y;
				transform.rotation.z = (float)key.mValue.z;
				transform.rotation.w = (float)key.mValue.w;

				transform.time = (float)srcNode->mRotationKeys[k].mTime;

				isFound = true;
			}

			if (k < srcNode->mNumScalingKeys && 
				abs((float)srcNode->mScalingKeys[k].mTime - (float)t) <= FLT_EPSILON)
			{
				aiVectorKey key = srcNode->mScalingKeys[k];
				memcpy_s(&transform.scale, sizeof(Float3),
					&key.mValue, sizeof(aiVector3D));
				transform.time = (float)srcNode->mScalingKeys[k].mTime;

				isFound = true;
			}

			if (isFound) // 3���� üũ���� �ϳ��� �����Ͱ� �ִٸ� Ű������ ������ ���Ϳ� ����.
				node.keyFrame.emplace_back(transform);
		}//KeyTransform


		if (node.keyFrame.size() < clip->frameCount) // Position, Rotation, Scale�� �����Ͱ� ������ �κ��� ä���ش�.
		{
			UINT count = clip->frameCount - node.keyFrame.size(); // ������ ��� ��������.
			
			KeyTransform keyTransform = node.keyFrame.back(); // ���������� �����ؼ� �� ä�����.

			for (UINT n = 0; n < count; n++) // ������ ���� ������ �����Ӹ�ŭ ������ ä���.
				node.keyFrame.emplace_back(keyTransform);
		}

		clip->duration = max(clip->duration, node.keyFrame.back().time); // Ŭ�� �������

		clipNodes.emplace_back(node); // �츮�� ����� ����ü�� ����.
	}//Bone

	ReadKeyFrame(clip, scene->mRootNode, clipNodes); // ��͸� Ÿ�鼭 ��� �� �������鼭 Ű������ �����Ϳ� ����.

	return clip;
}

void ModelExporter::ReadKeyFrame(Clip* clip, aiNode* node, vector<ClipNode>& clipNodes)
{
	KeyFrame* keyFrame = new KeyFrame();
	keyFrame->boneName = node->mName.C_Str();

	for (UINT i = 0; i < clip->frameCount; i++) // Ŭ���� �����Ӹ�ŭ
	{
		ClipNode* clipNode = nullptr;

		for (ClipNode& temp : clipNodes) // ���� Ű������ size ��ŭ 
		{
			if (temp.name == node->mName) // ��ġ�ϴ� ���� ������ ������.
			{
				clipNode = &temp;
				break;
			}
		}

		KeyTransform keyTransform;
		if (clipNode == nullptr) // ��ġ�ϴ� ���� ���ٸ�
		{
			// ���� ä���ش�.
			Matrix transform(node->mTransformation[0]);
			transform = XMMatrixTranspose(transform);

			Vector3 S, T;
			Vector4 R;
			XMMatrixDecompose(&S.data, &R, &T.data, transform);
			keyTransform.scale = S;
			XMStoreFloat4(&keyTransform.rotation, R);
			keyTransform.position = T;

			keyTransform.time = (float)i;
		}
		else // ��ġ�ϴ� ���� �ִٸ�
		{
			keyTransform = clipNode->keyFrame[i];
		}
		keyFrame->transforms.emplace_back(keyTransform);
	}
	clip->keyFrame.emplace_back(keyFrame);

	for (UINT i = 0; i < node->mNumChildren; i++) // ���
		ReadKeyFrame(clip, node->mChildren[i], clipNodes);
}

void ModelExporter::WriteClip(Clip* clip, string savePath)
{
	CreateFolders(savePath);

	BinaryWriter* w = new BinaryWriter(savePath);

	w->String(clip->name);
	w->Float(clip->duration);
	w->Float(clip->tickPerSecond);
	w->UInt(clip->frameCount);

	w->UInt(clip->keyFrame.size());
	for (KeyFrame* keyFrame : clip->keyFrame)
	{
		w->String(keyFrame->boneName);

		w->UInt(keyFrame->transforms.size());
		w->Byte(keyFrame->transforms.data(), sizeof(KeyTransform) * keyFrame->transforms.size());

		delete keyFrame;
	}

	clip->keyFrame.clear();

	delete clip;
	delete w;
}
