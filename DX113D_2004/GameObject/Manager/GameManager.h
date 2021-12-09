#pragma once

class GameManager : public Singleton<GameManager>
{
private:
	friend class Singleton;

	GameManager();
	~GameManager();


public:

	Player* GetPlayer() { return mPlayer; }
	vector<Monster*> GetMonsters() { return mMonsters; }
	map<Monster*, bool> GetHitCheckMap() { return mMonstersHitCheck; }
	void SetHitCheckMap(Monster* mMonster, bool hitCheck) { mMonstersHitCheck[mMonster] = hitCheck; }
	void SetDraggedFileList(vector<wstring>& draggedFileList) { mDraggedFileList = draggedFileList; }
	vector<wstring>& GetDraggedFileList() { return mDraggedFileList; }
	void SetWindowDropEvent(CallBack dropEvent) { mWindowDropEvents.emplace_back(dropEvent); }
	void PlayDropEvents();

private:
	Player* mPlayer;
	Monster* mMonster;
	vector<Monster*> mMonsters;
	map<Monster*, bool> mMonstersHitCheck;
	vector<wstring> mDraggedFileList;
	vector<CallBack> mWindowDropEvents;
};