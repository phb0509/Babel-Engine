#pragma once

class GameManager : public Singleton<GameManager>
{
private:
	friend class Singleton;

	GameManager();
	~GameManager();


public:

	Player* GetPlayer() { return player; }
	vector<Monster*> GetMonsters() { return monsters; }
	map<Monster*, bool> GetHitCheckMap() { return monstersHitCheck; }
	void SetHitCheckMap(Monster* monster, bool hitCheck) { monstersHitCheck[monster] = hitCheck; }
	void Export();
	void SetDraggedFileList(vector<wstring>& draggedFileList) { mDraggedFileList = draggedFileList; }
	vector<wstring>& GetDraggedFileList() { return mDraggedFileList; }
	void SetWindowDropEvent(CallBack dropEvent) { mWindowDropEvents.emplace_back(dropEvent); }
	void PlayDropEvents();

private:
	Player* player;
	Monster* monster;
	vector<Monster*> monsters;
	map<Monster*, bool> monstersHitCheck;
	vector<wstring> mDraggedFileList;
	vector<CallBack> mWindowDropEvents;

};