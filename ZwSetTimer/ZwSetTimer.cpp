#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "declarations.h"

HINSTANCE hInst;
static HWND hwndDlg;

void RestartProgram()
{
    TCHAR szFileName[MAX_PATH];
    GetModuleFileName(NULL, szFileName, MAX_PATH);

    // Cerrar el proceso actual
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcess(NULL, szFileName, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        printf("Error al reiniciar el programa: %d\n", GetLastError());
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    // Obtener el identificador del proceso actual y cerrarlo
    DWORD currentProcessId = GetCurrentProcessId();
    HANDLE currentProcessHandle = OpenProcess(PROCESS_TERMINATE, FALSE, currentProcessId);
    TerminateProcess(currentProcessHandle, 0);
    CloseHandle(currentProcessHandle);
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

void _ZwSetTimerResolution(ULONG customRes)
{
        //call ZwSetTimerResolution
        ZwSetTimerResolution(customRes, TRUE, &actRes);
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
    AppendMenu(hMenu, MF_STRING, IDM_EXIT, "Exit");

    //poner en primer plano la ventana actual y centrar en menú
    SetForegroundWindow(hWnd);
    TrackPopupMenu(hMenu, TPM_RIGHTALIGN | TPM_BOTTOMALIGN | TPM_NONOTIFY | TPM_LEFTBUTTON, pt.x, pt.y, 0, hWnd, NULL);

    //destruir menu
    DestroyMenu(hMenu);
}

//Drain Memory
void _drain()
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_SET_QUOTA, false, GetCurrentProcessId());
    //drenar WorkingSet
    SetProcessWorkingSetSize(hProcess, (SIZE_T) -1, (SIZE_T) -1);
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
            printf("Error al obtener la dirección de SetProcessInformation\n"); //No existe en Windows 7
        }
}

//buffer LTEXT
char buffer[21];

