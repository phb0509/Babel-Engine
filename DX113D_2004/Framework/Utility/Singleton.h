#pragma once

template <class T>
class Singleton
{
protected:
	static T* instance;

public:
	static T* Get()
	{
		if (instance == nullptr)
			instance = new T();

		return instance;
	}

	static void Delete()
	{
		delete instance;
	}
};

template<class T>
T* Singleton<T>::instance = nullptr;