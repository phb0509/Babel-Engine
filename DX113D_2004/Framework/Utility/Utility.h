#pragma once

class Transform;
class Vector3;


namespace Utility
{
	string ToString(wstring value);
	wstring ToWString(string value);

	void Replace(string* str, string comp, string rep);
	vector<string> SplitString(string origin, string tok);

	wstring GetExtension(wstring path);
	string GetDirectoryName(string path);
	string GetFileName(string path);
	string GetFileNameWithoutExtension(string path);

	void CreateFolders(string path);
	void ExecuteFunction(function<void(Transform*, Vector3)> func, float period);
	
	bool ExistDirectory(string path);
	bool ExistFile(string path);


	static float mFuncExecuteTime = 0.0f;
	static bool mbIsUpdateStandTime = true;
}