BOOL CALLBACK DlgMain(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
    case WM_INITDIALOG:
    {
        //establecer prioridad de segundo plano (low i/o y low mem priority)
        SetPriorityClass(GetCurrentProcess(), PROCESS_MODE_BACKGROUND_BEGIN);

        //call ZwQueryTimerResolution
        _ZwQueryTimerResolution();

        //establecer hIcon
        HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON));
        SendMessage(hwndDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

        // Inicializar la estructura NOTIFYICONDATA
        nid.cbSize = sizeof(NOTIFYICONDATA);
        nid.hWnd = hwndDlg;
        nid.uID = 1;
        nid.uFlags = NIF_ICON | NIF_MESSAGE;
        nid.uCallbackMessage = WM_ICON;
        nid.hIcon = hIcon;
        strcpy(nid.szTip, "ZwSetTimer");

        //destruir icono
        DestroyIcon(hIcon);

        // Agregar el icono a la bandeja del sistema
        Shell_NotifyIcon(NIM_ADD, &nid);

        // Llamar a ZwQueryTimerResolution
        _ZwQueryTimerResolution();

        // Convertir maxRes a cadena
        sprintf(buffer, "Maximun Timer Resolution: %lu ns", maxRes);
        SetDlgItemText(hwndDlg, _MAX, buffer);

        // Convertir minRes a cadena
        sprintf(buffer, "Minimun Timer Resolution: %lu ns", minRes);
        SetDlgItemText(hwndDlg, _MIN, buffer);

        // Convertir currRes a cadena
        sprintf(buffer, "Current Timer Resolution: %lu ns", currRes);
        SetDlgItemText(hwndDlg, _CURR, buffer);

        //Comprobar CustomTimer
        SendDlgItemMessage(hwndDlg, _CUSTOM, EM_SETLIMITTEXT, 6, 0); //limitar a 6 caracteres el EDIT

        char* custom = RegKeyQueryEx(HKEY_CURRENT_USER, "Software\\ZwSetTimer", "CustomTimer");
        LPCSTR f_custom = custom;
        if (custom)
        {
            SetDlgItemText(hwndDlg, _CUSTOM, f_custom); //rellenar edit con valor obtenido del registro
            ULONG f_res = strtoul(f_custom, NULL, 0);
            _ZwSetTimerResolution(f_res);
            // Convertir actRes a cadena
            sprintf(buffer, "Current Timer Resolution: %lu ns", actRes);
            SetDlgItemText(hwndDlg, _CURR, buffer);
        }
        else
        {
            SetDlgItemText(hwndDlg, _CUSTOM, "000000"); //rellenar edit
        }

        char* start = RegKeyQueryEx(HKEY_CURRENT_USER, "Software\\ZwSetTimer", "Start");
        if (start)
        {
            CheckDlgButton(hwndDlg, _START, BST_CHECKED);
        }


        //llamar a NtSetInformationProcess
        _SetProcessInformation();

        if (start)
        {
            //enviar mensaje de ocultar el dialogo
            PostMessage(hwndDlg, WM_HIDE, 0, 0);
        }

        _drain();

    }
    return TRUE;

    case WM_HIDE:
    {
        //ocultar dialogo
        ShowWindow(hwndDlg, SW_HIDE);
        _drain();
    }
    return TRUE;

    case WM_ICON:
    {
        switch (lParam)
        {
            case WM_RBUTTONDOWN:
            {
                // Mostrar el menú de la bandeja al hacer clic derecho en el icono
                ShowTrayMenu(hwndDlg);
            }
            return TRUE;
            case WM_LBUTTONDOWN:
            {
                //Mostrar dialogo al hacer clic izquierdo
                ShowWindow(hwndDlg, SW_SHOWDEFAULT);
            }
            return TRUE;
        }
    }
    return TRUE;

    case WM_CLOSE:
    {
    //comprobar valores
    UINT state = IsDlgButtonChecked(hwndDlg, _START);
        if (state == BST_CHECKED)
        {
            char path[MAX_PATH];
            if (GetModuleFileName(NULL, path, MAX_PATH) != 0)
                {
                    RegKeySetEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", "ZwSetTimer", path);
                    RegKeySetEx(HKEY_CURRENT_USER, "Software\\ZwSetTimer", "Start", "1");
                }
        }
        else
        {
            RegKeyDelete(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", "ZwSetTimer");
            RegKeyDelete(HKEY_CURRENT_USER, "Software\\ZwSetTimer", "Start");
        }
        //exit
        // Eliminar el icono de la bandeja y eliminar la ventana
        Shell_NotifyIcon(NIM_DELETE, &nid);
        DestroyWindow(hwndDlg);
    }
    return TRUE;

    case WM_COMMAND:
    {
        //buffer LTEXT
        char buffer[21];
        char lpbuffer[6];

        switch(LOWORD(wParam))
        {

            case IDM_EXIT:
                {
                PostMessage(hwndDlg, WM_CLOSE, 0, 0);
                }
                return TRUE;

            case SET_MAX:
                {

                    //call ZwSetTimerResolution
                    _ZwSetTimerResolution(maxRes);

                    // Convertir actRes a cadena
                    sprintf(buffer, "Current Timer Resolution: %lu ns", actRes);
                    SetDlgItemText(hwndDlg, _CURR, buffer);
                }
                return TRUE;

            case SET_MIN:
                {

                    //call ZwSetTimerResolution
                    _ZwSetTimerResolution(minRes);

                    // Convertir actRes a cadena
                    sprintf(buffer, "Current Timer Resolution: %lu ns", actRes);
                    SetDlgItemText(hwndDlg, _CURR, buffer);
                }
                return TRUE;

            case SET_CUSTOM:
                {
                    // Obtener texto del EDIT para DesiredResolution
                    GetDlgItemText(hwndDlg, _CUSTOM, lpbuffer, sizeof(lpbuffer));
                    ULONG lpres = strtoul(lpbuffer, NULL, 0);

                    // Call ZwSetTimerResolution
                    _ZwSetTimerResolution(lpres);

                    // Convertir actRes a cadena
                    sprintf(buffer, "Current Timer Resolution: %lu ns", actRes);
                    SetDlgItemText(hwndDlg, _CURR, buffer);
                    RegKeySetEx(HKEY_CURRENT_USER, "Software\\ZwSetTimer", "CustomTimer", lpbuffer);
                    RestartProgram();
                }
                return TRUE;

            }//end Switch wParam
            return TRUE;
        }
        return TRUE; //end WM_COMMAND
    }
    return FALSE; //end Switch
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    hInst=hInstance;
    InitCommonControls();
    // Cargar el acelerador
    HWND hwndR = CreateDialog(hInst, MAKEINTRESOURCE(DLG_MAIN), NULL, (DLGPROC)DlgMain);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if(msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE)
        {
            ShowWindow(hwndR, SW_HIDE);
            _drain();
        }
       if (!IsDialogMessage(hwndR, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (!IsWindow(hwndR)) {
            DestroyWindow(hwndR);
            DestroyWindow(hwndDlg);
        break; // Salir del bucle si el HWND ya no existe
        }
    }
    return TRUE;
}
