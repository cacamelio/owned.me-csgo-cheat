#include "../client/utilities/csgo.hpp"
#include "../client/utilities/data.hpp"
#include "../client/utilities/sec connection.h"
#include "features/features.hpp"
#include "menu\config\config.h"

DWORD WINAPI OnDLLAttach(LPVOID lpParameter) 
{
	c_api* api = new c_api;

	c_data data;
	static char login[32] = "", password[32] = "";

	char temp[256];
	if (data.get_registry((char*)"Software\\shark", (char*)"username", temp))
		data.get_registry((char*)"Software\\shark", (char*)"username", login);
	if (data.get_registry((char*)"Software\\shark", (char*)"password", temp))
		data.get_registry((char*)"Software\\shark", (char*)"password", password);

	while (!GetModuleHandleA("serverbrowser.dll"))
		Sleep(200);

	console::initialize("sharkhack");
	console::log("sharkhack\n");

	//if (api->auth(login, password))
	//{
		client::initialize();
	//}
	//else
	//	exit(-1);

	return NULL;
}

BOOL APIENTRY DllMain(const HMODULE hModule, const DWORD dwReason, const LPVOID lpReserved) {
	DisableThreadLibraryCalls(hModule);

	switch (dwReason) {
		case DLL_PROCESS_ATTACH: {
			const auto hThread = CreateThread(nullptr, 0U, OnDLLAttach, hModule, 0UL, nullptr);
			if (!hThread)
				return FALSE;

			g::hDLL = hModule;

			CloseHandle(hThread);

			return TRUE;
		}
		break;
		default:
			break;
	}

	return FALSE;
}