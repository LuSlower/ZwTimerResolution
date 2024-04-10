#ifndef __MAIN_H__
#define __MAIN_H__

#include <windows.h>

/*  To use this exported function of dll, include this header
 *  in your project.
 */

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif


#ifdef __cplusplus
extern "C"
{
#endif

//redeclaration
typedef struct _MEMORY_PRIORITY_INFORMATION {
    ULONG MemoryPriority;
  } MEMORY_PRIORITY_INFORMATION, *PMEMORY_PRIORITY_INFORMATION;

WINBASEAPI WINBOOL WINAPI SetProcessInformation (HANDLE hProcess, PROCESS_INFORMATION_CLASS ProcessInformationClass, LPVOID ProcessInformation, DWORD ProcessInformationSize);

//EntryPoint
BOOL DLL_EXPORT _SetProcessInformation(HANDLE hProcess, INT Option);

#ifdef __cplusplus
}
#endif

#endif // __MAIN_H__
