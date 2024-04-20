#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>

// Declaración de la función NtSetTimerResolution
typedef LONG NTSTATUS;
NTSTATUS NTAPI NtSetTimerResolution(ULONG DesiredResolution, BOOLEAN SetResolution, ULONG *CurrentResolution);
NTSTATUS NTAPI NtQueryTimerResolution(ULONG *MinimumResolution, ULONG *MaximumResolution, ULONG *CurrentResolution);

//declarar semaforo
static HANDLE hMutex;

// Declaración de SetProcessInformation
WINBASEAPI WINBOOL WINAPI SetProcessInformation(HANDLE hProcess, PROCESS_INFORMATION_CLASS ProcessInformationClass, LPVOID ProcessInformation, DWORD ProcessInformationSize);

//SetInformationProcess
void _SetProcessInformation()
{
    if (SetProcessInformation)
    {
        //Obtener el HANDLE del proceso actual
        HANDLE hProcess = OpenProcess(PROCESS_SET_INFORMATION, FALSE, GetCurrentProcessId());

        //pasa puntero de la estructura
        PROCESS_POWER_THROTTLING_STATE state;
        ZeroMemory(&state, sizeof(state));
        state.Version = PROCESS_POWER_THROTTLING_CURRENT_VERSION;
        state.ControlMask = PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION;
        state.StateMask = 0;

        //DISABLE IDLE TIMER RESOLUTION
        SetProcessInformation(hProcess, ProcessPowerThrottling, &state, sizeof(state));

        //cerrar HANDLE
        CloseHandle(hProcess);
    }
    else
    {
        printf("Error al obtener la dirección de SetProcessInformation\n"); //No existe en Windows 7
    }
}

int main(int argc, char *argv[])
{

    if (argc < 2 || strcmp(argv[1], "help") == 0 || strcmp(argv[1], "/?") == 0)
    {
        printf("Instrucciones de uso:\n");
        printf("\n<resolucion>\n");
        printf("\nEstablece una nueva resolucion del temporizador (debe especificarla en nanosegundos).\n");
        printf("como minimo 4 caracteres, maximo 6\n");
        printf("ejemplo: 'nt_timer_cs.exe 5000'\n");
        printf("\n<query>\n");
        printf("\nDevuelve informacion acerca de la resolucion del temporizador.\n");
        printf("\n<stop>\n");
        printf("\nDetiene todas las instancias.\n");
        return 0;
    }

    // Llamar a NtQueryTimerResolution
    ULONG min, max, current;
    NTSTATUS query_status = NtQueryTimerResolution(&min, &max, &current);
    if (strcmp(argv[1], "query") == 0)
    {
        printf("Resolucion Minima : %dns\n", min);
        printf("Resolucion Maxima : %dns\n", max);
        printf("Resolucion Actual : %dns\n", current);
        return 0;
    }

    if (strcmp(argv[1], "stop") == 0)
    {
        system("Taskkill -f -im nt_timer_cs.exe");
        return 0;
    }

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
                printf("Ya hay una instancia ejecutandose en segundo plano\n");
                printf("si desea detener la instancia ejecute nt_timer_cs.exe con el argumento <stop>");
                return 1;
            }

            ULONG res_act, res = atoi(argv[1]); // Convertir el argumento a ULONG
            NTSTATUS set_status = NtSetTimerResolution(res, TRUE, &res_act); // Establecer resolución del temporizador

            printf("Resolucion establecida correctamente a %dns", res_act);

            //llamar a SetProcessInformation
            _SetProcessInformation();

            //liberar consola
            FreeConsole();

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
    return 0;
}

