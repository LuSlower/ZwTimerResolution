# nt_timer
[![Total Downloads](https://img.shields.io/github/downloads/LuSlower/nt_timer/total.svg)](https://github.com/LuSlower/nt_timer/releases) [![PayPal Badge](https://img.shields.io/badge/PayPal-003087?logo=paypal&logoColor=fff&style=flat)](https://paypal.me/eldontweaks) 

Una herramienta más para cambiar la resolución del temporizador del sistema en el contexto del proceso de llamada

## ACTUALMENTE
GUI:

![image](https://github.com/LuSlower/nt_timer/assets/148411728/53bff5b4-5bdc-40e7-8694-3a95f27c7296)

* Presione ESC para ocultar la GUI, puede hacer clic en el icono de la bandeja para salir de la aplicación
* Se permiten solo 6 digitos como máximo en el cuadro de texto, debe ser un numero entero o arrojara un error
* La resolución del temporizador máxima será siempre 5000ns (5ms), asi que cualquier valor por debajo de ese ingresado sera reemplazado automaticamente por el, 
aplica la misma relatividad para el minimo 156250ns
* Esta GUI Win32 consume poca memoria (cerca de 1.3k de total bytes)

Consola:

![image](https://github.com/LuSlower/nt_timer/assets/148411728/376f9fe4-3eb5-4dd6-a0ea-93cfad5b2a07)

* Algo más ligero (cerca 400k de total bytes)
* Obtenga ayuda pasando el arg /? o help

# INFO
> Reducir el intervalo del temporizador tiene un efecto adverso en la duración de la batería al evitar que el sistema quede inactivo. Es particularmente importante que los programas que pueden permanecer inactivos durante largos períodos de tiempo (reproductores de video/música en pausa, programas minimizados, programas en segundo plano, etc.) no aumenten la frecuencia del temporizador cuando no la necesitan.

* Si posees Windows 11 debes tener en tu registro GlobalTimerResolutionRequests (reiniciar), referencia a un comentario en: [EL GRAN CAMBIO DE REGLAS](https://randomascii.wordpress.com/2020/10/04/windows-timer-resolution-the-great-rule-change/)

> [HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\kernel]
"GlobalTimerResolutionRequests"=dword:00000001

si desea establecer una resolución personalizada cree un acceso directo y específique

`nt_timer_cs.exe 5000`

## PRECISION

Al pasar el argumento 'test' nt_timer_cs se ejecutara en un bucle programando un temporizador, para comprobar si realmente sleep hace que la CPU se detenga 1ms

![image](https://github.com/LuSlower/nt_timer/assets/148411728/d9294204-d31a-41a0-81b3-400de04fb826)


## DERECHOS DE AUTOR Y LICENCIA
[LICENCIA](LICENSE)
