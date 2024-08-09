# ZwTimerResolution
One more tool to change the resolution of the system timer in the context of the calling process

[![Total Downloads](https://img.shields.io/github/downloads/LuSlower/ZwTimerResolution/total.svg)](https://github.com/LuSlower/ZwTimerResolution/releases) [![PayPal Badge](https://img.shields.io/badge/PayPal-003087?logo=paypal&logoColor=fff&style=flat)](https://paypal.me/eldontweaks) 

## CLI

* Opening it or running it without arguments will by default run the Sleep(1) accuracy test
* Somewhat lighter in consumption (less than 400k total bytes)
* Get help by passing the arg /? or help

## GUI

![image](https://github.com/LuSlower/ZwTimerResolution/assets/148411728/1f00528a-37f9-48bd-942f-1cdfc2044ecf)

* Press ESC to hide the GUI, you can right click on the tray icon to exit the app
* This Win32 GUI consumes little memory (about 1.3k total bytes)

## INFO
> Reducing the timer interval has an adverse effect on battery life by preventing the system from going idle. It is particularly important that programs that may remain inactive for long periods of time (paused video/music players, minimized programs, background programs, etc.) do not increase the timer frequency when they do not need it.

* If you have Windows Server 2022/Windows 11 you must have in your registry:

> [HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\kernel]
"GlobalTimerResolutionRequests"=dword:00000001

If you want to set a custom resolution, please specify

```
zwt.exe 5000
```

## PRECISION

Running zwtimer without arguments (opening it) or passing the 'test' argument will run zwtimer in a loop to check the accuracy of Sleep(1)

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

Apart from 'test' you can specify 'count' which will specify the number of times the precision of sleep(1) will be measured.
and will show more advanced metrics
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
![image](https://github.com/user-attachments/assets/628595da-4379-4317-86c7-fc9911397c15)

At the end the results will be saved in sleep-test.txt located in the same directory,
to visualize which of all the resolutions obtained the highest precision (lowest delta, lowest diff),
you can view them in https://list2chart.com/csv-to-chart/, just drag the file there and specify which column you want to compare

Here is an example

![image](https://github.com/user-attachments/assets/fedf693d-72b1-43e9-95b6-1bba936d5ca9)

# REF
[1](https://github.com/valleyofdoom/TimerResolution)
[2](https://xkln.net/blog/powershell-sleep-duration-accuracy-and-windows-timers/)
