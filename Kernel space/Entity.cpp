#include "headers.h"
#include "Entity.h"



using namespace hazedumper::signatures;
using namespace hazedumper::netvars;


struct Matrix3x4_t {
	float Matrix[3][4];
};

DWORD Entity::GetEntityAddress(SOCKET connection, DWORD procID, int index, DWORD moduleBase) {
	DWORD entityPlayer = driver::read<DWORD>(connection, procID, moduleBase + dwEntityList + (index * 0x10));
	return entityPlayer;
}

D3DXVECTOR3 Entity::GetPos(SOCKET connection, DWORD procID, DWORD base) {
	D3DXVECTOR3 pos;
	pos = driver::read<D3DXVECTOR3>(connection, procID, base + m_vecOrigin);
	return pos;
}
D3DXVECTOR3 Entity::GetBonePos(SOCKET connection, DWORD procID, DWORD base, int bone) {

	DWORD BoneMatrix = driver::read<DWORD>(connection, procID, base + m_dwBoneMatrix);
	Matrix3x4_t boneMatrix = driver::read<Matrix3x4_t>(connection, procID, BoneMatrix + (bone * 0x30));
	return{
	boneMatrix.Matrix[0][3],
	boneMatrix.Matrix[1][3],
	boneMatrix.Matrix[2][3]
	};
}

D3DXVECTOR3 Entity::GetVelocity(SOCKET connection, DWORD procID, DWORD base) {
	D3DXVECTOR3 velocity;
	velocity = driver::read<D3DXVECTOR3>(connection, procID, base + m_vecVelocity);
	return velocity;
}
bool Entity::SpottedByPlayer(SOCKET connection, DWORD procID, DWORD base, DWORD engineBase) {

	DWORD clientState = driver::read<DWORD>(connection, procID, engineBase + dwClientState);
	int localPlayerID = driver::read<int>(connection, procID, clientState + dwClientState_GetLocalPlayer);
	DWORD spottedByMask = driver::read<DWORD>(connection, procID, base + m_bSpottedByMask);
	if (spottedByMask & (1 << localPlayerID))
		return true;
	return false;

}
int Entity::GetHealth(SOCKET connection, DWORD procID, DWORD base) {
	int temp;
	temp = driver::read<int>(connection, procID, base + m_iHealth);
	return temp;
}
int Entity::GetTeam(SOCKET connection, DWORD procID, DWORD base) {
	int temp;
	temp = driver::read<int>(connection, procID, base + m_iTeamNum);
	return temp;
}
int Entity::Spotted(SOCKET connection, DWORD procID, DWORD base) {
	int temp;
	temp = driver::read<bool>(connection, procID, base + m_bSpotted);
	return temp;
}
int Entity::Dormant(SOCKET connection, DWORD procID, DWORD base) {
	int temp;
	temp = driver::read<bool>(connection, procID, base + m_bDormant);
	return temp;
}
DWORD Entity::GetFlag(SOCKET connection, DWORD procID, DWORD base) {
	DWORD flag;
	flag = driver::read<DWORD>(connection, procID, base + m_fFlags);
	return flag;
}