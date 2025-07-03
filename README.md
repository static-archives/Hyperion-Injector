# Hyperion Injector

This injector safely maps a dll into memory in a suspended state, protecting it from hyperion's queries and yara scans via an instrumentation callback. <br>
The injector provides FFE0000h bytes of RWX memory to inject to. <br>

Then, by use of the IC, it detours a thread at random (or an optional targeted thread) to call the injected dll's entry point. <br>

# Files

bin/hyperion_inject.dll - The api which provides you with the functions "inject", to inject a dll, and "update", which must be called in a while loop. <br>
bin/hello_world.dll - A simple test dll that displays a message box <br>
bin/hyperion_injector.exe - Basically an empty project compiled with the source file here. <br>
main.cop - A demo injector source using the api <br>

# How to use

The api is pretty self explanatory, take a look at main.cpp. I've placed comments in the source file to walk you through it.  <br>
The "inject" function takes the process id and the parh to the dll you want to inject. <br>
the "update" function MUST be called in a while loop with zero delays, immediately after calling inject. It resumes the process and sustains the bypass as long as the process is open and only returns when the process is closed. You may spawn this loop in a new thead.<br>


# PLEASE NOTE:
# You MUST run as ADMIN
Whether its hyperion_injector.exe or your own compiled exe using this api, admin rights are required for the api to work.

