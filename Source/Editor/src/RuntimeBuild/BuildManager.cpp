//BuildManager.cpp
#include "SurvivantEditor/RuntimeBuild/BuildManager.h"

#include "SurvivantCore/Utility/LeanWindows.h"
#include <tchar.h>

namespace SvEditor::RuntimeBuild
{
    BuildManager& BuildManager::GetInstance()
    {
        static BuildManager s_instance;

        return s_instance;
    }

    void BuildManager::CreateBuild(const std::string& p_buildFileName, const BuildConfig& /*p_setup*/)
    {
        LPSTR path = const_cast<char*>(p_buildFileName.c_str());

        STARTUPINFOA  si;
        PROCESS_INFORMATION  pi;

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        //si.hStdInput = GetStdHandle(STD_INPUT_HANDLE); heritated already

        if (!CreateProcessA(
            NULL, path, NULL,
            NULL, NULL, FALSE, 0, NULL,
            &si, &pi))
        {
            SV_LOG_ERROR("Can't run build (%s): CreateProcess failed (%d).", p_buildFileName.c_str(), GetLastError());
            return;
        }

        char cmd[] = "echo allo";
        DWORD numWrittenBytes;
        WriteFile(GetStdHandle(STD_INPUT_HANDLE), cmd, sizeof(cmd), &numWrittenBytes, NULL);

        char close[] = "exit";
        WriteFile(GetStdHandle(STD_INPUT_HANDLE), close, sizeof(close), &numWrittenBytes, NULL);

        // Wait until child process exits.
        WaitForSingleObject(pi.hProcess, INFINITE);

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    bool BuildManager::RunBuild(const std::string& p_buildFileName)
    {
        HANDLE readChild, writeParent, readParent, writeChild;

        if (!CreatePipe(&readChild, &writeParent, NULL, 0))
            return false;

        if (!CreatePipe(&readParent, &writeChild, NULL, 0))
            return false;

        if (!SetHandleInformation(readChild, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT))
            return false;

        if (!SetHandleInformation(writeChild, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT))
            return false;


        CHAR cmd[] = "cmd /q"; // /q bcs using pipes
        STARTUPINFOA  si;
        PROCESS_INFORMATION  pi;

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        si.hStdInput = readChild;      
        si.hStdOutput = writeChild;
        si.hStdError = writeChild;
        si.dwFlags = STARTF_USESTDHANDLES;

        if (!CreateProcessA(
            NULL, cmd, NULL,
            NULL, TRUE, CREATE_NO_WINDOW,
            NULL, NULL,
            &si, &pi))
        {
            SV_LOG_ERROR("Can't run build (%s): CreateProcess failed (%d).", p_buildFileName.c_str(), GetLastError());
            return false;
        }

        Sleep(100);
        DWORD count;
        char out[4096];
        ReadFile(readParent, out, sizeof(out), &count, NULL);
        WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), out, count, &count, NULL);

        char findVcvarsall[] = "WHERE /R \"C:\\Program Files\\Microsoft Visual Studio\" /F \"vcvars64.bat\"\n";
        WriteFile(writeParent, findVcvarsall, sizeof(findVcvarsall) - 1, &count, NULL); // -1 to remove \0 from char[]

        char vcvarsallPath[MAX_PATH];
        ReadFile(readParent, vcvarsallPath, sizeof(vcvarsallPath), &count, NULL);
        WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), vcvarsallPath, count, &count, NULL);
        
        WriteFile(writeParent, vcvarsallPath, count, &count, NULL);

        ReadFile(readParent, out, sizeof(out), &count, NULL);
        WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), out, count, &count, NULL);

        char close[] = "exit\n";
        WriteFile(writeParent, close, sizeof(close) - 1, &count, NULL);


        // Wait until child process exits.
        WaitForSingleObject(pi.hProcess, INFINITE);

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        CloseHandle(writeParent);
        CloseHandle(readChild);
        CloseHandle(writeChild);
        CloseHandle(readParent);

        return true;

        //LPSTR path = const_cast<char*>(p_buildFileName.c_str());

        //STARTUPINFOA  si;
        //PROCESS_INFORMATION  pi;

        //ZeroMemory(&si, sizeof(si));
        //si.cb = sizeof(si);
        //ZeroMemory(&pi, sizeof(pi));

        //if(!CreateProcessA(
        //    NULL, path, NULL,
        //    NULL, NULL, FALSE, 0, NULL,
        //    &si, &pi))  
        //{
        //    SV_LOG_ERROR("Can't run build (%s): CreateProcess failed (%d).", p_buildFileName.c_str(), GetLastError());
        //    return;
        //}

        //// Wait until child process exits.
        //WaitForSingleObject(pi.hProcess, INFINITE);

        //CloseHandle(pi.hProcess);
        //CloseHandle(pi.hThread);
    }
}
