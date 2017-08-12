#pragma once

#ifdef DLL_REPLREADERWRITER
#define DECLSPEC_DLLPORT __declspec(dllexport)
#else
#define DECLSPEC_DLLPORT __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

DECLSPEC_DLLPORT int REPLStartW(const wchar_t* command, const wchar_t* templateName);
DECLSPEC_DLLPORT int REPLStop(int processID);
DECLSPEC_DLLPORT void REPLSetTimeout(int processID, int timeoutMilliseconds);
DECLSPEC_DLLPORT void REPLWriteLineW(int processID, const wchar_t* inputText);
DECLSPEC_DLLPORT const wchar_t* REPLReadLineW(int processID);

#ifdef __cplusplus
}
#endif
