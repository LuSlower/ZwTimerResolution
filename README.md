# ZwTimerResolution
One more tool to change the resolution of the system timer in the context of the calling process

[![Total Downloads](https://img.shields.io/github/downloads/LuSlower/ZwTimerResolution/total.svg)](https://github.com/LuSlower/ZwTimerResolution/releases)

## CLI

* Opening it or running it without arguments will by default run the Sleep(1) accuracy test
* Somewhat lighter in consumption (less than 400k total bytes)
* Get help by passing the arg /? or help

## GUI

![image](https://github.com/user-attachments/assets/55c16ad5-7e6a-45a5-a8f4-4a3d4c311e8a)

* Press ESC to hide the GUI, you can right click on the tray icon to exit the app
* This Win32 GUI consumes little memory (about 1.3k total bytes)

## INFO
> Reducing the timer interval has an adverse effect on battery life by preventing the system from going idle. It is particularly important that programs that may remain inactive for long periods of time (paused video/music players, minimized programs, background programs, etc.) do not increase the timer frequency when they do not need it.

* If you have Windows Server 2022/Windows 11 you must have in your registry:

> [HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\kernel]
"GlobalTimerResolutionRequests"=dword:00000001

If you want to set a custom resolution, please specify

```
zwt.exe --res 5000
```

## PRECISION

Running zwtimer without arguments (opening it) or passing the '--test' argument will run zwtimer in a loop to check the accuracy of Sleep(1)

```
C:\...>zwt.exe --test

sleep(1): 1.0087 (delta: 0.0087) | zwres: 5060 hns
sleep(1): 1.0099 (delta: 0.0099) | zwres: 5060 hns
sleep(1): 1.0083 (delta: 0.0083) | zwres: 5060 hns
sleep(1): 1.0096 (delta: 0.0096) | zwres: 5060 hns
sleep(1): 1.0089 (delta: 0.0089) | zwres: 5060 hns
sleep(1): 1.0088 (delta: 0.0088) | zwres: 5060 hns
sleep(1): 1.0083 (delta: 0.0083) | zwres: 5060 hns
sleep(1): 1.0086 (delta: 0.0086) | zwres: 5060 hns
sleep(1): 1.0078 (delta: 0.0078) | zwres: 5060 hns
sleep(1): 1.0071 (delta: 0.0071) | zwres: 5060 hns
sleep(1): 1.0106 (delta: 0.0106) | zwres: 5060 hns
sleep(1): 1.0076 (delta: 0.0076) | zwres: 5060 hns
sleep(1): 1.0089 (delta: 0.0089) | zwres: 5060 hns
sleep(1): 1.0099 (delta: 0.0099) | zwres: 5060 hns
sleep(1): 1.0090 (delta: 0.0090) | zwres: 5060 hns
sleep(1): 1.0068 (delta: 0.0068) | zwres: 5060 hns
```

Apart from 'test' you can specify 'count' which will specify the number of times the precision of sleep(1) will be measured.
and will show more advanced metrics
```
C:\...>zwt.exe --test 10

sleep(1): 1.0115 (delta: 0.0115) | zwres: 5060 hns
sleep(1): 1.0117 (delta: 0.0117) | zwres: 5060 hns
sleep(1): 1.0110 (delta: 0.0110) | zwres: 5060 hns
sleep(1): 1.0084 (delta: 0.0084) | zwres: 5060 hns
sleep(1): 1.0099 (delta: 0.0099) | zwres: 5060 hns
sleep(1): 1.0126 (delta: 0.0126) | zwres: 5060 hns
sleep(1): 1.0083 (delta: 0.0083) | zwres: 5060 hns
sleep(1): 1.0100 (delta: 0.0100) | zwres: 5060 hns
sleep(1): 1.0092 (delta: 0.0092) | zwres: 5060 hns
sleep(1): 1.0116 (delta: 0.0116) | zwres: 5060 hns

min: 1.0083 ms
max: 1.0126 ms
avg: 1.0104 ms
stdev: 0.0014 ms
```

## SLEEP-TESTER

![image](https://github.com/user-attachments/assets/798ea272-cdda-4617-8265-a82761f7215b)

At the end the results will be saved in SleepTest.txt located in the same directory,
Resolutions with a delta < 150 will be marked with *, to visualize which of all the resolutions obtained the highest precision (lowest delta, lowest diff),
you can view them in https://list2chart.com/csv-to-chart/, just drag the file there and specify which column you want to compare



Here is an example

![image](https://github.com/user-attachments/assets/b58589c5-758e-4b61-928a-eaccaf094676)

# REF
[1](https://github.com/valleyofdoom/TimerResolution)
[2](https://xkln.net/blog/powershell-sleep-duration-accuracy-and-windows-timers/)
