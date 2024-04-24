#pragma once
#include <stdio.h>
#include <windows.h>
#include <limits.h>
#include <tchar.h>

// Definir valores
ULONG min, max, current, res_act, res, res_new;

// Definir contadores
LARGE_INTEGER frq, start, end;

// Definir semaforo, evento nulo
HANDLE hMutex, hEvent;

// Declaraci�n de ZwSetTimerResolution y ZwQueryTimerResolution
typedef LONG NTSTATUS;
NTSTATUS NTAPI ZwSetTimerResolution(ULONG DesiredResolution, BOOLEAN SetResolution, ULONG *CurrentResolution);
NTSTATUS NTAPI ZwQueryTimerResolution(ULONG *MinimumResolution, ULONG *MaximumResolution, ULONG *CurrentResolution);

// Declaraci�n de SetProcessInformation
WINBASEAPI WINBOOL WINAPI SetProcessInformation(HANDLE hProcess, PROCESS_INFORMATION_CLASS ProcessInformationClass, LPVOID ProcessInformation, DWORD ProcessInformationSize);

void _SetProcessInformation()
{
    if (SetProcessInformation)
    {

        // Pasa un puntero a la estructura
        PROCESS_POWER_THROTTLING_STATE state;
        ZeroMemory(&state, sizeof(state));
        state.Version = PROCESS_POWER_THROTTLING_CURRENT_VERSION;
        state.ControlMask = PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION;
        state.StateMask = 0;

        // Deshabilitar la resoluci�n del temporizador de inactividad
        SetProcessInformation(GetCurrentProcess(), ProcessPowerThrottling, &state, sizeof(state));
    }
    else
    {
        printf("Error al obtener la direcci�n de SetProcessInformation\n"); // No existe en Windows 7
    }
}

TCHAR* _get_folder_path()
{
    static TCHAR szPath[MAX_PATH];
    GetModuleFileName(NULL, szPath, MAX_PATH);

    // Encuentra la �ltima aparici�n de '\\' en la ruta completa
    TCHAR* lastBackslash = _tcsrchr(szPath, '\\');
    if (lastBackslash != NULL) {
        // Coloca un terminador de cadena nula despu�s del �ltimo '\\' para obtener solo la carpeta
        *(lastBackslash + 1) = '\0';
    }

    return szPath;
}

// declarar medidas
double time, tsleep, delta;

void sleep_test()
{
    // Medir el tiempo de espera Sleep(1) y calcular el tiempo
    QueryPerformanceCounter(&start);
    Sleep(1);
    QueryPerformanceCounter(&end);

    // Convertir la diferencia a milisegundos y restar el sleep(1)
    time = (double)(end.QuadPart - start.QuadPart) / frq.QuadPart;
    tsleep = time * 1000;
    delta = tsleep - 1;
}

void loop_test()
{
    //bucle predeterminado
    printf("comienza la prueba...\n");
    for (int i = 1; ; i++) {
    ZwQueryTimerResolution(&min, &max, &res_act);
    sleep_test();
    printf("\ntime: %.4f s | sleep(1): %.4f ms | delta: %.4f ms | zwres: %d ns", time, tsleep, delta, res_act);
    Sleep(700);
    }
}

