#SingleInstance Force
#NoEnv
;@Ahk2Exe-SetCompanyName LuSlower
;@Ahk2Exe-SetDescription SetTimerRes
;@Ahk2Exe-SetCopyright Copyright - Luis Garcia
;@Ahk2Exe-SetFileVersion 1.0.1
;@Ahk2Exe-SetProductName SetTimerRes
;@Ahk2Exe-SetProductVersion 1.0.1
;@Ahk2Exe-SetVersion 1.0.0.1

;Cargar librerias
global ntdll := DllCall("LoadLibrary", "Str", "ntdll.dll", "Ptr")
global psapi := DllCall("LoadLibrary", "Str", "psapi.dll", "Ptr")
global kernel32 := DllCall("LoadLibrary", "Str", "kernel32.dll", "Ptr")
global INI := a_scriptdir "\SetTimerRes.ini"
IniRead, _save_custom, %INI%, TimerConfig, CustomTimer, 0000000
IniRead, _save_global_timer, %INI%, TimerConfig, GlobalTimer, 0
IniRead, _save_start, %INI%, Start, Windows, 0

EmptyWorkingSet() {
;llamada a EmptyWorkingSet
pid := DllCall("kernel32.dll\GetCurrentProcessId")
hProcess := DllCall("OpenProcess", "uint", 0x001F0FFF, "int", 0, "uint", pid)
DllCall("Kernel32.dll\	SetProcessWorkingSetSize", "ptr", hProcess, "uptr", -1, "uptr", -1)
DllCall("psapi.dll\EmptyWorkingSet", "Ptr", hProcess)
DllCall("CloseHandle", "ptr", hProcess)
}

SetProcessInformation() {
pid := DllCall("kernel32.dll\GetCurrentProcessId")
hProcess := DllCall("OpenProcess", "uint", 0x001F0FFF, "int", 0, "uint", pid)
PROCESS_POWER_THROTTLING := 0x1E
PROCESS_POWER_THROTTLING_STATE := 1  ; Valor para ignorar la resolución del temporizador
result := DllCall("Kernel32.dll\SetProcessInformation", "Ptr", hProcess, "UInt", PROCESS_POWER_THROTTLING, "Ptr", &PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION, "UInt", 4)
DllCall("CloseHandle", "ptr", hProcess)
}

NTQueryTimerResolution() {
;llamada a NtQueryTimerResolution
SetProcessInformation()
DllCall("ntdll.dll\NtQueryTimerResolution", "Int64P", Mn, "Int64P", Mx, "Int64P", Ct)
global _max := Mx
global _min := Mn
global _current := Ct
EmptyWorkingSet()
}

NTSetTimerResolution(Ds, St) {
;llamada a NtSetTimerResolution
DllCall("ntdll.dll\NtSetTimerResolution", "UInt", Ds, "UInt", St, "Int64P", At)
global _current := At
EmptyWorkingSet()
}

;detectar custom timer
If (_save_custom <> 0000000) {
	NtSetTimerResolution(_save_custom, 1)
}

NTQueryTimerResolution()
Menu, Tray, NoStandard
Menu, Tray, MainWindow
Menu, Tray, Click, 1
Menu, Tray, Add, Exit, GuiClose
Menu, Tray, Add, Show, GuiShow
Menu, Tray, Default, Show
Gui, add, groupbox, x5 y5 w185 h240, SetTimer
Gui, add, text, x20 y30, Maximum Resolution :  %_max% (ns)
Gui, add, text, x20 y50, Minimum Resolution :  %_min% (ns)
Gui, add, text, x20 y70 vCur, Current Resolution :  %_current% (ns)
Gui, add, button, x40 y90, Set Max
Gui, add, button, x110 y90, Set Min
Gui, add, Text, x55 y120, Custom Resolution
Gui, add, edit, x75 y140 w50 h15 vCust, %_save_custom%
Gui, add, button, x40 y165 w115 h20, Set Custom
Gui, add, checkbox, x15 y200 vGlobalT, GlobalTimerRequest (Win11)
Gui, add, checkbox, x15 y220 vHide, Cargar con Windows (Hide)
GuiControl, , GlobalT, %_save_global_timer%
GuiControl, , Hide, %_save_start%
EmptyWorkingSet() ;recolectar basura
switch _save_start
{
	case 0:
		Gui, show, w195 h250, SetTimerRes | LuSlower
	Case 1:
		Gui, hide
}
return

buttonsetmax:
NTQueryTimerResolution()
NTSetTimerResolution(_max, 1)
GuiControl, Text, Cur, Current Resolution :  %_current% (ns)
return

buttonsetmin:
NTQueryTimerResolution()
NTSetTimerResolution(_min, 1)
GuiControl, Text, Cur, Current Resolution :  %_current% (ns)
return

buttonsetcustom:
GuiControlGet, _scustom, , Cust
If (_scustom is not integer) {
MsgBox, 16, Error, Por favor escriba un numero entero con un maximo de 7 digitos
return
}
Else If (StrLen(_scustom) > 7) {
MsgBox, 16, Error, Por favor escriba un numero entero con un maximo de 7 digitos
return
}
Else if (_scustom < 5000) {
	_scustom := 5000
}
Else if (_scustom > 156250) {
	_scustom := 156250
}
NTQueryTimerResolution()
NTSetTimerResolution(_scustom, 1)
GuiControl, Text, Cur, Current Resolution :  %_current% (ns)
GuiControl, , Cust, %_scustom%
IniWrite, %_scustom%, %INI%, TimerConfig, CustomTimer
return

GuiEscape:
Gui, hide
EmptyWorkingSet()
return

GuiShow:
Gui, show, w195 h250, SetTimerRes | LuSlower
EmptyWorkingSet()
return

GuiClose:
DllCall("FreeLibrary ", "Ptr", ntdll)
DllCall("FreeLibrary ", "Ptr", psapi)
DllCall("FreeLibrary ", "Ptr", kernel32)
key_run := "HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Run"
GuiControlGet, _Hvalue, , Hide
switch _Hvalue
{
	case 1:
		RegWrite, REG_SZ, %key_run%, SetTimerRes, %A_ScriptFullPath%
		IniWrite, %_Hvalue%, %INI%, Start, Windows
	case 0:
		RegDelete, %key_run%, SetTimerRes
		IniWrite, %_Hvalue%, %INI%, Start, Windows
}
krnl_key := "HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\kernel"
GuiControlGet, _Gvalue, , GlobalT
switch _Gvalue
{
	case 1:
		RegWrite, REG_DWORD, %krnl_key%, GlobalTimerResolutionRequests, 1
		IniWrite, %_Gvalue%, %INI%, TimerConfig, GlobalTimer
	case 0:
		RegDelete, %krnl_key%, GlobalTimerResolutionRequests
		IniWrite, %_Gvalue%, %INI%, TimerConfig, GlobalTimer
}
ExitApp