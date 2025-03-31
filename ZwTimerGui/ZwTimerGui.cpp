#include "ZwTimerGui.h"

BOOL CALLBACK DlgMain(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
    case WM_INITDIALOG:
    {

        _QueryTimerResolution();

        HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON));
        SendMessage(hwndDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

        nid.cbSize = sizeof(NOTIFYICONDATA);
        nid.hWnd = hwndDlg;
        nid.uID = 1;
        nid.uFlags = NIF_ICON | NIF_MESSAGE;
        nid.uCallbackMessage = WM_ICON;
        nid.hIcon = hIcon;
        strcpy(nid.szTip, "ZwTimer");

        DestroyIcon(hIcon);

        Shell_NotifyIcon(NIM_ADD, &nid);

        _QueryTimerResolution();

        char buffer[21];

        sprintf(buffer, "Maximum Resolution: %d hns", maxRes);
        SetDlgItemText(hwndDlg, _MAX, buffer);

        sprintf(buffer, "Minimum Resolution: %d hns", minRes);
        SetDlgItemText(hwndDlg, _MIN, buffer);

        sprintf(buffer, "Current Resolution: %d hns", currRes);
        SetDlgItemText(hwndDlg, _CURR, buffer);

        SendDlgItemMessage(hwndDlg, _CUSTOM, EM_SETLIMITTEXT, 6, 0);

        char* custom = RegKeyQueryEx(HKEY_CURRENT_USER, "Software\\ZwTimer", "CustomTimer");
        LPCSTR f_custom = custom;
        if (custom)
        {
            SetDlgItemText(hwndDlg, _CUSTOM, f_custom);
            ULONG f_res = strtoul(f_custom, NULL, 10);
            _SetTimerResolution(f_res);
            sprintf(buffer, "Current Resolution: %d hns", actRes);
            SetDlgItemText(hwndDlg, _CURR, buffer);
        }
        else
        {
            SetDlgItemText(hwndDlg, _CUSTOM, "000000");
        }

        char* start = RegKeyQueryEx(HKEY_CURRENT_USER, "Software\\ZwTimer", "Start");
        if (start)
        {
            CheckDlgButton(hwndDlg, _START, BST_CHECKED);
        }

        _SetProcessInformation();

        if (start)
        {
            PostMessage(hwndDlg, WM_HIDE, 0, 0);
            Sleep(5000);
        }

        _drain();

    }
    return TRUE;

    case WM_HIDE:
    {
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
                ShowTrayMenu(hwndDlg);
            }
            return TRUE;
            case WM_LBUTTONDOWN:
            {
                ShowWindow(hwndDlg, SW_SHOWDEFAULT);
                SetPriorityClass(GetCurrentProcess(), PROCESS_MODE_BACKGROUND_END);
            }
            return TRUE;
        }
    }
    return TRUE;

    case WM_CLOSE:
    {
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
        Shell_NotifyIcon(NIM_DELETE, &nid);
        DestroyWindow(hwndDlg);
    }
    return TRUE;

    case WM_COMMAND:
    {
        _QueryTimerResolution();
         char buffer[21];
        sprintf(buffer, "Maximum Resolution: %d hns", maxRes);
        SetDlgItemText(hwndDlg, _MAX, buffer);

        sprintf(buffer, "Minimum Resolution: %d hns", minRes);
        SetDlgItemText(hwndDlg, _MIN, buffer);

        sprintf(buffer, "Current Resolution: %d hns", currRes);
        SetDlgItemText(hwndDlg, _CURR, buffer);
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

                    _SetTimerResolution(maxRes);
                    sprintf(buffer, "Current Resolution: %d hns", actRes);
                    SetDlgItemText(hwndDlg, _CURR, buffer);
                }
                return TRUE;

            case SET_MIN:
                {
                    _SetTimerResolution(minRes);
                    sprintf(buffer, "Current Resolution: %d hns", actRes);
                    SetDlgItemText(hwndDlg, _CURR, buffer);
                }
                return TRUE;

            case SET_CUSTOM:
                {
                    GetDlgItemText(hwndDlg, _CUSTOM, lpbuffer, sizeof(lpbuffer));
                    ULONG lpres = strtoul(lpbuffer, NULL, 10);

                    _SetTimerResolution(lpres);

                    sprintf(buffer, "Current Resolution: %d hns", actRes);
                    SetDlgItemText(hwndDlg, _CURR, buffer);
                    RegKeySetEx(HKEY_CURRENT_USER, "Software\\ZwTimer", "CustomTimer", lpbuffer);
                }
                return TRUE;

            case UNSET_CUSTOM:
                {
                    _SetTimerResolution(0, FALSE);

                    sprintf(buffer, "Current Resolution: %d hns", actRes);
                    SetDlgItemText(hwndDlg, _CURR, buffer);
                }
                return TRUE;

            }
            return TRUE;
        }
        return TRUE;
    }
    return FALSE;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    hInst=hInstance;
    InitCommonControls();
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
            _QueryTimerResolution();

            char buffer[21];

            sprintf(buffer, "Maximum Resolution: %d hns", maxRes);
            SetDlgItemText(hwndR, _MAX, buffer);

            sprintf(buffer, "Minimum Resolution: %lu hns", minRes);
            SetDlgItemText(hwndR, _MIN, buffer);

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
        break;
        }
    }
    return TRUE;
}
