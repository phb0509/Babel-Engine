#pragma once

class Transform;
class Vector3;


namespace utility
{
	string ToString(wstring value);
	wstring ToWString(string value);

	void Replace(string* str, string comp, string rep);
	vector<string> SplitString(string origin, string tok);

	wstring GetExtension(wstring path);
	string GetExtension(string path);
	string GetDirectoryName(string path);
	string GetFileName(string path);
	string GetFileNameWithoutExtension(string path);
	string OpenFileDialog();
	char* ToCharPointer(string string);

	void CreateFolders(string path);
	void IndentRepeatedly(int loopCount);
	void UnIndentRepeatedly(int loopCount);
	void SpacingRepeatedly(int loopCount);

	bool ExistDirectory(string path);
	bool ExistFile(string path);

	HRESULT CDialogEventHandler_CreateInstance(REFIID riid, void** ppv);
	

}