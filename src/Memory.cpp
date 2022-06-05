#include "Memory.h"
#include <TlHelp32.h>
#include "Utils.h"
#include <stdio.h>
#include <iostream>

static SIZE_T processID     = 0LL;
static HANDLE processHandle = nullptr;

inline SIZE_T getAowProcessID() {

    constexpr const char processName[] = "aow_exe.exe";

    SIZE_T               procThreadCount = 0L;
    SIZE_T               processID       = 0L;
    HANDLE               processSnap     = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32       processEntry;

    if (processSnap == INVALID_HANDLE_VALUE)
        goto Label;

    processEntry.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(processSnap, &processEntry)) {
        do
        {
            if (strncmp(processEntry.szExeFile, processName, sizeof(processName)) == 0 && processEntry.cntThreads > procThreadCount) {
                processID       = processEntry.th32ProcessID;
                procThreadCount = processEntry.cntThreads;
            }

        } while (Process32Next(processSnap, &processEntry));
    }

    CloseHandle(processSnap);

Label:
    if (processID == NULL) {
        SIZE_T errorCode = GetLastError();
        fatal("processID == NULL", "Unknown", errorCode);
    }

    return processID;
}

HANDLE Memory::getProcessHandle()
{
    SIZE_T errorCode;

    if (processHandle == nullptr) {
        processID     = getAowProcessID();
        processHandle = reinterpret_cast<HANDLE>(OpenProcess(PROCESS_ALL_ACCESS, FALSE, static_cast<DWORD>(processID)));
        if (processHandle == nullptr) {
            errorCode = GetLastError();
            fatal("processHandle == nullptr", "OpenProcess", errorCode);
        }
    }

    return processHandle;
}

BOOL Memory::read(SIZE_T address, PVOID buffer, SIZE_T bufferSize)
{
    return ReadProcessMemory(processHandle, reinterpret_cast<PVOID>(address), buffer, bufferSize, nullptr);
}

BOOL Memory::write(SIZE_T address, PVOID buffer, SIZE_T bufferSize)
{
    return WriteProcessMemory(processHandle, reinterpret_cast<PVOID>(address), buffer, bufferSize, nullptr);
}

SIZE_T Memory::getGameModuleBase()
{
    constexpr BYTE  gameModuleBaseAOB[] = {
        0x7F, 0x45, 0x4C, 0x46, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x03, 0x00, 0x28, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00,
        0x80, 0x47, 0x3E, 0x03, 0x00, 0x02, 0x00, 0x05, 0x34, 0x00, 0x20, 0x00, 0x08, 0x00, 0x28, 0x00,
        0x1A, 0x00, 0x19, 0x00, 0x06, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00,
        0x34, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
        0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    
    SIZE_T                   i, j;
    PBYTE                    buffer;
    MEMORY_BASIC_INFORMATION memBasicInfo;
    UINT64                   baseAddress = 0LL;

    while (VirtualQueryEx(processHandle, reinterpret_cast<LPCVOID>(baseAddress), &memBasicInfo, sizeof(memBasicInfo))) {
        if (baseAddress >= 0xFFFFFFFF)
            break;

        if (memBasicInfo.State & MEM_COMMIT) {
            if (memBasicInfo.Protect == PAGE_READONLY) {
                if (memBasicInfo.RegionSize > sizeof(gameModuleBaseAOB)) {
                    if ((memBasicInfo.AllocationProtect & PAGE_NOCACHE) == 0L) {
                        if (buffer = reinterpret_cast<PBYTE>(malloc(memBasicInfo.RegionSize)); buffer != nullptr) {
                            if (Memory::read(reinterpret_cast<SIZE_T>(memBasicInfo.BaseAddress), buffer, memBasicInfo.RegionSize)) {
                                for (i = 0; i < (memBasicInfo.RegionSize - sizeof(gameModuleBaseAOB)); i++) {
                                    /*for (j = 0; j < sizeof(gameModuleBaseAOB); j++) {
                                        if (buffer[i + j] != gameModuleBaseAOB[j] && gameModuleBaseAOB[j] != '?')
                                            break;
                                    }

                                    if (j == sizeof(gameModuleBaseAOB)) {
                                        free(buffer);
                                        return reinterpret_cast<SIZE_T>(memBasicInfo.BaseAddress) + i;
                                    }*/

                                    if (memcmp(PVOID(SIZE_T(buffer) + i), gameModuleBaseAOB, sizeof(gameModuleBaseAOB)) == 0) {
                                        free(buffer);
                                        return reinterpret_cast<SIZE_T>(memBasicInfo.BaseAddress) + i;
                                    }
                                }
                            }

                            free(buffer);
                        }
                    }
                }
            }
        }

        baseAddress = reinterpret_cast<SIZE_T>(memBasicInfo.BaseAddress) + memBasicInfo.RegionSize;
    }

    fatal("Game module base found", "getGameModuleBase", 0);

    return 0ULL;
}
