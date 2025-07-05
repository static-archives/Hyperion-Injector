# Hyperion Injector

This injector safely maps a dll into memory in a suspended state, protecting it from hyperion's queries and yara scans via an instrumentation callback. <br>
The injector provides FFE0000h bytes of RWX memory to inject to. <br>

By using an IC, it detours a thread at random (or an optional targeted thread) to call the injected entry point. <br>

# Files in this repo

bin/hyperion_inject.dll - The injector API<br>
bin/hello_world.dll - A simple test dll that displays a message box <br>
bin/hyperion_injector.exe - The compiled demo source. Injects "hello_world.dll" using the api when run (as admin*). All files must be in the same directory as this exe<br>
main.cpp - A demo source using the api <br>

# How to use

To try out injecting hello_world.dll, you can download the bin folder, join a server and wait for the game to load fully, then run the hyperion_injector.exe (make sure to run it as admin). <br>
The api is pretty self explanatory, take a look at main.cpp. I've placed comments in the source file to walk you through it. The api functions are:<br>
<br>
`inject(DWORD pid, DWORD tid, const char* path)` - takes the process id to inject to (pid), target thread id to run on (tid), and the path to the dll you want to inject. You can set "tid" to 0 if you want your DLL to run on any thread. It is recommended you use a less important thread to work with.<br>
<br>
`update()` - this MUST be called in a while loop with zero delays, immediately after calling inject. It resumes the process and sustains the bypass as long as the process is open and only returns when the process is closed. Optionally you can spawn this loop in a new thead.<br>
<br>

There is also built-in functionality for setting a kiuser hook, as well as a detour for syscalls.<br>
Here is a sample DLL source code you can use which logs an exception.<br>

```cpp
// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include <sstream>

bool logged_exception = false;

int64_t kiuser_hook(PEXCEPTION_RECORD precord, PCONTEXT pctx)
{
    if (precord->ExceptionCode == EXCEPTION_SINGLE_STEP && !logged_exception)
    {
        logged_exception = true;
        
        std::stringstream ss;
        ss << std::hex << "Exception address: " << precord->ExceptionAddress << ". Code: " << precord->ExceptionCode << std::endl;
        MessageBoxA(0, ss.str().c_str(), "Exception", MB_OK);
    }

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        // Enable support for core C++ features
        LoadLibraryA("MSVCP140.dll");
        LoadLibraryA("VCRUNTIME140.dll");
        LoadLibraryA("VCRUNTIME140_1.dll");

        // Set kiuser hook
        reinterpret_cast<PVOID*>(lpReserved)[1] = kiuser_hook;

        // load_exploit();

        break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
```

# INPORTANT -- PLEASE READ:
# 1. You MUST run as ADMIN
Whether its hyperion_injector.exe or your own compiled exe using this api, admin rights are required for the api to work.

# 2. You cant create new threads
Since this is borrowing a thread, make sure you return from DllMain as quick as possible or else it may sense somethings up. This injector is meant for performing quick tasks. <br>
Alternatively, if you wish to create a new thread you will have to use your own bypass for thread creation.

# 3. DLL Crashes, lacking support :(
To enable support for core features in your dll, run this before anything else (place in your DllMain):<br>
LoadLibraryA("MSVCP140.dll");<br>
LoadLibraryA("VCRUNTIME140.dll");<br>
<br>
Now it will be able to support most of the std library, string/file streams, and functions like memcpy/strcpy/etc.

# 4. Keep it minimal
To heavily reduce the risk of detection, try to use C code and C functions if at all possible.
C++17 and up has a lot of great features but they all rely heavily on dependencies. The std library uses a ton of dependencies. Every import you use has a chance of being hooked -- even malloc. I recommend use very minimal functions and avoid complex code. <br>
<br>
