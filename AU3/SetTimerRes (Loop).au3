#RequireAdmin
#Region ;**** Directives created by AutoIt3Wrapper_GUI ****
#AutoIt3Wrapper_Version=Beta
#AutoIt3Wrapper_Icon=..\..\..\..\..\Pictures\TimerRes.ico
#AutoIt3Wrapper_Compression=0
#AutoIt3Wrapper_UseX64=y
#AutoIt3Wrapper_Res_Description=SetTimerRes
#AutoIt3Wrapper_Res_Fileversion=1.0.0.21
#AutoIt3Wrapper_Res_Fileversion_AutoIncrement=y
#AutoIt3Wrapper_Res_ProductName=SetTimerRes
#AutoIt3Wrapper_Res_ProductVersion=1.0.0.0
#AutoIt3Wrapper_Res_CompanyName=LuSlower
#AutoIt3Wrapper_Res_LegalCopyright=Copyright © Luis Garcia
#AutoIt3Wrapper_Res_requestedExecutionLevel=requireAdministrator
#AutoIt3Wrapper_Run_AU3Check=n
#AutoIt3Wrapper_Tidy_Stop_OnError=
#AutoIt3Wrapper_Run_Debug_Mode=n
#EndRegion ;**** Directives created by AutoIt3Wrapper_GUI ****
#include <GUIConstants.au3>
#include <TrayConstants.au3>
#include <WinAPIProc.au3>
#include <WinAPITheme.au3>
_WinAPI_SetThemeAppProperties(1)
_WinAPI_SetPriorityClass($HIGH_PRIORITY_CLASS)
Global $bColor = 0x464646, _
		$gColor = 0x2c2c2c, _
		$Color = 0xFFFFFF, _
		$Main, _
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
;Tray Group
Global $tray_max = TrayCreateItem("Set Max")
TrayCreateItem("")
Global $tray_min = TrayCreateItem("Set Min")
TrayCreateItem("")
Global $tray_custom = TrayCreateItem("Set Custom")
TrayCreateItem("")
Global $tray_exit = TrayCreateItem("Exit")
TraySetState($TRAY_ICONSTATE_SHOW)
TraySetClick(8) ;solo clic derecho

Switch $save_start
	Case 1
		Global $gui_state = @SW_HIDE
	Case Else
		Global $gui_state = @SW_SHOW
EndSwitch

Func _state($state = 0)
	_WinAPI_EmptyWorkingSet()
	$main_state = WinGetState($Main)
	Switch $state
		Case 0
			GUIDelete($Main)
			While True

				Switch TrayGetMsg()
					Case $TRAY_EVENT_MOUSEOVER, $TRAY_EVENT_PRIMARYDOUBLE, $TRAY_EVENT_SECONDARYDOUBLE, $TRAY_EVENT_NONE

					Case $TRAY_EVENT_PRIMARYDOWN
						_WinAPI_EmptyWorkingSet()
						_state(2)

					Case $tray_max
						NtSetTimerResolution($max, True)
						NtQueryTimerResolution()

					Case $tray_min
						NtSetTimerResolution($min, True)
						NtQueryTimerResolution()

					Case $tray_custom
						Global $custom = GUICtrlRead($custom_timer)
						Select
							Case StringLen($custom) > 6 Or StringIsInt($custom) = 0
								_state(1)
								MsgBox(16, "Error", "Por favor escriba un numero entero con un máximo de 6 digitos", 5)
								_state(2)
							Case StringIsInt($custom) = 1
								IniWrite($INI, "TimerConfig", "CustomTimer", $custom)
								NtSetTimerResolution($custom, True)
								NtQueryTimerResolution()
						EndSelect

					Case $tray_exit
						Exit
				EndSwitch
			WEnd
		Case 1
			GUISetState(@SW_DISABLE)
		Case 2
			Switch $main_state
				Case 0
					Main()
					Return
				Case Else
					GUISetState(@SW_SHOW)
					Return
			EndSwitch
	EndSwitch
EndFunc   ;==>_state


