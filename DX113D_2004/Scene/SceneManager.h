#pragma once

class SceneManager : public Singleton<SceneManager>
{
private:
	friend class Singleton;

	SceneManager();
	~SceneManager();

public:
	void Update();

	void PreRender();
	void Render();
	void PostRender();

	Scene* Add(string key, Scene* scene);
	Scene* AddScene(string key);
	void DeleteScene(string key);

private:
	map<string, Scene*> mScenes;
	vector<Scene*> mPlayedScenes;
};