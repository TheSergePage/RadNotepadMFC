#include "stdafx.h"
#include "Tools.h"
#include "OutputWnd.h"

// TODO
// Load from registry
// Save before execute
// Stop too many outputting to the same window at the same time

void InitTools(std::vector<Tool>& rTools)
{
    // TODO Load form registry
    rTools.push_back(Tool(_T("Run"), _T("{file}")));
    rTools.push_back(Tool(_T("CMD"), _T("cmd.exe")));
    rTools.push_back(Tool(_T("Explorer"), _T("explorer.exe"), _T("/select,\"{file}\"")));
    rTools.push_back(Tool(_T("Google"), _T("https://www.google.com.au/search?q={selected}")));
    rTools.push_back(Tool(_T("FindStr"), _T("findstr.exe"), _T("/L /S /N \"{selected}\" *.txt *.cpp *.h *.java"), TRUE));
    rTools.push_back(Tool(_T("VCMake"), _T("%HOMEDRIVE%%HOMEPATH%\\Dropbox\\Utils\\CommandLine\\bin\\vcmake.bat"), _T(""), TRUE));
    rTools.push_back(Tool(_T("Output"), _T("cmd.exe"), _T("/C type {file}"), TRUE));

    for (Tool& t : rTools)
    {
        TCHAR exe[MAX_PATH];
        PTSTR pArgs = PathGetArgs(t.cmd);
        while (pArgs > t.cmd && pArgs[-1] == _T(' '))
            --pArgs;
        wcsncpy_s(exe, t.cmd, pArgs - t.cmd);
        PathUnquoteSpaces(exe);

        if (t.hIcon == NULL)
            //t.hIcon = ExtractIcon(AfxGetInstanceHandle(), t.cmd, 0);
            ExtractIconEx(exe, 0, nullptr, &t.hIcon, 1);
#if 0
        if (t.hIcon == NULL)
        {
            SHFILEINFO fi = {};
            SHGetFileInfo(exe, 0, &fi, sizeof(fi), SHGFI_ICON | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES);
            t.hIcon = fi.hIcon;
        }
#endif
    }
}
struct CaptureOutputData
{
    HANDLE hProcess;
    HANDLE hRead;
    COutputWnd* pWndOutput; // TODO Maybe into the Scintilla control
};

static DWORD WINAPI CaptureOutput(LPVOID lpParameter)
{
    CaptureOutputData* pData = reinterpret_cast<CaptureOutputData*>(lpParameter);

    char Buffer[1024];
    DWORD dwRead = 0;
    while (ReadFile(pData->hRead, Buffer, ARRAYSIZE(Buffer), &dwRead, nullptr))
    {
        COutputList* pOutputList = pData->pWndOutput->Get(OW_OUTPUT);
        if (pOutputList)
            pOutputList->AppendText(Buffer, dwRead);
    }
    CloseHandle(pData->hRead);

    DWORD dwExitCode = 0;
    GetExitCodeProcess(pData->hProcess, &dwExitCode);
    CString msg;
    msg.Format(_T("Exit code: %d\n"), dwExitCode);
    COutputList* pOutputList = pData->pWndOutput->Get(OW_OUTPUT);
    if (pOutputList)
        pOutputList->AppendText(msg);

    CloseHandle(pData->hProcess);

    delete pData;

    return 0;
}

void ExecuteTool(const Tool& tool, const ToolExecuteData& ted)
{
    if (tool.bCapture)
    {
        SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE };

        HANDLE hRead, hWrite;
        CreatePipe(&hRead, &hWrite, &sa, 1024);

        STARTUPINFO si = { sizeof(STARTUPINFO) };
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdOutput = hWrite;
        si.hStdError = hWrite;
        PROCESS_INFORMATION pi = {};
        TCHAR cmdline[MAX_PATH] = _T("");
        StrCpy(cmdline, ted.cmd);
        StrCat(cmdline, _T(" "));
        StrCat(cmdline, ted.param);
        if (CreateProcess(nullptr, cmdline, nullptr, nullptr, TRUE, CREATE_NO_WINDOW, nullptr, ted.directory, &si, &pi))
        {
            ted.pWndOutput->ShowPane(TRUE, FALSE, FALSE);
            COutputList* pOutputList = ted.pWndOutput->Get(OW_OUTPUT);
            if (pOutputList)
            {
                pOutputList->SetDirectory(ted.directory);
                pOutputList->Clear();
                pOutputList->AppendText(_T("Execute: "), -1);
                pOutputList->AppendText(cmdline, -1);
                pOutputList->AppendText(_T("\n"), -1);
            }

            CloseHandle(hWrite);
            CloseHandle(pi.hThread);

            CaptureOutputData* cd = new CaptureOutputData;
            cd->hProcess = pi.hProcess;
            cd->hRead = hRead;
            cd->pWndOutput = ted.pWndOutput;
            if (CreateThread(nullptr, 0, CaptureOutput, cd, 0, nullptr) == NULL)
            {
                CloseHandle(hRead);
                CloseHandle(pi.hProcess);

                CString msg;
                msg.Format(_T("Error CreateThread: %d"), GetLastError());
                AfxMessageBox(msg, MB_ICONSTOP | MB_OK);
            }
        }
        else
        {
            CloseHandle(hRead);
            CloseHandle(hWrite);

            CString msg;
            msg.Format(_T("Error CreateProcess: %d"), GetLastError());
            AfxMessageBox(msg, MB_ICONSTOP | MB_OK);
        }
    }
    else
    {
        HINSTANCE hExeInst = ShellExecute(AfxGetMainWnd()->GetSafeHwnd(), L"open", ted.cmd, ted.param, ted.directory, SW_SHOW);
        if (hExeInst <= HINSTANCE(32))
        {
            CString msg;
            msg.Format(_T("Error ShellExecute: %d"), reinterpret_cast<INT_PTR>(hExeInst));
            AfxMessageBox(msg, MB_ICONSTOP | MB_OK);
        }
    }
}
