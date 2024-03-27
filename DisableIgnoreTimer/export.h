#ifndef DLL_H
#define DLL_H

#include <Windows.h>
#ifdef __cplusplus
extern "C" {
#endif

#ifdef BUILD_DLL
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif

DLL_EXPORT int __stdcall _SetProcessInformation(HANDLE hProcess);

#ifdef __cplusplus
}
#endif

#endif // DLL_H
