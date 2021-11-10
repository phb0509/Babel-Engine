#include "Framework.h"
#include "TestScene.h"



const COMDLG_FILTERSPEC c_rgSaveTypes[] =
{
    {L"Word Document (*.doc)",       L"*.doc"},
    {L"Web Page (*.htm; *.html)",    L"*.htm;*.html"},
    {L"Text Document (*.txt)",       L"*.txt"},
    {L"All Documents (*.*)",         L"*.*"}
};


TestScene::TestScene()
{
    IFileDialog* pfd = NULL;

    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&pfd));

    if (SUCCEEDED(hr))
    {
        // Create an event handling object, and hook it up to the dialog.
        IFileDialogEvents* pfde = NULL;
        hr = CDialogEventHandler_CreateInstance(IID_PPV_ARGS(&pfde));
        if (SUCCEEDED(hr))
        {
            // Hook up the event handler.
            DWORD dwCookie;
            hr = pfd->Advise(pfde, &dwCookie);

            if (SUCCEEDED(hr))
            {
                // Set the options on the dialog.
                DWORD dwFlags;

                // Before setting, always get the options first in order 
                // not to override existing options.
                hr = pfd->GetOptions(&dwFlags);
                if (SUCCEEDED(hr))
                {
                    // In this case, get shell items only for file system items.
                    hr = pfd->SetOptions(dwFlags | FOS_FORCEFILESYSTEM);
                    if (SUCCEEDED(hr))
                    {
                        // Set the file types to display only. 
                        // Notice that this is a 1-based array.
                        hr = pfd->SetFileTypes(ARRAYSIZE(c_rgSaveTypes), c_rgSaveTypes); // 확장자 표시하는곳.
                        if (SUCCEEDED(hr))
                        {
                            // Set the selected file type index to Word Docs for this example.
                            hr = pfd->SetFileTypeIndex(INDEX_WORDDOC);
                            if (SUCCEEDED(hr))
                            {
                                // Set the default extension to be ".doc" file.
                                hr = pfd->SetDefaultExtension(L"doc;docx");
                                if (SUCCEEDED(hr))
                                {
                                    // Show the dialog
                                    hr = pfd->Show(NULL);
                                    if (SUCCEEDED(hr))
                                    {
                                        // Obtain the result once the user clicks 
                                        // the 'Open' button.
                                        // The result is an IShellItem object.
                                        IShellItem* psiResult;
                                        hr = pfd->GetResult(&psiResult); 
                                        if (SUCCEEDED(hr))
                                        {
                                            // We are just going to print out the 
                                            // name of the file for sample sake.
                                            
                                            PWSTR pszFilePath = NULL; // 선택된 파일경로.
                                            hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH,&pszFilePath);

                                            if (SUCCEEDED(hr))
                                            {
                                                TaskDialog(NULL,
                                                    NULL,
                                                    L"CommonFileDialogApp", // 파일고르고 확인누르면 뜨는 확인차뜨는 팝업창.
                                                    pszFilePath,
                                                    NULL,
                                                    TDCBF_OK_BUTTON,
                                                    TD_INFORMATION_ICON,
                                                    NULL);
                                                CoTaskMemFree(pszFilePath);
                                            }

                                            psiResult->Release();
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                // Unhook the event handler.
                pfd->Unadvise(dwCookie);
            }
            pfde->Release();
        }
        pfd->Release();
    }
}

TestScene::~TestScene()
{
}

void TestScene::Update()
{

}

void TestScene::PreRender()
{
}

void TestScene::Render()
{

}

void TestScene::PostRender()
{
	
}



HRESULT TestScene::CDialogEventHandler_CreateInstance(REFIID riid, void** ppv)
{
    *ppv = NULL;
    CDialogEventHandler* pDialogEventHandler = new (std::nothrow) CDialogEventHandler();
    HRESULT hr = pDialogEventHandler ? S_OK : E_OUTOFMEMORY;

    if (SUCCEEDED(hr))
    {
        hr = pDialogEventHandler->QueryInterface(riid, ppv);
        pDialogEventHandler->Release();
    }

    return hr;
}