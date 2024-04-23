#pragma once
#include <stdio.h>
#include <windows.h>
#include <limits.h>
#include <tchar.h>

// Definir valores
ULONG min, max, current, res_act, res;

// Definir contadores
LARGE_INTEGER frq, start, end;

// Definir semaforo, evento nulo
HANDLE hMutex, hEvent;

// Declaración de NtSetTimerResolution y NtQueryTimerResolution
typedef LONG NTSTATUS;
NTSTATUS NTAPI NtSetTimerResolution(ULONG DesiredResolution, BOOLEAN SetResolution, ULONG *CurrentResolution);
NTSTATUS NTAPI NtQueryTimerResolution(ULONG *MinimumResolution, ULONG *MaximumResolution, ULONG *CurrentResolution);

// Declaración de SetProcessInformation
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

        // Deshabilitar la resolución del temporizador de inactividad
        SetProcessInformation(GetCurrentProcess(), ProcessPowerThrottling, &state, sizeof(state));
    }
    else
    {
        printf("Error al obtener la dirección de SetProcessInformation\n"); // No existe en Windows 7
    }
}

TCHAR* _get_folder_path()
{
    static TCHAR szPath[MAX_PATH];
    GetModuleFileName(NULL, szPath, MAX_PATH);

    // Encuentra la última aparición de '\\' en la ruta completa
    TCHAR* lastBackslash = _tcsrchr(szPath, '\\');
    if (lastBackslash != NULL) {
        // Coloca un terminador de cadena nula después del último '\\' para obtener solo la carpeta
        *(lastBackslash + 1) = '\0';
    }

    return szPath;
}

// declarar medidas
double time, tsleep, delta;

double test_sleep_precision()
{
    // Medir el tiempo de espera Sleep(1) y calcular el tiempo
    QueryPerformanceCounter(&start);
    Sleep(1);
    QueryPerformanceCounter(&end);

    // Convertir la diferencia a milisegundos y restar el sleep(1)
    time = (double)(end.QuadPart - start.QuadPart) / frq.QuadPart;
    tsleep = time * 1000.0;
    delta = tsleep - 1.0;
    printf("\ntime: %.4f s | tsleep: %.4f ms | delta: %.4f ms | nt-timer: %d ns", time, tsleep, delta, res_act);

    return (double)delta;
}

void loop_get_time()
{
    //bucle predeterminado
    printf("comienza la prueba...\n");
    for (int i = 1; ; i++) {
    NtQueryTimerResolution(&min, &max, &res_act);
    test_sleep_precision();
    Sleep(1000);
    }
}

