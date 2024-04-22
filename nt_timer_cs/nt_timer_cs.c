#include <stdio.h>
#include <windows.h>

// Declaración de NtSetTimerResolution y NtQueryTimerResolution
typedef LONG NTSTATUS;
NTSTATUS NTAPI NtSetTimerResolution(ULONG DesiredResolution, BOOLEAN SetResolution, ULONG *CurrentResolution);
NTSTATUS NTAPI NtQueryTimerResolution(ULONG *MinimumResolution, ULONG *MaximumResolution, ULONG *CurrentResolution);

// Declaración de SetProcessInformation
WINBASEAPI WINBOOL WINAPI SetProcessInformation(HANDLE hProcess, PROCESS_INFORMATION_CLASS ProcessInformationClass, LPVOID ProcessInformation, DWORD ProcessInformationSize);

// Declarar semáforo
static HANDLE hMutex;

void _SetProcessInformation()
{
    if (SetProcessInformation)
    {
        // Obtener el HANDLE del proceso actual
        HANDLE hProcess = OpenProcess(PROCESS_SET_INFORMATION, FALSE, GetCurrentProcessId());

        // Pasa un puntero a la estructura
        PROCESS_POWER_THROTTLING_STATE state;
        ZeroMemory(&state, sizeof(state));
        state.Version = PROCESS_POWER_THROTTLING_CURRENT_VERSION;
        state.ControlMask = PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION;
        state.StateMask = 0;

        // Deshabilitar la resolución del temporizador de inactividad
        SetProcessInformation(hProcess, ProcessPowerThrottling, &state, sizeof(state));

        // Cerrar HANDLE
        CloseHandle(hProcess);
    }
    else
    {
        printf("Error al obtener la dirección de SetProcessInformation\n"); // No existe en Windows 7
    }
}

// Definir contadores
LARGE_INTEGER ctr, frq, start, end;

// Función para obtener el tiempo preciso
double get_precise_time()
{
    // Medir el tiempo de espera Sleep(1) y calcular el tiempo
    QueryPerformanceCounter(&start);
    Sleep(1);
    QueryPerformanceCounter(&end);

    // Convertir la diferencia a milisegundos y restar el sleep(1)
    double time_ms = (double)(end.QuadPart - start.QuadPart) / frq.QuadPart;
    double time_sleep = time_ms * 1000.0;
    double diff = time_sleep - 1.0;
    printf("\ntime: %.4f ms | sleep_time: %.4f ms | diff: %.4f ms", time_ms, time_sleep, diff);

    return (double)diff;
}

int main(int argc, char *argv[])
{

    //obtener ayuda
    if (argc < 2 || strcmp(argv[1], "help") == 0 || strcmp(argv[1], "/?") == 0)
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
        printf("Por defecto se ejecuta en un bucle\npuede especificar un inicio y un final para verificar que resolucion tiene una mejor precision\n");
        printf("ejemplo: 'nt_timer_cs.exe test 5000 6000'\n");
        printf("\n<stop>\n");
        printf("\nDetiene todas las instancias.\n");
        return 0;
    }

    // Llamar a NtQueryTimerResolution
    ULONG min, max, current, res_act, res;
    NTSTATUS query_status = NtQueryTimerResolution(&min, &max, &current);

    // Obtener la frecuencia del contador de rendimiento y el valor del contador
    QueryPerformanceFrequency(&frq);
    QueryPerformanceCounter(&ctr);

    // Elevar prioridad
    //SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);

    if (strcmp(argv[1], "query") == 0)
    {
        // Obtener resoluciones del temporizador
        printf("informacion del temporizador:\n");
        printf("\nresolucion minima : %d ns\n", min);
        printf("resolucion maxima : %d ns\n", max);
        printf("resolucion actual : %d ns\n", current);
        printf("QPF : %d\nQPC: %d", frq, ctr);
        get_precise_time();
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
        printf("presione ESCAPE tecla para salir\n");
        printf("\ninicio : %d\nfinal : %d\n", start_res, end_res);

        // Definir numero de pruebas
        double num_samples = 0;
        double sum_samples = 0;
        double min_sample = INT_MAX;
        double max_sample = INT_MIN;

        //ejecutar bucle con un aumento de 10 nanosegundos por
        for (int res = start_res; res <= end_res; res += 15)
        {
            NtSetTimerResolution(res, TRUE, &res_act);
            Sleep(100);
            double sample_time = get_precise_time();
            printf(" | resolucion %d ns\n", res_act);
            Sleep(100);

            // Actualizar valores de min, max, sum y contador
            if (sample_time < min_sample) {
                min_sample = sample_time;
            }
            if (sample_time > max_sample) {
                max_sample = sample_time;
            }
            sum_samples += sample_time;
            num_samples++;

        // Verificar si se ha presionado ESCAPE
        if(GetAsyncKeyState(VK_ESCAPE)){
            printf("\nsaliendo de la prueba...\n");
            break;
        }
        }

        // Calcular promedio
        double avg_sample = (double)sum_samples / num_samples;

        printf("\nprueba finalizada...\n");
        printf("\nminimo: %.4f ms\n", min_sample);
        printf("maximo: %.4f ms\n", max_sample);
        printf("promedio: %.4f ms\n", avg_sample);
        return 0;
    }
    else if (strcmp(argv[1], "test") == 0)
    {
        //bucle predeterminado
        printf("comienza la prueba...\npresione ESCAPE tecla para salir\n");
        for (int i = 1; ; i++) {
            NtQueryTimerResolution(&min, &max, &current);
            get_precise_time();
            printf(" | resolucion: %d ns\n", current);
            Sleep(1000);
            // Verificar si se ha presionado ESCAPE
            if(GetAsyncKeyState(VK_ESCAPE)){
                printf("\nsaliendo de la prueba...");
                break;
            }
        }
        return 0;
    }

    //detener instancias

    if (strcmp(argv[1], "stop") == 0)
    {
        system("Taskkill -f -im nt_timer_cs.exe");
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
        hMutex = CreateMutex(NULL, FALSE, "nt_timer_cs");
        if (GetLastError() == ERROR_ALREADY_EXISTS)
            {
                //Ya hay una instancia
                CloseHandle(hMutex);
                printf("ya hay una instancia ejecutandose en segundo plano\n");
                printf("si desea detener la instancia ejecute nt_timer_cs.exe con el argumento <stop>");
                return 1;
            }

            res = atoi(argv[1]); // Convertir el argumento a ULONG
            NTSTATUS set_status = NtSetTimerResolution(res, TRUE, &res_act); // Establecer resolución del temporizador

            printf("resolucion establecida correctamente a %d ns", res_act);

            //llamar a SetProcessInformation
            _SetProcessInformation();

            //liberar consola
            FreeConsole();

            //liberar memoria
            SetProcessWorkingSetSize(GetCurrentProcess(), (SIZE_T) -1, (SIZE_T) -1);

            //prioridad de segundo plano
            SetPriorityClass(GetCurrentProcess(), PROCESS_MODE_BACKGROUND_BEGIN);

            //permanecer activo
            Sleep(INFINITE);
    }
    else
    {
        printf("/? o help para obtener ayuda");
        return 1;
    }

    //cerrar HANDLE
    CloseHandle(hMutex);
}

