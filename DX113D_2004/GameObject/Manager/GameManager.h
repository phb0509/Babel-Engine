#pragma once

class GameManager : public Singleton<GameManager>
{
private:

	friend class Singleton;

	GameManager();
	~GameManager();

public:

	template<class T>
	inline void SafeDelete(T* t)
	{
		delete t;
		t = nullptr;
	}

	template<class T>
	inline void SafeDeleteArray(T* t)
	{
		delete[] t;
		t = nullptr;
	}

	template<class T>
	inline void SafeDeleteVector(vector<T>& t)
	{
		for (int i = 0; i < t.size(); i++)
		{
			delete t[i];
			t[i] = nullptr;
		}
	}

	Player* GetPlayer() { return mPlayer; }
	void SetDraggedFileList(vector<wstring>& draggedFileList) { mDraggedFileList = draggedFileList; }
	vector<wstring>& GetDraggedFileList() { return mDraggedFileList; }
	void SetWindowDropEvent(CallBack dropEvent) { mWindowDropEvents.emplace_back(dropEvent); }
	void ExecuteFileDropEvents(int eventIndex);

	void SetIsHoveredAssetBrowserWindow(bool isHovered) { mbIsHoveredAssetBrowserWindow = isHovered; }
	bool GetIsHoveredAssetBrowserWindow() { return mbIsHoveredAssetBrowserWindow; }

private:
	Player* mPlayer;
	vector<wstring> mDraggedFileList;
	vector<CallBack> mWindowDropEvents; 
	bool mbIsHoveredAssetBrowserWindow;


};


