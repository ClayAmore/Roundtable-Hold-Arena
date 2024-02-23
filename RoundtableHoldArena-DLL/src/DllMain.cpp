#include <Windows.h>

#include "DirectXHook.h"
#include "MemoryUtils.h"
#include "RoundtableHoldArena.h"

FARPROC forwardExports[6];

char dll_path[2048];

void init_console()
{
	if (!GetConsoleWindow() && !AllocConsole()) {
		return;
	}

	// std::cout, std::clog, std::cerr, std::cin
	FILE* fDummy;
	freopen_s(&fDummy, "CONOUT$", "w", stdout);
	freopen_s(&fDummy, "CONOUT$", "w", stderr);
	freopen_s(&fDummy, "CONIN$", "r", stdin);
	std::cout.clear();
	std::clog.clear();
	std::cerr.clear();
	std::cin.clear();

	// std::wcout, std::wclog, std::wcerr, std::wcin
	HANDLE hConOut = CreateFile(TEXT("CONOUT$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE hConIn = CreateFile(TEXT("CONIN$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	SetStdHandle(STD_OUTPUT_HANDLE, hConOut);
	SetStdHandle(STD_ERROR_HANDLE, hConOut);
	SetStdHandle(STD_INPUT_HANDLE, hConIn);
	std::wcout.clear();
	std::wclog.clear();
	std::wcerr.clear();
	std::wcin.clear();
}

HMODULE LoadDllFromSystemFolder(std::string dllName)
{
	std::string systemFolderPath = "";
	char dummy[1];
	UINT pathLength = GetSystemDirectoryA(dummy, 1);
	systemFolderPath.resize(pathLength);
	LPSTR lpSystemFolderPath = const_cast<char*>(systemFolderPath.c_str());
	GetSystemDirectoryA(lpSystemFolderPath, systemFolderPath.size());
	systemFolderPath = lpSystemFolderPath;


	HMODULE dll = LoadLibraryA(std::string(systemFolderPath + "\\" + dllName).c_str());
	return dll;
}

bool CreateDinputForwardExports(HMODULE dll)
{
	if (!dll)
	{
		return false;
	}

	forwardExports[0] = GetProcAddress(dll, "DirectInput8Create");
	forwardExports[1] = GetProcAddress(dll, "DllCanUnloadNow");
	forwardExports[2] = GetProcAddress(dll, "DllGetClassObject");
	forwardExports[3] = GetProcAddress(dll, "DllRegisterServer");
	forwardExports[4] = GetProcAddress(dll, "DllUnregisterServer");
	forwardExports[5] = GetProcAddress(dll, "GetdfDIJoystick");

	for (int i = 0; i < 6; i++)
	{
		if (forwardExports[i] == NULL)
		{
			logger.Log("Failed to get function address for index %d", i);
			return false;
		}
	}

	return true;
}

void OpenDebugTerminal()
{
	std::fstream terminalEnableFile;
	terminalEnableFile.open("hook_enable_terminal.txt", std::fstream::in);
	if (terminalEnableFile.is_open())
	{
		if (AllocConsole())
		{
			freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
			SetWindowText(GetConsoleWindow(), "DirectXHook");
			logger.Log("Debug terminal enabled.");
		}
		terminalEnableFile.close();
	}
}


DWORD WINAPI HookThread(LPVOID lpParam)
{
	//init_console();
	HMODULE originalDll = LoadDllFromSystemFolder("dinput8.dll");
	if (!originalDll)
	{
		logger.Log("Failed to load 'dinput8.dll' from the system folder.");
		return 1;
	}

	if (!CreateDinputForwardExports(originalDll))
	{
		logger.Log("Failed to set up forward exports.");
		return 1;
	}

	static Renderer renderer;
	static DirectXHook dxHook(&renderer);
	static RoundTableHoldArena overlay(dll_path);
	dxHook.AddRenderCallback(&overlay);
	dxHook.Hook();

	logger.Log("Hooking completed successfully.");
	return 0;
}

extern "C"
{
	int AsmJmp();
	FARPROC address = NULL;
	void JumpToFunction(unsigned int index)
	{
		address = forwardExports[index];
		AsmJmp();
	}
	void PROXY_DirectInput8Create()
	{
		JumpToFunction(0);
	}
	void PROXY_DllCanUnloadNow()
	{
		JumpToFunction(1);
	}
	void PROXY_DllGetClassObject()
	{
		JumpToFunction(2);
	}
	void PROXY_DllRegisterServer()
	{
		JumpToFunction(3);
	}
	void PROXY_DllUnregisterServer()
	{
		JumpToFunction(4);
	}
	void PROXY_GetdfDIJoystick()
	{
		JumpToFunction(5);
	}
}

BOOL WINAPI DllMain(HMODULE module, DWORD reason, LPVOID)
{
	GetModuleFileNameA(module, dll_path, 2048);

	if (reason == DLL_PROCESS_ATTACH)
	{
		OpenDebugTerminal();
		CreateThread(0, 0, &HookThread, 0, 0, NULL);
	}

	return 1;
}