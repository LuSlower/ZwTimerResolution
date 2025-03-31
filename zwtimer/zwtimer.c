#include "zwtimer.h"

int main(int argc, char *argv[])
{

    if (argc < 2){
        get_frq();
        loop_test();
    }

    if (argc > 1 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "/?") == 0))
    {
        printf("Usage:\n");
        printf("  zwt.exe --res <value>      Set a new timer resolution (in hns units).\n");
        printf("  zwt.exe --query            Returns information about the timer resolution.\n");
        printf("  zwt.exe --test <count>     Generate a test on the precision of Sleep(1).\n");
        printf("  zwt.exe --stop             Stops all instances.\n");
        return 0;
    }

    if (strcmp(argv[1], "--query") == 0)
    {
        ZwQueryTimerResolution(&min, &max, &current);
        printf("minimum resolution : %d hns\n", min);
        printf("maximum resolution : %d hns\n", max);
        printf("current resolution : %d hns", current);
        return 0;
    }

    if(strcmp(argv[1], "--test") == 0 && argc == 3)
    {
        int count = atoi(argv[2]);

        if (!isdigit(*argv[2])) {
            printf("Count must be a single digit.");
            return 1;
        }

        get_frq();

        double min_sleep = INT_MAX, min_delta = INT_MAX, max_sleep = INT_MIN, max_delta = INT_MIN;
        double sum_sleep = 0, sum_delta = 0, sum_squared_sleep = 0, sum_squared_delta = 0;

        for (int i = 0; i < count; ++i)
        {
            ZwQueryTimerResolution(&min, &max, &res_act);
            Sleep(50);
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
            printf("zwres: %d hns, sleep(1): %.4f ms (delta: %.4f)\n", res_act, tsleep, delta);
        }

        double avg_sleep = sum_sleep / count;

        double stdev_sleep = sqrt((sum_squared_sleep / count) - (avg_sleep * avg_sleep));

        printf("\nmin: %.4f ms", min_sleep);
        printf("\nmax: %.4f ms", max_sleep);
        printf("\navg: %.4f ms", avg_sleep);
        printf("\nstdev: %.4f ms", stdev_sleep);

        return 0;
    }
    else if (strcmp(argv[1], "--test") == 0)
    {
        get_frq();
        loop_test();
    }

    if (strcmp(argv[1], "--stop") == 0)
    {
        printf("stopped instances");
        system("taskkill -f -im zwtimer.exe >nul 2>&1");
        return 0;
    }

if (argc == 3 && strcmp(argv[1], "--res") == 0)
    {
        char *endptr;
        long res = strtol(argv[2], &endptr, 10);

        ULONG res_new;
        ZwSetTimerResolution((ULONG)res, TRUE, &res_new);
        Sleep(100);
        printf("Resolution set correctly to %d hns\n", res_new);

        SetPriorityClass(GetCurrentProcess(), PROCESS_MODE_BACKGROUND_BEGIN);

        _SetProcessInformation();
        FreeConsole();
        SetProcessWorkingSetSize(GetCurrentProcess(), (SIZE_T)-1, (SIZE_T)-1);

        hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        WaitForSingleObject(hEvent, INFINITE);
    }
    else
    {
        printf("Use --help or /? to get help\n");
        return 1;
    }

    return 0;
}

