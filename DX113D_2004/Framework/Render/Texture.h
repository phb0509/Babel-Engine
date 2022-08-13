#pragma once

class Texture
{

public:
	static Texture* Add(wstring file);
	static Texture* AddUsingSRV(ID3D11ShaderResourceView* _srv);
	static Texture* Load(wstring file);
	static void Delete();

	void PSSet(UINT slot);
	void DSSet(UINT slot);

	vector<Float4> ReadPixels();

	UINT GetWidth() { return mWidth; }
	UINT GetHeight() { return mHeight; }

	ID3D11ShaderResourceView*& GetSRV() { return mSRV; }
	ID3D11Texture2D* GetTexture() { return mTexture; }

private:
	Texture(ID3D11ShaderResourceView* srv, ScratchImage& image);
	Texture(ID3D11ShaderResourceView* srv);
	~Texture();

	void getImages();

private:
	ScratchImage mImage;
	ID3D11ShaderResourceView* mSRV;
	ID3D11Texture2D* mTexture;
	UINT mWidth;
	UINT mHeight;

	static map<wstring, Texture*> totalTexture;
	static map<ID3D11ShaderResourceView*, Texture*> totalSRVTexture;
	
	vector<Float4> mPixels;
	unsigned char* mCharArray;
};