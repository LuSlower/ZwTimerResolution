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
        printf("example: 'zwtimer.exe 5000'\n");
        printf("\n<query>\n");
        printf("\nReturns information about the timer resolution.\n");
        printf("\n<test> start end\n");
        printf("\nGenerate a test on the precision of Sleep(1).\n");
        printf("by default it runs in a loop\n");
        printf("you can specify a start, end to check which resolution has better precision\n");
        printf("the results will be saved in sleep-test.txt\n");
        printf("example: 'zwtimer.exe test 5000 6000'\n");
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
    if(strcmp(argv[1], "test") == 0 && argc == 4)
    {

        // get frequency
        get_frq();

        // start
        for (int i = 0; argv[2][i] != '\0'; i++)
        {
            if (!isdigit(argv[2][i]) || strlen(argv[2]) < 4 || strlen(argv[2]) > 6)
            {
                printf("/? or help to get help\n");
                return 1;
            }
        }

        //end
        for (int i = 0; argv[3][i] != '\0'; i++)
        {
            if (!isdigit(argv[3][i]) || strlen(argv[3]) < 4 || strlen(argv[3]) > 6)
            {
                printf("/? or help to get help\n");
                return 1;
            }
        }

        // Convertir a entero start, end
        int start_res = atoi(argv[2]);
        int end_res = atoi(argv[3]);

        // Verificar que end_res sea mayor que start_res
        if (end_res <= start_res)
        {
            printf("The final resolution must be greater than the initial resolution.\n");
            return 1;
        }

        printf("test begins...\n");
        printf("\nstart : %d\nend : %d\n", start_res, end_res);

        //obtener path y abrir archivo
        TCHAR* dir = _get_folder_path();
        TCHAR test[MAX_PATH];

        _snprintf(test, MAX_PATH, _T("%s%s"), dir, _T("sleep-test.txt"));
        FILE *outputFile = fopen(test, "w");

        // declarar limites
        double min_sleep = INT_MAX, min_delta = INT_MAX, max_sleep = INT_MIN, max_delta = INT_MIN, avg_sleep, avg_delta;

        //heads
        fprintf(outputFile,"Sleep(1), DeltaMs, ZwResolution\n");

        //ejecutar bucle
        for (int res = start_res; res_act < end_res; res += 3)
        {
            // redefinir mediciones
            double num_sleep = 0, num_delta = 0, sum_sleep = 0, sum_delta = 0;

            // Realizar '25' mediciones para cada valor de res
            for (int i = 0; i < 25; ++i)
            {
                ZwSetTimerResolution(res, TRUE, &res_act);
                Sleep(50);
                sleep_test();

                sum_delta += delta;
                num_delta++;

                sum_sleep += tsleep;
                num_sleep++;
            }

            // Calcular promedio
            avg_sleep = (double)sum_sleep / num_sleep;
            avg_delta = (double)sum_delta / num_delta;

            // actualizar avg_delta y avg_sleep

            if (avg_sleep < min_sleep) {
                min_sleep = avg_sleep;
            }

            if (avg_delta < min_delta) {
                min_delta = avg_delta;
            }

            if (avg_sleep > max_sleep) {
                max_sleep = avg_sleep;
            }
            if (avg_delta > max_delta) {
                max_delta = avg_delta;
            }

            printf("\nsleep(1): %.4f ms | delta: %.4f ms | zwres: %d ns", avg_sleep, avg_delta, res_act);
            // Guardar resultados en el archivo de salida
            fprintf(outputFile,"%.4f, %.4f, %d\n", avg_sleep, avg_delta, res_act);

            // incrementar resolucion de inicio
            res+= 10;
        }


        printf("\n\ntest completed...\n");
        printf("\nmin: %.4f ms | %.4f ms\n",min_sleep, min_delta);
        printf("max: %.4f ms | %.4f ms\n", max_sleep, max_delta);

        //cerrar archivo
        fclose(outputFile);
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
            //detener instancias
            hMutex = CreateMutex(NULL, TRUE, "zwtimer.exe");
            if (GetLastError() == ERROR_ALREADY_EXISTS) {
                CloseHandle(hMutex);
                printf("there is already an instance running in the background");
                return 1;
            }

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

