#RequireAdmin
#Region ;**** Directives created by AutoIt3Wrapper_GUI ****
#AutoIt3Wrapper_Version=Beta
#AutoIt3Wrapper_Icon=..\..\..\..\..\..\Pictures\TimerRes.ico
#AutoIt3Wrapper_Outfile=SetTimerRes (Deprecated).exe
#AutoIt3Wrapper_Outfile_x64=SetTimerRes_x64.exe
#AutoIt3Wrapper_Compression=4
#AutoIt3Wrapper_Res_Description=SetTimerRes
#AutoIt3Wrapper_Res_Fileversion=1.0.0.32
#AutoIt3Wrapper_Res_Fileversion_AutoIncrement=y
#AutoIt3Wrapper_Res_ProductName=SetTimerRes
#AutoIt3Wrapper_Res_ProductVersion=1.0.0.0
#AutoIt3Wrapper_Res_CompanyName=LuSlower
#AutoIt3Wrapper_Res_LegalCopyright=Copyright © Luis Garcia
#AutoIt3Wrapper_Res_requestedExecutionLevel=requireAdministrator
#EndRegion ;**** Directives created by AutoIt3Wrapper_GUI ****
#include <Constants.au3>
#include <GUIConstants.au3>
#include <TrayConstants.au3>
#include <WinAPIProc.au3>
#include <WinAPITheme.au3>
#include <Misc.au3>
#include <WinAPIHObj.au3>
_Singleton("SetTimerRes_x64.exe")
_WinAPI_SetThemeAppProperties(1)
Global $bColor = 0x464646, _
		$gColor = 0x2c2c2c, _
		$Color = 0xFFFFFF, _
		$global_timer, _
		$state, _
		$max, _
		$start, _
		$min, _
		$current, _
		$butt_max, _
		$butt_min, _
		$butt_current, _
		$custom_timer

#Region ;INI Params
Global $INI = @ScriptDir & "\SetTimerRes.ini"
Global $save_custom_timer = IniRead($INI, "TimerConfig", "CustomTimer", "set timer")
Global $save_global_timer = IniRead($INI, "TimerConfig", "GlobalTimer", "4")
Global $save_start = IniRead($INI, "Start", "Windows", "4")
#EndRegion ;INI Params


Opt("TrayMenuMode", 3) ;enable tray personalizado
Opt("MustDeclareVars", 0) ;0=no, 1=require pre-declaration
Opt("TrayOnEventMode", 1) ;enable tray event mode
Opt("GUIOnEventMode", 1) ; Change to OnEvent mode
TraySetClick(8) ;solo clic derecho
;Tray Group
TrayCreateItem("Exit")
TrayItemSetOnEvent(-1, "_close")
TraySetState($TRAY_ICONSTATE_SHOW)

; Llamar a NtQueryTimerResolution
Func NtQueryTimerResolution()
	_WinAPI_EmptyWorkingSet()
	Global $save_custom_timer = IniRead($INI, "TimerConfig", "CustomTimer", "set timer")
	Local $global = GUICtrlRead($global_timer)
	Switch $global
		Case $global = 1
			RegWrite("HKLM64\SYSTEM\CurrentControlSet\Control\Session Manager\kernel", _
					"GlobalTimerResolutionRequests", _
					"REG_DWORD", _
					"1")
			_WinAPI_SetProcessInformation()
		Case $global = 4
			RegDelete("HKLM64\SYSTEM\CurrentControlSet\Control\Session Manager\kernel", _
					"GlobalTimerResolutionRequests")
	EndSwitch
	IniWrite($INI, "TimerConfig", "GlobalTimer", $global)
	GUICtrlSetState($global_timer, $global)
	Local $QUERY_TIMER_RES = DllStructCreate("ULONG_PTR MinimumResolution; ULONG_PTR MaximumResolution; ULONG_PTR CurrentResolution")
	Local $result = DllCall("ntdll.dll", _
			"INT", "NtQueryTimerResolution", _
			"PTR", DllStructGetPtr($QUERY_TIMER_RES, 1), _
			"PTR", DllStructGetPtr($QUERY_TIMER_RES, 2), _
			"PTR", DllStructGetPtr($QUERY_TIMER_RES, 3))
	Global $current = DllStructGetData($QUERY_TIMER_RES, "CurrentResolution")
	Global $max = DllStructGetData($QUERY_TIMER_RES, "MaximumResolution")
	Global $min = DllStructGetData($QUERY_TIMER_RES, "MinimumResolution")
	;update data
	GUICtrlSetData($custom_timer, $current)
	GUICtrlSetData($butt_current, "Current Resolution : " & $current & " (ns)")
	GUICtrlSetData($butt_max, "Maximum Resolution : " & $max & " (ns)")
	GUICtrlSetData($butt_min, "Minimum Resolution : " & $min & " (ns)")
