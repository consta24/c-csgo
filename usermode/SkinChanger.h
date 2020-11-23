#pragma once
#include "headers.h"
#include "Weapons.h"
#include "LocalPlayer.h"

using namespace hazedumper::netvars;
using namespace hazedumper::signatures;

void SkinChanger(SOCKET connection, DWORD procID, DWORD localPlayer, DWORD moduleBase) {

	LocalPlayer player;
	int currentWeaponID = player.GetCurrentWeapon(connection, procID, localPlayer, moduleBase);
	int weapon = driver::read<int>(connection, procID, localPlayer + m_hActiveWeapon);
	int weaponEnt = driver::read<int>(connection, procID, moduleBase + dwEntityList + ((weapon & 0xFFF) - 1) * 0x10);

	int PaintKit = 0;
	int EntityQuality = 0;
	float Wear = 0.0001f;
	int Seed = 0;
	int ST = 1337;

	cout << currentWeaponID << endl;

	switch (currentWeaponID) {

	case WEAPON_AK47:
		PaintKit = 180;
		break;
	}

	if (PaintKit != 0) {
		if (driver::read<int>(connection, procID, weaponEnt + m_iItemIDHigh != -1))
			driver::write<int>(connection, procID, weaponEnt + m_iItemIDHigh, -1);
		driver::write<int>(connection, procID, weaponEnt + m_OriginalOwnerXuidHigh, 0);
		driver::write<int>(connection, procID, weaponEnt + m_OriginalOwnerXuidLow, 0);
		driver::write<int>(connection, procID, weaponEnt + m_nFallbackPaintKit, PaintKit);
		driver::write<int>(connection, procID, weaponEnt + m_nFallbackSeed, Seed);
		driver::write<int>(connection, procID, weaponEnt + m_nFallbackStatTrak, ST);
		driver::write<float>(connection, procID, weaponEnt + m_flFallbackWear, Wear);
		if (ST >= 0) {
			driver::write<int>(connection, procID, weaponEnt + m_iEntityQuality, 9);
		}
		else {
			driver::write<int>(connection, procID, weaponEnt + m_iEntityQuality, EntityQuality);
		}
	}

	cout << driver::read<int>(connection, procID, weaponEnt + m_nFallbackPaintKit) << endl;
}