int main(int argc, char *argv[])
{
    // Obtener la frecuencia del contador de rendimiento
    QueryPerformanceFrequency(&frq);

    if (argc < 2){
        loop_test();
    }

    //help
    if(strcmp(argv[1], "help") == 0 || strcmp(argv[1], "/?") == 0)
    {
        printf("Instrucciones de uso:\n");
        printf("\n<resolution>\n");
        printf("\nEstablece una nueva resolucion del temporizador (debe especificarla en nanosegundos).\n");
        printf("como minimo 4 caracteres, maximo 6\n");
        printf("ejemplo: 'zwtimer.exe 5000'\n");
        printf("\n<query>\n");
        printf("\nDevuelve informacion acerca de la resolucion del temporizador.\n");
        printf("\n<test> start end count\n");
        printf("\nGenera una prueba sobre la precision de Sleep(1).\n");
        printf("por defecto se ejecuta en un bucle\n");
        printf("puede especificar un inicio, final y los conteos para verificar que resolucion tiene una mejor precision\n");
        printf("los resultados se guardaran en sleep-test.txt\n");
        printf("ejemplo: 'zwtimer.exe test 5000 6000 20'\n");
        printf("\n<stop>\n");
        printf("\nDetiene todas las instancias.\n");
        return 0;
    }

    if (strcmp(argv[1], "query") == 0)
    {
        ZwQueryTimerResolution(&min, &max, &current);

        // Obtener resoluciones del temporizador
        printf("informacion del temporizador:\n");
        printf("\nresolucion minima : %d ns\n", min);
        printf("resolucion maxima : %d ns\n", max);
        printf("resolucion actual : %d ns\n", current);
        return 0;
    }

    //test de precision
    if(strcmp(argv[1], "test") == 0 && argc == 5)
    {
        // start
        for (int i = 0; argv[2][i] != '\0'; i++)
        {
            if (!isdigit(argv[2][i]) || strlen(argv[2]) < 4 || strlen(argv[2]) > 6)
            {
                printf("/? o help para obtener ayuda\n");
                return 1;
            }
        }

        //end
        for (int i = 0; argv[3][i] != '\0'; i++)
        {
            if (!isdigit(argv[3][i]) || strlen(argv[3]) < 4 || strlen(argv[3]) > 6)
            {
                printf("/? o help para obtener ayuda\n");
                return 1;
            }
        }

        // leer valor de counts
        int counts = atoi(argv[4]);
        for (int i = 0; argv[4][i] != '\0'; i++)
        {
            if (!isdigit(argv[4][i]) || counts <= 5 || counts > 200)
            {
                printf("los conteos deben ser mayor o igual a 5 y menor o igual a 200\n");
                return 1;
            }

        }

        // Convertir a entero start, end y counts
        int start_res = atoi(argv[2]);
        int end_res = atoi(argv[3]);

        // Verificar que end_res sea mayor que start_res
        if (end_res <= start_res)
        {
            printf("La resolucion final debe ser mayor que la resolucion inicial.\n");
            return 1;
        }

        printf("comienza la prueba...\n");
        printf("\ninicio : %d\nfinal : %d\nconteo : %d\n", start_res, end_res, counts);

        //obtener path y abrir archivo
        TCHAR* dir = _get_folder_path();
        TCHAR test[MAX_PATH];

        _snprintf(test, MAX_PATH, _T("%s%s"), dir, _T("sleep-test.txt"));
        FILE *outputFile = fopen(test, L"w");

        // declarar limites
        double min_sleep = INT_MAX, min_delta = INT_MAX, max_sleep = INT_MIN, max_delta = INT_MIN, avg_sleep, avg_delta;

        //heads
        fprintf(outputFile,"Sleep(1), DeltaMs, ZwResolution\n");

        //ejecutar bucle con un aumento de 13 ns
        for (int res = start_res; res <= end_res; res += 13)
        {
            // redefinir mediciones
            double num_sleep = 0, num_delta = 0, sum_sleep = 0, sum_delta = 0;

            // Realizar 'counts' mediciones para cada valor de res
            for (int i = 0; i < counts; ++i)
            {
                ZwSetTimerResolution(res, TRUE, &res_act);
                Sleep(50);
                sleep_test();
                Sleep(50);

                // actualizar delta y sleep
                if (delta < min_delta) {
                    min_delta = delta;
                }
                if (delta > max_delta) {
                    max_delta = delta;
                }
                sum_delta += delta;
                num_delta++;

                if (tsleep < min_sleep) {
                    min_sleep = tsleep;
                }
                if (tsleep > max_sleep) {
                    max_sleep = tsleep;
                }
                sum_sleep += tsleep;
                num_sleep++;
            }
            // Calcular promedio
            avg_sleep = (double)sum_sleep / num_sleep;
            avg_delta = (double)sum_delta / num_delta;
            printf("\nsleep(1): %.4f ms | delta: %.4f ms | zwres: %d ns", avg_sleep, avg_delta, res_act);
            // Guardar resultados en el archivo de salida
            fprintf(outputFile,"%.4f, %.4f, %d\n", avg_sleep, avg_delta, res_act);
        }

        printf("\n\nprueba finalizada...\n");
        printf("\nminimo: %.4f ms | %.4f ms\n",min_sleep, min_delta);
        printf("maximo: %.4f ms | %.4f ms\n", max_sleep, max_delta);

        //cerrar archivo
        fclose(outputFile);
        return 0;
    }
    else if (strcmp(argv[1], "test") == 0)
    {
        //default
        loop_test();
    }

    //detener instancias

    if (strcmp(argv[1], "stop") == 0)
    {
        printf("instancias detenidas");
        system("taskkill -f -im zwtimer.exe >nul");
        return 0;
    }

       //set zwresolucion

    // Verificar si argv[1] es un n�mero entero
    for (int i = 0; argv[1][i] != '\0'; i++)
    {
    if (!isdigit(argv[1][i]))
        {
            printf("/? o help para obtener ayuda");
            return 1;
        }
    }

    //Verificar tama�o del argumento
    if (strlen(argv[1]) >= 4 && strlen(argv[1]) < 6)
    {
            //detener instancias
            hMutex = CreateMutex(NULL, TRUE, "zwtimer.exe");
            if (GetLastError() == ERROR_ALREADY_EXISTS) {
                CloseHandle(hMutex);
                printf("ya hay una instancia ejecutandose en segundo plano");
                return 1;
            }

            res = atoi(argv[1]); // Convertir el argumento a ULONG
            ZwSetTimerResolution(res, TRUE, &res_new); // Establecer resoluci�n del temporizador
            Sleep(250);
            printf("resolucion establecida correctamente a %d ns", res_new);

            //prioridad de segundo plano
            SetPriorityClass(GetCurrentProcess(), PROCESS_MODE_BACKGROUND_BEGIN);

            //llamar a SetProcessInformation
            _SetProcessInformation();

            //liberar consola
            FreeConsole();

            //liberar memoria
            SetProcessWorkingSetSize(GetCurrentProcess(), (SIZE_T) -1, (SIZE_T) -1);

            hEvent = CreateEvent(NULL, TRUE, FALSE, NULL); // Manual reset event, inicialmente no se�alizado

            // Suspender el hilo hasta que el evento se se�alice
            WaitForSingleObject(hEvent, INFINITE);
    }
    else
    {
        printf("/? o help para obtener ayuda");
        return 1;
    }

    return 0;
}