; Llamar a NtQueryTimerResolution
Func NtQueryTimerResolution()
	_WinAPI_EmptyWorkingSet()
	Local $global = GUICtrlRead($global_timer)
	Switch $global
		Case $global = 1
			RegWrite("HKLM64\SYSTEM\CurrentControlSet\Control\Session Manager\kernel", _
					"GlobalTimerResolutionRequests", _
					"REG_DWORD", _
					"1")
		Case $global = 4
			RegDelete("HKLM64\SYSTEM\CurrentControlSet\Control\Session Manager\kernel", _
					"GlobalTimerResolutionRequests")
	EndSwitch
	IniWrite($INI, "TimerConfig", "GlobalTimer", $global)
	GUICtrlSetState($global_timer, $global)
	Local $QUERY_TIMER_RES = DllStructCreate("ULONG_PTR MinimumResolution; ULONG_PTR MaximumResolution; ULONG_PTR CurrentResolution")
	Local $result = DllCall("ntdll.dll", _
			"int", "NtQueryTimerResolution", _
			"ptr", DllStructGetPtr($QUERY_TIMER_RES, 1), _
			"ptr", DllStructGetPtr($QUERY_TIMER_RES, 2), _
			"ptr", DllStructGetPtr($QUERY_TIMER_RES, 3))
	Global $current = DllStructGetData($QUERY_TIMER_RES, "CurrentResolution")
	Global $max = DllStructGetData($QUERY_TIMER_RES, "MaximumResolution")
	Global $min = DllStructGetData($QUERY_TIMER_RES, "MinimumResolution")
	;update data
	GUICtrlSetData($custom_timer, $current)
	GUICtrlSetData($butt_current, "Current Resolution : " & $current & " (ns)")
	GUICtrlSetData($butt_max, "Maximum Resolution : " & $max & " (ns)")
	GUICtrlSetData($butt_min, "Minimum Resolution : " & $min & " (ns)")


	If $result[0] = 0 Then
		ConsoleWrite("NtQueryTimerResolution successful" & @CRLF)
		ConsoleWrite("Current Resolution: " & DllStructGetData($QUERY_TIMER_RES, "CurrentResolution") & @CRLF)
		ConsoleWrite("Maximum Resolution: " & DllStructGetData($QUERY_TIMER_RES, "MaximumResolution") & @CRLF)
		ConsoleWrite("Minimum Resolution: " & DllStructGetData($QUERY_TIMER_RES, "MinimumResolution") & @CRLF)
	Else
		ConsoleWrite("NtQueryTimerResolution failed with status code: " & $result[0] & @CRLF)
	EndIf
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

	If $result[0] = 0 Then
		ConsoleWrite("NtSetTimerResolution successful" & @CRLF)
		ConsoleWrite("Current Resolution: " & DllStructGetData($SET_TIMER_RES, "CurrentResolution") & @CRLF)
	Else
		ConsoleWrite("NtSetTimerResolution failed with status code: " & $result[0] & @CRLF)
	EndIf
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
	Global $Main = GUICreate("SetTimerRes | LuSlower", 195, 250, Default, Default, $WS_POPUP, $WS_EX_CONTROLPARENT)
	GUISetBkColor($gColor)

	;Set Timer Group
	GUICtrlCreateGroup("SetTimer", 5, 5, 185, 240)
	GUICtrlSetColor(-1, $Color)
	Global $butt_max = GUICtrlCreateLabel("Maximum Resolution : " & $max & " (ns)", 20, 30)
	GUICtrlSetColor(-1, $Color)
	Global $butt_min = GUICtrlCreateLabel("Minimum Resolution : " & $min & " (ns)", 20, 50)
	GUICtrlSetColor(-1, $Color)
	Global $butt_current = GUICtrlCreateLabel("Current Resolution : " & $current & " (ns)", 20, 70)
	GUICtrlSetColor(-1, $Color)
	Global $set_max = GUICtrlCreateButton("Set Max", 20, 90, 80, 20)
	GUICtrlSetBkColor(-1, $bColor)
	GUICtrlSetColor(-1, $Color)
	Global $set_min = GUICtrlCreateButton("Set Min", 100, 90, 80, 20)
	GUICtrlSetBkColor(-1, $bColor)
	GUICtrlSetColor(-1, $Color)

	;Custom
	GUICtrlCreateLabel("Custom Resolution :", 55, 120)
	GUICtrlSetColor(-1, $Color)
	Global $custom_timer = GUICtrlCreateInput($save_custom_timer, 75, 140, 45, 15)
	Global $set_custom = GUICtrlCreateButton("Set Custom", 40, 165, 115, 20)
	GUICtrlSetState(-1, 256)
	GUICtrlSetBkColor(-1, $bColor)
	GUICtrlSetColor(-1, $Color)
	Global $global_timer = GUICtrlCreateCheckbox("GlobalTimerRequest (Win11)", 15, 200)
	GUICtrlSetColor(-1, $Color)
	GUICtrlSetState(-1, $save_global_timer)
	Global $start = GUICtrlCreateCheckbox("Cargar con Windows (Minim)", 15, 220)
	GUICtrlSetColor(-1, $Color)
	GUICtrlSetState(-1, $save_start)

	GUISetState($gui_state)
	_WinAPI_EmptyWorkingSet()
	MainLoop()
