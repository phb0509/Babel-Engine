#pragma once

class TessellationScene : public Scene
{
private:
	Material* material;
	HullShader* hullShader;
	DomainShader* domainShader;

	VertexBuffer* vertexBuffer;

	RasterizerState* rsState;

	TypeBuffer* edgeBuffer;
	TypeBuffer* insideBuffer;
public:
	TessellationScene();
	~TessellationScene();

	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
};