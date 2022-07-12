#pragma once

template <class T>
class Singleton
{

public:
	static T* Get()
	{
		if (mInstance == nullptr)
			mInstance = new T();

		return mInstance;
	}

	static void Delete()
	{
		delete mInstance;
	}

protected:
	static T* mInstance;

};

template<class T>
T* Singleton<T>::mInstance = nullptr;