# Hyperion Injector

This injector safely maps a dll into memory in a suspended state, protecting it from hyperion's queries and yara scans via an instrumentation callback (*). <br>
The injector provides FFE0000h bytes of RWX memory to inject to. <br>

Then, by use of the IC, it detours a thread at random (or an optional targeted thread) to call the injected dll's entry point. <br>

# Files

bin/hyperion_inject.dll - The api which provides you with the functions "inject", to inject a dll, and "update", which must be called in a while loop. <br>
bin/hello_world.dll - A simple test dll that displays a message box <br>
bin/hyperion_injector.exe - Basically an empty project compiled with the source file here. <br>

# How to use

The api is pretty self explanatory, take a look at main.cpp. <br>
The inject function takes the process id and the parh to the dll you want to inject. <br>
the update function MUST be called in a while loop, immediately after calling inject. It sustains the bypass as long as rhe process is open by updating the IC of the process. <br>

# PLEASE NOTE:
# You MUST run as ADMIN, whether its hyperion_injector.exe or your own compiled exe using this api. Admin rights are required to modify certain properties of another process 

