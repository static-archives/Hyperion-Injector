#include <Windows.h>
#include <iostream>
#include <direct.h> 

// Declarations for hyperion_inject.dll imports
typedef int(__fastcall* t_inject)(const DWORD pid, const char* const dll_path);
typedef bool(__fastcall* t_update)();

std::string get_current_directory()
{
	CHAR exe_path[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, exe_path, MAX_PATH);

	std::string current_dir(exe_path);
	while (!(current_dir.back() == '\\' || current_dir.back() == '/'))
		current_dir.pop_back();

	return current_dir;
}

int main()
{
	HWND hwnd = FindWindowA(NULL, "Roblox");
	if (hwnd != INVALID_HANDLE_VALUE)
	{
		DWORD pid;
		GetWindowThreadProcessId(hwnd, &pid);

		std::string current_dir = get_current_directory();

		HMODULE inject_module = LoadLibraryA(std::string(current_dir + "hyperion_inject.dll").c_str());
		if (!inject_module)
			return 0;

		std::cout << "Injecting... Module: " << std::hex << inject_module << std::endl;

		t_inject f_inject = reinterpret_cast<t_inject>(GetProcAddress(inject_module, "inject"));
		t_update f_update = reinterpret_cast<t_update>(GetProcAddress(inject_module, "update"));

		if (f_inject(pid, std::string(current_dir + "hello_world.dll").c_str()) != 0)
		{
			while (f_update());

			std::cout << "Finished injecting" << std::endl;
		}
	}

	return 0;
}