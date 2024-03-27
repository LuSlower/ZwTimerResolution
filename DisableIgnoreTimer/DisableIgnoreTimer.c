#include "export.h"

#ifndef PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION
#define PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION 0x4
#endif

typedef BOOL (WINAPI *LPFN_SetProcessInformation)(
        HANDLE hProcess,
        PROCESS_INFORMATION_CLASS ProcessInformationClass,
        PVOID ProcessInformation,
        DWORD ProcessInformationLength
    );
    
typedef enum PROCESS_INFORMATION_CLASS {
    ProcessPowerThrottling,
} _PROCESS_INFORMATION_CLASS;

typedef struct PROCESS_POWER_THROTTLING_STATE {
    DWORD Version;
    DWORD ControlMask;
    DWORD StateMask;
} pPROCESS_POWER_THROTTLING_STATE;

#ifndef PROCESS_POWER_THROTTLING_CURRENT_VERSION
#define PROCESS_POWER_THROTTLING_CURRENT_VERSION 1
#endif
DLL_EXPORT int __stdcall _SetProcessInformation(HANDLE hProcess)
{
    // Obtener el handle del módulo Kernel32.dll
    HMODULE hKernel32 = GetModuleHandle("Kernel32.dll");

    if (hKernel32 == NULL)
    {
        // Error al obtener el handle del módulo
        return -1;
    }

    // Obtener el puntero a la función SetProcessInformation
    LPFN_SetProcessInformation pSetProcessInformation =
        (LPFN_SetProcessInformation)GetProcAddress(hKernel32, "SetProcessInformation");

    if (pSetProcessInformation == NULL)
    {
        // Error al obtener el puntero a la función
        return -2;
    }

    // Llamar a la función SetProcessInformation
    pPROCESS_POWER_THROTTLING_STATE state = { 0 };
    state.Version = PROCESS_POWER_THROTTLING_CURRENT_VERSION;
    state.ControlMask = PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION;
    state.StateMask = 0;

    if (!pSetProcessInformation(hProcess, ProcessPowerThrottling, &state, sizeof(state)))
    {
        // Error al llamar a la función
        return -3;
    }

    // Operación exitosa
    return 1;
}

