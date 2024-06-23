# ZwTimerResolution
[![Total Downloads](https://img.shields.io/github/downloads/LuSlower/ZwTimerResolution/total.svg)](https://github.com/LuSlower/ZwTimerResolution/releases) [![PayPal Badge](https://img.shields.io/badge/PayPal-003087?logo=paypal&logoColor=fff&style=flat)](https://paypal.me/eldontweaks) 

Una herramienta más para cambiar la resolución del temporizador del sistema en el contexto del proceso de llamada

## ACTUALMENTE
GUI:

![image](https://github.com/LuSlower/ZwTimerResolution/assets/148411728/0a3e3c6c-74ea-4109-acb1-e892a68f443f)

* Presione ESC para ocultar la GUI, puede hacer clic en el icono de la bandeja para salir de la aplicación
* Se permiten solo 6 digitos como máximo en el cuadro de texto, debe ser un numero entero o arrojara un error
* La resolución del temporizador máxima será siempre 5000 (*100) ns = 5ms, asi que cualquier valor por debajo de ese ingresado sera reemplazado automaticamente por el, 
aplica la misma relatividad para el minimo 156250ns
* Esta GUI Win32 consume poca memoria (cerca de 1.3k de total bytes)

Consola:

* Abrirla o ejecutarla sin argumentos por defecto ejecutara el test de precision de Sleep(1)
* Algo más ligero en consumo (menos de 400k de total bytes)
* Obtenga ayuda pasando el arg /? o help

# INFO
> Reducir el intervalo del temporizador tiene un efecto adverso en la duración de la batería al evitar que el sistema quede inactivo. Es particularmente importante que los programas que pueden permanecer inactivos durante largos períodos de tiempo (reproductores de video/música en pausa, programas minimizados, programas en segundo plano, etc.) no aumenten la frecuencia del temporizador cuando no la necesitan.

* Si posees Windows Server 2022/Windows 11 debes tener en tu registro:

> [HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\kernel]
"GlobalTimerResolutionRequests"=dword:00000001

si desea establecer una resolución personalizada específique

```
zwtimer.exe 5000
```

## PRECISION

al ejecutar zwtimer sin argumentos (abriendo) o pasar el argumento <test> zwtimer se ejecutara en un bucle, para comprobar la precision de Sleep(1)

```
C:\...>zwtimer.exe test

sleep(1): 15.6224 ms | delta: 14.6224 ms | zwres: 156244 ns
sleep(1): 15.6221 ms | delta: 14.6221 ms | zwres: 156244 ns
sleep(1): 15.6219 ms | delta: 14.6219 ms | zwres: 156244 ns
sleep(1): 15.6217 ms | delta: 14.6217 ms | zwres: 156244 ns
sleep(1): 15.6247 ms | delta: 14.6247 ms | zwres: 156244 ns
sleep(1): 15.6241 ms | delta: 14.6241 ms | zwres: 156244 ns
sleep(1): 15.6224 ms | delta: 14.6224 ms | zwres: 156244 ns
```

aparte de 'test' puedes específicar <count>, específicará el numero de veces que se medira la precision de sleep(1)
y mostrará metricas más avanzadas
```
C:\...>zwtimer.exe test 10

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

puede ejecutar una prueba donde puede epecíficar las resoluciones de inicio, final y conteo

> recuerde que siempre habrá una fluctuación así que usted debe hacer pruebas para verificar cual de todas le conviene...

al finalizar los resultados se guardarán en sleep-test.txt ubicado en el mismo directorio,
para visualizar cual de todas las resoluciones obtuvo una mayor precisión (menor delta, menor diff)

puede visualizarlos en https://list2chart.com/csv-to-chart/

simplemente arraste el archivo allí y específique que columna desea comparar

aquí hay un ejemplo

![image](https://github.com/LuSlower/nt_timer/assets/148411728/18d45865-1cf7-4a3d-9811-48ee45c53426)
