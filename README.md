# ZwTimerResolution
[![Total Downloads](https://img.shields.io/github/downloads/LuSlower/ZwTimerResolution/total.svg)](https://github.com/LuSlower/ZwTimerResolution/releases) [![PayPal Badge](https://img.shields.io/badge/PayPal-003087?logo=paypal&logoColor=fff&style=flat)](https://paypal.me/eldontweaks) 

Una herramienta más para cambiar la resolución del temporizador del sistema en el contexto del proceso de llamada

## ACTUALMENTE
GUI:

![image](https://github.com/LuSlower/ZwTimerResolution/assets/148411728/1f00528a-37f9-48bd-942f-1cdfc2044ecf)

* Presione ESC para ocultar la GUI, puede hacer clic en el icono de la bandeja para salir de la aplicación
* Se permiten solo 6 dígitos como máximo en el cuadro de texto, debe ser un número entero o arrojara un error
* La resolución del temporizador máxima será siempre 5000 (*100) ns = 5ms, así que cualquier valor por debajo de ese ingresado sera reemplazado automáticamente por el, 
aplica la misma relatividad para el mínimo 156250ns
* Esta GUI Win32 consume poca memoria (cerca de 1.3k de total bytes)

Consola:

* Abrirla o ejecutarla sin argumentos por defecto ejecutara el test de precisión de Sleep(1)
* Algo más ligero en consumo (menos de 400k de total bytes)
* Obtenga ayuda pasando el arg /? o help

# INFO
> Reducir el intervalo del temporizador tiene un efecto adverso en la duración de la batería al evitar que el sistema quede inactivo. Es particularmente importante que los programas que pueden permanecer inactivos durante largos períodos de tiempo (reproductores de video/música en pausa, programas minimizados, programas en segundo plano, etc.) no aumenten la frecuencia del temporizador cuando no la necesitan.

* Si posees Windows Server 2022/Windows 11 debes tener en tu registro:

> [HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\kernel]
"GlobalTimerResolutionRequests"=dword:00000001

Si desea establecer una resolución personalizada especifique

```
zwt.exe 5000
```

## PRECISION

Al ejecutar zwtimer sin argumentos (abriendola) o al pasar el argumento 'test' zwtimer se ejecutará en un bucle para comprobar la precisión de Sleep(1)

```
C:\...>zwt.exe test

sleep(1): 15.6224 ms | delta: 14.6224 ms | zwres: 156244 ns
sleep(1): 15.6221 ms | delta: 14.6221 ms | zwres: 156244 ns
sleep(1): 15.6219 ms | delta: 14.6219 ms | zwres: 156244 ns
sleep(1): 15.6217 ms | delta: 14.6217 ms | zwres: 156244 ns
sleep(1): 15.6247 ms | delta: 14.6247 ms | zwres: 156244 ns
sleep(1): 15.6241 ms | delta: 14.6241 ms | zwres: 156244 ns
sleep(1): 15.6224 ms | delta: 14.6224 ms | zwres: 156244 ns
```

Aparte de 'test' puedes especificar 'count' que especificará el número de veces que se medirá la precisión de sleep(1)
y mostrará métricas más avanzadas
```
C:\...>zwt.exe test 10

sleep(1): 1.0072 ms | delta: 0.0072 ms | zwres: 5054 ns
sleep(1): 1.0122 ms | delta: 0.0122 ms | zwres: 5054 ns
sleep(1): 1.0104 ms | delta: 0.0104 ms | zwres: 5054 ns
sleep(1): 1.0063 ms | delta: 0.0063 ms | zwres: 5054 ns
sleep(1): 1.0105 ms | delta: 0.0105 ms | zwres: 5054 ns
sleep(1): 1.0103 ms | delta: 0.0103 ms | zwres: 5054 ns
sleep(1): 1.0116 ms | delta: 0.0116 ms | zwres: 5054 ns
sleep(1): 1.0111 ms | delta: 0.0111 ms | zwres: 5054 ns
sleep(1): 1.0111 ms | delta: 0.0111 ms | zwres: 5054 ns
sleep(1): 1.0113 ms | delta: 0.0113 ms | zwres: 5054 ns

min: 1.0063 ms | 0.0063 ms
max: 1.0122 ms | 0.0122 ms
avg: 1.0102 ms | 0.0102 ms
stdev: 0.0018 ms | 0.0018 ms
```

[Sleep-Tester](https://github.com/LuSlower/ZwTimerResolution/blob/main/Sleep-Tester.ps1)

![image](https://github.com/LuSlower/ZwTimerResolution/assets/148411728/f0564228-b59b-4052-b896-4d91c902e53e)

Puede ejecutar una prueba donde puede epecificar las resoluciones de inicio, final y conteo,
recuerde que siempre habrá una fluctuación, así que usted debe hacer pruebas para verificar cuál de todas le conviene...

Al finalizar los resultados se guardarán en sleep-test.txt ubicado en el mismo directorio,
para visualizar cuál de todas las resoluciones obtuvo una mayor precisión (menor delta, menor diff),
puede visualizarlos en https://list2chart.com/csv-to-chart/, simplemente arrastre el archivo allí y especifique qué columna desea comparar

Aquí hay un ejemplo

![image](https://github.com/LuSlower/nt_timer/assets/148411728/18d45865-1cf7-4a3d-9811-48ee45c53426)
