#pragma once
#include <windows.h>
#include <shellapi.h>
#include <commctrl.h>
#include <stdio.h>
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
#define UNSET_CUSTOM 1004
#define _START 1005
#define IDI_ICON 1006
#define IDM_EXIT 1007
#define WM_HIDE 1008
#define WM_ICON 1009
#define WM_TRAYICON (WM_ICON)

extern "C" {
WINBASEAPI WINBOOL WINAPI SetProcessInformation (HANDLE hProcess, PROCESS_INFORMATION_CLASS ProcessInformationClass, LPVOID ProcessInformation, DWORD ProcessInformationSize);

typedef long NTSTATUS;
//Declarar ZwQueryTimerResolution
NTSTATUS __stdcall ZwQueryTimerResolution(ULONG *MinimumResolution, ULONG *MaximumResolution, ULONG *CurrentResolution);
//Declarar ZwSetTimerResolution
NTSTATUS __stdcall ZwSetTimerResolution(ULONG DesiredResolution, BOOLEAN SetResolution, ULONG *CurrentResolution);
}

HINSTANCE hInst;
HWND hwndDlg;

void _drain()
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_SET_QUOTA, false, GetCurrentProcessId());
    //drenar WorkingSet
    SetProcessWorkingSetSize(hProcess, (SIZE_T) -1, (SIZE_T) -1);
    CloseHandle(hProcess);
}

//Leer REG_SZ del registro
char* RegKeyQueryEx(HKEY hKey, LPCSTR lpSubKey, LPCSTR lpValueName)
{
    static char result[MAX_PATH]; // Static para que persista fuera del ámbito de la función
    HKEY hSubKey;
    if (RegOpenKeyExA(hKey, lpSubKey, 0, KEY_QUERY_VALUE, &hSubKey) == ERROR_SUCCESS)
    {
        DWORD dwType;
        DWORD dwSize = MAX_PATH;
        if (RegQueryValueExA(hSubKey, lpValueName, NULL, &dwType, reinterpret_cast<LPBYTE>(result), &dwSize) == ERROR_SUCCESS && dwType == REG_SZ)
        {
            RegCloseKey(hSubKey);
            return result;
        }
        RegCloseKey(hSubKey);
    }
    return nullptr; // Devolver nullptr si no se pudo leer el valor
}

//Escribir REG_SZ en el registro
bool RegKeySetEx(HKEY hKey, LPCSTR lpSubKey, LPCSTR lpValueName, LPCSTR lpData)
{
    HKEY hSubKey;
    if (RegCreateKeyExA(hKey, lpSubKey, 0, NULL, 0, KEY_SET_VALUE, NULL, &hSubKey, NULL) == ERROR_SUCCESS)
    {
        DWORD dwSize = static_cast<DWORD>(strlen(lpData) + 1);
        if (RegSetValueExA(hSubKey, lpValueName, 0, REG_SZ, reinterpret_cast<const BYTE*>(lpData), dwSize) == ERROR_SUCCESS)
        {
            RegCloseKey(hSubKey);
            return true;
        }
        RegCloseKey(hSubKey);
    }
    return false;
}

//Eliminar REG_SZ del registro
bool RegKeyDelete(HKEY hKey, LPCSTR lpSubKey, LPCSTR lpValueName)
{
    HKEY hSubKey;
    if (RegOpenKeyExA(hKey, lpSubKey, 0, KEY_SET_VALUE, &hSubKey) == ERROR_SUCCESS)
    {
        if (RegDeleteValueA(hSubKey, lpValueName) == ERROR_SUCCESS)
        {
            RegCloseKey(hSubKey);
            return true;
        }
        RegCloseKey(hSubKey);
    }
    return false;
}

//Global
ULONG minRes, maxRes, currRes, actRes;

void _ZwQueryTimerResolution()
{
    //call ZwQueryTimerResolution
    ZwQueryTimerResolution(&minRes, &maxRes, &currRes);
}

void _ZwSetTimerResolution(ULONG customRes, BOOL setRes = TRUE)
{
        //call ZwSetTimerResolution
        ZwSetTimerResolution(customRes, setRes, &actRes);
}

// Declaración del trayicon
NOTIFYICONDATA nid;
bool isIconVisible = false;

//ShowTrayMenu
void ShowTrayMenu(HWND hWnd)
{
    //obtener posición del puntero (x,y)
    POINT pt;
    GetCursorPos(&pt);

    //crear menú
    HMENU hMenu = CreatePopupMenu();
    AppendMenu(hMenu, MF_STRING, IDM_EXIT, "exit");

    //poner en primer plano la ventana actual y centrar en menú
    SetForegroundWindow(hWnd);
    TrackPopupMenu(hMenu, TPM_RIGHTALIGN | TPM_BOTTOMALIGN | TPM_NONOTIFY | TPM_LEFTBUTTON, pt.x, pt.y, 0, hWnd, NULL);

    //destruir menu
    DestroyMenu(hMenu);
}

//SetInformationProcess
void _SetProcessInformation()
{
        if (SetProcessInformation)
        {
            //Obtener el HANDLE del proceso actual
            HANDLE hProcess = OpenProcess(PROCESS_SET_INFORMATION, FALSE, GetCurrentProcessId());

            //pasa puntero de la estructura
            PROCESS_POWER_THROTTLING_STATE state;
            ZeroMemory(&state, sizeof(state));
            state.Version = PROCESS_POWER_THROTTLING_CURRENT_VERSION;
            state.ControlMask = PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION;
            state.StateMask = 0;

            //DISABLE IDLE TIMER RESOLUTION
            SetProcessInformation(hProcess, ProcessPowerThrottling, &state, sizeof(state));


            //cerrar HANDLE
            CloseHandle(hProcess);
        }
        else
        {
            printf("Error getting address from SetProcessInformation\n"); //No existe en Windows 7
        }
}
