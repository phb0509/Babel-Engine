#pragma once

class TerrainData
{
private:
	friend class QuadTreeTerrain;
	typedef VertexUVNormal VertexType;

	Texture* heightMap;

	vector<VertexType> prevData;
	vector<VertexType> vertices;
	vector<UINT> indices;

	UINT width, height;

public:
	TerrainData(wstring heightFile);
	~TerrainData();

private:
	void CreateData();
	void CreateNormal();
	void AlignVertexData();
};