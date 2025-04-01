#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <thread>
#include <chrono>
#include <string>

#include "helper.h"

bool InjectDLL(DWORD processId, const wchar_t* dllPath) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (!hProcess) return false;

    void* allocatedMemory = VirtualAllocEx(hProcess, nullptr, (wcslen(dllPath) + 1) * sizeof(wchar_t), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!allocatedMemory) {
        CloseHandle(hProcess);
        return false;
    }

    if (!WriteProcessMemory(hProcess, allocatedMemory, dllPath, (wcslen(dllPath) + 1) * sizeof(wchar_t), nullptr)) {
        VirtualFreeEx(hProcess, allocatedMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
    
    if (!hKernel32) {
        VirtualFreeEx(hProcess, allocatedMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    LPTHREAD_START_ROUTINE loadLibraryAddr = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "LoadLibraryW");
    if (!loadLibraryAddr) {
        VirtualFreeEx(hProcess, allocatedMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0, loadLibraryAddr, allocatedMemory, 0, nullptr);
    if (!hThread) {
        VirtualFreeEx(hProcess, allocatedMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    VirtualFreeEx(hProcess, allocatedMemory, 0, MEM_RELEASE);
    CloseHandle(hProcess);

    return true;
}

int main() {
    std::wcout << L"[INDEX] Process Name $ ";
    std::wstring procInput;
    std::wcin >> procInput;
    const wchar_t* proc = procInput.c_str();

    std::wcout << L"[INDEX] DLL Name $ ";
    std::wstring dllInput;
    std::wcin >> dllInput;
    const wchar_t* dllName = dllInput.c_str();

    if (dllInput.empty()) {
        std::wcerr << L"[Error] DLL Not Integrated." << std::endl;
        std::wcout << L"Press any key to exit..." << std::endl;
        std::wcin.get();
        return 1;
    }    

    wchar_t dllPath[MAX_PATH];
    if (!GetFullPathNameW(dllName, MAX_PATH, dllPath, nullptr)) {
        std::wcerr << L"[Error] Failed to get full path of " << dllName << std::endl;
        return 1;
    }

    std::wcout << L"[INFO] Searching for process: " << proc << std::endl;
    DWORD processId = pidconverter(proc);

    if (!processId) {
        std::wcerr << L"[Error] Process not found: " << proc << std::endl;
        std::wcout << L"Press any key to exit..." << std::endl;
        std::wcin.get();
        return 1;
    }

    std::wcout << L"[INFO] Process found: " << proc << L" (PID: " << processId << L")" << std::endl;
    std::wcout << L"[INFO] Injecting DLL: " << dllPath << std::endl;

    if (InjectDLL(processId, dllPath)) {
        std::wcout << L"[INFO] DLL injected successfully" << std::endl;
    } else {
        std::wcerr << L"[ERROR] DLL injection failed." << std::endl;
        return 1;
    }

    std::wcout << L"Press any key to exit..." << std::endl;
    std::wcin.get();
    return 0;
}