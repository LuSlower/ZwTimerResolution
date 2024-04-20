#define WIN32_LEAN_AND_MEAN
#pragma once
#ifndef NTAPI_H
#define NTAPI_H
#endif
#define IDC_STATIC (-1)
#define DLG_MAIN 1
#define _MAX 101
#define _MIN 102
#define _CURR 103
#define _CUSTOM 1000
#define SET_MAX 1001
#define SET_MIN 1002
#define SET_CUSTOM 1003
#define _START 1004
#define IDI_ICON 1005
#define IDM_EXIT 1006
#define WM_HIDE 1008
#define WM_ICON 1009
#define WM_TRAYICON (WM_ICON)

#ifdef __cplusplus
extern "C" {
#endif

WINBASEAPI WINBOOL WINAPI SetProcessInformation (HANDLE hProcess, PROCESS_INFORMATION_CLASS ProcessInformationClass, LPVOID ProcessInformation, DWORD ProcessInformationSize);

typedef long NTSTATUS;
//Declarar NtQueryTimerResolution
NTSTATUS __stdcall NtQueryTimerResolution(ULONG *MinimumResolution, ULONG *MaximumResolution, ULONG *CurrentResolution);
//Declarar NtSetTimerResolution
NTSTATUS __stdcall NtSetTimerResolution(ULONG DesiredResolution, BOOLEAN SetResolution, ULONG *CurrentResolution);
}
