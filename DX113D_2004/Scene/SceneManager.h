#pragma once

class SceneManager : public Singleton<SceneManager>
{
private:
	friend class Singleton;

	map<string, Scene*> scenes;

	vector<Scene*> playScene;

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
};