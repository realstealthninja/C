/*
 * Scilab ( http://www.scilab.org/ ) - This file is part of Scilab
 * Copyright (C) DIGITEO - 2010 - Allan CORNET
 *
 * Copyright (C) 2012 - 2016 - Scilab Enterprises
 *
 * This file is hereby licensed under the terms of the GNU GPL v2.0,
 * pursuant to article 5.3.4 of the CeCILL v.2.1.
 * This file was originally licensed under the terms of the CeCILL v2.1,
 * and continues to be available under such terms.
 * For more information, see the COPYING file which you should have received
 * along with this program.
 *
 */
/*--------------------------------------------------------------------------*/
#ifndef __FORK_WINDOWS_H__
#define __FORK_WINDOWS_H__

#include "bool.h"

/* http://technet.microsoft.com/en-us/library/bb497007.aspx */
/* http://undocumented.ntinternals.net/ */

/**
 * simulate fork on Windows
 */
int fork(void)
{
    HANDLE hProcess = 0, hThread = 0;
    OBJECT_ATTRIBUTES oa = {sizeof(oa)};
    MEMORY_BASIC_INFORMATION mbi;
    CLIENT_ID cid;
    USER_STACK stack;
    PNT_TIB tib;
    THREAD_BASIC_INFORMATION tbi;

    CONTEXT context = {CONTEXT_FULL | CONTEXT_DEBUG_REGISTERS |
                       CONTEXT_FLOATING_POINT};

    if (setjmp(jenv) != 0)
    {
        return 0; /* return as a child */
    }

    /* check whether the entry points are initilized and get them if necessary
     */
    if (!ZwCreateProcess && !haveLoadedFunctionsForFork())
    {
        return -1;
    }

    /* create forked process */
    ZwCreateProcess(&hProcess, PROCESS_ALL_ACCESS, &oa, NtCurrentProcess(),
                    TRUE, 0, 0, 0);

    /* set the Eip for the child process to our child function */
    ZwGetContextThread(NtCurrentThread(), &context);

    /* In x64 the Eip and Esp are not present, their x64 counterparts are Rip
    and Rsp respectively.
    */
#if _WIN64
    context.Rip = (ULONG)child_entry;
#else
    context.Eip = (ULONG)child_entry;
#endif

#if _WIN64
    ZwQueryVirtualMemory(NtCurrentProcess(), (PVOID)context.Rsp,
                         MemoryBasicInformation, &mbi, sizeof mbi, 0);
#else
    ZwQueryVirtualMemory(NtCurrentProcess(), (PVOID)context.Esp,
                         MemoryBasicInformation, &mbi, sizeof mbi, 0);
#endif

    stack.FixedStackBase = 0;
    stack.FixedStackLimit = 0;
    stack.ExpandableStackBase = (PCHAR)mbi.BaseAddress + mbi.RegionSize;
    stack.ExpandableStackLimit = mbi.BaseAddress;
    stack.ExpandableStackBottom = mbi.AllocationBase;

    /* create thread using the modified context and stack */
    ZwCreateThread(&hThread, THREAD_ALL_ACCESS, &oa, hProcess, &cid, &context,
                   &stack, TRUE);

    /* copy exception table */
    ZwQueryInformationThread(NtCurrentThread(), ThreadMemoryPriority, &tbi,
                             sizeof tbi, 0);
    tib = (PNT_TIB)tbi.TebBaseAddress;
    ZwQueryInformationThread(hThread, ThreadMemoryPriority, &tbi, sizeof tbi,
                             0);
    ZwWriteVirtualMemory(hProcess, tbi.TebBaseAddress, &tib->ExceptionList,
                         sizeof tib->ExceptionList, 0);

    /* start (resume really) the child */
    ZwResumeThread(hThread, 0);

    /* clean up */
    ZwClose(hThread);
    ZwClose(hProcess);

    /* exit with child's pid */
    return (int)cid.UniqueProcess;
}

/**
 * check if symbols to simulate fork are present
 * and load these symbols
 */
BOOL haveLoadedFunctionsForFork(void);

#endif /* __FORK_WINDOWS_H__ */
/*--------------------------------------------------------------------------*/
