#include "headers.h"
#include "WTS.h"
#include "Entity.h"

#define _USE_MATH_DEFINES
#define M_PI 3.141592653589793238462643383279502884197169399375105820974944


using namespace hazedumper::signatures;
using namespace hazedumper::netvars;

D3DXVECTOR3 CalcAngle(register const D3DXVECTOR3& src, register const D3DXVECTOR3& dst)
{

    const auto sqrtss = [](float in) {
        __m128 reg = _mm_load_ss(&in);
        return _mm_mul_ss(reg, _mm_rsqrt_ss(reg)).m128_f32[0];
    };

    D3DXVECTOR3 angles;

    D3DXVECTOR3 delta = src - dst;

    float hyp = sqrtss(delta.x * delta.x + delta.y * delta.y);

    angles.x = atan2f(delta.z, hyp) * (180.0f / M_PI);
    angles.y = atanf(delta.y / delta.x) * (180.0f / M_PI) + !((*(DWORD*)&delta.x) >> 31 & 1) * 180.0f;

    angles.z = 0.0f;

    return angles;
}

void NormalizeAngle(D3DXVECTOR3& angle) {
    while (angle.x > 89.f)
        angle.x -= 180.f;

    while (angle.x < -89.f)
        angle.x += 180.f;

    while (angle.y > 180.f)
        angle.y -= 360.f;

    while (angle.y < -180.f)
        angle.y += 360.f;
}

void ClampAngle(D3DXVECTOR3& angle) {
    if (angle.y > 180.0f)
        angle.y = 180.0f;
    else if (angle.y < -180.0f)
        angle.y = -180.0f;

    if (angle.x > 89.0f)
        angle.x = 89.0f;
    else if (angle.x < -89.0f)
        angle.x = -89.0f;

    angle.z = 0;
}

D3DXVECTOR3 aim = { 0,0,0 };

DWORD FindClosestEnemy(SOCKET connection, DWORD procID, DWORD localPlayer, DWORD moduleBase, DWORD engineBase, int ResXValue, int ResYValue) {

    Entity entity;
    LocalPlayer player;
    D3DXVECTOR3 bone2D;

    double lowDist = 9999;
    DWORD PTA = NULL;

    for (short int i = 0; i < 65; i++) {

        DWORD ebase = entity.GetEntityAddress(connection, procID, i, moduleBase);

        if (localPlayer == NULL)
            continue;
        if (ebase == NULL)
            continue;
        if (player.GetTeam(connection, procID, localPlayer) == 1)
            continue;
        if (player.GetHealth(connection, procID, localPlayer) < 1)
            continue;
        if (player.IsFlashed(connection, procID, localPlayer, engineBase) > 140.0)
            continue;
        if (entity.GetHealth(connection, procID, ebase) < 1)
            continue;
        if (entity.Dormant(connection, procID, ebase) == 1)
            continue;
        if (entity.Spotted(connection, procID, ebase) != 1)
            continue;
        if (entity.SpottedByPlayer(connection, procID, ebase, engineBase) == false)
            continue;
   
        D3DXVECTOR3 bone3D;
        bone3D = entity.GetBonePos(connection, procID, ebase, 8);
        WorldToScreen(connection, procID, bone3D, bone2D, ResXValue, ResYValue, moduleBase);
        double distance = sqrt(((ResXValue / 2 - bone2D.x) * (ResXValue / 2 - bone2D.x)) + ((ResYValue / 2 - bone2D.y) * (ResYValue / 2 - bone2D.y)));
        if (distance < lowDist) {
            lowDist = distance;
            PTA = ebase;
        }
    }

   
    if (PTA != NULL) {
        return PTA;
    }
    return NULL;
}

