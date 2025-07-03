# Hyperion Injector

This injector safely maps a dll into memory in a suspended state, protecting it from hyperion's queries via an instrumentation callback.
The injector provides FFE0000h bytes of RWX memory to inject to.

By use of the IC, it detours a thread at random (or an optional targeted thread) to run your DllMain.
