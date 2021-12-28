#include "Framework.h"

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
    for (auto scene : mScenes)
        delete scene.second;
}

void SceneManager::Update()
{
    for (Scene* scene : mPlayedScenes)
        scene->Update();
}

void SceneManager::PreRender()
{
    for (Scene* scene : mPlayedScenes)
        scene->PreRender();
}

void SceneManager::Render()
{
    for (Scene* scene : mPlayedScenes)
        scene->Render();
}

void SceneManager::PostRender()
{
    for (Scene* scene : mPlayedScenes)
        scene->PostRender();
}

Scene* SceneManager::Add(string key, Scene* scene)
{
    if (mScenes.count(key) > 0)
        return mScenes[key];

    mScenes[key] = scene;

    return scene;
}

Scene* SceneManager::AddScene(string key)
{
    mPlayedScenes.emplace_back(mScenes[key]);

    return mScenes[key];
}

void SceneManager::DeleteScene(string key)
{
    vector<Scene*>::iterator iter = mPlayedScenes.begin();

    while (iter != mPlayedScenes.end())
    {
        if ((*iter) == mScenes[key])
        {
            iter = mPlayedScenes.erase(iter);
        }
        else
        {
            iter++;
        }
    }
}