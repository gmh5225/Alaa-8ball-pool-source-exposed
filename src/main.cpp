#include <iostream>
#include "GlobalVars.h"
#include "Memory.h"
#include "GUI.h"
#include "Menu.h"
#include "GameManager.h"
#include "Prediction.h"

#include <thread>
#include "Utils.h"

bool gUnload;

void autoThread()
{
    while (gUnload == false) {
        if (gGlobalVars->features.automatic) {
            if (GameManager::isPlayerTurn()) {
                gPrediction->initAutoAim();
            }
        }

        Sleep(10);
    }
}

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
{
    INT         result = EXIT_SUCCESS;
    HANDLE      gMutex, process = Memory::getProcessHandle();
    std::thread newThread;

    gMutex = OpenMutex(MUTEX_ALL_ACCESS, 0, "8BPH");
    if (gMutex) {
        fatal("application is already running", "unknown", 0);
        return EXIT_FAILURE;
    }
    else
    {
        gMutex = CreateMutex(0, 0, "8BPH");
    }

    srand(static_cast<UINT32>(time(nullptr)));
    if (process != nullptr) {
        if (gGlobalVars->init() && Menu::init(hInstance)) {
            gUnload = false;
            newThread = std::thread(autoThread); // init auto aim/play thread
            GUI::init();
            Menu::runLoop();
            Menu::end(hInstance);
            gUnload = true;
            newThread.join(); // wait for our thread to finish.
        }
        else
        {
            result = EXIT_FAILURE;
        }

        CloseHandle(process);
    }
    else 
    {
        result = EXIT_FAILURE;
    }

    if (gMutex)
        ReleaseMutex(gMutex);

    return result;
}

