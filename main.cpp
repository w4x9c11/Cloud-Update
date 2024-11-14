#include <windows.h>
#include <urlmon.h>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include <TlHelp32.h>

#pragma comment(lib, "urlmon.lib")

    // �����ļ�
    bool DownloadFile(const std::wstring & url, const std::wstring & savePath) {
        HRESULT hr = URLDownloadToFile(NULL, url.c_str(), savePath.c_str(), 0, NULL);
        return SUCCEEDED(hr);
    }

    // ��ȡ����ID
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

    // ɾ���ļ�
    bool DeleteFileIfExists(const std::wstring & filePath) {
        return DeleteFile(filePath.c_str()) || GetLastError() == ERROR_FILE_NOT_FOUND;
    }

    // ��ֹ����
    bool TerminateProcessByID(DWORD processID) {
        HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processID);
        if (hProcess == NULL) return false;

        BOOL result = TerminateProcess(hProcess, 0);
        CloseHandle(hProcess);
        return result;
    }

    void CheckAndUpdateSoftware() {
         DWORD processID = GetProcessID(L"�Զ�����.exe"); 
         const std::wstring softwarePath = L"C:\\Users\\4911\\Downloads\\������\\�Զ�����.exe"; 
         const std::wstring downloadUrl = L"https://github.com/hiddify/hiddify-app/releases/download/v2.5.7/Hiddify-Windows-Setup-x64.exe";

        if (processID == 0) {
            if (DeleteFileIfExists(softwarePath)) {
                if (DownloadFile(downloadUrl, softwarePath)) {
                    std::wcout << L"������سɹ������������..." << std::endl;
                    ShellExecute(NULL, L"open", softwarePath.c_str(), NULL, NULL, SW_SHOWNORMAL);  // ���������
                    ExitProcess(0);
                }
                else {
                    std::wcout << L"����ʧ��" << std::endl;
                    ExitProcess(0);
                }
            }
        }
        else {
            if (TerminateProcessByID(processID)) {
                if (DeleteFileIfExists(softwarePath)) {
                    if (DownloadFile(downloadUrl, softwarePath)) {
                        std::wcout << L"������سɹ������������..." << std::endl;
                        ShellExecute(NULL, L"open", softwarePath.c_str(), NULL, NULL, SW_SHOWNORMAL);  // ���������
                        ExitProcess(0); 
                    }
                    else {
                        std::wcout << L"����ʧ��" << std::endl;
                        ExitProcess(0);
                    }
                }
            }
        }
    }

    // ��ʱ��
    void TimerThread() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // ÿ��ִ��һ��
            CheckAndUpdateSoftware();
        }
    }

    int main() {
        // ��ʱ��
        std::thread timerThread(TimerThread);
        timerThread.detach();  


        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));  // ѭ��
        }

        return 0;
    }
