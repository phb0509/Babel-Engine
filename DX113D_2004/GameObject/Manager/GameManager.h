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



private:
	Player* player;
	Monster* monster;
	vector<Monster*> monsters;
	map<Monster*, bool> monstersHitCheck;

};