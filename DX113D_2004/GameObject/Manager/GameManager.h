#pragma once

class GameManager : public Singleton<GameManager>
{
private:
	friend class Singleton;

	GameManager();
	~GameManager();

public:

	Player* GetPlayer() { return mPlayer; }
	void SetDraggedFileList(vector<wstring>& draggedFileList) { mDraggedFileList = draggedFileList; }
	vector<wstring>& GetDraggedFileList() { return mDraggedFileList; }
	void SetWindowDropEvent(CallBack dropEvent) { mWindowDropEvents.emplace_back(dropEvent); }
	void ExecuteDropEvents();

private:
	Player* mPlayer;
	vector<wstring> mDraggedFileList;
	vector<CallBack> mWindowDropEvents;
};