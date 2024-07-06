#include "zwtimer.h"

int main(int argc, char *argv[])
{

    if (argc < 2){

        // get frequency
        get_frq();

        //loop
        loop_test();
    }

    //help
    if(strcmp(argv[1], "help") == 0 || strcmp(argv[1], "/?") == 0)
    {
        printf("Instructions for use:\n");
        printf("\n<resolution>\n");
        printf("\nSet a new timer resolution (must be specified in nanoseconds).\n");
        printf("minimum 4 characters, maximum 6\n");
        printf("example: 'zwt.exe 5000'\n");
        printf("\n<query>\n");
        printf("\nReturns information about the timer resolution.\n");
        printf("\n<test> <count>\n");
        printf("\nGenerate a test on the precision of Sleep(1).\n");
        printf("by default it runs in a loop\n");
        printf("\n<stop>\n");
        printf("\nStops all instances.\n");
        return 0;
    }

    if (strcmp(argv[1], "query") == 0)
    {
        ZwQueryTimerResolution(&min, &max, &current);

        // Obtener resoluciones del temporizador
        printf("\nminimum resolution : %d ns\n", min);
        printf("maximum resolution : %d ns\n", max);
        printf("current resolution : %d ns\n", current);
        return 0;
    }

    //test de precision
    if(strcmp(argv[1], "test") == 0 && argc == 3)
    {
        int count = atoi(argv[2]);

        // verificar que count sea un dígito
        if (!isdigit(*argv[2])) {
            printf("Count must be a single digit.");
            return 1;
        }

        // get frequency
        get_frq();

        // declarar limites
        double min_sleep = INT_MAX, min_delta = INT_MAX, max_sleep = INT_MIN, max_delta = INT_MIN;
        double sum_sleep = 0, sum_delta = 0, sum_squared_sleep = 0, sum_squared_delta = 0;

        //ejecutar bucle
        for (int i = 0; i < count; ++i)
        {
            // Realizar una medición
            ZwQueryTimerResolution(&min, &max, &res_act);
            Sleep(100);
            sleep_test();

            sum_delta += delta;
            sum_squared_delta += delta * delta;

            sum_sleep += tsleep;
            sum_squared_sleep += tsleep * tsleep;

            if (tsleep < min_sleep) {
                min_sleep = tsleep;
            }

            if (delta < min_delta) {
                min_delta = delta;
            }

            if (tsleep > max_sleep) {
                max_sleep = tsleep;
            }

            if (delta > max_delta) {
                max_delta = delta;
            }
            printf("sleep(1): %.4f ms | delta: %.4f ms | zwres: %d ns\n", tsleep, delta, res_act);
        }

        // Calcular promedio
        double avg_sleep = sum_sleep / count;
        double avg_delta = sum_delta / count;

        // Calcular desviación estándar
        double stdev_sleep = sqrt((sum_squared_sleep / count) - (avg_sleep * avg_sleep));
        double stdev_delta = sqrt((sum_squared_delta / count) - (avg_delta * avg_delta));

        printf("\nmin: %.4f ms | %.4f ms", min_sleep, min_delta);
        printf("\nmax: %.4f ms | %.4f ms", max_sleep, max_delta);
        printf("\navg: %.4f ms | %.4f ms", avg_sleep, avg_delta);
        printf("\nstdev: %.4f ms | %.4f ms", stdev_sleep, stdev_delta);

        return 0;
    }
    else if (strcmp(argv[1], "test") == 0)
    {

        // get frequency
        get_frq();

        //default
        loop_test();
    }

    //detener instancias

    if (strcmp(argv[1], "stop") == 0)
    {
        printf("stopped instances");
        system("taskkill -f -im zwtimer.exe >nul");
        return 0;
    }

       //set zwresolucion

    // Verificar si argv[1] es un número entero
    for (int i = 0; argv[1][i] != '\0'; i++)
    {
    if (!isdigit(argv[1][i]))
        {
            printf("/? or help to get help");
            return 1;
        }
    }

    //Verificar tamaño del argumento
    if (strlen(argv[1]) >= 4 && strlen(argv[1]) < 6)
    {

            res = atoi(argv[1]); // Convertir el argumento a ULONG
            ZwSetTimerResolution(res, TRUE, &res_new); // Establecer resolución del temporizador
            Sleep(100);
            printf("resolution set correctly to %d ns", res_new);

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
        printf("/? or help to get help");
        return 1;
    }

    return 0;
}