EndFunc   ;==>NtQueryTimerResolution
;==============================================================

; Llamar a NtSetTimerResolution
Func NtSetTimerResolution($desiredRes, $setRes)
	_WinAPI_EmptyWorkingSet()
	Local $SET_TIMER_RES = DllStructCreate("ULONG_PTR CurrentResolution")
	Local $result = DllCall("ntdll.dll", "int", "NtSetTimerResolution", _
			"ptr", $desiredRes, _
			"bool", $setRes, _
			"ptr", DllStructGetPtr($SET_TIMER_RES, 1))
	Global $max = DllStructGetData($SET_TIMER_RES, "CurrentResolution")
EndFunc   ;==>NtSetTimerResolution
;==============================================================
NtQueryTimerResolution()
If $save_custom_timer <> "set timer" Then
	NtSetTimerResolution($save_custom_timer, True)
	NtQueryTimerResolution()
EndIf

Main()
;==============================================================
Func Main()
	GUIDelete()
	Global $Main = GUICreate("SetTimerRes | LuSlower", 195, 250, Default, Default, $WS_POPUP, $WS_EX_CONTROLPARENT)
	NtQueryTimerResolution()
	_WinAPI_SetProcessInformation()
	GUISetBkColor($gColor)

	;Set Timer Group
	GUICtrlCreateGroup("SetTimerRes", 5, 5, 185, 240)
	GUICtrlSetColor(-1, $Color)
	Global $butt_max = GUICtrlCreateLabel("Maximum Resolution : " & $max & " (ns)", 20, 30)
	GUICtrlSetColor(-1, $Color)
	Global $butt_min = GUICtrlCreateLabel("Minimum Resolution : " & $min & " (ns)", 20, 50)
	GUICtrlSetColor(-1, $Color)
	Global $butt_current = GUICtrlCreateLabel("Current Resolution : " & $current & " (ns)", 20, 70)
	GUICtrlSetColor(-1, $Color)
	Global $set_max = GUICtrlCreateButton("Set Max", 20, 90, 80, 20)
	GUICtrlSetOnEvent(-1, "_setmax")
	GUICtrlSetBkColor(-1, $bColor)
	GUICtrlSetColor(-1, $Color)
	Global $set_min = GUICtrlCreateButton("Set Min", 100, 90, 80, 20)
	GUICtrlSetOnEvent(-1, "_setmin")
	GUICtrlSetBkColor(-1, $bColor)
	GUICtrlSetColor(-1, $Color)

	;Custom
	GUICtrlCreateLabel("Custom Resolution :", 55, 120)
	GUICtrlSetColor(-1, $Color)
	Global $custom_timer = GUICtrlCreateInput($save_custom_timer, 75, 140, 45, 15)
	Global $set_custom = GUICtrlCreateButton("Set Custom", 40, 165, 115, 20)
	GUICtrlSetOnEvent(-1, "_setcustom")
	GUICtrlSetState(-1, 256)
	GUICtrlSetBkColor(-1, $bColor)
	GUICtrlSetColor(-1, $Color)
	Global $global_timer = GUICtrlCreateCheckbox("GlobalTimerRequest (Win11)", 15, 200)
	GUICtrlSetColor(-1, $Color)
	GUICtrlSetState(-1, $save_global_timer)
	Global $start = GUICtrlCreateCheckbox("Cargar con Windows (Minim)", 15, 220)
	GUICtrlSetColor(-1, $Color)
	GUICtrlSetState(-1, $save_start)
	GUISetOnEvent($GUI_EVENT_CLOSE, "_hide")
	TraySetOnEvent($TRAY_EVENT_PRIMARYDOWN, "_show")
	GUISetState()
	_WinAPI_EmptyWorkingSet()
