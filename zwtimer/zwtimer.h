#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <limits.h>
#include <math.h>
#include <tchar.h>

ULONG min, max, current, res_act, res, res_new;

LARGE_INTEGER frq, start, end;

HANDLE hMutex, hEvent;

typedef LONG NTSTATUS;
NTSYSAPI NTSTATUS NTAPI ZwSetTimerResolution(ULONG DesiredResolution, BOOLEAN SetResolution, ULONG *CurrentResolution);
NTSYSAPI NTSTATUS NTAPI ZwQueryTimerResolution(ULONG *MinimumResolution, ULONG *MaximumResolution, ULONG *CurrentResolution);

WINBASEAPI WINBOOL WINAPI SetProcessInformation(HANDLE hProcess, PROCESS_INFORMATION_CLASS ProcessInformationClass, LPVOID ProcessInformation, DWORD ProcessInformationSize);

void _SetProcessInformation()
{
    if (SetProcessInformation)
    {

        PROCESS_POWER_THROTTLING_STATE state;
        ZeroMemory(&state, sizeof(state));
        state.Version = PROCESS_POWER_THROTTLING_CURRENT_VERSION;
        state.ControlMask = PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION;
        state.StateMask = 0;

        SetProcessInformation(GetCurrentProcess(), ProcessPowerThrottling, &state, sizeof(state));
    }
    else
    {
        printf("Error getting address from SetProcessInformation\n"); // does not exist in w7
    }
}

double time, tsleep, delta;

void sleep_test()
{
    QueryPerformanceCounter(&start);
    Sleep(1);
    QueryPerformanceCounter(&end);

    time = (double)(end.QuadPart - start.QuadPart) / frq.QuadPart;
    tsleep = time * 1000;
    delta = tsleep - 1;
}

void loop_test()
{
    for (int i = 1; ; i++) {
    ZwQueryTimerResolution(&min, &max, &res_act);
    sleep_test();
    printf("zwres: %d hns, sleep(1): %.4f ms (delta: %.4f)\n", res_act, tsleep, delta);
    Sleep(250);
    }
}

void get_frq()
{
    QueryPerformanceFrequency(&frq);
    SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
}
