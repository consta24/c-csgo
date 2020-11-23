#include "headers.h"
#include "LocalPlayer.h"

using namespace hazedumper::signatures;
using namespace hazedumper::netvars;


D3DXVECTOR3 LocalPlayer::GetPos(SOCKET connection, DWORD procID, DWORD localPlayer) {

	D3DXVECTOR3 pos;
	pos = driver::read<D3DXVECTOR3>(connection, procID, localPlayer + m_vecOrigin);
	return pos;
}

D3DXVECTOR3 LocalPlayer::GetPunchAngle(SOCKET connection, DWORD procID, DWORD localPlayer) {

	D3DXVECTOR3 pos;
	pos = driver::read<D3DXVECTOR3>(connection, procID, localPlayer + m_aimPunchAngle);
	return pos;
}

D3DXVECTOR3 LocalPlayer::GetViewOffset(SOCKET connection, DWORD procID, DWORD localPlayer) {

	D3DXVECTOR3 pos;
	pos = driver::read<D3DXVECTOR3>(connection, procID, localPlayer + m_vecViewOffset);
	return pos;
}

D3DXVECTOR3 LocalPlayer::GetViewAngles(SOCKET connection, DWORD procID, DWORD localPlayer, DWORD engineBase) {

	DWORD clientState = driver::read<DWORD>(connection, procID, engineBase + dwClientState);
	D3DXVECTOR3 pos;
	pos = driver::read<D3DXVECTOR3>(connection, procID, clientState + dwClientState_ViewAngles);
	return pos;
}

D3DXVECTOR3 LocalPlayer::GetVelocity(SOCKET connection, DWORD procID, DWORD localPlayer) {
	D3DXVECTOR3 velocity;
	velocity = driver::read<D3DXVECTOR3>(connection, procID, localPlayer + m_vecVelocity);
	return velocity;
}

int LocalPlayer::GetHealth(SOCKET connection, DWORD procID, DWORD localPlayer) {
	int hp;
	hp = driver::read<int>(connection, procID, localPlayer + m_iHealth);
	return hp;
}

int LocalPlayer::GetTeam(SOCKET connection, DWORD procID, DWORD localPlayer) {
	int team;
	team = driver::read<int>(connection, procID, localPlayer + m_iTeamNum);
	return team;
}

int LocalPlayer::CrosshairID(SOCKET connection, DWORD procID, DWORD localPlayer) {
	int crossid;
	crossid = driver::read<int>(connection, procID, localPlayer + m_iCrosshairId);
	return crossid;
}

int LocalPlayer::GetShotsFired(SOCKET connection, DWORD procID, DWORD localPlayer) {
	int shotsFired;
	shotsFired = driver::read<int>(connection, procID, localPlayer + m_iShotsFired);
	return shotsFired;

}

DWORD LocalPlayer::GetFlag(SOCKET connection, DWORD procID, DWORD localPlayer) {
	DWORD flag;
	flag = driver::read<DWORD>(connection, procID, localPlayer + m_fFlags);
	return flag;
}

D3DXMATRIX LocalPlayer::GetViewMatrix(SOCKET connection, DWORD procID, DWORD moduleBase) {
	
	D3DXMATRIX vM;
	vM = driver::read<D3DXMATRIX>(connection, procID, moduleBase + dwViewMatrix);
	return vM;
}

void LocalPlayer::SetViewAngles(SOCKET connection, DWORD procID, DWORD engineBase, D3DXVECTOR3 angles) {

	DWORD clientState = driver::read<DWORD>(connection, procID, engineBase + dwClientState);
	DWORD viewAngles = driver::read <DWORD>(connection, procID, clientState + dwClientState_ViewAngles);

	driver::write(connection, procID, clientState + dwClientState_ViewAngles + 0x0, angles.x);
	driver::write(connection, procID, clientState + dwClientState_ViewAngles + 0x4, angles.y);

}

void LocalPlayer::Shoot(SOCKET connection, DWORD procID, DWORD localPlayer, DWORD moduleBase) {

	int random = rand() % (70 + 1 - 35) + 35;

	D3DXVECTOR3 vel = GetVelocity(connection, procID, localPlayer);
	float acc = 30;
	if (vel.x < acc && vel.x > -acc && vel.y < acc && vel.y > -acc) {
		Sleep(random);
		driver::write(connection, procID, moduleBase + dwForceAttack, 5);
		Sleep(100);
		driver::write(connection, procID, moduleBase + dwForceAttack, 4);
	}

}

int LocalPlayer::GetCurrentWeapon(SOCKET connection, DWORD procID, DWORD localPlayer, DWORD moduleBase) {
	
	short int myWeaponID = -1;
	int weapon = driver::read<int>(connection, procID, localPlayer + m_hActiveWeapon);
	int weaponEnt = driver::read<int>(connection, procID, moduleBase + dwEntityList + ((weapon & 0xFFF) - 1) * 0x10);
	myWeaponID = driver::read<int>(connection, procID, weaponEnt + m_iItemDefinitionIndex);
	if (myWeaponID != -1)
		return myWeaponID;
	return -1;
}

float LocalPlayer::IsFlashed(SOCKET connection, DWORD procID, DWORD localPlayer, DWORD engineBase) {

	float flashDur = driver::read<float>(connection, procID, localPlayer + m_flFlashDuration - 0x10);
	return flashDur;
}

string LocalPlayer::GetCurrentWeaponClass(short int WeaponID) {

	if (WeaponID == 4 || WeaponID == 2 ||
		WeaponID == 36 || WeaponID == 1 ||
		WeaponID == 31 || WeaponID == 32 ||
		WeaponID == 3 || WeaponID == 30 || WeaponID == 61)
		return "adfhga1234x";
	else if (WeaponID == 38 || WeaponID == 11)
		return "123z241rfa";
	else if (WeaponID == 40)
		return "asdzf4312";
	else if (WeaponID == 9)
		return "asdx2345";
	else if (WeaponID == 43 || WeaponID == 47 ||
		WeaponID == 44 || WeaponID == 48 ||
		WeaponID == 46 || WeaponID == 45 ||
		WeaponID == 49)
		return "asdfz123erz";
	else if (WeaponID == 29 || WeaponID == 25 ||
		WeaponID == 35 || WeaponID == 27)
		return "zd12wd1dsa";
	else if (WeaponID == 59 || WeaponID == 42
		|| WeaponID >= 500 && WeaponID <= 530)
		return "zasd31zwsda";
	else if (WeaponID == -1)
		return "null";
	else
		return "aszd3124as";

}