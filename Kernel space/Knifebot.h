#pragma once
#include "headers.h"
#include "Entity.h"
#include "LocalPlayer.h"
#include <math.h>

using namespace hazedumper::signatures;
using namespace hazedumper::netvars;

float CalcDistance(D3DXVECTOR3 playerPos, D3DXVECTOR3 entityPos) {

    D3DXVECTOR3 vec0 = { 0,0,0 };
    vec0.x = entityPos.x - playerPos.x;
    vec0.y = entityPos.y - playerPos.y;
    vec0.z = entityPos.z - playerPos.z;

    float distance = sqrt(vec0.x * vec0.x + vec0.y * vec0.y + vec0.z * vec0.z);
    return distance;
}

DWORD FindClosestEnemyToKnife(SOCKET connection, DWORD procID, DWORD localPlayer, DWORD moduleBase) {

    Entity entity;
    LocalPlayer player;
    double lowDist = 9999;
    DWORD PTK = NULL;

    int weaponID = player.GetCurrentWeapon(connection, procID, localPlayer, moduleBase);

    for (short int i = 0; i < 65; i++) {

        DWORD ebase = entity.GetEntityAddress(connection, procID, i, moduleBase);

        if(player.GetCurrentWeaponClass(weaponID) != "zasd31zwsda")
            continue;
        if (localPlayer == NULL)
            continue;
        if (ebase == NULL)
            continue;
        if (localPlayer == ebase)
            continue;
        if (player.GetHealth(connection, procID, localPlayer) < 1)
            continue;
        if (entity.GetHealth(connection, procID, ebase) < 1)
            continue;
        if (entity.Dormant(connection, procID, ebase) == 1)
            continue;
        if (player.GetTeam(connection, procID, localPlayer) == 2)
            continue;
        if (entity.GetTeam(connection, procID, ebase) == player.GetTeam(connection, procID, localPlayer))
            continue;

        
        D3DXVECTOR3 playerPos = player.GetPos(connection, procID, localPlayer);
        D3DXVECTOR3 entityPos = entity.GetPos(connection, procID, ebase);
        float distance = CalcDistance(playerPos, entityPos);
        if (distance < lowDist && distance < 66) {
            cout << distance << endl;
            lowDist = distance;
            PTK = ebase;
        }
    }
    if (PTK != NULL) {
        return PTK;
    }
    return NULL;
}

void Knife(SOCKET connection, DWORD procID, DWORD base, DWORD moduleBase) {

    if (base) {
        driver::write(connection, procID, moduleBase + dwForceAttack, 6);
    }
}