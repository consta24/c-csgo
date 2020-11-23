#include <windows.h>
#include <iostream>
#include "csgo.hpp"
#include "Entity.h"
#include "LocalPlayer.h"


using namespace hazedumper::netvars;
using namespace hazedumper::signatures;

struct GlowStruct {

    BYTE base[4];
    float red;
    float green;
    float blue;
    float alpha;
    BYTE buffer1[16];
    bool renderWhenOccluded;
    bool renderWhenUnOccluded;
    bool fullBloom;
    BYTE buffer2[5];
    int glowStyle;

}GLOW;

struct ClrRender {

    BYTE red, green, blue;

};



GlowStruct SetGlowColor(GlowStruct Glow, DWORD entBase, SOCKET connection, DWORD procID, DWORD moduleBase) {

    bool isDefusing = driver::read<bool>(connection, procID, entBase + m_bIsDefusing);
    if (isDefusing) {
        Glow.red = 1.0f;
        Glow.green = 1.0f;
        Glow.blue = 1.0f;
        Glow.alpha = 1.0f;
    }
    else {
        int health = driver::read<int>(connection, procID, entBase + off.healthAddy);
        Glow.red = health * -0.01 + 1;
        Glow.green = health * 0.01;
        Glow.alpha = 1.0f;
    }
    Glow.renderWhenOccluded = true;
    Glow.renderWhenUnOccluded = false;

    return Glow;
}
void SetTeamGlow(uintptr_t entity, int glowIndex, SOCKET connection, DWORD procID) {

    GlowStruct TGlow;
    TGlow = driver::read<GlowStruct>(x.glowObject + (glowIndex * 0x38));

    TGlow.blue = 1.0f;
    TGlow.alpha = 1.0f;
    TGlow.renderWhenOccluded = true;
    TGlow.renderWhenUnOccluded = false;

    driver::write<GlowStruct>(x.glowObject + (glowIndex * 0x38), TGlow);
}

void SetEnemyGlow(uintptr_t entity, int glowIndex, SOCKET connection, DWORD procID) {

    GlowStruct EGlow;
    EGlow = driver::read<GlowStruct>(x.glowObject + (glowIndex * 0x38));
    EGlow = SetGlowColor(EGlow, entity);

    driver::write<GlowStruct>(x.glowObject + (glowIndex * 0x38), EGlow);

}

void SetBrightness(float value, SOCKET connection, DWORD procID) {

    float brightness = value;
    int ptr = driver::read<int>(x.engineModule + model_ambient_min);
    int xorptr = *(int*)&brightness ^ ptr;
    driver::write<int>(x.engineModule + model_ambient_min, xorptr);

}

ClrRender enemyRender;

ClrRender teamRender;

void ChamsOn() {

    teamRender.red = 0;
    teamRender.blue = 255;
    teamRender.green = 0;

    enemyRender.red = 255;
    enemyRender.blue = 0;
    enemyRender.green = 0;

}
void ChamsOffFunction(SOCKET connection, DWORD procID, DWORD moduleBase) {

    teamRender.red = -1;
    teamRender.blue = -1;
    teamRender.green = -1;

    enemyRender.red = -1;
    enemyRender.blue = -1;
    enemyRender.green = -1;

    for (short int i = 0; i < 64; i++) {
        x.entity = driver::read<uintptr_t>(connection, procID, moduleBase + off.entityListAddy + i * 0x10);
        if (x.entity != NULL) {

            driver::write<ClrRender>(connection, procID, moduleBase + m_clrRender, teamRender);
            driver::write<ClrRender>(connection, procID, moduleBase + m_clrRender, enemyRender);

        }
    }

}

void HandleGlow(SOCKET connection, DWORD procID, DWORD moduleBase, DWORD localPlayerAdd) {

    LocalPlayer localPlayer;
    Entity entityPlayer;
    int glowIndex;

    

    ChamsOn();

    for (short int i = 0; i < 64; i++) {
        DWORD entBase = entityPlayer.GetEntityAddress(connection, procID, i, moduleBase);
        if (entBase != NULL) {

            glowIndex = driver::read<int>(connection, procID, entBase + m_iGlowIndex);
            if (localPlayer.GetTeam(connection, procID, localPlayerAdd) == entityPlayer.GetTeam(connection, procID, entBase)) {
                SetTeamGlow(entBase, glowIndex, connection, procID);
                driver::write<ClrRender>(connection, procID, entBase + m_clrRender, teamRender);
            }
            else {
                SetEnemyGlow(entBase, glowIndex, connection, procID);
                driver::write<ClrRender>(connection, procID, entBase + m_clrRender, enemyRender);
            }
        }
    }

}



