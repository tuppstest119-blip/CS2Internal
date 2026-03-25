#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include <stdexcept>


#include "core/interfaces.hpp"
#include "core/hooks.hpp"
#include "core/menu.hpp"
#include "core/debug_log.hpp"


DWORD WINAPI cheat_thread(LPVOID instance) {
	logger::DebugLog("[DLL] cheat_thread started");
	try {
		interfaces::create();
		logger::DebugLog("[DLL] interfaces::create() done");
		menu::create();
		logger::DebugLog("[DLL] menu::create() done");
		hooks::create();
		logger::DebugLog("[DLL] hooks::create() done");
	} catch (const std::exception& e) {
		logger::DebugLog("[DLL] Exception: %s", e.what());
		hooks::destroy();
		menu::destroy();
		interfaces::destroy();
		MessageBoxA(nullptr, e.what(), "Error", MB_ICONERROR);
		FreeLibraryAndExitThread(reinterpret_cast<HMODULE>(instance), 1);
	}

	while (!(GetAsyncKeyState(VK_END) & 1)) {
		Sleep(100);
	}

	logger::DebugLog("[DLL] Shutting down...");
	hooks::destroy();
	menu::destroy();
	interfaces::destroy();

	MessageBoxA(nullptr, "Cheat wird beendet...", "Info", MB_ICONINFORMATION);

	FreeLibraryAndExitThread(reinterpret_cast<HMODULE>(instance), 0);

	return 0;
}

DWORD APIENTRY DllMain(HINSTANCE instance, DWORD reason, LPVOID) {
	if (reason == DLL_PROCESS_ATTACH) 
{
		DisableThreadLibraryCalls(instance);

		HANDLE thread = CreateThread(nullptr, 0, cheat_thread, instance, 0, nullptr);
		if (thread != nullptr && thread != INVALID_HANDLE_VALUE) 
		{
			CloseHandle(thread);
		}
			return TRUE;
	}
	 
	return FALSE;
}