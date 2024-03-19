# SetTimerRes
Una herramienta más para cambiar la resolución del temporizador del sistema en el contexto del proceso de llamada

## ACTUALMENTE
* Se permiten solo 7 digitos en el cuadro de texto, debe ser un numero entero o arrojara un error
* La resolución del temporizador máxima será siempre 5000ns (5ms), asi que cualquier valor por debajo de ese ingresado sera reemplazado automaticamente por el, 
aplica la misma relatividad para el minimo 156250ns
* Hago uso de LoadLibrary y FreeLibrary para maximizar el rendimiento de DllCall(), las llamadas dinamicas constantes pueden bloquear el script
* Tambien hago uso de EmptyWorkingSet y SetProcessWorkingSetSize para recolectar algo de basura (este lenguaje no posee uno)

## INFO
* La primera version de este programa se baso en una version de AutoIT3, puedes verificar el archivo [AQUI](AU3/SetTimerRes(OnEvent).au3)
deje de programarlo alli pq su unico hilo sondea demasiado, la GUI siempre tiene que estar en un WHILE, DO/UNITL en cambio AHK usa un mejor manejo para su GUI lo que hace que el proceso no sondee todo el tiempo, este script posee un rendimiento excelente a comparación del de AutoIT3

* Si posees Windows 11 debes tener marcada la casilla GlobalTimerRequest, referencia: [EL GRAN CAMBIO DE REGLAS](https://randomascii.wordpress.com/2020/10/04/windows-timer-resolution-the-great-rule-change/)

* Nose si funciona en Windows 11 y Windows Server 2022+ luego de minimizar/ocultar el proceso, debido a que aun no implemento una forma correcta para llamar dinamicamente a SetProcessInformation, estaré trabajando en aquello

## CONTRIBUYENDO
Gracias AutoHotkey y AutoIT Comunmity

## CONSTRUIR
* Debes tener instalada la ultima versión de AHK v1.1 (AHK_L), la puedes encontrar en: [AutoHotKey GitHub](https://github.com/AutoHotkey/AutoHotkey)
* Simplemente edite el script a su gusto

## DERECHOS DE AUTOR Y LICENCIA
[LICENCIA](LICENSE)
