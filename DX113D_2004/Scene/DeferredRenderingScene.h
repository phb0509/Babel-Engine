#pragma once

class DeferredRenderingScene : public Scene
{
private:
	ModelObject* bunny;
	ModelObject* plane;
	ModelAnimObject* groot;
	Sphere* sphere;

	GBuffer* gBuffer;

	Material* material;
	VertexBuffer* vertexBuffer;
public:
	DeferredRenderingScene();
	~DeferredRenderingScene();

	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;

private:
	void CreateModels();
	void CreateMesh();
};