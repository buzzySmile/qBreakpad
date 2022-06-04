#include "WinVeh.h"

#include <QMap>
#include <QFileInfo>
#include <QDateTime>
#include <QDebug>
#include <dbghelp.h>  //for minidump
#include <QUuid>

// refer from google::breakpad::HandleException
// This define is new to Windows 10.
#ifndef DBG_PRINTEXCEPTION_WIDE_C
#define DBG_PRINTEXCEPTION_WIDE_C ((DWORD)0x4001000A)
#endif

// Function pointer type for MiniDumpWriteDump, which is looked up
// dynamically.
typedef BOOL (WINAPI *MiniDumpWriteDump_type)(
    HANDLE hProcess,
    DWORD dwPid,
    HANDLE hFile,
    MINIDUMP_TYPE DumpType,
    CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
    CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
    CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

PVOID WinVeh::previous_filter_veh = NULL;

void WinVeh::AddVeh()
{
    previous_filter_veh = AddVectoredExceptionHandler(TRUE, HandleException_Veh);
}

LONG WinVeh::HandleException_Veh(PEXCEPTION_POINTERS exinfo)
{
    // ignore debug exception and EXCEPTION_INVALID_HANDLE
    switch (exinfo->ExceptionRecord->ExceptionCode) {
    case EXCEPTION_BREAKPOINT:
    case EXCEPTION_SINGLE_STEP:
    case DBG_PRINTEXCEPTION_C:
    case DBG_PRINTEXCEPTION_WIDE_C:
    case EXCEPTION_INVALID_HANDLE:
        return EXCEPTION_CONTINUE_EXECUTION;
    // veh may catch some unnecessary exception, such as 0x000006ba(print server err), you can ignore it if you don't need it.
    case 0x000006ba:
        return EXCEPTION_CONTINUE_EXECUTION;
    }

    // start to write dump
    // May can use google_breakpad::ExceptionHandler's WriteMinidump() WriteMinidumpForException()... functions to write dumps, but it seems more complex.
    printf(" ---- veh ExceptionFlags: %lX \n", exinfo->ExceptionRecord->ExceptionFlags);  //If it is can be continue to execute. Mostly is 0.
    printf(" ---- veh ExceptionCode: %lX \n", exinfo->ExceptionRecord->ExceptionCode);

    RemoveVectoredExceptionHandler(previous_filter_veh);  //del veh, avoid to self-recursion

    bool bSuccess = false;

    QString sFileName = QUuid::createUuid().toString();
    sFileName.replace(0, 1, "V");  // replace { to V, distinguish this veh's dump and google break's dump(seh' dump)
    sFileName.chop(1);  // del }
    sFileName += ".dmp";
    std::wstring wsFileName = sFileName.toStdWString();
    HANDLE lhDumpFile = CreateFile(wsFileName.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (!lhDumpFile) {
        printf(" ---- veh CreateFile failed: \n");
        return EXCEPTION_EXECUTE_HANDLER;
    }

    MINIDUMP_EXCEPTION_INFORMATION loExceptionInfo;
    loExceptionInfo.ExceptionPointers = exinfo;
    loExceptionInfo.ThreadId = GetCurrentThreadId();
    loExceptionInfo.ClientPointers = TRUE;

    HMODULE dbghelp_module_ = LoadLibrary(L"dbghelp.dll");
    if (dbghelp_module_) {
//       function MiniDumpWriteDump() defined in MinGW but not in msvc2015-32bit, use MiniDumpWriteDump_type and find it from dll
        MiniDumpWriteDump_type minidump_write_dump_ = reinterpret_cast<MiniDumpWriteDump_type>(GetProcAddress(dbghelp_module_, "MiniDumpWriteDump"));
        if (!minidump_write_dump_) {
            printf(" ---- veh MiniDumpWriteDump() failed: \n");
            return EXCEPTION_EXECUTE_HANDLER;
        }
        bSuccess = minidump_write_dump_(GetCurrentProcess(), GetCurrentProcessId(), lhDumpFile, MiniDumpNormal, &loExceptionInfo, NULL, NULL);
    } else {
        printf(" ---- veh LoadLibrary 'dbghelp' failed: \n");
        return EXCEPTION_EXECUTE_HANDLER;
    }

    if (bSuccess)
        printf(" ---- veh write file success \n");
    else
        printf(" ---- veh write file failed: \n");
    fflush(stdout);  // ouput immediately, or may not print cause the crash

    CloseHandle(lhDumpFile);

    return EXCEPTION_EXECUTE_HANDLER;
}
