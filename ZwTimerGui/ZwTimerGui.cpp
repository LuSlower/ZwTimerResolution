#include "ZwTimerGui.h"

BOOL CALLBACK DlgMain(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
    case WM_INITDIALOG:
    {

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
        strcpy(nid.szTip, "ZwTimer");

        //destruir icono
        DestroyIcon(hIcon);

        // Agregar el icono a la bandeja del sistema
        Shell_NotifyIcon(NIM_ADD, &nid);

        // Llamar a ZwQueryTimerResolution
        _ZwQueryTimerResolution();

        // LTEXT
        char buffer[21];

        // Convertir maxRes a cadena
        sprintf(buffer, "Maximun Resolution: %d hns", maxRes);
        SetDlgItemText(hwndDlg, _MAX, buffer);

        // Convertir minRes a cadena
        sprintf(buffer, "Minimun Resolution: %d hns", minRes);
        SetDlgItemText(hwndDlg, _MIN, buffer);

        // Convertir currRes a cadena
        sprintf(buffer, "Current Resolution: %d hns", currRes);
        SetDlgItemText(hwndDlg, _CURR, buffer);

        //Comprobar CustomTimer
        SendDlgItemMessage(hwndDlg, _CUSTOM, EM_SETLIMITTEXT, 6, 0); //limitar a 6 caracteres el EDIT

        char* custom = RegKeyQueryEx(HKEY_CURRENT_USER, "Software\\ZwTimer", "CustomTimer");
        LPCSTR f_custom = custom;
        if (custom)
        {
            SetDlgItemText(hwndDlg, _CUSTOM, f_custom); //rellenar edit con valor obtenido del registro
            ULONG f_res = strtoul(f_custom, NULL, 10);
            _ZwSetTimerResolution(f_res);
            // Convertir actRes a cadena
            sprintf(buffer, "Current Resolution: %d hns", actRes);
            SetDlgItemText(hwndDlg, _CURR, buffer);
        }
        else
        {
            SetDlgItemText(hwndDlg, _CUSTOM, "000000"); //rellenar edit
        }

        char* start = RegKeyQueryEx(HKEY_CURRENT_USER, "Software\\ZwTimer", "Start");
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
        SetPriorityClass(GetCurrentProcess(), PROCESS_MODE_BACKGROUND_BEGIN);
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
                SetPriorityClass(GetCurrentProcess(), PROCESS_MODE_BACKGROUND_END);
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
                    RegKeySetEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", "ZwTimer", path);
                    RegKeySetEx(HKEY_CURRENT_USER, "Software\\ZwTimer", "Start", "1");
                }
        }
        else
        {
            RegKeyDelete(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", "ZwTimer");
            RegKeyDelete(HKEY_CURRENT_USER, "Software\\ZwTimer", "Start");
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
                    sprintf(buffer, "Current Resolution: %d hns", actRes);
                    SetDlgItemText(hwndDlg, _CURR, buffer);
                }
                return TRUE;

            case SET_MIN:
                {

                    //call ZwSetTimerResolution
                    _ZwSetTimerResolution(minRes);

                    // Convertir actRes a cadena
                    sprintf(buffer, "Current Resolution: %d hns", actRes);
                    SetDlgItemText(hwndDlg, _CURR, buffer);
                }
                return TRUE;

            case SET_CUSTOM:
                {
                    // Obtener texto del EDIT para DesiredResolution
                    GetDlgItemText(hwndDlg, _CUSTOM, lpbuffer, sizeof(lpbuffer));
                    ULONG lpres = strtoul(lpbuffer, NULL, 10);

                    // Call ZwSetTimerResolution
                    _ZwSetTimerResolution(lpres);

                    // Convertir actRes a cadena
                    sprintf(buffer, "Current Resolution: %d hns", actRes);
                    SetDlgItemText(hwndDlg, _CURR, buffer);
                    RegKeySetEx(HKEY_CURRENT_USER, "Software\\ZwTimer", "CustomTimer", lpbuffer);
                }
                return TRUE;

            case UNSET_CUSTOM:
                {
                    // Call ZwSetTimerResolution
                    _ZwSetTimerResolution(0, FALSE);

                    // Convertir actRes a cadena
                    sprintf(buffer, "Current Resolution: %d hns", actRes);
                    SetDlgItemText(hwndDlg, _CURR, buffer);
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
            SetPriorityClass(GetCurrentProcess(), PROCESS_MODE_BACKGROUND_BEGIN);
            _drain();
        }
        if (msg.message == WM_KEYDOWN && msg.wParam == VK_F5)
        {
            // Llamar a ZwQueryTimerResolution
            _ZwQueryTimerResolution();

            // LTEXT
            char buffer[21];

            // Convertir maxRes a cadena
            sprintf(buffer, "Maximun Resolution: %d hns", maxRes);
            SetDlgItemText(hwndR, _MAX, buffer);

            // Convertir minRes a cadena
            sprintf(buffer, "Minimun Resolution: %lu hns", minRes);
            SetDlgItemText(hwndR, _MIN, buffer);

            // Convertir currRes a cadena
            sprintf(buffer, "Current Resolution: %lu hns", currRes);
            SetDlgItemText(hwndR, _CURR, buffer);
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