int main(int argc, char *argv[])
{
    // Obtener la frecuencia del contador de rendimiento
    QueryPerformanceFrequency(&frq);

    if (argc < 2){
        loop_get_time();
    }

    //help
    if(strcmp(argv[1], "help") == 0 || strcmp(argv[1], "/?") == 0)
    {
        printf("Instrucciones de uso:\n");
        printf("\n<resolution>\n");
        printf("\nEstablece una nueva resolucion del temporizador (debe especificarla en nanosegundos).\n");
        printf("como minimo 4 caracteres, maximo 6\n");
        printf("ejemplo: 'nt_timer_cs.exe 5000'\n");
        printf("\n<query>\n");
        printf("\nDevuelve informacion acerca de la resolucion del temporizador.\n");
        printf("\n<test> start end\n");
        printf("\nGenera una prueba sobre la precision de Sleep(1).\n");
        printf("por defecto se ejecuta en un bucle\npuede especificar un inicio y un final para verificar que resolucion tiene una mejor precision\n");
        printf("lo resultados se guardaran en c:\sleep-test.txt\n");
        printf("ejemplo: 'nt_timer_cs.exe test 5000 6000'\n");
        printf("\n<stop>\n");
        printf("\nDetiene todas las instancias.\n");
        return 0;
    }

    if (strcmp(argv[1], "query") == 0)
    {
        NtQueryTimerResolution(&min, &max, &current);

        // Obtener resoluciones del temporizador
        printf("informacion del temporizador:\n");
        printf("\nresolucion minima : %d ns\n", min);
        printf("resolucion maxima : %d ns\n", max);
        printf("resolucion actual : %d ns\n", current);
        return 0;
    }

    //test de precision
    if(strcmp(argv[1], "test") == 0 && argc == 4)
    {
        // Verificar si el segundo / tercer argumento son enteros
        for (int i = 0; argv[2][i] != '\0'; i++)
        {
            if (!isdigit(argv[2][i]) || strlen(argv[2]) < 4 || strlen(argv[2]) > 6)
            {
                printf("/? o help para obtener ayuda\n");
                return 1;
            }
        }
        for (int i = 0; argv[3][i] != '\0'; i++)
        {
            if (!isdigit(argv[3][i]) || strlen(argv[3]) < 4 || strlen(argv[3]) > 6)
            {
                printf("/? o help para obtener ayuda\n");
                return 1;
            }
        }

        // Convertir a entero start y end
        int start_res = atoi(argv[2]);
        int end_res = atoi(argv[3]);

        // Verificar que end_res sea mayor que start_res
        if (end_res <= start_res)
        {
            printf("La resolucion final debe ser mayor que la resolucion inicial.\n");
            return 1;
        }

        printf("comienza la prueba...\n");
        printf("\ninicio : %d\nfinal : %d\n", start_res, end_res);

        // Definir numero de pruebas
        double num_samples = 0;
        double sum_samples = 0;
        double min_sample = INT_MAX;
        double max_sample = INT_MIN;

        //obtener path y abrir archivo
        TCHAR* dir = _get_folder_path();
        TCHAR test[MAX_PATH];

        _snprintf(test, MAX_PATH, _T("%s%s"), dir, _T("sleep-test.txt"));
        FILE *outputFile = fopen(test, L"w");

        if (outputFile == NULL)
        {
            printf("Error al abrir el archivo de salida.\n");
            return 1;
        }

        //heads
        fprintf(outputFile,"sleep-time, delta-ms, nt-timer\n");

        //ejecutar bucle con un aumento de 11 nanosegundos
        for (int res = start_res; res <= end_res; res += 11)
        {
            NtSetTimerResolution(res, TRUE, &res_act);
            Sleep(100);
            double sample_time = test_sleep_precision();
            Sleep(500);

            //stdout
            fprintf(outputFile,"%.4f, %.4f, %d\n",
                   tsleep, delta, res_act);

            // Actualizar valores de min, max, sum y contador
            if (sample_time < min_sample) {
                min_sample = sample_time;
            }
            if (sample_time > max_sample) {
                max_sample = sample_time;
            }
            sum_samples += sample_time;
            num_samples++;


        }

        // Calcular promedio
        double avg_sample = (double)sum_samples / num_samples;

        printf("\nprueba finalizada...\n");
        printf("\nminimo: %.4f ms\n", min_sample);
        printf("maximo: %.4f ms\n", max_sample);
        printf("promedio: %.4f ms\n", avg_sample);

        //cerrar archivo
        fclose(outputFile);
        return 0;
    }
    else if (strcmp(argv[1], "test") == 0)
    {
        loop_get_time();
    }

    //detener instancias

    if (strcmp(argv[1], "stop") == 0)
    {
        printf("instancias detenidas");
        system("taskkill -f -im nt_timer_cs.exe >nul");
        return 0;
    }

       //resolucion

    // Verificar si argv[1] es un número entero
    for (int i = 0; argv[1][i] != '\0'; i++)
    {
    if (!isdigit(argv[1][i]))
        {
            printf("/? o help para obtener ayuda");
            return 1;
        }
    }

    //Verificar tamaño del argumento
    if (strlen(argv[1]) >= 4 && strlen(argv[1]) < 6)
    {
            //detener instancias
            hMutex = CreateMutex(NULL, TRUE, "nt_timer_cs.exe");
            if (GetLastError() == ERROR_ALREADY_EXISTS) {
                CloseHandle(hMutex);
                printf("ya hay una instancia ejecutandose en segundo plano");
                return 1;
            }

            res = atoi(argv[1]); // Convertir el argumento a ULONG
            NtSetTimerResolution(res, TRUE, &res_act); // Establecer resolución del temporizador
            Sleep(100);
            printf("resolucion establecida correctamente a %d ns", res_act);

            //prioridad de segundo plano
            SetPriorityClass(GetCurrentProcess(), PROCESS_MODE_BACKGROUND_BEGIN);

            //llamar a SetProcessInformation
            _SetProcessInformation();

            //liberar consola
            FreeConsole();

            //liberar memoria
            SetProcessWorkingSetSize(GetCurrentProcess(), (SIZE_T) -1, (SIZE_T) -1);

            hEvent = CreateEvent(NULL, TRUE, FALSE, NULL); // Manual reset event, inicialmente no señalizado

            // Suspender el hilo hasta que el evento se señalice
            WaitForSingleObject(hEvent, INFINITE);
    }
    else
    {
        printf("/? o help para obtener ayuda");
        return 1;
    }

    return 0;
}

