#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <limits.h>
#include <math.h>
#include <tchar.h>

// Definir valores
ULONG min, max, current, res_act, res, res_new;

// Definir contadores
LARGE_INTEGER frq, start, end;

// Definir semaforo, evento nulo
HANDLE hMutex, hEvent;

// Declaración de funciones Zw = Nt
typedef LONG NTSTATUS;
NTSYSAPI NTSTATUS NTAPI ZwSetTimerResolution(ULONG DesiredResolution, BOOLEAN SetResolution, ULONG *CurrentResolution);
NTSYSAPI NTSTATUS NTAPI ZwQueryTimerResolution(ULONG *MinimumResolution, ULONG *MaximumResolution, ULONG *CurrentResolution);

// Declaración de SetProcessInformation
WINBASEAPI WINBOOL WINAPI SetProcessInformation(HANDLE hProcess, PROCESS_INFORMATION_CLASS ProcessInformationClass, LPVOID ProcessInformation, DWORD ProcessInformationSize);

void _SetProcessInformation()
{
    if (SetProcessInformation)
    {

        // Pasa un puntero a la estructura
        PROCESS_POWER_THROTTLING_STATE state;
        ZeroMemory(&state, sizeof(state));
        state.Version = PROCESS_POWER_THROTTLING_CURRENT_VERSION;
        state.ControlMask = PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION;
        state.StateMask = 0;

        // Deshabilitar la resolución del temporizador de inactividad
        SetProcessInformation(GetCurrentProcess(), ProcessPowerThrottling, &state, sizeof(state));
    }
    else
    {
        printf("Error getting address from SetProcessInformation\n"); // No existe en Windows 7
    }
}

// declarar medidas
double time, tsleep, delta;

void sleep_test()
{
    QueryPerformanceCounter(&start);
    Sleep(1);
    QueryPerformanceCounter(&end);

    // Convertir la diferencia a milisegundos y restar el sleep(1)
    time = (double)(end.QuadPart - start.QuadPart) / frq.QuadPart;
    tsleep = time * 1000;
    delta = tsleep - 1;
}

void loop_test()
{
    //bucle predeterminado
    for (int i = 1; ; i++) {
    ZwQueryTimerResolution(&min, &max, &res_act);
    sleep_test();
    printf("sleep(1): %.4f ms | delta: %.4f ms | zwres: %d ns\n", tsleep, delta, res_act);
    Sleep(500);
    }
}

void get_frq()
{
    // Obtener la frecuencia del contador de rendimiento establecer prioridad
    QueryPerformanceFrequency(&frq);

    // 16 + 15 = 31, la prioridad más alta
    SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
}
