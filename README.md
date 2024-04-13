# SetTimerRes
Una herramienta más para cambiar la resolución del temporizador del sistema en el contexto del proceso de llamada

![image](https://github.com/LuSlower/SetTimerRes/assets/148411728/80fc285e-8187-4805-a54c-8419eb1f309e)


## ACTUALMENTE
* Presione ESC para ocultar la GUI, puede hacer clic en el icono de la bandeja para salir de la aplicación
* Se permiten solo 6 digitos como máximo en el cuadro de texto, debe ser un numero entero o arrojara un error
* La resolución del temporizador máxima será siempre 5000ns (5ms), asi que cualquier valor por debajo de ese ingresado sera reemplazado automaticamente por el, 
aplica la misma relatividad para el minimo 156250ns
* Esta GUI Win32 consume poca memoria (cerca de 1.3k de total bytes)

# INFO
* La primera version de este programa se baso en una version de AutoIT3, puedes verificar el archivo [AQUI](AU3/SetTimerRes(OnEvent).au3)
deje de programarlo alli por que sondea demasiado, este script posee un rendimiento excelente a comparación del de AutoIT3
* La segunda versión se baso en esta [version](SetTimerRes.ahk) de un script AHK, ligero pero depende de un DLL
* MSDN: Reducir el intervalo del temporizador tiene un efecto adverso en la duración de la batería al evitar que el sistema quede inactivo. Es particularmente importante que los programas que pueden permanecer inactivos durante largos períodos de tiempo (reproductores de video/música en pausa, programas minimizados, programas en segundo plano, etc.) no aumenten la frecuencia del temporizador cuando no la necesitan.

* Si posees Windows 11 debes tener en tu registro GlobalTimerResolutionRequests (reiniciar), referencia a un comentario en: [EL GRAN CAMBIO DE REGLAS](https://randomascii.wordpress.com/2020/10/04/windows-timer-resolution-the-great-rule-change/)

## CONTRIBUYENDO
Gracias AutoHotkey y AutoIT Comunmity

## CONSTRUIR
* Descarga SetTimerRes.ahk
* Debes tener instalada la ultima versión de AHK v1.1 (AHK_L), la puedes encontrar en: [AutoHotKey GitHub](https://github.com/AutoHotkey/AutoHotkey)
* Simplemente edite el script a su gusto, mi IDE preferido es SciTE4AHK

## DERECHOS DE AUTOR Y LICENCIA
[LICENCIA](LICENSE)