EndFunc   ;==>Main
;==============================================================
Func MainLoop()
	While True
		Switch GUIGetMsg()
			Case $GUI_EVENT_MOUSEMOVE, $GUI_EVENT_DROPPED, $GUI_EVENT_RESIZED, 0

			Case $set_max
				NtSetTimerResolution($max, True)
				NtQueryTimerResolution()

			Case $set_min
				NtSetTimerResolution($min, True)
				NtQueryTimerResolution()

			Case $set_custom
				Global $custom = GUICtrlRead($custom_timer)
				Select
					Case StringLen($custom) > 6 Or StringIsInt($custom) = 0
						_state(1)
						MsgBox(16, "Error", "Por favor escriba un numero entero con un máximo de 6 digitos", 5)
						_state(2)
					Case StringIsInt($custom) = 1
						IniWrite($INI, "TimerConfig", "CustomTimer", $custom)
						NtSetTimerResolution($custom, True)
						NtQueryTimerResolution()
				EndSelect
			Case $GUI_EVENT_CLOSE
				_state(0)
		EndSwitch



		Switch TrayGetMsg()
			Case $TRAY_EVENT_MOUSEOVER, $TRAY_EVENT_PRIMARYDOUBLE, $TRAY_EVENT_SECONDARYDOUBLE, 0

			Case $TRAY_EVENT_PRIMARYDOWN
				_WinAPI_EmptyWorkingSet()
				_state(2)

			Case $tray_max
				NtSetTimerResolution($max, True)
				NtQueryTimerResolution()

			Case $tray_min
				NtSetTimerResolution($min, True)
				NtQueryTimerResolution()

			Case $tray_custom
				Global $custom = GUICtrlRead($custom_timer)
				Select
					Case StringLen($custom) > 6 Or StringIsInt($custom) = 0
						_state(1)
						MsgBox(16, "Error", "Por favor escriba un numero entero con un máximo de 6 digitos", 5)
						_state(2)
					Case StringIsInt($custom) = 1
						IniWrite($INI, "TimerConfig", "CustomTimer", $custom)
						NtSetTimerResolution($custom, True)
						NtQueryTimerResolution()
				EndSelect

			Case $tray_exit
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
				Exit
		EndSwitch
	WEnd
EndFunc   ;==>MainLoop

;~ 		$MEMORY_PRIORITY_VERY_LOW = 1, _
;~ 		$MEMORY_PRIORITY_LOW = 2, _
;~ 		$MEMORY_PRIORITY_MEDIUM = 3, _
;~ 		$MEMORY_PRIORITY_BELOW_NORMAL = 4, _
;~ 		$MEMORY_PRIORITY_NORMAL = 5
;~ Func PagePriority($MEM_PRIORITY)
;~ 	Local $PID = @AutoItPID
;~ 	Local $hHandle = _WinAPI_OpenProcess($PROCESS_ALL_ACCESS, 0, $PID, True)
;~ 	DllCall("Kernel32.dll", _
;~ 			"long", "SetProcessInformation", _
;~ 			"handle", $hHandle, _
;~ 			"int", $MEM_PRIORITY, _
;~ 			"ptr", $MEMORY_PRIORITY_INFORMATION, _
;~ 			"dword", Size($MEM_PRIORITY))

;~ EndFunc   ;==>PagePriority