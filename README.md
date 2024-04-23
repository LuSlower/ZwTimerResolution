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

![image](https://github.com/LuSlower/nt_timer/assets/148411728/f53b4609-3a62-481d-8bc7-f1722673c408)

* Abrirla o ejecutarla sin argumentos por defecto ejecutara el test de precision de Sleep(1)
* Algo más ligero en consumo (cerca 400k de total bytes)
* Obtenga ayuda pasando el arg /? o help

# INFO
> Reducir el intervalo del temporizador tiene un efecto adverso en la duración de la batería al evitar que el sistema quede inactivo. Es particularmente importante que los programas que pueden permanecer inactivos durante largos períodos de tiempo (reproductores de video/música en pausa, programas minimizados, programas en segundo plano, etc.) no aumenten la frecuencia del temporizador cuando no la necesitan.

* Si posees Windows 11 debes tener en tu registro GlobalTimerResolutionRequests (reiniciar), referencia a un comentario en: [EL GRAN CAMBIO DE REGLAS](https://randomascii.wordpress.com/2020/10/04/windows-timer-resolution-the-great-rule-change/)

> [HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\kernel]
"GlobalTimerResolutionRequests"=dword:00000001

si desea establecer una resolución personalizada específique

```
nt_timer_cs.exe 5000
```

## PRECISION

Al pasar el argumento 'test' nt_timer_cs se ejecutara en un bucle, para comprobar la precision de Sleep(1)

```
comienza la prueba...

time: 0.0010 s | tsleep: 1.0316 ms | delta: 0.0316 ms | nt timer: 5054 ns
time: 0.0010 s | tsleep: 1.0056 ms | delta: 0.0056 ms | nt timer: 5054 ns
time: 0.0010 s | tsleep: 1.0064 ms | delta: 0.0064 ms | nt timer: 5054 ns
```

aparte de 'test' puede pasar dos argumentos más 'start' y 'end', esto ejecutará una prueba específica para verificar que resolución posee valores más precisos
esto no quiere decir que sean estables, siempre habrá una fluctuación así usted debe hacer pruebas para verificar cual de todos le conviene...

```
comienza la prueba...

inicio : 5000
final : 5100

time: 0.0015 s | tsleep: 1.4936 ms | delta: 0.4936 ms | nt timer: 4990 ns
time: 0.0015 s | tsleep: 1.4976 ms | delta: 0.4976 ms | nt timer: 5003 ns
time: 0.0015 s | tsleep: 1.5064 ms | delta: 0.5064 ms | nt timer: 5016 ns
time: 0.0010 s | tsleep: 1.0082 ms | delta: 0.0082 ms | nt timer: 5029 ns
time: 0.0010 s | tsleep: 1.0049 ms | delta: 0.0049 ms | nt timer: 5029 ns
time: 0.0010 s | tsleep: 1.0067 ms | delta: 0.0067 ms | nt timer: 5041 ns
time: 0.0015 s | tsleep: 1.5087 ms | delta: 0.5087 ms | nt timer: 5054 ns
time: 0.0010 s | tsleep: 1.0112 ms | delta: 0.0112 ms | nt timer: 5067 ns
time: 0.0010 s | tsleep: 1.0103 ms | delta: 0.0103 ms | nt timer: 5067 ns
time: 0.0010 s | tsleep: 1.0187 ms | delta: 0.0187 ms | nt timer: 5080 ns
time: 0.0010 s | tsleep: 1.0171 ms | delta: 0.0171 ms | nt timer: 5093 ns
prueba finalizada...

minimo: 0.0049 ms
maximo: 0.5087 ms
promedio: 0.1894 ms
```

al finalizar los resultados se guardarán en sleep-test.txt ubicado en el mismo directorio de nt_timer_cs
para visualizar cual de todas las resoluciones obtuvo una mayor precisión (menor delta, menor diff)
puede visualizarlos en https://list2chart.com/csv-to-chart/

simplemente arraste el archivo allí y específique que columna desea comparar

aquí hay un ejemplo

![image](https://github.com/LuSlower/nt_timer/assets/148411728/475d5904-d60c-4a86-892b-0878958d6907)

## DERECHOS DE AUTOR Y LICENCIA
[LICENCIA](LICENSE)
