format PE GUI 4.0 DLL
entry DisableIgnoreTimer

include 'win32ax.inc'

section '.data' data readable writeable
    ProcessPowerThrottling dd 19
    IgnoreTimerResolution dd 1

section '.code' code readable executable
DisableIgnoreTimer:
    ; Obtener el handle pasado como parámetro
    pop ebx ; ebx = handle del proceso

    ; Definir la información de energía para deshabilitar PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION
    mov eax, IgnoreTimerResolution
    mov edx, 4 ; Tamaño de IgnoreTimerResolution en bytes

    ; Llamar a SetProcessInformation para deshabilitar PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION
    invoke SetProcessInformation, ebx, ProcessPowerThrottling, eax, edx

    ; Verificar si la llamada fue exitosa
    test eax, eax
    jnz exit_failure ; Si eax no es cero, la llamada falló

    ; La llamada fue exitosa
    jmp exit

    exit_failure:
    ; Puedes realizar alguna acción si la llamada falla, como registrar un mensaje en un archivo de registro

    exit:
    ; Salir de la DLL
    ret

section '.idata' import data readable writeable

    library kernel32, 'KERNEL32.DLL'

    import kernel32, \
           SetProcessInformation, 'SetProcessInformation'