#pragma once
#include "headers.h"


struct LocalPlayer {

	D3DXMATRIX GetViewMatrix(SOCKET connection, DWORD procID, DWORD moduleBase);
	D3DXVECTOR3 GetPos(SOCKET connection, DWORD procID, DWORD localPlayer);
	D3DXVECTOR3 GetPunchAngle(SOCKET connection, DWORD procID, DWORD localPlayer);
	D3DXVECTOR3 GetViewAngles(SOCKET connection, DWORD procID, DWORD localPlayer, DWORD engineBase);
	D3DXVECTOR3 GetViewOffset(SOCKET connection, DWORD procID, DWORD localPlayer);
	D3DXVECTOR3 GetVelocity(SOCKET connection, DWORD procID, DWORD localPlayer);
	

	int GetHealth(SOCKET connection, DWORD procID, DWORD localPlayer);
	int GetTeam(SOCKET connection, DWORD procID, DWORD localPlayer);
	int GetShotsFired(SOCKET connection, DWORD prociD, DWORD localPlayer);
	int CrosshairID(SOCKET connection, DWORD procID, DWORD localPlayer);
	float IsFlashed(SOCKET connection, DWORD procID, DWORD localPlayer, DWORD engineBase);

	DWORD GetFlag(SOCKET connection, DWORD procID, DWORD localPlayer);

	string GetCurrentWeaponClass(short int WeaponID);
	int GetCurrentWeapon(SOCKET connection, DWORD procID, DWORD localPlayer, DWORD moduleBase);

	void SetViewAngles(SOCKET connection, DWORD procID, DWORD engineBase, D3DXVECTOR3 angles);
	void Shoot(SOCKET connection, DWORD procID, DWORD localPlayer, DWORD moduleBase);

};