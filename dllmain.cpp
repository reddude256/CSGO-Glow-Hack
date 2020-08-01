#include <Windows.h>
#include <cstdint>
#include "offsets.h"

struct variables {
	DWORD clientMod; 
	DWORD localPlayer; 
	DWORD Ent; 
	DWORD glowManger;
	DWORD LocalTeam; 
	DWORD Health; 
	DWORD bDormant; 
	DWORD glowIndex; 
	DWORD EntTeam; 
}val;

struct EnemyGlow {
	float red = 1.f;
	float green = 0.f;
	float blue = 0.f;
	float alpha = 1.f;
	uint8_t padding[8];
	float unknown = 1.f;
	uint8_t padding2[4];
	BYTE renderOccluded = true;
	BYTE renderUnoccluded = false;
	BYTE fullBloom = false;
}enm;

struct localGlow {
	float red = 0.f;
	float green = 1.f;
	float blue = 0.f;
	float alpha = 1.f;
	uint8_t padding[8];
	float unknown = 1.f;
	uint8_t padding2[4];
	BYTE renderOccluded = true;
	BYTE renderUnoccluded = false;
	BYTE fullBloom = false;
}loc;

DWORD WINAPI HackThread(HMODULE hModule)
{
	val.clientMod = (DWORD)GetModuleHandle("client.dll"); 
	val.localPlayer = *(DWORD*)(val.clientMod + hazedumper::signatures::dwLocalPlayer); 

	if (val.localPlayer == NULL)
	{
		while (val.localPlayer == NULL)
		{
			val.localPlayer = *(DWORD*)(val.clientMod + hazedumper::signatures::dwLocalPlayer);
		}
	}

	while (!GetAsyncKeyState(VK_F10))
	{
		val.glowManger = *(DWORD*)(val.clientMod + hazedumper::signatures::dwGlowObjectManager);
		val.LocalTeam = *(DWORD*)(val.clientMod + hazedumper::netvars::m_iTeamNum);

		for (int i = 0; i < 64; i++)
		{
			val.Ent = *(DWORD*)(val.clientMod + hazedumper::signatures::dwEntityList + i * 0x10); if (val.Ent == NULL) continue; 
			val.bDormant = *(DWORD*)(val.Ent + hazedumper::signatures::m_bDormant); if (val.bDormant) continue;
			val.glowIndex = *(DWORD*)(val.Ent + hazedumper::netvars::m_iGlowIndex);
			val.Health = *(DWORD*)(val.Ent + hazedumper::netvars::m_iHealth); if (val.Health < 1 || val.Health > 100) continue;
			val.EntTeam = *(DWORD*)(val.Ent + hazedumper::netvars::m_iTeamNum);


			if (val.LocalTeam == val.EntTeam)
			{
				*(localGlow*)(val.glowManger + (val.glowIndex * 0x38) + 0x4) = loc; 
			}
			else if(val.LocalTeam != val.EntTeam)
			{
				*(EnemyGlow*)(val.glowManger + (val.glowIndex * 0x38) + 0x4) = enm; 
			}
		}
		Sleep(1); 
	}

	FreeLibraryAndExitThread(hModule, 0); 
	return 0; 
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {

	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, 0); 
	return TRUE;
}