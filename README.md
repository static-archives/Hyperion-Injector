# Hyperion Injector

This injector safely maps a dll into memory in a suspended state, protecting it from hyperion's queries and yara scans via an instrumentation callback. <br>
The injector provides FFE0000h bytes of RWX memory to inject to. <br>

Then, by use of the IC, it detours a thread at random (or an optional targeted thread) to call the injected entry point. <br>

# Files

bin/hyperion_inject.dll - The api which provides you with the functions "inject", to inject a dll, and "update", which must be called in a while loop. <br>
bin/hello_world.dll - A simple test dll that displays a message box <br>
bin/hyperion_injector.exe - The compiled demo source. Injects "hello_world.dll" which must be in the same directory as the exe<br>
main.cop - A demo injector source using the api <br>

# How to use

To try out injecting hello_world.dll, you can download the bin folder, join a server and wait for the game to load fully, then run the hyperion_injector.exe (make sure to run it as admin). <br>
The api is pretty self explanatory, take a look at main.cpp. I've placed comments in the source file to walk you through it.  <br>
The "inject" function takes the process id and the parh to the dll you want to inject. <br>
the "update" function MUST be called in a while loop with zero delays, immediately after calling inject. It resumes the process and sustains the bypass as long as the process is open and only returns when the process is closed. You may spawn this loop in a new thead.<br>


# INPORTANT -- PLEASE READ:
# 1. You MUST run as ADMIN
Whether its hyperion_injector.exe or your own compiled exe using this api, admin rights are required for the api to work.

# 2. You cant create new threads
Since this is borrowing a thread, make sure you return from DllMain as quick as possible or else it may sense somethings up. This injector is meant for performing quick tasks. <br>
Alternatively, if you wish to create a new thread you will have to use your own bypass for thread creation.

# 3. Keep it minimal
To heavily reduce the risk of detection, try to use C code and C functions if at all possible.
C++17 and up has a lot of great features but they all rely heavily on dependencies. The std library uses a ton of dependencies. Every import you use has a chance of being hooked -- even malloc. I recommend use very minimal functions and avoid complex C++ code, virtual classes etc. 
