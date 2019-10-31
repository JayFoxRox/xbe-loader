#include "write_log.h"

#ifndef QUIET
#include <hal/debug.h>
#endif
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <xboxkrnl/xboxkrnl.h>

// Section will be copied to new versions of loader
#define __PERSIST_NAME "!persist"
#define __PERSIST __attribute__((section(__PERSIST_NAME)))

extern char* loader_path;

void write_log(const char* format, ...) {
#ifndef QUIET
    va_list argList;
    va_start(argList, format);
    debugPrint(format, argList);
    va_end(argList);
#endif
}

void write_log_crit(const char* format, ...) {
    char buffer[512];
    sprintf(buffer, "%d          ", KeTickCount);
    va_list argList;
    va_start(argList, format);
    vsprintf(&buffer[10], format, argList);
    va_end(argList);

    static LONG skipped_writes __PERSIST = 0;

    //FIXME: Possibly buffer data
#define PASSIVE_LEVEL 0 // Passive release level
#define LOW_LEVEL 0 // Lowest interrupt level
#define APC_LEVEL 1 // APC interrupt level
#define DISPATCH_LEVEL 2 // Dispatcher level
    if (KeGetCurrentIrql() > PASSIVE_LEVEL) {
        InterlockedIncrement(&skipped_writes);
        return;
    }

    //FIXME: log path might be invalid (if we don't have loader_path for example)
    //       We should also buffer in that case

    // Report that we messed up!
    if (skipped_writes > 0) {
        char buffer[1024];
        sprintf(buffer, "Skipped %d write(s)\n", skipped_writes);
        skipped_writes = 0; //FIXME: Do interlocked exchange
        write_log(buffer);
    }

#if 0
    static bool internal = false;
    if (internal == false) {
        internal = true;
        probe_memory(0x40000, 0x1000);
        internal = false;
    }
#endif

    // Initialize log and pick wether we want to overwrite, or append
    static bool initialize_log __PERSIST = true;
    const char* access;
    if (initialize_log) {
        access = "wb";
        initialize_log = false;
    } else {
        access = "ab";
    }

    // The kernel does not like relocating the RTL_CRITICAL_SECTION
    // That means we have to create a new one
    //FIXME: What happens to the old one?!
    static bool initialize_section = true;
    static RTL_CRITICAL_SECTION log_section;
    if (initialize_section) {
        RtlInitializeCriticalSection(&log_section);
        initialize_section = false;
    }

    // We have to protect the log from access of multiple threads
    RtlEnterCriticalSection(&log_section);

    // Generate the log path
    //FIXME: Do this somewhere else?
    char loader_directory[520];
    strcpy(loader_directory, loader_path);
    char *lastSlash = strrchr(loader_directory, '\\');
    *lastSlash = '\0';
    char log_path[520];
    sprintf(log_path, "%s\\log.txt", loader_directory);

    // Open file and if successful, write to it
    FILE* f = fopen(log_path, access);
    if (f != NULL) {
        fwrite(buffer, 1, strlen(buffer), f);
        fclose(f);
    } else {
#ifndef QUIET
        debugPrint("Failed to open log\n");
#else
        //FIXME: Do some kind of panic here? LED maybe?
#endif
    }

    // Print message to display
#ifndef QUIET
    debugPrint(buffer);
#endif

    // Leave the multi-threading critical section
    RtlLeaveCriticalSection(&log_section);
}
