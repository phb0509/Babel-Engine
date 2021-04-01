#include "Framework.h"

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
    for (auto scene : scenes)
        delete scene.second;
}

void SceneManager::Update()
{
    for (Scene* scene : playScene)
        scene->Update();
}

void SceneManager::PreRender()
{
    for (Scene* scene : playScene)
        scene->PreRender();
}

void SceneManager::Render()
{
    for (Scene* scene : playScene)
        scene->Render();
}

void SceneManager::PostRender()
{
    for (Scene* scene : playScene)
        scene->PostRender();
}

Scene* SceneManager::Add(string key, Scene* scene)
{
    if (scenes.count(key) > 0)
        return scenes[key];

    scenes[key] = scene;

    return scene;
}

Scene* SceneManager::AddScene(string key)
{
    playScene.emplace_back(scenes[key]);

    return scenes[key];
}

void SceneManager::DeleteScene(string key)
{
    vector<Scene*>::iterator iter = playScene.begin();

    while (iter != playScene.end())
    {
        if ((*iter) == scenes[key])
        {
            iter = playScene.erase(iter);
        }
        else
        {
            iter++;
        }
    }
}