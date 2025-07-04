#include <Windows.h>
#include <vector>
#include <iostream>
#include <tlhelp32.h>

// Declarations for hyperion_inject.dll imports
typedef int(__fastcall* t_inject)(const DWORD process_id, const DWORD thread_id, const char* const dll_path);
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

static std::vector<DWORD> enumerate_threads(const DWORD pid)
{
	std::vector<DWORD> tids = {};

	HANDLE h_snap_thread = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (h_snap_thread != INVALID_HANDLE_VALUE)
	{
		THREADENTRY32 te{ 0 };
		te.dwSize = sizeof(te);

		if (Thread32First(h_snap_thread, &te))
		{
			do {
				if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) +
					sizeof(te.th32OwnerProcessID))
				{
					if (te.th32OwnerProcessID == pid)
						tids.push_back(te.th32ThreadID);
				}

				te.dwSize = sizeof(te);
			} while (Thread32Next(h_snap_thread, &te));
		}
		CloseHandle(h_snap_thread);
	}

	return tids;
}

const DWORD get_target_thread_id(const DWORD pid, const wchar_t* name)
{
	for (const auto tid : enumerate_threads(pid))
	{
		HANDLE h_thread = OpenThread(THREAD_ALL_ACCESS, false, tid);
		if (h_thread == INVALID_HANDLE_VALUE)
			continue;

		PWSTR data;
		HRESULT hr = GetThreadDescription(h_thread, &data);
		if (SUCCEEDED(hr))
		{
			if (lstrcmpW(data, name) == 0)
			{
				LocalFree(data);
				return tid;
			}

			LocalFree(data);
		}
	}

	return 0;
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

		const DWORD target_thread_id = get_target_thread_id(pid, L"RakNet::RecvFromLoop");
		std::cout << "Injecting on thread " << target_thread_id << "." << std::endl;

		t_inject f_inject = reinterpret_cast<t_inject>(GetProcAddress(inject_module, "inject"));
		t_update f_update = reinterpret_cast<t_update>(GetProcAddress(inject_module, "update"));

		if (f_inject(pid, target_thread_id, std::string(current_dir + "hello_world.dll").c_str()) != 0)
		{
			while (f_update());

			std::cout << "Finished injecting" << std::endl;
		}
	}

	return 0;
}
