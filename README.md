# nt_timer
[![Total Downloads](https://img.shields.io/github/downloads/LuSlower/nt_timer/total.svg)](https://github.com/LuSlower/nt_timer/releases) [![PayPal Badge](https://img.shields.io/badge/PayPal-003087?logo=paypal&logoColor=fff&style=flat)](https://paypal.me/eldontweaks) 

Una herramienta más para cambiar la resolución del temporizador del sistema en el contexto del proceso de llamada

## ACTUALMENTE
GUI:

![image](https://github.com/LuSlower/nt_timer/assets/148411728/6c81dfa6-6f49-4c51-850f-deb36be22b84)

* Presione ESC para ocultar la GUI, puede hacer clic en el icono de la bandeja para salir de la aplicación
* Se permiten solo 6 digitos como máximo en el cuadro de texto, debe ser un numero entero o arrojara un error
* La resolución del temporizador máxima será siempre 5000ns (5ms), asi que cualquier valor por debajo de ese ingresado sera reemplazado automaticamente por el, 
aplica la misma relatividad para el minimo 156250ns
* Esta GUI Win32 consume poca memoria (cerca de 1.3k de total bytes)

Consola:

![image](https://github.com/LuSlower/nt_timer/assets/148411728/b83c7f09-b0d2-45b6-bf6c-245de0b7fee6)


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

Al pasar el argumento 'test' nt_timer_cs se ejecutara en un bucle, para comprobar la precision de Sleep(1)
aparte de 'test' puede pasar dos argumentos más 'start' y 'end', esto ejecutará una prueba específica para verificar que resolución posee valores más precisos
esto no quiere decir que sean estables, siempre habrá una fluctuación así usted debe hacer pruebas para verificar cual de todos le conviene...

![image](https://github.com/LuSlower/nt_timer/assets/148411728/a77c8795-0b6d-4c31-bd95-2ff83beb7d5f)



## DERECHOS DE AUTOR Y LICENCIA
[LICENCIA](LICENSE)