EndFunc   ;==>Main

Switch $save_start
	Case 1
		_hide()
	Case Else
		_show()
EndSwitch

While True
	If WinExists($Main) = 1 Then
		Sleep(2500)
	Else
		_pause()
	EndIf
WEnd

Func _pause()
	WinWaitNotActive($Main)
EndFunc

Func _setcustom()
	_WinAPI_EmptyWorkingSet()
	Global $custom = GUICtrlRead($custom_timer)
	Select
		Case StringLen($custom) > 6 Or StringIsInt($custom) = 0
			GUISetState(@SW_DISABLE)
			MsgBox(16, "Error", "Por favor escriba un numero entero con un máximo de 6 digitos", 5)
			GUISetState(@SW_ENABLE)
			GUISetState(@SW_RESTORE)
		Case StringIsInt($custom) = 1
			IniWrite($INI, "TimerConfig", "CustomTimer", $custom)
			NtSetTimerResolution($custom, True)
			NtQueryTimerResolution()
	EndSelect
EndFunc   ;==>_setcustom

Func _setmin()
	_WinAPI_EmptyWorkingSet()
	NtQueryTimerResolution()
	NtSetTimerResolution($min, True)
EndFunc   ;==>_setmin

Func _setmax()
	_WinAPI_EmptyWorkingSet()
	NtQueryTimerResolution()
	NtSetTimerResolution($max, True)
EndFunc   ;==>_setmax

Func _close()
	_WinAPI_EmptyWorkingSet()
	Local $key_run = "HKLM64\SOFTWARE\Microsoft\Windows\CurrentVersion\Run"
	Local $value = GUICtrlRead($start)
	Switch $value
		Case 1
			RegWrite($key_run, "SetTimerRes", "reg_sz", @ScriptFullPath)
			IniWrite($INI, "Start", "Windows", $value)
		Case 4
			RegDelete($key_run, "SetTimerRes")
			IniWrite($INI, "Start", "Windows", $value)
	EndSwitch
	GUIDelete()
	Exit
EndFunc   ;==>_close

Func _show()
	_WinAPI_EmptyWorkingSet()
	_WinAPI_SetPriorityClass($HIGH_PRIORITY_CLASS)
	Main()
EndFunc   ;==>_show

Func _hide()
	_WinAPI_EmptyWorkingSet()
	_WinAPI_SetPriorityClass($IDLE_PRIORITY_CLASS)
	GUIDelete()
EndFunc   ;==>_hide

Func _WinAPI_SetProcessInformation()
$hProcess = _WinAPI_GetCurrentProcess()
Local $ProcessPowerThrottling = 0x1E ;PROCESS_POWER_THROTTLING
Local $PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION = 0
If $hProcess = 0 Then
    MsgBox(16, "Error", "Error al abrir el proceso")
    Return
EndIf
Local $PROCESS_POWER_THROTTLING_STATE = DllStructCreate("UInt")
DllStructSetData($PROCESS_POWER_THROTTLING_STATE, 1, $PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION)
Local $status = DllCall("kernel32.dll", _
		"LONG", "SetProcessInformation", _
		"HANDLE", $hProcess, _
		"INT", $ProcessPowerThrottling, _
		"PTR", DllStructGetPtr($PROCESS_POWER_THROTTLING_STATE), _
		"DWORD", DllStructGetSize($PROCESS_POWER_THROTTLING_STATE))
ConsoleWrite($status [0] & @CRLF)
EndFunc