#include <windows.h>
#include <urlmon.h>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include <TlHelp32.h>

#pragma comment(lib, "urlmon.lib")

    // 下载文件
    bool DownloadFile(const std::wstring & url, const std::wstring & savePath) {
        HRESULT hr = URLDownloadToFile(NULL, url.c_str(), savePath.c_str(), 0, NULL);
        return SUCCEEDED(hr);
    }

    // 获取进程ID
    DWORD GetProcessID(const std::wstring & processName) {
        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnap == INVALID_HANDLE_VALUE) return 0;

        PROCESSENTRY32 pe;
        pe.dwSize = sizeof(PROCESSENTRY32);

        if (Process32First(hSnap, &pe)) {
            do {
                if (processName == pe.szExeFile) {
                    CloseHandle(hSnap);
                    return pe.th32ProcessID;
                }
            } while (Process32Next(hSnap, &pe));
        }

        CloseHandle(hSnap);
        return 0;
    }

    // 删除文件
    bool DeleteFileIfExists(const std::wstring & filePath) {
        return DeleteFile(filePath.c_str()) || GetLastError() == ERROR_FILE_NOT_FOUND;
    }

    // 终止进程
    bool TerminateProcessByID(DWORD processID) {
        HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processID);
        if (hProcess == NULL) return false;

        BOOL result = TerminateProcess(hProcess, 0);
        CloseHandle(hProcess);
        return result;
    }

    void CheckAndUpdateSoftware() {
         DWORD processID = GetProcessID(L"自动更新.exe"); 
         const std::wstring softwarePath = L"C:\\Users\\4911\\Downloads\\更新器\\自动更新.exe"; 
         const std::wstring downloadUrl = L"https://github.com/hiddify/hiddify-app/releases/download/v2.5.7/Hiddify-Windows-Setup-x64.exe";

        if (processID == 0) {
            if (DeleteFileIfExists(softwarePath)) {
                if (DownloadFile(downloadUrl, softwarePath)) {
                    std::wcout << L"软件下载成功，启动新软件..." << std::endl;
                    ShellExecute(NULL, L"open", softwarePath.c_str(), NULL, NULL, SW_SHOWNORMAL);  // 启动新软件
                    ExitProcess(0);
                }
                else {
                    std::wcout << L"更新失败" << std::endl;
                    ExitProcess(0);
                }
            }
        }
        else {
            if (TerminateProcessByID(processID)) {
                if (DeleteFileIfExists(softwarePath)) {
                    if (DownloadFile(downloadUrl, softwarePath)) {
                        std::wcout << L"软件下载成功，启动新软件..." << std::endl;
                        ShellExecute(NULL, L"open", softwarePath.c_str(), NULL, NULL, SW_SHOWNORMAL);  // 启动新软件
                        ExitProcess(0); 
                    }
                    else {
                        std::wcout << L"更新失败" << std::endl;
                        ExitProcess(0);
                    }
                }
            }
        }
    }

    // 定时器
    void TimerThread() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // 每秒执行一次
            CheckAndUpdateSoftware();
        }
    }

    int main() {
        // 定时器
        std::thread timerThread(TimerThread);
        timerThread.detach();  


        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));  // 循环
        }

        return 0;
    }
