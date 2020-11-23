#pragma once
#include "headers.h"

struct Entity {

	DWORD GetEntityAddress(SOCKET connection, DWORD procID, int index, DWORD moduleBase);
	DWORD GetFlag(SOCKET connection, DWORD procID, DWORD base);
	D3DXVECTOR3 GetPos(SOCKET connection, DWORD procID, DWORD base);
	D3DXVECTOR3 GetBonePos(SOCKET connection, DWORD procID, DWORD base, int bone);
	D3DXVECTOR3 GetVelocity(SOCKET connection, DWORD procID, DWORD base);
	int GetHealth(SOCKET connection, DWORD procID, DWORD base);
	int GetTeam(SOCKET connection, DWORD procID, DWORD base);
	int Spotted(SOCKET connection, DWORD procID, DWORD base);
	int Dormant(SOCKET connection, DWORD procID, DWORD base);
	bool SpottedByPlayer(SOCKET connection, DWORD procID, DWORD base, DWORD engineBase);

};