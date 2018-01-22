#define NOMINMAX
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <TlHelp32.h>
#include <string>
#include <map>
#include <codecvt>
#include <sstream>

#define EXPORT extern "C" __declspec(dllexport)

std::string WstringToString(std::wstring wstr)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;

	try
	{
		return converter.to_bytes(wstr);
	} catch (std::range_error)
	{
		std::stringstream s;
		s << wstr.c_str();
		return s.str();
	}
}

std::wstring StringToWstring(std::string str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;

	try
	{
		return converter.from_bytes(str);
	} catch (std::range_error)
	{
		std::wostringstream s;
		s << str.c_str();
		return s.str();
	}
}

bool file_exists(const wchar_t* name)
{
	std::ifstream infile{ name };
	return infile.good();
}

bool process_exists(const wchar_t* name, uint32_t& pid)
{
	auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (snapshot == INVALID_HANDLE_VALUE)
		return false;

	auto entry = PROCESSENTRY32{ sizeof(PROCESSENTRY32) };

	if (Process32First(snapshot, &entry))
	{
		do
		{
			if (!wcscmp(entry.szExeFile, name))
			{
				pid = entry.th32ProcessID;
				CloseHandle(snapshot);
				return true;
			}
		} while (Process32Next(snapshot, &entry));
	}
	CloseHandle(snapshot);
	return false;
}

bool enable_debug_privilege(HANDLE process)
{
	LUID luid;
	HANDLE token;
	TOKEN_PRIVILEGES newPrivileges;

	if (!OpenProcessToken(process, TOKEN_ADJUST_PRIVILEGES, &token))
		return false;

	if (!LookupPrivilegeValue(nullptr, SE_DEBUG_NAME, &luid))
		return false;

	newPrivileges.PrivilegeCount = 1;
	newPrivileges.Privileges[0].Luid = luid;
	newPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	return AdjustTokenPrivileges(
		token,                     // TokenHandle
		FALSE,                     // DisableAllPrivileges
		&newPrivileges,            // NewPrivileges
		sizeof(newPrivileges),     // BufferLength
		nullptr,                   // PreviousState (OPTIONAL)
		nullptr                    // ReturnLength (OPTIONAL)
	);
}

bool process_open(uint32_t pid, HANDLE& handle)
{
	handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION |
						 PROCESS_VM_READ | PROCESS_VM_WRITE |
						 PROCESS_CREATE_THREAD, FALSE, pid);

	return handle != nullptr;
}

void inject(HANDLE process, const wchar_t* dll)
{
	auto thread = HANDLE{ nullptr };
	auto exit_code = 0;
	auto dllpath = VirtualAllocEx(process, nullptr, 0x1000, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	if (!dllpath) goto fail;

	auto success = WriteProcessMemory(process, dllpath, dll, (wcslen(dll) + 1) * sizeof(wchar_t), nullptr);

	if (!success) goto fail;

	thread = CreateRemoteThread(process, nullptr, 0,
		(LPTHREAD_START_ROUTINE) LoadLibraryW, dllpath, 0, nullptr);

	if (!thread) goto fail;

	if (WaitForSingleObject(thread, 4000) == WAIT_OBJECT_0)
	{
		exit_code = 0;
		GetExitCodeThread(thread, (DWORD*) &exit_code);
	}

	return;

fail:
	wchar_t err[256];
	FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
				   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), err, 256, NULL);
	if (thread)
		CloseHandle(thread);
	if (dllpath)
		VirtualFreeEx(process, dllpath, 0, MEM_RELEASE);
	throw std::runtime_error{ WstringToString(err) };
}

EXPORT int __cdecl inject()
{
	constexpr auto TARGET_FILE = L"cheat.dll";
	constexpr auto TARGET_PROCESS = L"csgo.exe";

	wchar_t fullpath[MAX_PATH] = { 0 };
	auto    proc_id = 0u;
	auto    proc_handle = HANDLE{ nullptr };

	try
	{
		if(!enable_debug_privilege(GetCurrentProcess()))
			throw std::runtime_error({ "Could not elevate privileges" });
		if (!file_exists(TARGET_FILE))
			throw std::runtime_error{ "DLL not found." };

		if (!process_exists(TARGET_PROCESS, proc_id))
			throw std::runtime_error{ "Process is not running." };

		if (!process_open(proc_id, proc_handle))
		{
			throw std::runtime_error{ "Failed to open process." };
		}

		_wfullpath(fullpath, TARGET_FILE, MAX_PATH);

		inject(proc_handle, fullpath);
	} catch (const std::exception& ex)
	{
		OutputDebugStringA("[Error]  ");
		OutputDebugStringA(ex.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}