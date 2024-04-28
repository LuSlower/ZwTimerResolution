# ZwTimerResolution
[![Total Downloads](https://img.shields.io/github/downloads/LuSlower/ZwTimerResolution/total.svg)](https://github.com/LuSlower/ZwTimerResolution/releases) [![PayPal Badge](https://img.shields.io/badge/PayPal-003087?logo=paypal&logoColor=fff&style=flat)](https://paypal.me/eldontweaks) 

Una herramienta más para cambiar la resolución del temporizador del sistema en el contexto del proceso de llamada

## ACTUALMENTE
GUI:

![image](https://github.com/LuSlower/ZwTimerResolution/assets/148411728/0a3e3c6c-74ea-4109-acb1-e892a68f443f)

* Presione ESC para ocultar la GUI, puede hacer clic en el icono de la bandeja para salir de la aplicación
* Se permiten solo 6 digitos como máximo en el cuadro de texto, debe ser un numero entero o arrojara un error
* La resolución del temporizador máxima será siempre 5000ns (5ms), asi que cualquier valor por debajo de ese ingresado sera reemplazado automaticamente por el, 
aplica la misma relatividad para el minimo 156250ns
* Esta GUI Win32 consume poca memoria (cerca de 1.3k de total bytes)

Consola:

![image](https://github.com/LuSlower/ZwTimerResolution/assets/148411728/acf5bbde-7ee3-477c-a11b-5a87fef54481)

* Abrirla o ejecutarla sin argumentos por defecto ejecutara el test de precision de Sleep(1)
* Algo más ligero en consumo (menos de 400k de total bytes)
* Obtenga ayuda pasando el arg /? o help

# INFO
> Reducir el intervalo del temporizador tiene un efecto adverso en la duración de la batería al evitar que el sistema quede inactivo. Es particularmente importante que los programas que pueden permanecer inactivos durante largos períodos de tiempo (reproductores de video/música en pausa, programas minimizados, programas en segundo plano, etc.) no aumenten la frecuencia del temporizador cuando no la necesitan.

* Si posees Windows 11 debes tener en tu registro GlobalTimerResolutionRequests (reiniciar), referencia a un comentario en: [EL GRAN CAMBIO DE REGLAS](https://randomascii.wordpress.com/2020/10/04/windows-timer-resolution-the-great-rule-change/)

> [HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\kernel]
"GlobalTimerResolutionRequests"=dword:00000001

si desea establecer una resolución personalizada específique

```
zwtimer.exe 5000
```

## PRECISION

Al pasar el argumento 'test' zwtimer se ejecutara en un bucle, para comprobar la precision de Sleep(1)

```
comienza la prueba...

time: 0.0014 s | sleep(1): 1.4140 ms | delta: 0.4140 ms | zwres: 5054 ns
time: 0.0010 s | sleep(1): 1.0058 ms | delta: 0.0058 ms | zwres: 5054 ns
time: 0.0010 s | sleep(1): 1.0062 ms | delta: 0.0062 ms | zwres: 5054 ns
time: 0.0010 s | sleep(1): 1.0054 ms | delta: 0.0054 ms | zwres: 5054 ns
time: 0.0010 s | sleep(1): 1.0064 ms | delta: 0.0064 ms | zwres: 5054 ns
```

aparte de 'test' puede pasar dos argumentos más 'start' y 'end', esto ejecutará una prueba específica para verificar que resolución posee valores más precisos
esto no quiere decir que sean estables, siempre habrá una fluctuación así usted debe hacer pruebas para verificar cual de todas le conviene...

el TEST no funcionará si poseen aplicaciones abiertas como DISCORD y CHROME, ya que configuran la resolucion del temporizador a 1ms

```
comienza la prueba...

inicio : 5000
final : 5200
conteo : 20

sleep(1): 1.4955 ms | delta: 0.4955 ms | zwres: 4990 ns
sleep(1): 1.4998 ms | delta: 0.4998 ms | zwres: 5003 ns
sleep(1): 1.4287 ms | delta: 0.4287 ms | zwres: 5016 ns
sleep(1): 1.2060 ms | delta: 0.2060 ms | zwres: 5029 ns
sleep(1): 1.1324 ms | delta: 0.1324 ms | zwres: 5041 ns
sleep(1): 1.0341 ms | delta: 0.0341 ms | zwres: 5054 ns
sleep(1): 1.0375 ms | delta: 0.0375 ms | zwres: 5067 ns
sleep(1): 1.0145 ms | delta: 0.0144 ms | zwres: 5080 ns
sleep(1): 1.0175 ms | delta: 0.0175 ms | zwres: 5093 ns
sleep(1): 1.0197 ms | delta: 0.0197 ms | zwres: 5105 ns
sleep(1): 1.0470 ms | delta: 0.0470 ms | zwres: 5118 ns
sleep(1): 1.0249 ms | delta: 0.0249 ms | zwres: 5131 ns
sleep(1): 1.0273 ms | delta: 0.0273 ms | zwres: 5144 ns
sleep(1): 1.0308 ms | delta: 0.0308 ms | zwres: 5157 ns
sleep(1): 1.0323 ms | delta: 0.0323 ms | zwres: 5170 ns
sleep(1): 1.0353 ms | delta: 0.0353 ms | zwres: 5182 ns

prueba finalizada...

minimo: 1.0029 ms | 0.0029 ms
maximo: 1.5182 ms | 0.5182 ms
```

al finalizar los resultados se guardarán en sleep-test.txt ubicado en el mismo directorio de zwtimer.exe
para visualizar cual de todas las resoluciones obtuvo una mayor precisión (menor delta, menor diff)
puede visualizarlos en https://list2chart.com/csv-to-chart/

simplemente arraste el archivo allí y específique que columna desea comparar

aquí hay un ejemplo

![image](https://github.com/LuSlower/nt_timer/assets/148411728/18d45865-1cf7-4a3d-9811-48ee45c53426)

## DERECHOS DE AUTOR Y LICENCIA
[LICENCIA](LICENSE)
