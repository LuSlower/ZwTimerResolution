#include "main.h"

BOOL DLL_EXPORT _SetProcessInformation(HANDLE hProcess, INT Option)
{
        switch (Option)
        {
            case 1:
                {
                    // Define la estructura PROCESS_POWER_THROTTLING_STATE
                    PROCESS_POWER_THROTTLING_STATE state;
                    ZeroMemory(&state, sizeof(state));
                    state.Version = PROCESS_POWER_THROTTLING_CURRENT_VERSION;
                    state.ControlMask = PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION;
                    state.StateMask = 0;

                    // Llama a SetProcessInformation
                    return SetProcessInformation(hProcess, ProcessPowerThrottling, &state, sizeof(state));
                }
                break;
            case 2:
                {
                    //define la estructura de MEMORY_PRIORITY_INFORMATION
                    MEMORY_PRIORITY_INFORMATION memprio;
                    ZeroMemory(&memprio, sizeof(memprio));
                    memprio.MemoryPriority = MEMORY_PRIORITY_VERY_LOW; //0x1
                    // Llama a SetProcessInformation
                    return SetProcessInformation(hProcess, ProcessMemoryPriority, &memprio, sizeof(memprio));

                }
                break;
    }
    return FALSE;
}

extern "C" BOOL WINAPI DllMain(HANDLE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            // attach to process
            // return FALSE to fail DLL load
            break;

        case DLL_PROCESS_DETACH:
            // detach from process
            break;

        case DLL_THREAD_ATTACH:
            // attach to thread
            break;

        case DLL_THREAD_DETACH:
            // detach from thread
            break;
    }
    return TRUE; // succesful
}