void ChangeAngles(SOCKET connection, DWORD procID, DWORD base, int bonePos, float punchAngle, int smoothness, DWORD localPlayer, DWORD moduleBase, DWORD engineBase) {

    LocalPlayer player;
    Entity target;

    CGlobalVarsBase GlobalVarsBase;
    GlobalVarsBase = driver::read<CGlobalVarsBase>(connection, procID, engineBase + dwGlobalVars);

    D3DXVECTOR3 myPos = player.GetPos(connection, procID, localPlayer);
    D3DXVECTOR3 viewOff = player.GetViewOffset(connection, procID, localPlayer);
    D3DXVECTOR3 playerVel = player.GetVelocity(connection, procID, localPlayer);
    D3DXVECTOR3 pos = (myPos + viewOff) + playerVel * GlobalVarsBase.interval_per_tick;

    D3DXVECTOR3 currAngles = player.GetViewAngles(connection, procID, localPlayer, engineBase);
    NormalizeAngle(currAngles);
    ClampAngle(currAngles);

    D3DXVECTOR3 targetBonePos = target.GetBonePos(connection, procID, base, bonePos);
    D3DXVECTOR3 targetVel = target.GetVelocity(connection, procID, base);
    targetBonePos = targetBonePos + targetVel * GlobalVarsBase.interval_per_tick;
    D3DXVECTOR3 aimAngles = CalcAngle(pos, targetBonePos);
    NormalizeAngle(aimAngles);
    ClampAngle(aimAngles);


    D3DXVECTOR3 punchAngles = player.GetPunchAngle(connection, procID, localPlayer);
    NormalizeAngle(punchAngles);
    ClampAngle(punchAngles);

    aimAngles -= punchAngles * punchAngle;

    D3DXVECTOR3 delta = (aimAngles - currAngles);
    NormalizeAngle(delta);
    ClampAngle(delta);

    D3DXVECTOR3 newAngle = currAngles + delta / smoothness;
    NormalizeAngle(newAngle);
    ClampAngle(newAngle);

    player.SetViewAngles(connection, procID, engineBase, newAngle);

}

void Aimbot(SOCKET connection, DWORD procID, DWORD base, DWORD localPlayer, DWORD moduleBase, DWORD engineBase,
    int SmoothValue, int FovValue, float RecoilControlValue, int RifleBone,
    int AwpBone, int ShotgunBone, int AutoSniperBone, int PistolBone, int ResXValue, int ResYValue) {

    if (base != NULL) {

        Entity target;

        LocalPlayer player;

        int fov = 30;
        int extraFov = FovValue;

        if (target.GetHealth(connection, procID, base) > 0) {
            int boneFov = 8;
            int extraFovX = 4;
            int extraFovY = 3;

            int weaponID = player.GetCurrentWeapon(connection, procID, localPlayer, moduleBase);
            if (player.GetCurrentWeaponClass(weaponID) == "aszd3124as" || player.GetCurrentWeaponClass(weaponID) == "123z241rfa")
                boneFov = RifleBone;
            else if (player.GetCurrentWeaponClass(weaponID) == "adfhga1234x")
                boneFov = PistolBone;
            else if (player.GetCurrentWeaponClass(weaponID) == "asdx2345" || player.GetCurrentWeaponClass(weaponID) == "asdzf4312" || player.GetCurrentWeaponClass(weaponID) == "123z241rfa") {
                boneFov = 7;
                extraFov += extraFov * 2;
            }
  
            D3DXVECTOR3 targetBone = target.GetBonePos(connection, procID, base, boneFov);

            D3DXVECTOR3 targetBone2D;
            WorldToScreen(connection, procID, targetBone, targetBone2D, ResXValue, ResYValue, moduleBase);


            aim.x = targetBone2D.x - (ResXValue / 2);
            aim.y = targetBone2D.y - (ResYValue / 2);

            if (!(player.GetFlag(connection, procID, localPlayer) & 1 == 1) && !(target.GetFlag(connection, procID, base) & 1 == 1))
                SmoothValue += 50;

            if (aim.x < extraFov && aim.x > -extraFov && aim.y > -extraFov * 2 && aim.y < extraFov * 2) { 
                if (aim.x < fov / extraFovX && aim.x > -fov / extraFovX && aim.y < fov / extraFovY && aim.y > -fov / extraFovY)
                    SmoothValue *= 10000;
                int weaponID = player.GetCurrentWeapon(connection, procID, localPlayer, moduleBase);
                if (player.GetCurrentWeaponClass(weaponID) == "aszd3124as")
                    ChangeAngles(connection, procID, base, RifleBone, RecoilControlValue, SmoothValue, localPlayer, moduleBase, engineBase);
                if (player.GetCurrentWeaponClass(weaponID) == "adfhga1234x")
                    ChangeAngles(connection, procID, base, PistolBone, 1, SmoothValue, localPlayer, moduleBase, engineBase);
                if (player.GetCurrentWeaponClass(weaponID) == "123z241rfa")
                    ChangeAngles(connection, procID, base, AutoSniperBone, 1, SmoothValue + 10, localPlayer, moduleBase, engineBase);
                if (player.GetCurrentWeaponClass(weaponID) == "asdx2345")
                    ChangeAngles(connection, procID, base, AwpBone, 1, SmoothValue + 10, localPlayer, moduleBase, engineBase);
                if (player.GetCurrentWeaponClass(weaponID) == "asdzf4312")
                    ChangeAngles(connection, procID, base, 8, 1, SmoothValue + 10, localPlayer, moduleBase, engineBase);
            }

        }
        
    }
}
